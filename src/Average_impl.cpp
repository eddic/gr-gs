/*!
 * @file      Average_impl.cpp
 * @brief     Defines the "Average" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 29, 2017
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

#include "Average_impl.hpp"

#include <gnuradio/io_signature.h>

template<typename Internal, typename External>
int gr::gs::Implementations::Average_impl<Internal, External>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const External* input = reinterpret_cast<const External*>(input_items[0]);
    External* output(nullptr);
    if(m_output)
        output = reinterpret_cast<External*>(output_items[0]);

    for(int i=0; i<noutput_items; ++i)
    {
        for(unsigned j=0; j<this->decimation(); ++j)
        {
            ++m_count;
            for(unsigned k=0; k<m_sum.size(); ++k)
            {
                m_sum[k] += *input++;
                m_average[k] = m_sum[k]/static_cast<double>(m_count);
            }
        }

        if(m_output)
            for(const Internal& value: m_average)
                *output++ = value;
    }

    return noutput_items;
}

template<typename Internal, typename External>
gr::gs::Implementations::Average_impl<Internal, External>::Average_impl(
        const unsigned vectorSize,
        const bool output,
        const unsigned decimation):
    gr::sync_decimator("Average",
        io_signature::make(1,1,sizeof(External)*vectorSize),
        io_signature::make(
            output?1:0,
            output?1:0,
            sizeof(External)*vectorSize),
        decimation),
    m_output(output),
    m_sum(vectorSize),
    m_count(0),
    m_average(vectorSize)
{
    this->enable_update_rate(false);
}

template<typename Internal, typename External>
typename gr::gs::Average<Internal, External>::sptr
gr::gs::Average<Internal, External>::make(
        const unsigned vectorSize,
        const bool output,
        const unsigned decimation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Average_impl<Internal, External>(
                vectorSize,
                output,
                decimation));
}

template<typename Internal, typename External> const std::vector<Internal>&
gr::gs::Implementations::Average_impl<Internal, External>::average() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_average;
}

template<typename Internal, typename External>
void gr::gs::Implementations::Average_impl<Internal, External>::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count = 0;
    for(Internal& value: m_average)
        value=0;
    for(Internal& value: m_sum)
        value=0;
}

template class gr::gs::Implementations::Average_impl<double, float>;
template class gr::gs::Implementations::Average_impl<
    std::complex<double>,
    std::complex<float>>;
template class gr::gs::Average<double, float>;
template class gr::gs::Average<std::complex<double>, std::complex<float>>;
