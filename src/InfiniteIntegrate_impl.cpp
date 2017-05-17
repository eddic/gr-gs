/*!
 * @file      InfiniteIntegrate_impl.cpp
 * @brief     Defines the "Infinite Integrate" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 16, 2017
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

#include <iostream>
#include "InfiniteIntegrate_impl.hpp"

#include <gnuradio/io_signature.h>

int gr::gs::Implementations::InfiniteIntegrate_impl::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const float* input = reinterpret_cast<const float*>(input_items[0]);
    float* output = reinterpret_cast<float*>(output_items[0]);

    for(int i=0; i<noutput_items; ++i)
    {
        for(unsigned j=0; j<this->decimation(); ++j)
            m_sum += *input++;

        *output++ = m_sum;
    }

    return noutput_items;
}

gr::gs::Implementations::InfiniteIntegrate_impl::InfiniteIntegrate_impl(
        const unsigned decimation):
    gr::sync_decimator("Infinite Integrate",
        io_signature::make(1,1,sizeof(float)),
        io_signature::make(1,1,sizeof(float)),
        decimation),
    m_sum(0)
{
    this->enable_update_rate(false);
}

gr::gs::InfiniteIntegrate::sptr gr::gs::InfiniteIntegrate::make(
        const unsigned decimation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::InfiniteIntegrate_impl(decimation));
}

void gr::gs::Implementations::InfiniteIntegrate_impl::reset()
{
    m_sum = 0;
}
