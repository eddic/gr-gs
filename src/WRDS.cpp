/*!
 * @file       WRDS.cpp
 * @brief      Defines the gr::Isatec::GuidedScrambling::WRDS class
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 10, 2015
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

#include "gr-isatec/config.h"
#include "WRDS.hpp"

gr::Isatec::GuidedScrambling::Analyzer::Feedback* gr::Isatec::GuidedScrambling::WRDS::Feedback::clone() const
{
   return new Feedback(*this);
}

gr::Isatec::GuidedScrambling::WRDS::Feedback::Feedback():
   RDS(startingRDS)
{}

void gr::Isatec::GuidedScrambling::WRDS::analyze(const std::vector<Symbol>& codeword, const Analyzer::Feedback& feedback, const std::vector<std::complex<float>>& constellation)
{
   m_feedback.RDS = static_cast<const Feedback&>(feedback).RDS;

   for(const Symbol& symbol: codeword)
      m_feedback.RDS += constellation[symbol];
}

const gr::Isatec::GuidedScrambling::Analyzer::Feedback& gr::Isatec::GuidedScrambling::WRDS::feedback() const
{
   return m_feedback;
}

float gr::Isatec::GuidedScrambling::WRDS::analysis() const
{
   return std::abs(m_feedback.RDS);
}
