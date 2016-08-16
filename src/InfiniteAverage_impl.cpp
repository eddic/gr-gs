/*!
 * @file      InfiniteAverage_impl.cpp
 * @brief     Defines the "Infinite Average" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 11, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2016 Eddie Carle
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

#include "InfiniteAverage_impl.hpp"

#include <gnuradio/io_signature.h>
#include <array>

int gr::gs::Implementations::InfiniteAverage_impl::work(
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
        {
            ++m_count;
            for(unsigned k=0; k<m_sum.size(); ++k)
            {
                m_sum[k] += *input++;
                m_average[k] = m_sum[k]/m_count;
            }
        }

        for(const float& value: m_average)
            *output++ = value;
    }

    return noutput_items;
}

gr::gs::Implementations::InfiniteAverage_impl::InfiniteAverage_impl(
        const unsigned vectorSize,
        const unsigned decimation):
    gr::sync_decimator("Infinite Average",
        io_signature::make(1,1,sizeof(float)*vectorSize),
        io_signature::make(1,1,sizeof(float)*vectorSize),
        decimation),
    m_sum(vectorSize),
    m_count(0),
    m_average(vectorSize)
{
    this->enable_update_rate(false);
}

gr::gs::InfiniteAverage::sptr gr::gs::InfiniteAverage::make(
        const unsigned vectorSize,
        const unsigned decimation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::InfiniteAverage_impl(
                vectorSize,
                decimation));
}

const std::vector<float>&
gr::gs::Implementations::InfiniteAverage_impl::average() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_average;
}

void gr::gs::Implementations::InfiniteAverage_impl::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count = 0;
    for(float& value: m_average)
        value=0;
    for(double& value: m_sum)
        value=0;
}
