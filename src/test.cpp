/*!
 * @file			test.cpp
 * @brief		Defines the main unit test suite
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 6, 2015
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

#include <iostream>
#include <exception>

#include <test.hpp>

int main()
{
	try
	{
		if(gr::Isatec::GuidedScrambling::Tests::Word_test())
		{
			std::cout << "\n!!!! TEST FAILED !!!!\n";
			return 1;
		}

		if(gr::Isatec::GuidedScrambling::Tests::Analyzer_test())
		{
			std::cout << "\n!!!! TEST FAILED !!!!\n";
			return 1;
		}

		if(gr::Isatec::GuidedScrambling::Tests::Descrambler_test())
		{
			std::cout << "\n!!!! TEST FAILED !!!!\n";
			return 1;
		}

		if(gr::Isatec::GuidedScrambling::Tests::Scrambler_test())
		{
			std::cout << "\n!!!! TEST FAILED !!!!\n";
			return 1;
		}

		if(gr::Isatec::GuidedScrambling::Tests::ScramblerGroup_test())
		{
			std::cout << "\n!!!! TEST FAILED !!!!\n";
			return 1;
		}

		if(gr::Isatec::GuidedScrambling::Tests::GuidedScrambler_test())
		{
			std::cout << "\n!!!! TEST FAILED !!!!\n";
			return 1;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "\nError: " << e.what() << '\n';
		return 1;
	}
	return 0;
}
