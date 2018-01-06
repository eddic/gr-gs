/*!
 * @file      WRDS.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::WRDS class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      January 6, 2018
 * @copyright Copyright &copy; 2018 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2018 Eddie Carle
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

#include "gr-gs/config.h"
#include "WRDS.hpp"

template<typename Symbol>
typename gr::gs::GuidedScrambling::Analyzer<Symbol>::Feedback*
gr::gs::GuidedScrambling::WRDS<Symbol>::Feedback::clone() const
{
    return new Feedback(*this);
}

template<typename Symbol>
gr::gs::GuidedScrambling::WRDS<Symbol>::Feedback::Feedback():
    RDS(startingRDS)
{}

template<typename Symbol>
void gr::gs::GuidedScrambling::WRDS<Symbol>::analyze(
        const std::vector<Symbol>& codeword,
        const typename Analyzer<Symbol>::Feedback& feedback,
        const std::vector<ComplexInteger>& constellation)
{
    m_feedback.RDS = static_cast<const Feedback&>(feedback).RDS;

    for(const Symbol& symbol: codeword)
        m_feedback.RDS += constellation[symbol];
}

template<typename Symbol>
const typename gr::gs::GuidedScrambling::Analyzer<Symbol>::Feedback&
gr::gs::GuidedScrambling::WRDS<Symbol>::feedback() const
{
    return m_feedback;
}

template<typename Symbol>
uint64_t gr::gs::GuidedScrambling::WRDS<Symbol>::analysis() const
{
    return m_feedback.RDS.norm();
}

template class gr::gs::GuidedScrambling::WRDS<unsigned char>;
template class gr::gs::GuidedScrambling::WRDS<unsigned short>;
template class gr::gs::GuidedScrambling::WRDS<unsigned int>;
