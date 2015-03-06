/*!
 * @file			Scrambler.cpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::Scrambler class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 3, 2015
 * @copyright	Copyright &copy; 2015 %Isatec Inc.  This project is released
 *					under the GNU General Public License Version 3.
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

#include "Scrambler.hpp"
#include "gr-isatec/Exceptions.hpp"

#include "Word.hpp"
#include "GF2.hpp"
#include "GF4.hpp"

std::vector<std::complex<float>> gr::Isatec::defaultConstellation(unsigned int fieldSize)
{
	switch(fieldSize)
	{
		case 2:
			return std::vector<std::complex<float>>({{-1,0},{1,0}});
		case 4:
			return std::vector<std::complex<float>>({{1,0},{0,1},{0,-1},{-1,0}});
		default:
			return std::vector<std::complex<float>>();
	}
}

template<typename Field>
void gr::Isatec::GuidedScrambling::Word::divide(
		const std::vector<Symbol>& dividend,
		const std::vector<Symbol>& divider,
		std::vector<Symbol>& quotient,
		std::vector<Symbol>& remainder)
{
	for(unsigned int i=0; i<quotient.size(); ++i)
	{
		Symbol& output=quotient[i];
		output =
			(
				Field(dividend[i])+Field(remainder.front())
			)/Field(divider.front());

		for(unsigned int j=1; j<remainder.size(); ++j)

			remainder[j-1] = Field(output) * (-Field(divider[j])) + Field(remainder[j]);
		remainder.back() = Field(output) * (-Field(divider.back()));
	}
}

void gr::Isatec::GuidedScrambling::Scrambler::configure(
		const unsigned int length,
		unsigned int index,
		const unsigned int augmentingLength,
		const unsigned int remainderLength,
		const unsigned int method,
		const unsigned int fieldSize)
{
	m_dividend.resize(length);
	m_output.resize(length);
	m_remainder.resize(remainderLength);
	std::fill(m_remainder.begin(), m_remainder.end(), 0);
	m_analyzer.reset(manufactureAnalyzer(method));

	for(int i=augmentingLength-1; i>=0; --i)
	{
		m_dividend[i] = index%fieldSize;
		index /= fieldSize;
	}

	switch(fieldSize)
	{
		case 2:
			m_divide = Word::divide<GF2>;
			break;
		case 4:
			m_divide = Word::divide<GF4>;
			break;
		default:
			throw Exceptions::BadFieldSize();
	}
}
