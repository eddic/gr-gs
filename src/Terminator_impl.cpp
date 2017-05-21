/*!
 * @file      Terminator_impl.cpp
 * @brief     Defines the "Terminator" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 20, 2017
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

#include "Terminator_impl.hpp"

#include <gnuradio/io_signature.h>

int gr::gs::Implementations::Terminator_impl::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(m_samples >= m_end)
        return -1;

    m_samples += noutput_items;
    if(m_samples > m_end)
        noutput_items -= m_samples-m_end;
    
    return noutput_items;
}

gr::gs::Implementations::Terminator_impl::Terminator_impl(
        const unsigned sampleSize,
        const unsigned long long sampleCount):
    gr::sync_block("Terminator",
        io_signature::make(1,1,sampleSize),
        io_signature::make(0,0,0)),
    m_end(sampleCount),
    m_samples(0)
{
    this->enable_update_rate(false);
}

gr::gs::Terminator::sptr gr::gs::Terminator::make(
        const unsigned sampleSize,
        const unsigned long long sampleCount)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Terminator_impl(
                sampleSize,
                sampleCount));
}

unsigned long long gr::gs::Implementations::Terminator_impl::samples() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_samples;
}

bool gr::gs::Implementations::Terminator_impl::finished() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_samples >= m_end;
}

void gr::gs::Implementations::Terminator_impl::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_samples = 0;
}
