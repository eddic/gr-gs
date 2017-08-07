/*!
 * @file      Detector_impl.cpp
 * @brief     Defines the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 7, 2017
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

template<typename Symbol>
int gr::gs::Implementations::Detector_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    const Symbol* input = reinterpret_cast<const Symbol*>(input_items[0]);
    float* output = reinterpret_cast<float*>(output_items[0]);

    m_rds = m_mapper.map(
            input,
            m_started,
            output,
            m_rds,
            noutput_items,
            m_codewordPosition);

    for(
            float* const outputEnd=output+noutput_items;
            output < outputEnd;
            ++output)
        *output = -std::log2(*output);

    m_codewordPosition = (m_codewordPosition+noutput_items)%m_codewordLength;
    m_started = true;

    return noutput_items;
}

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Detector_impl(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const std::string& framingTag):
    gr::sync_block("Guided Scrambling Detector",
        io_signature::make(1,1,fieldSize*sizeof(float)),
        io_signature::make(1,1,sizeof(Symbol))),
    m_framingTag(framingTag),
    m_framingTagPMT(pmt::string_to_symbol(framingTag)),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_rds(GuidedScrambling::startingRDS),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength,
            minCorrelation),
    m_started(false)
{
    this->enable_update_rate(false);
    this->set_history(m_mapper.history()+1);
}

template<typename Symbol>
typename gr::gs::Detector<Symbol>::sptr gr::gs::Detector<Symbol>::make(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const std::string& framingTag)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Detector_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                minCorrelation,
                framingTag));
}

template class gr::gs::Detector<unsigned char>;
template class gr::gs::Detector<unsigned short>;
template class gr::gs::Detector<unsigned int>;
