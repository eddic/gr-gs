/*!
 * @file      Entropy_impl.cpp
 * @brief     Defines the "Entropy" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 31, 2018
 * @copyright Copyright &copy; 2018 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2018 Eddie Carle
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
    float* const outputEnd = output+noutput_items;

    // Get our codeword lined up
    if(!m_aligned)
    {
        std::vector<gr::tag_t> tags;

        this->get_tags_in_range(
                tags,
                0,
                this->nitems_read(0),
                this->nitems_read(0)+noutput_items,
                m_alignmentTag);
        const std::vector<gr::tag_t>::const_iterator tag = tags.cbegin();

        if(tag != tags.cend())
        {
            const size_t offset = tag->offset - this->nitems_read(0);
            input += offset;
            output += offset;
        }
        m_aligned = true;
    }

    while(output < outputEnd)
    {
        const Symbol real = m_mapper.realConstellationPoint(*input);
        const Symbol imag = m_mapper.imagConstellationPoint(*input);

        *output = -std::log2(
                m_mapper.probability(
                    m_codewordPosition,
                    m_realRDS,
                    real) * 
                m_mapper.probability(
                    m_codewordPosition,
                    m_imagRDS,
                    imag));

        if(++m_codewordPosition == m_codewordLength)
            m_codewordPosition=0;
        ++output;
        ++input;
        m_realRDS += m_mapper.collapsed()[real];
        m_imagRDS += m_mapper.collapsed()[imag];
    }

    return noutput_items;
}

template<typename Symbol>
gr::gs::Implementations::Entropy_impl<Symbol>::Entropy_impl(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const std::string& alignmentTag):
    gr::sync_block("Entropy",
        io_signature::make(1, 1 ,sizeof(Symbol)),
        io_signature::make(1, 1, sizeof(float))),
    m_alignmentTag(pmt::string_to_symbol(alignmentTag)),
    m_aligned(alignmentTag.empty()),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_realRDS(0),
    m_imagRDS(0),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength)
{
    this->enable_update_rate(false);
}

template<typename Symbol>
typename gr::gs::Entropy<Symbol>::sptr gr::gs::Entropy<Symbol>::make(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const std::string& alignmentTag)
{
    return gnuradio::get_initial_sptr(
            new ::gr::gs::Implementations::Entropy_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                alignmentTag));
}

template class gr::gs::Entropy<unsigned char>;
template class gr::gs::Implementations::Entropy_impl<unsigned char>;
template class gr::gs::Entropy<unsigned short>;
template class gr::gs::Implementations::Entropy_impl<unsigned short>;
template class gr::gs::Entropy<unsigned int>;
template class gr::gs::Implementations::Entropy_impl<unsigned int>;
