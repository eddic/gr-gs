/*!
 * @file      MSW2.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::MSW2 class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      July 8, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2016 Eddie Carle
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

gr::gs::GuidedScrambling::Analyzer::Feedback*
gr::gs::GuidedScrambling::MSW2::Feedback::clone() const
{
    return new Feedback(*this);
}

gr::gs::GuidedScrambling::MSW2::Feedback::Feedback():
    RDS(startingRDS)
{}

void gr::gs::GuidedScrambling::MSW2::analyze(
        const Word& codeword,
        const Analyzer::Feedback& feedback,
        const std::vector<std::complex<float>>& constellation)
{
    m_feedback.RDS = static_cast<const Feedback&>(feedback).RDS;
    m_feedback.RDSS = static_cast<const Feedback&>(feedback).RDSS;
    m_analysis = 0;

    for(const Symbol& symbol: codeword)
    {
        m_feedback.RDS += constellation[symbol];
        m_feedback.RDSS += m_feedback.RDS;
        m_analysis += std::norm(m_feedback.RDSS);
    }
}

const gr::gs::GuidedScrambling::Analyzer::Feedback&
gr::gs::GuidedScrambling::MSW2::feedback() const
{
    return m_feedback;
}

float gr::gs::GuidedScrambling::MSW2::analysis() const
{
    return m_analysis;
}
