/*!
 * @file      Entropy_impl.cpp
 * @brief     Defines the "Entropy" GNU Radio block implementation
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

#include <cmath>

#include "Entropy_impl.hpp"

template<typename Symbol>
int gr::gs::Implementations::Entropy_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    const Symbol* input = reinterpret_cast<const Symbol*>(input_items[0]);
    float* output = reinterpret_cast<float*>(output_items[0]);
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

    while(noutput_items-outputted >= m_mapper.inputSize())
    {
        m_mapper.collapseConstellation(
                m_realSymbols.get(),
                m_imagSymbols.get(),
                input,
                windowSize + m_mapper.history());

        m_mapper.map(
                m_realSymbols.get(),
                m_started,
                m_realProbabilities.get(),
                m_codewordPosition,
                true);
        m_mapper.map(
                m_imagSymbols.get(),
                m_started,
                m_imagProbabilities.get(),
                m_codewordPosition,
                false);

        for(unsigned i=0; i<windowSize; ++i)
            output[i] =
                -std::log2(m_realProbabilities[i] * m_imagProbabilities[i]);

        m_codewordPosition = (m_codewordPosition+windowSize)%m_codewordLength;
        input += windowSize;
        outputted += windowSize;
        m_started = true;
    }

    return outputted;
}

template<typename Symbol>
gr::gs::Implementations::Entropy_impl<Symbol>::Entropy_impl(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const double minCorrelation,
        const std::string& framingTag):
    gr::sync_block("Entropy",
        io_signature::make(1, 1 ,sizeof(Symbol)),
        io_signature::make(1, 1, sizeof(float))),
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
            false),
    m_started(false),
    m_realSymbols(new Symbol[windowSize]),
    m_imagSymbols(new Symbol[windowSize]),
    m_realProbabilities(new float[windowSize]),
    m_imagProbabilities(new float[windowSize])
{
    this->enable_update_rate(false);
    this->set_history(m_mapper.history()+1);
    this->set_min_noutput_items(windowSize);
}

template<typename Symbol>
typename gr::gs::Entropy<Symbol>::sptr gr::gs::Entropy<Symbol>::make(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const double minCorrelation,
        const std::string& framingTag)
{
    return gnuradio::get_initial_sptr(
            new ::gr::gs::Implementations::Entropy_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                minCorrelation,
                framingTag));
}

template class gr::gs::Entropy<unsigned char>;
template class gr::gs::Implementations::Entropy_impl<unsigned char>;
template class gr::gs::Entropy<unsigned short>;
template class gr::gs::Implementations::Entropy_impl<unsigned short>;
template class gr::gs::Entropy<unsigned int>;
template class gr::gs::Implementations::Entropy_impl<unsigned int>;
