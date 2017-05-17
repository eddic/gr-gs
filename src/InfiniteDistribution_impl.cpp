/*!
 * @file      InfiniteDistribution_impl.cpp
 * @brief     Defines the "Infinite Distribution" GNU Radio block implementation
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

#include "InfiniteDistribution_impl.hpp"

#include <gnuradio/io_signature.h>

int gr::gs::Implementations::InfiniteDistribution_impl::work(
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

            double realIndex = *input++;
            realIndex -= m_leftEdge;
            realIndex /= m_binSize;
            const ssize_t index = static_cast<ssize_t>(realIndex);                        

            if(0 <= index && index < static_cast<ssize_t>(m_bins.size()))
                ++m_bins[index];
        }

        for(unsigned j=0; j<m_bins.size(); ++j)
        {
            const double probability = static_cast<double>(m_bins[j])
                / static_cast<double>(m_count);
            m_distribution[j] = static_cast<float>(probability);
            *output++ = m_distribution[j];
        }
    }

    return noutput_items;
}

gr::gs::Implementations::InfiniteDistribution_impl::InfiniteDistribution_impl(
        const unsigned bins,
        const double binSize,
        const double leftBinCenter,
        const unsigned decimation):
    gr::sync_decimator("Infinite Distribution",
        io_signature::make(1,1,sizeof(float)),
        io_signature::make(1,1,sizeof(float)*bins),
        decimation),
    m_bins(bins),
    m_leftEdge(leftBinCenter-binSize/2),
    m_binSize(binSize),
    m_count(0),
    m_distribution(bins)
{
    this->enable_update_rate(false);
}

gr::gs::InfiniteDistribution::sptr gr::gs::InfiniteDistribution::make(
        const unsigned bins,
        const double binSize,
        const double leftBinCenter,
        const unsigned decimation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::InfiniteDistribution_impl(
                bins,
                binSize,
                leftBinCenter,
                decimation));
}

const std::vector<float>&
gr::gs::Implementations::InfiniteDistribution_impl::distribution() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_distribution;
}

void gr::gs::Implementations::InfiniteDistribution_impl::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count = 0;
    for(auto& value: m_distribution)
        value=0;
    for(auto& value: m_bins)
        value=0;
}
