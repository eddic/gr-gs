/*!
 * @file      MSW2.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::MSW2 class
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
#include "MSW2.hpp"

#include <iostream>

template<typename Symbol>
typename gr::gs::GuidedScrambling::Analyzer<Symbol>::Feedback*
gr::gs::GuidedScrambling::MSW2<Symbol>::Feedback::clone() const
{
    return new Feedback(*this);
}

template<typename Symbol>
void gr::gs::GuidedScrambling::MSW2<Symbol>::analyze(
        const std::vector<Symbol>& codeword,
        const typename Analyzer<Symbol>::Feedback& feedback,
        const std::vector<ComplexInteger>& constellation)
{
    m_feedback.RDS = static_cast<const Feedback&>(feedback).RDS;
    m_feedback.RDSS = static_cast<const Feedback&>(feedback).RDSS;
    uint64_t rdsAnalysis = 0;
    uint64_t rdssAnalysis = 0;

    for(const Symbol& symbol: codeword)
    {
        // Do the RDS analysis
        if(m_feedback.RDS.overflow())
        {
            m_analysis = std::numeric_limits<uint64_t>::max();
            return;
        }
        m_feedback.RDS += constellation[symbol];
        rdsAnalysis += m_feedback.RDS.norm();
        if(rdsAnalysis > std::numeric_limits<uint32_t>::max())
        {
            m_analysis = std::numeric_limits<uint64_t>::max();
            return;
        }

        // Do the RDSS analysis
        if(m_feedback.RDSS.overflow())
        {
            rdssAnalysis = std::numeric_limits<uint32_t>::max();
            continue;
        }
        m_feedback.RDSS += m_feedback.RDS;
        rdssAnalysis += m_feedback.RDSS.norm();
        if(rdssAnalysis > std::numeric_limits<uint32_t>::max())
        {
            rdssAnalysis = std::numeric_limits<uint32_t>::max();
            continue;
        }
    }

    m_analysis = (rdsAnalysis << 32) | rdssAnalysis;
}

template<typename Symbol>
const typename gr::gs::GuidedScrambling::Analyzer<Symbol>::Feedback&
gr::gs::GuidedScrambling::MSW2<Symbol>::feedback() const
{
    return m_feedback;
}

template<typename Symbol>
uint64_t gr::gs::GuidedScrambling::MSW2<Symbol>::analysis() const
{
    return m_analysis;
}

template class gr::gs::GuidedScrambling::MSW2<unsigned char>;
template class gr::gs::GuidedScrambling::MSW2<unsigned short>;
template class gr::gs::GuidedScrambling::MSW2<unsigned int>;
