/*!
 * @file      Detector_impl.cpp
 * @brief     Defines the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 28, 2017
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
#include <list>

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
        // Calculate our euclidean distances and winners
        {
            Symbol* symbol = m_symbols[0].get();
            double* distance = m_distances.get();
            while(symbol < m_symbols[0].get()+inputSize)
            {
                for(unsigned point=0; point<fieldSize; ++point)
                    distance[point] = std::norm(
                            static_cast<std::complex<double>>(*input)
                            -constellation[point]);
                ++input;
                *symbol++ = static_cast<Symbol>(
                        std::min_element(
                            distance,
                            distance+fieldSize)-distance);
                distance += fieldSize;
            }
            input -= 2*history;
        }

        double metric[2];
        metric[0] = 0;

        m_mapper.map(
                m_symbols[0].get(),
                m_started,
                m_probabilities[0].get(),
                m_codewordPosition);

        for(
                unsigned symbol=0;
                symbol < m_windowSize+history;
                ++symbol)
        {
            const unsigned offsetSymbol = history+symbol;
            m_metrics[0][symbol] =
                m_distances[offsetSymbol*fieldSize+m_symbols[0][offsetSymbol]]
                -m_noisePower*std::log(m_probabilities[0][symbol]);
            metric[0] += m_metrics[0][symbol];
        }

        
        for(
                unsigned symbol=0;
                symbol < inputSize;
                ++symbol)
        {
            const double& currentDistance = m_distances[symbol*fieldSize+m_symbols[0][symbol]];
            if(currentDistance < 0.36)
                continue;

            metric[1] = 0;

            std::copy(
                    m_symbols[0].get(),
                    m_symbols[0].get()+inputSize,
                    m_symbols[1].get());

            // Flip a symbol
            {
                double* distance = m_distances.get()+symbol;
                Symbol& current = m_symbols[1][symbol];
                Symbol next = static_cast<Symbol>(
                        std::max_element(
                            distance,
                            distance+fieldSize)-distance);
                for(Symbol point=0; point<fieldSize; ++point)
                    if(
                            distance[next] > distance[point]
                            && distance[point] > distance[current])
                        next = point;
                current = next;
            }

            m_mapper.map(
                    m_symbols[1].get(),
                    m_started,
                    m_probabilities[1].get(),
                    m_codewordPosition);

            for(
                    unsigned symbol=0;
                    symbol < m_windowSize+history;
                    ++symbol)
            {
                const unsigned offsetSymbol = history+symbol;
                m_metrics[1][symbol] =
                    m_distances[offsetSymbol*fieldSize+m_symbols[1][offsetSymbol]]
                    -m_noisePower*std::log(m_probabilities[1][symbol]);
                metric[1] += m_metrics[1][symbol];
            }
            
            if(metric[1] < metric[0])
            {
                metric[0] = metric[1];
                m_symbols[0].swap(m_symbols[1]);
                m_probabilities[0].swap(m_probabilities[1]);
                m_metrics[0].swap(m_metrics[1]);
            }
        }

        m_codewordPosition=(m_codewordPosition+noutput_items)%m_codewordLength;
        m_started = true;

        output = std::copy(
                m_symbols[0].get()+history,
                m_symbols[0].get()+history+m_windowSize,
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
    m_distances(new double[m_mapper.inputSize()*fieldSize])
{
    m_symbols[0].reset(new Symbol[m_mapper.inputSize()]);
    m_symbols[1].reset(new Symbol[m_mapper.inputSize()]);
    m_probabilities[0].reset(new float[windowSize + m_mapper.history()]);
    m_probabilities[1].reset(new float[windowSize + m_mapper.history()]);
    m_metrics[0].reset(new double[windowSize + m_mapper.history()]);
    m_metrics[1].reset(new double[windowSize + m_mapper.history()]);

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

template class gr::gs::Detector<unsigned char>;
template class gr::gs::Detector<unsigned short>;
template class gr::gs::Detector<unsigned int>;
