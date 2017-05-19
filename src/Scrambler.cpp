/*!
 * @file      Scrambler.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::Scrambler class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 19, 2017
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

#include "Scrambler.hpp"
#include "Words.hpp"


template<typename Symbol>
void gr::gs::GuidedScrambling::Scrambler<Symbol>::configure(
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
    m_analyzer.reset(manufactureAnalyzer<Symbol>(method));
    m_divide = Words::getDivide<Symbol>(fieldSize);

    for(int i=augmentingLength-1; i>=0; --i)
    {
        m_dividend[i] = index%fieldSize;
        index /= fieldSize;
    }
}

template class gr::gs::GuidedScrambling::Scrambler<unsigned char>;
template class gr::gs::GuidedScrambling::Scrambler<unsigned short>;
template class gr::gs::GuidedScrambling::Scrambler<unsigned int>;
