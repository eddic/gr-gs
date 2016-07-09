/*!
 * @file      Analyzer.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::Analyzer class
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

#include "Analyzer.hpp"
#include "gr-gs/Exceptions.hpp"

#include "MSW.hpp"
#include "WRDS.hpp"
#include "MSW2.hpp"

const std::vector<std::string> gr::gs::GuidedScrambling::Analyzer::names
{
	 "MSW",
	 "WRDS",
	 "MSW2"
};

gr::gs::GuidedScrambling::Analyzer*
gr::gs::GuidedScrambling::manufactureAnalyzer(const unsigned int method)
{
	switch(method)
	{
		case 0:
			return new gr::gs::GuidedScrambling::MSW;
		case 1:
			return new gr::gs::GuidedScrambling::WRDS;
		case 2:
			return new gr::gs::GuidedScrambling::MSW2;
		default:
			throw Exceptions::BadSelectionMethod();
	}
}

gr::gs::GuidedScrambling::Analyzer::Feedback*
gr::gs::GuidedScrambling::manufactureFeedback(const unsigned int method)
{
	switch(method)
	{
		case 0:
			return new gr::gs::GuidedScrambling::MSW::Feedback;
		case 1:
			return new gr::gs::GuidedScrambling::WRDS::Feedback;
		case 2:
			return new gr::gs::GuidedScrambling::MSW2::Feedback;
		default:
			throw Exceptions::BadSelectionMethod();
	}
}
