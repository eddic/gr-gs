/*!
 * @file      ErrorCount_impl.cpp
 * @brief     Defines the "Error Count" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      October 7, 2017
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

#include "ErrorCount_impl.hpp"

#include <gnuradio/io_signature.h>

template<typename Symbol>
int gr::gs::Implementations::ErrorCount_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(m_maxSymbols > 0)
    {
        if(m_symbols >= m_maxSymbols)
            return -1;

        if(m_symbols+noutput_items > m_maxSymbols)
            noutput_items = m_maxSymbols-m_symbols;
    }

    if(m_maxErrors > 0 && m_errors >= m_maxErrors)
        return -1;

    float* const outputStart = m_output?
        reinterpret_cast<float*>(output_items[0]) : nullptr;
    float* const outputEnd = m_output?
        outputStart+noutput_items : nullptr;

    if(!m_synchronized)
    {
        std::vector<gr::tag_t> fastTags;
        this->get_tags_in_range(
                fastTags,
                0,
                this->nitems_read(0),
                this->nitems_read(0)+noutput_items,
                m_framingTag);

        std::vector<gr::tag_t> slowTags;
        this->get_tags_in_range(
                slowTags,
                1,
                this->nitems_read(1),
                this->nitems_read(1)+noutput_items,
                m_framingTag);

        if(!fastTags.empty() && !slowTags.empty())
        {
            const uint64_t fastId = pmt::to_uint64(fastTags.front().value);
            const size_t fastOffset
                = fastTags.front().offset - this->nitems_read(0);
            for(const auto& slowTag: slowTags)
                if(fastId == pmt::to_uint64(slowTag.value))
                {
                    m_synchronized = true;
                    const size_t slowOffset
                        = slowTag.offset - this->nitems_read(1);
                    this->set_history(slowOffset-fastOffset+1);

                    if(m_output)
                        std::fill(outputStart, outputStart+slowOffset, 0);
                    return slowOffset;
                }
        }
        if(m_output)
            std::fill(outputStart, outputEnd, 0);
        return noutput_items;
    }

    const Symbol* fastInput = reinterpret_cast<const Symbol*>(input_items[0]);
    const Symbol* const fastEnd = fastInput+noutput_items;
    const Symbol* slowInput
        = reinterpret_cast<const Symbol*>(input_items[1]) + this->history()-1;

    float* output = outputStart;

    while(true)
    {
        const auto result = std::mismatch(fastInput, fastEnd, slowInput);
        const size_t offset = result.first-fastInput;
        m_symbols += offset;

        if(m_output)
        {
            std::fill(output, output+offset, m_rate);
            output += offset;
        }

        if(result.first == fastEnd)
        {
            m_rate = m_symbols==0 ? 0 : static_cast<double>(m_errors)/m_symbols;
            break;
        }

        fastInput = result.first+1;
        slowInput = result.second+1;
        ++m_symbols;
        ++m_errors;
        m_rate = m_symbols==0?0:static_cast<double>(m_errors)/m_symbols;
        if(m_errors >= m_maxErrors)
        {
            if(m_output)
                *output++ = m_rate;
            return noutput_items - (fastEnd-fastInput);
        }
    }

    return noutput_items;
}

template<typename Symbol>
gr::gs::Implementations::ErrorCount_impl<Symbol>::ErrorCount_impl(
        const bool output,
        const std::string& framingTag,
        const unsigned long long maxErrors,
        const unsigned long long maxSymbols):
    gr::sync_block("Error Count",
        io_signature::make(2,2,sizeof(Symbol)),
        io_signature::make(output?1:0, output?1:0, sizeof(float))),
    m_output(output),
    m_maxErrors(maxErrors),
    m_maxSymbols(maxSymbols),
    m_symbols(0),
    m_errors(0),
    m_rate(0),
    m_framingTag(framingTag.empty()?
            pmt::pmt_t():
            pmt::string_to_symbol(framingTag)),
    m_synchronized(framingTag.empty())
{
}

template<typename Symbol> typename gr::gs::ErrorCount<Symbol>::sptr
gr::gs::ErrorCount<Symbol>::make(
        const bool output,
        const std::string& framingTag,
        const unsigned long long maxErrors,
        const unsigned long long maxSymbols)
{
    return gnuradio::get_initial_sptr(
            new Implementations::ErrorCount_impl<Symbol>(
                output,
                framingTag,
                maxErrors,
                maxSymbols));
}

template<typename Symbol>
void gr::gs::Implementations::ErrorCount_impl<Symbol>::reset()
{
    m_symbols = 0;
    m_errors = 0;
    m_rate = 0;
}

template<typename Symbol> unsigned long long
gr::gs::Implementations::ErrorCount_impl<Symbol>::symbols() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_symbols;
}

template<typename Symbol> unsigned long long
gr::gs::Implementations::ErrorCount_impl<Symbol>::errors() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_errors;
}

template<typename Symbol>
double gr::gs::Implementations::ErrorCount_impl<Symbol>::rate() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_rate;
}

template<typename Symbol>
bool gr::gs::Implementations::ErrorCount_impl<Symbol>::finished() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_maxErrors>0 && m_errors >= m_maxErrors)
        return true;
    if(m_maxSymbols>0 && m_symbols >= m_maxSymbols)
        return true;
    return false;
}

template class gr::gs::ErrorCount<unsigned char>;
template class gr::gs::Implementations::ErrorCount_impl<unsigned char>;
template class gr::gs::ErrorCount<unsigned short>;
template class gr::gs::Implementations::ErrorCount_impl<unsigned short>;
template class gr::gs::ErrorCount<unsigned int>;
template class gr::gs::Implementations::ErrorCount_impl<unsigned int>;
