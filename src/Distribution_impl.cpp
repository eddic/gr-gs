/*!
 * @file      Distribution_ff_impl.cpp
 * @brief     Defines the "Distribution_ff" GNU Radio block implementation
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

#include "Distribution_impl.hpp"

#include <gnuradio/io_signature.h>

// Real Values

int gr::gs::Implementations::Distribution_ff_impl::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const float* input = reinterpret_cast<const float*>(input_items[0]);
    float* output = nullptr;
    
    if(m_output)
        output = reinterpret_cast<float*>(output_items[0]);

    for(int i=0; i<noutput_items; ++i)
    {
        for(unsigned j=0; j<this->decimation(); ++j)
        {
            ++m_count;

            double realIndex = *input++;
            realIndex -= m_leftEdge;
            realIndex /= m_binSize;
            const ssize_t index = static_cast<ssize_t>(realIndex);                        

            if(0 <= index && index < static_cast<ssize_t>(m_bins.size()))
                ++m_bins[index];
        }

        if(m_output)
            for(unsigned j=0; j<m_bins.size(); ++j)
                *output++ = static_cast<float>(m_bins[j])
                    / static_cast<float>(m_count);
    }

    return noutput_items;
}

gr::gs::Implementations::Distribution_ff_impl::Distribution_ff_impl(
        const unsigned bins,
        const double binSize,
        const double leftBinCenter,
        const bool output,
        const unsigned decimation):
    gr::sync_decimator("Distribution_ff",
        io_signature::make(1,1,sizeof(float)),
        io_signature::make(
            output?1:0,
            output?1:0,
            sizeof(float)*bins),
        decimation),
    m_output(output),
    m_bins(bins),
    m_leftEdge(leftBinCenter-binSize/2),
    m_binSize(binSize),
    m_count(0)
{
    this->enable_update_rate(false);
}

gr::gs::Distribution_ff::sptr gr::gs::Distribution_ff::make(
        const unsigned bins,
        const double binSize,
        const double leftBinCenter,
        const bool output,
        const unsigned decimation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Distribution_ff_impl(
                bins,
                binSize,
                leftBinCenter,
                output,
                decimation));
}

std::vector<double>
gr::gs::Implementations::Distribution_ff_impl::distribution() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<double> dist(m_bins.size());
    for(unsigned index=0; index<m_bins.size(); ++index)
        dist[index] = static_cast<double>(m_bins[index])
            / static_cast<double>(m_count);
    return dist;
}

void gr::gs::Implementations::Distribution_ff_impl::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count = 0;
    for(auto& value: m_bins)
        value=0;
}

// Complex Values

int gr::gs::Implementations::Distribution_cf_impl::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const Complex* input = reinterpret_cast<const Complex*>(input_items[0]);
    float* output = nullptr;
    if(m_output)
        output = reinterpret_cast<float*>(output_items[0]);

    for(int i=0; i<noutput_items; ++i)
    {
        for(unsigned j=0; j<this->decimation(); ++j)
        {
            ++m_count;
            
            double index;

            index = input->imag();
            index -= m_leastEdge.imag();
            index /= m_binSize;
            const ssize_t imagIndex = static_cast<ssize_t>(index);                        

            index = input->real();
            index -= m_leastEdge.real();
            index /= m_binSize;
            const ssize_t realIndex = static_cast<ssize_t>(index);                        

            ++input;

            if(
                    0 <= realIndex &&
                    realIndex < static_cast<ssize_t>(m_binCount) &&
                    0 <= imagIndex &&
                    imagIndex < static_cast<ssize_t>(m_binCount))
                ++m_bins[imagIndex][realIndex];
        }

        if(m_output)
            for(unsigned real=0; real<m_binCount; ++real)
                *output++ = static_cast<float>(m_bins[m_zeroRow][real])
                    / static_cast<float>(m_count);
    }

    return noutput_items;
}

gr::gs::Implementations::Distribution_cf_impl::Distribution_cf_impl(
        const unsigned bins,
        const double binSize,
        const std::complex<double> leastBinCenter,
        const bool output,
        const unsigned decimation):
    gr::sync_decimator("Distribution_cf",
        io_signature::make(1,1,sizeof(Complex)),
        io_signature::make(
            output?1:0,
            output?1:0,
            sizeof(float)*bins),
        decimation),
    m_output(output),
    m_binCount(bins),
    m_bins(bins, std::vector<unsigned long long>(bins, 0)),
    m_leastEdge(leastBinCenter-std::complex<double>(binSize/2, binSize/2)),
    m_binSize(binSize),
    m_zeroRow(static_cast<size_t>(-m_leastEdge.imag()/m_binSize)),
    m_count(0)
{
    this->enable_update_rate(false);
}

gr::gs::Distribution_cf::sptr gr::gs::Distribution_cf::make(
        const unsigned bins,
        const double binSize,
        const std::complex<double> leftBinCenter,
        const bool output,
        const unsigned decimation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Distribution_cf_impl(
                bins,
                binSize,
                leftBinCenter,
                output,
                decimation));
}

std::vector<std::vector<double>>
gr::gs::Implementations::Distribution_cf_impl::distribution() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::vector<double>> dist(
            m_binCount,
            std::vector<double>(m_binCount));
    for(unsigned imag=0; imag<m_binCount; ++imag)
        for(unsigned real=0; real<m_binCount; ++real)
            dist[imag][real] = static_cast<double>(m_bins[imag][real])
                / static_cast<double>(m_count);
    return dist;
}

void gr::gs::Implementations::Distribution_cf_impl::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count = 0;
    for(auto& row: m_bins)
        for(auto& column: row)
            column=0;
}
