/*!
 * @file      Autocovariance_impl.cpp
 * @brief     Defines the "Autocovariance" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      June 2, 2017
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
            int gr::gs::Implementations::Autocovariance_impl<float>::work(
                    int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                const float* input = 
                    reinterpret_cast<const float*>(input_items[0])
                    +this->history()-1+m_offset;
                const float* const inputEnd =
                    input + noutput_items*this->decimation();

                float* output = reinterpret_cast<float*>(output_items[0]);

                while(input<inputEnd)
                {
                    float currentValue = *input - m_mean;

                    float* const outputEnd = output + m_length;
                    const float* pastValue = input - (this->history()-1);

                    while(output<outputEnd)
                        *output++ = currentValue * (*pastValue++ - m_mean);

                    input += this->decimation();
                }

                return (output-reinterpret_cast<float*>(output_items[0]))
                    /m_length;
            }

            template<>
            int gr::gs::Implementations::Autocovariance_impl<gr::gs::Complex>
                ::work(
                    int noutput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items)
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                const Complex* input = 
                    reinterpret_cast<const Complex*>(input_items[0])
                    +this->history()-1+m_offset;
                const Complex* const inputEnd =
                    input + noutput_items*this->decimation();

                std::array<float*,4> output
                {
                    reinterpret_cast<float*>(output_items[0]),
                    reinterpret_cast<float*>(output_items[1]),
                    reinterpret_cast<float*>(output_items[2]),
                    reinterpret_cast<float*>(output_items[3])
                };

                while(input<inputEnd)
                {
                    Complex currentValue = *input - m_mean;

                    float* const outputEnd = output[0] + m_length;
                    const Complex* pastValue_ptr = input - (this->history()-1);

                    while(output[0]<outputEnd)
                    {
                        const Complex pastValue = *pastValue_ptr++ - m_mean;
                        *output[0]++ = currentValue.real() * pastValue.real();
                        *output[1]++ = currentValue.real() * pastValue.imag();
                        *output[2]++ = currentValue.imag() * pastValue.real();
                        *output[3]++ = currentValue.imag() * pastValue.imag();
                    }

                    input += this->decimation();
                }

                return (output[0]-reinterpret_cast<float*>(output_items[0]))
                    /m_length;
            }

            template<>
            int gr::gs::Implementations::Autocovariance_impl<float>::streams()
            {
                return 1;
            }

            template<>
            int gr::gs::Implementations::Autocovariance_impl<gr::gs::Complex>::
                streams()
            {
                return 4;
            }
        }
    }
}

template<typename T>
gr::gs::Implementations::Autocovariance_impl<T>::Autocovariance_impl(
        unsigned length,
        T mean,
        const unsigned decimation,
        const unsigned offset):
    gr::sync_decimator("Autocovariance",
        io_signature::make(1,1,sizeof(T)),
        io_signature::make(streams(),streams(),sizeof(float)*length),
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

template<typename T>
T gr::gs::Implementations::Autocovariance_impl<T>::mean() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_mean;
}

template<typename T>
void gr::gs::Implementations::Autocovariance_impl<T>::set_mean(T mean)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mean = mean;
}

template class gr::gs::Implementations::Autocovariance_impl<float>;
template class gr::gs::Implementations::Autocovariance_impl<gr::gs::Complex>;
template class gr::gs::Autocovariance<float>;
template class gr::gs::Autocovariance<gr::gs::Complex>;
