/*!
 * @file       SymbolGenerator_impl.cpp
 * @brief      Defines the "Random Symbol Generator" GNU Radio block implementation
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 6, 2015
 * @copyright  Copyright &copy; 2015 %Isatec Inc.  This project is released
 *             under the GNU General Public License Version 3.
 */

/* Copyright (C) 2015 %Isatec Inc.
 *
 * This file is part of the %Isatec GNU Radio Module
 *
 * The %Isatec GNU Radio Module is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The %Isatec GNU Radio Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * The %Isatec GNU Radio Module.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SymbolGenerator_impl.hpp"
#include <gnuradio/io_signature.h>

const std::vector<double>& gr::Isatec::Implementations::SymbolGenerator_impl::weightings() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_weightings;
}

void gr::Isatec::Implementations::SymbolGenerator_impl::set_weightings(const std::vector<double>& weightings)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_weightings = weightings;

	std::discrete_distribution<Symbol> distribution(
			m_weightings.begin(),
			m_weightings.end());
	m_distribution.param(distribution.param());
}

int gr::Isatec::Implementations::SymbolGenerator_impl::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	Symbol* const start = reinterpret_cast<Symbol*>(output_items[0]);
	Symbol* const end = start+noutput_items;

	std::lock_guard<std::mutex> lock(m_mutex);

	for(auto output=start; output != end; ++output)
		*output = m_distribution(m_generator);

	m_count += noutput_items;
	return noutput_items;
}

gr::Isatec::Implementations::SymbolGenerator_impl::SymbolGenerator_impl():
	gr::sync_block("Symbol Generator",
		io_signature::make(0,0,0),
		io_signature::make(1,1,sizeof(Symbol))),
	m_count(0),
	m_generator(1984)
{
}

gr::Isatec::SymbolGenerator::sptr gr::Isatec::SymbolGenerator::make()
{
	return gnuradio::get_initial_sptr(new Implementations::SymbolGenerator_impl());
}

unsigned int gr::Isatec::Implementations::SymbolGenerator_impl::count()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	const unsigned int count=m_count;
	m_count=0;
	return count;
}
