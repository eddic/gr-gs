/*!
 * @file      BCJR_impl.cpp
 * @brief     Defines the "Guided Scrambling BCJR" GNU Radio block
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

#include "BCJR_impl.hpp"

#include <gnuradio/io_signature.h>
#include <algorithm>
#include <limits>

template<typename Symbol>
double gr::gs::Implementations::BCJR_impl<Symbol>::noisePower() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_noisePower;
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::set_noisePower(
        const double noise)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_noisePower = noise;
}

template<typename Symbol>
int gr::gs::Implementations::BCJR_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(m_maxSymbols > 0)
    {
        if(m_symbols >= m_maxSymbols)
            return -1;

        if(m_symbols+noutput_items > m_maxSymbols)
            noutput_items = m_maxSymbols-m_symbols;
    }

    if(m_maxErrors > 0 && m_errors >= m_maxErrors)
        return -1;

    const auto& inputSize = m_mapper.inputSize();
    const auto& history = m_mapper.history();
    const Complex*& input = reinterpret_cast<const Complex*&>(input_items[0]);
    const Symbol*& properSymbols = reinterpret_cast<const Symbol*&>(input_items[1]);
    unsigned outputted=0;

    while(noutput_items-outputted >= inputSize-history && m_errors<m_maxErrors)
    {
        detect(input, m_realSymbols, true);
        detect(input, m_imagSymbols, false);

        m_codewordPosition
            = (m_codewordPosition+noutput_items)%m_codewordLength;
        m_started = true;

        for(unsigned i=history; i<inputSize; ++i)
            if(m_mapper.decollapseConstellation(
                        m_realSymbols[i],
                        m_imagSymbols[i]) != properSymbols[i])
                ++m_errors;

        m_symbols += m_windowSize;
        outputted += m_windowSize;
        input += m_windowSize;
        properSymbols += m_windowSize;
    }

    return outputted;
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::detect(
        const Complex* input,
        std::unique_ptr<Symbol[]>& symbols,
        const bool real)
{
    const auto& inputSize = m_mapper.inputSize();
    const auto& constellation = m_mapper.constellation(real);

    // Calculate our euclidean distances, winners and ranks
    {
        const unsigned constellationSize = constellation.size();
        const unsigned distanceSize =  inputSize*constellationSize;
        auto output = symbols.get();
        for(
                unsigned symbol=0;
                symbol < distanceSize;
                symbol += constellationSize)
        {
            const double value = real?input->real():input->imag();
            ++input;
            for(Symbol point=0; point < constellationSize; ++point)
            {
                const double difference = value - constellation[point];
                m_distances[symbol + point] = difference*difference;
            }

            *output++ = std::min_element(
                    m_distances.get()+symbol,
                    m_distances.get()+symbol+constellationSize)
                -(m_distances.get()+symbol);

        }
    }

    return;
}

template<typename Symbol>
gr::gs::Implementations::BCJR_impl<Symbol>::BCJR_impl(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const double noise,
        const unsigned windowSize,
        const unsigned long long maxErrors,
        const unsigned long long maxSymbols):
    gr::sync_block("Guided Scrambling BCJR",
        io_signature::make2(2,2,sizeof(gr::gs::Complex), sizeof(Symbol)),
        io_signature::make(0,0,0)),
    m_noisePower(noise),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength,
            minCorrelation,
            windowSize,
            false),
    m_started(false),
    m_windowSize(windowSize),
    m_realSymbols(new Symbol[m_mapper.inputSize()]),
    m_imagSymbols(new Symbol[m_mapper.inputSize()]),
    m_distances(new double[
            m_mapper.inputSize()*m_mapper.constellation(true).size()]),
    m_probabilities(new float[windowSize + m_mapper.history()]),
    m_metrics(new double[windowSize + m_mapper.history()]),
    m_maxErrors(maxErrors),
    m_maxSymbols(maxSymbols),
    m_errors(0),
    m_symbols(0)
{
    this->enable_update_rate(false);
    this->set_history(m_mapper.history()+1);
    this->set_min_noutput_items(m_windowSize);
}

template<typename Symbol>
typename gr::gs::BCJR<Symbol>::sptr gr::gs::BCJR<Symbol>::make(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const double noise,
        const unsigned windowSize,
        const unsigned long long maxErrors,
        const unsigned long long maxSymbols)
{
    return gnuradio::get_initial_sptr(
            new Implementations::BCJR_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                minCorrelation,
                noise,
                windowSize,
                maxErrors,
                maxSymbols));
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::reset()
{
    m_symbols = 0;
    m_errors = 0;
}

template<typename Symbol> unsigned long long
gr::gs::Implementations::BCJR_impl<Symbol>::symbols() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_symbols;
}

template<typename Symbol> unsigned long long
gr::gs::Implementations::BCJR_impl<Symbol>::errors() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_errors;
}

template<typename Symbol>
double gr::gs::Implementations::BCJR_impl<Symbol>::rate() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<double>(m_errors)/static_cast<double>(m_symbols);
}

template<typename Symbol>
bool gr::gs::Implementations::BCJR_impl<Symbol>::finished() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_maxErrors>0 && m_errors >= m_maxErrors)
        return true;
    if(m_maxSymbols>0 && m_symbols >= m_maxSymbols)
        return true;
    return false;
}

template class gr::gs::BCJR<unsigned char>;
template class gr::gs::BCJR<unsigned short>;
template class gr::gs::BCJR<unsigned int>;
