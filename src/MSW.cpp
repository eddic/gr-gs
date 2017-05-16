/*!
 * @file      MSW.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::MSW class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 16, 2017
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

#include "gr-gs/config.h"
#include "MSW.hpp"

gr::gs::GuidedScrambling::Analyzer::Feedback*
gr::gs::GuidedScrambling::MSW::Feedback::clone() const
{
    return new Feedback(*this);
}

gr::gs::GuidedScrambling::MSW::Feedback::Feedback():
    RDS(startingRDS)
{}

void gr::gs::GuidedScrambling::MSW::analyze(
        const Word& codeword,
        const Analyzer::Feedback& feedback,
        const std::vector<Complex>& constellation)
{
    m_feedback.RDS = static_cast<const Feedback&>(feedback).RDS;
    m_analysis = 0;

    for(const Symbol& symbol: codeword)
    {
        m_feedback.RDS += constellation[symbol];
        m_analysis += std::norm(m_feedback.RDS);
    }
}

const gr::gs::GuidedScrambling::Analyzer::Feedback&
gr::gs::GuidedScrambling::MSW::feedback() const
{
    return m_feedback;
}

double gr::gs::GuidedScrambling::MSW::analysis() const
{
    return m_analysis;
}
