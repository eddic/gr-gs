/*!
 * @file      Detector_impl.cpp
 * @brief     Defines the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 3, 2017
 * @copyright Copyright &copy; 2017 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2017 Eddie Carle
 *
 * This file is part of the Guided Scrambling GNU Radio Module
 *
 * The Guided Scrambling GNU Radio Module is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The Guided Scrambling GNU Radio Module is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * The Guided Scrambling GNU Radio Module.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "Detector_impl.hpp"

#include <gnuradio/io_signature.h>
#include <algorithm>
#include <limits>

template<typename Symbol>
double gr::gs::Implementations::Detector_impl<Symbol>::noisePower() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_noisePower;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::set_noisePower(
        const double noise)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_noisePower = noise;
}

template<typename Symbol>
int gr::gs::Implementations::Detector_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const auto& inputSize = m_mapper.inputSize();
    const auto& history = m_mapper.history();
    const auto& constellation = m_mapper.constellation();
    const auto& fieldSize = constellation.size();
    const Complex*& input = reinterpret_cast<const Complex*&>(input_items[0]);
    Symbol*& output = reinterpret_cast<Symbol*&>(output_items[0]);
    unsigned outputted=0;

    // Get our codeword lined up
    if(!m_started && !m_framingTag.empty())
    {
        std::vector<gr::tag_t> tags;
        std::vector<gr::tag_t>::const_iterator tag;

        this->get_tags_in_range(
                tags,
                0,
                this->nitems_read(0),
                this->nitems_read(0)+noutput_items,
                m_framingTagPMT);
        tag = tags.cbegin();

        if(tag != tags.cend())
        {
            const size_t offset = tag->offset - this->nitems_read(0);

            input += offset;
            output += offset;
            noutput_items -= offset;
        }
    }

    while(noutput_items-outputted >= inputSize-history)
    {
        Ranks ranks;

        // Calculate our euclidean distances, winners and ranks
        {
            for(unsigned symbol=0; symbol < inputSize; ++symbol)
            {
                typename Rank::Distances distances;
                for(Symbol point=0; point<fieldSize; ++point)
                    distances.push_back({std::norm(
                                static_cast<std::complex<double>>(*input)
                                -constellation[point]), point});
                sort(distances);
                m_symbols[symbol] = distances.front().symbol;
                m_distances[symbol] = distances.front().distance;

                ranks.push_back(Rank());
                ranks.back().score = (++distances.cbegin())->distance
                    -distances.front().distance;
                ranks.back().index = symbol;
                ranks.back().distances = std::move(distances);
                ranks.back().winner = ranks.back().distances.cbegin();

                ++input;
            }
            input -= 2*history;
            sort(ranks);
        }

        // Get our starting RDS probabilities
        double metric[2];
        metric[0] = 0;

        m_mapper.map(
                m_symbols.get(),
                m_started,
                m_probabilities.get(),
                m_codewordPosition);

        // And now compute our metrics
        for(
                unsigned symbol=0;
                symbol < m_windowSize+history;
                ++symbol)
        {
            m_metrics[symbol] =
                m_distances[history+symbol]
                -m_noisePower*std::log(m_probabilities[symbol]);
            metric[0] += m_metrics[symbol];
        }

        const unsigned m_depth = 8;
        
        // Now we optimize
        while(true)
        {
            unsigned depth = 0;
            bool improving = false;
            for(Rank& rank: ranks)
            {
                metric[1] = 0;

                // Flip a symbol
                increment(rank);
                m_distances[rank.index] = rank.winner->distance;
                m_symbols[rank.index] = rank.winner->symbol;

                // Map our RDS probabilities
                m_mapper.map(
                        m_symbols.get(),
                        m_started,
                        m_probabilities.get(),
                        m_codewordPosition);

                // Build our MAP metrics
                for(
                        unsigned symbol=0;
                        symbol < m_windowSize+history;
                        ++symbol)
                {
                    m_metrics[symbol] =
                        m_distances[history+symbol]
                        -m_noisePower*std::log(m_probabilities[symbol]);
                    metric[1] += m_metrics[symbol];
                }
                
                // Are things any better?
                if(metric[1] < metric[0])
                {
                    metric[0] = metric[1];
                    depth = 0;
                    improving = true;
                    rank.distances.splice(
                            rank.distances.begin(),
                            rank.distances,
                            rank.winner);
                    rank.winner = rank.distances.begin();
                }
                else
                {
                    decrement(rank);
                    m_distances[rank.index] = rank.winner->distance;
                    m_symbols[rank.index] = rank.winner->symbol;

                    ++depth;
                    if(depth >= m_depth)
                        break;
                }
            }
            if(improving)
                sort(ranks);
            else
                break;
        }

        m_codewordPosition
            = (m_codewordPosition+noutput_items)%m_codewordLength;
        m_started = true;

        output = std::copy(
                m_symbols.get()+history,
                m_symbols.get()+history+m_windowSize,
                output);
        outputted += m_windowSize;
    }

    return outputted;
}

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Detector_impl(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const double noise,
        const unsigned windowSize,
        const std::string& framingTag):
    gr::sync_block("Guided Scrambling Detector",
        io_signature::make(1,1,sizeof(gr::gs::Complex)),
        io_signature::make(1,1,sizeof(Symbol))),
    m_noisePower(noise),
    m_framingTag(framingTag),
    m_framingTagPMT(pmt::string_to_symbol(framingTag)),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_rds(GuidedScrambling::startingRDS),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength,
            minCorrelation,
            windowSize,
            true),
    m_started(false),
    m_windowSize(windowSize),
    m_symbols(new Symbol[m_mapper.inputSize()]),
    m_distances(new double[m_mapper.inputSize()]),
    m_probabilities(new float[windowSize + m_mapper.history()]),
    m_metrics(new double[windowSize + m_mapper.history()])
{
    this->enable_update_rate(false);
    this->set_history(m_mapper.history()+1);
    this->set_min_noutput_items(m_windowSize+m_mapper.history());
}

template<typename Symbol>
typename gr::gs::Detector<Symbol>::sptr gr::gs::Detector<Symbol>::make(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const double noise,
        const unsigned windowSize,
        const std::string& framingTag)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Detector_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                minCorrelation,
                noise,
                windowSize,
                framingTag));
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::sort(Ranks& ranks)
{
    ranks.sort(
            [] (const Rank& x, const Rank& y)
            {
                return x.score < y.score;
            });
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::sort(
        typename Rank::Distances& distances)
{
    distances.sort(
            [] (
                const typename Rank::Distance& x,
                const typename Rank::Distance& y)
            {
                return x.distance < y.distance;
            });
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::increment(Rank& rank)
{
    ++rank.winner;
    if(rank.winner == rank.distances.cend())
    {
        rank.score = 0;
        return;
    }

    auto next = rank.winner;
    ++next;
    if(next == rank.distances.cend())
    {
        rank.score = 0;
        return;
    }

    rank.score = rank.winner->distance - next->distance;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::decrement(Rank& rank)
{
    const auto next = rank.winner;
    --rank.winner;
    rank.score = rank.winner->distance - next->distance;
}

template class gr::gs::Detector<unsigned char>;
template class gr::gs::Detector<unsigned short>;
template class gr::gs::Detector<unsigned int>;
