/*!
 * @file      SymbolGenerator_impl.cpp
 * @brief     Defines the "Random Symbol Generator" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 29, 2017
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

#include "SymbolGenerator_impl.hpp"
#include <gnuradio/io_signature.h>

template<typename Symbol> const std::vector<double>&
gr::gs::Implementations::SymbolGenerator_impl<Symbol>::weightings() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_weightings;
}

template<typename Symbol>
void gr::gs::Implementations::SymbolGenerator_impl<Symbol>::set_weightings(
        const std::vector<double>& weightings)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_weightings = weightings;

    std::discrete_distribution<Symbol> distribution(
            m_weightings.begin(),
            m_weightings.end());
    m_distribution.param(distribution.param());
}

template<typename Symbol>
int gr::gs::Implementations::SymbolGenerator_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    Symbol* const start = reinterpret_cast<Symbol*>(output_items[0]);
    Symbol* const end = start+noutput_items;

    std::lock_guard<std::mutex> lock(m_mutex);

    if(!m_aligned)
    {
        this->add_item_tag(
                0,
                0,
                m_alignmentTag,
                pmt::from_uint64(0));
        m_aligned = true;
    }

    for(auto output=start; output != end; ++output)
        *output = m_distribution(m_generator);

    return noutput_items;
}

template<typename Symbol>
gr::gs::Implementations::SymbolGenerator_impl<Symbol>::SymbolGenerator_impl(
        const std::vector<double>& weightings,
        const std::string& alignmentTag):
    gr::sync_block("Symbol Generator",
        io_signature::make(0,0,0),
        io_signature::make(1,1,sizeof(Symbol))),
    m_weightings(weightings),
    m_generator(1984),
    m_distribution(m_weightings.cbegin(), m_weightings.cend()),
    m_alignmentTag(pmt::string_to_symbol(alignmentTag)),
    m_aligned(alignmentTag.empty())
{
    this->enable_update_rate(false);
}

template<typename Symbol> typename gr::gs::SymbolGenerator<Symbol>::sptr
gr::gs::SymbolGenerator<Symbol>::make(
        const std::vector<double>& weightings,
        const std::string& alignmentTag)
{
    return gnuradio::get_initial_sptr(
            new Implementations::SymbolGenerator_impl<Symbol>(
                weightings,
                alignmentTag));
}

template class gr::gs::SymbolGenerator<unsigned char>;
template class gr::gs::SymbolGenerator<unsigned short>;
template class gr::gs::SymbolGenerator<unsigned int>;
