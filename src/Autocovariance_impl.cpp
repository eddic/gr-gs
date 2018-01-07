/*!
 * @file      Autocovariance_impl.cpp
 * @brief     Defines the "Autocovariance" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      January 6, 2018
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

#include "Autocovariance_impl.hpp"

#include <gnuradio/io_signature.h>
#include <array>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            template<>
            float gr::gs::Implementations::Autocovariance_impl<float>::conj(
                    float x)
            {
                return x;
            }

            template<> gr::gs::Complex
            gr::gs::Implementations::Autocovariance_impl<gr::gs::Complex>::conj(
                    gr::gs::Complex x)
            {
                return std::conj(x);
            }
        }
    }
}

template<typename T>
int gr::gs::Implementations::Autocovariance_impl<T>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const T* input = reinterpret_cast<const T*>(input_items[0])
        +this->history()-1+m_offset;
    const T* const inputEnd = input + noutput_items*this->decimation();

    T* output = reinterpret_cast<T*>(output_items[0]);

    while(input<inputEnd)
    {
        T currentValue = *input - m_mean;

        T* const outputEnd = output + m_length;
        const T* pastValue = input - (this->history()-1);

        while(output<outputEnd)
            *output++ = currentValue * conj(*pastValue++ - m_mean);

        input += this->decimation();
    }

    return (output-reinterpret_cast<T*>(output_items[0]))/m_length;
}

template<typename T>
gr::gs::Implementations::Autocovariance_impl<T>::Autocovariance_impl(
        unsigned length,
        T mean,
        const unsigned decimation,
        const unsigned offset):
    gr::sync_decimator("Autocovariance",
        io_signature::make(1,1,sizeof(T)),
        io_signature::make(1,1,sizeof(T)*length),
        decimation),
    m_mean(mean),
    m_length(length),
    m_offset(offset % decimation)
{
    this->enable_update_rate(false);
    this->set_history(length);
}

template<typename T>
typename gr::gs::Autocovariance<T>::sptr gr::gs::Autocovariance<T>::make(
        unsigned length,
        T mean,
        const unsigned decimation,
        const unsigned offset)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Autocovariance_impl<T>(
                length,
                mean,
                decimation,
                offset));
}

template class gr::gs::Implementations::Autocovariance_impl<float>;
template class gr::gs::Implementations::Autocovariance_impl<gr::gs::Complex>;
template class gr::gs::Autocovariance<float>;
template class gr::gs::Autocovariance<gr::gs::Complex>;
