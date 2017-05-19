/*!
 * @file      SymbolMapper_impl.cpp
 * @brief     Defines the "Symbol Mapper" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 19, 2017
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
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

#include "SymbolMapper_impl.hpp"

#include <gnuradio/io_signature.h>

template<typename Symbol> const std::vector<gr::gs::Complex>&
gr::gs::Implementations::SymbolMapper_impl<Symbol>::constellation() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_constellation;
}

template<typename Symbol>
void gr::gs::Implementations::SymbolMapper_impl<Symbol>::set_constellation(
        const std::vector<Complex>& constellation)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_constellation = constellation;
}

template<typename Symbol>
int gr::gs::Implementations::SymbolMapper_impl<Symbol>::work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const Symbol* const& start = reinterpret_cast<const Symbol*>(input_items[0]);
    const Symbol* const& end = start + noutput_items;

    Complex* sample = reinterpret_cast<Complex*>(
            output_items[0]);

    for(const Symbol* input=start; input!=end; ++input, ++sample)
        *sample = m_constellation[*input];

    return noutput_items;
}

template<typename Symbol>
gr::gs::Implementations::SymbolMapper_impl<Symbol>::SymbolMapper_impl(
        const std::vector<Complex>& constellation):
    gr::sync_block("Symbol Mapper",
        io_signature::make(1,1,sizeof(Symbol)),
        io_signature::make(1,1,sizeof(Complex))),
    m_constellation(constellation)
{
    this->enable_update_rate(false);
}

template<typename Symbol>
typename gr::gs::SymbolMapper<Symbol>::sptr gr::gs::SymbolMapper<Symbol>::make(
        const std::vector<Complex>& constellation)
{
    return gnuradio::get_initial_sptr(
            new Implementations::SymbolMapper_impl<Symbol>(
                constellation));
}

template class gr::gs::SymbolMapper<unsigned char>;
template class gr::gs::SymbolMapper<unsigned short>;
template class gr::gs::SymbolMapper<unsigned int>;
