/*!
 * @file			Analyzer.cpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::Analyzer class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 3, 2015
 * @copyright	Copyright &copy; 2015 %Isatec Inc.
 * 				This project is released under the GNU General Public License
 * 				Version 3.
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

#include "Analyzer.hpp"
#include "gr-isatec/Exceptions.hpp"

#include "MSW.hpp"

const std::vector<std::string> gr::Isatec::GuidedScrambling::Analyzer::names = { "MSW" };

gr::Isatec::GuidedScrambling::Analyzer* gr::Isatec::GuidedScrambling::manufactureAnalyzer(const unsigned int method)
{
	switch(method)
	{
		case 0:
			return new gr::Isatec::GuidedScrambling::MSW;
		default:
			throw Exceptions::BadSelectionMethod();
	}
}

gr::Isatec::GuidedScrambling::Analyzer::Feedback* gr::Isatec::GuidedScrambling::manufactureFeedback(const unsigned int method)
{
	switch(method)
	{
		case 0:
			return new gr::Isatec::GuidedScrambling::MSW::Feedback;
		default:
			throw Exceptions::BadSelectionMethod();
	}
}
