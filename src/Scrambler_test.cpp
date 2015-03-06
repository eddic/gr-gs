/*!
 * @file			Scrambler_test.cpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::Scrambler unit test function
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

#include "test.hpp"
#include "Scrambler.hpp"
#include "Descrambler_impl.hpp"
#include "GF2.hpp"
#include "GF4.hpp"
#include "Word.hpp"

int gr::Isatec::GuidedScrambling::Tests::Scrambler_test()
{
	std::cout << "\n*** Initiating gr::Isatec::GuidedScrambling::Scrambler unit tests ***\n\n";

	std::cout << "Testing Scrambler::scrambler() in GF2... ";
	{
		std::cout.flush();
		const std::vector<Symbol> input({0,1,0,0,1,1,0,1,0,0,0,1});
		const std::vector<Symbol> dividend({1,0,0,1,0,1,0,0,1,1,0,1,0,0,0,1});
		const std::vector<Symbol> divider({1,1,0,1});
		const std::vector<Symbol> output({1,1,1,1,0,0,1,1,0,0,1,0,0,1,1,0});
		
		Scrambler scrambler;
		scrambler.configure(input.size()+4,9,4,divider.size()-1, 0, 2);
		scrambler.scramble(divider, input);

		if(output != scrambler.output() || dividend != scrambler.dividend())
		{
			std::cout << "failed!" << std::endl;
			return 1;
		}

		std::cout << "success." << std::endl;
	}

	std::cout << "Testing Scrambler::scrambler() in GF4... ";
	{
		std::cout.flush();
		const std::vector<Symbol> input({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
		const std::vector<Symbol> dividend({0,0,2,1,3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
		const std::vector<Symbol> divider({2,3,1,1,3,3});
		const std::vector<Symbol> output({0,0,1,1,3,2,3,1,2,0,3,0,1,2,0,1,3,1,3,1,1,2,2,1,2,1,2});
		
		Scrambler scrambler;
		scrambler.configure(input.size()+4,9,4,divider.size()-1,0,4);
		scrambler.scramble(divider, input);

		if(output != scrambler.output() || dividend != scrambler.dividend())
		{
			std::cout << "failed!" << std::endl;
			return 1;
		}

		std::cout << "success." << std::endl;
	}

	std::cout << "Testing random block scrambling->descrambling cycle in GF2 with gr::Isatec::GuidedScrambling::Scrambler... ";
	{
		std::cout.flush();

		const std::vector<Symbol> divider({1,0,1,1,0,1,1});
		std::vector<Symbol> input;
		input.resize(32);

		const std::vector<Symbol> remainder(divider.size()-1);

		Scrambler scrambler;
		scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 2);

		Descrambler_impl descrambler;
		descrambler.set_fieldSize(2);
		descrambler.set_augmentingLength(4);
		descrambler.set_codewordLength(input.size()+4);
		descrambler.set_continuous(false);
		descrambler.set_multiplier(divider);

		for(unsigned int i=0; i<64; ++i)
		{
			Word::randomize<GF2>(input);
			scrambler.setRemainder(remainder);
			scrambler.scramble(divider, input);
			descrambler.descramble(scrambler.output());
			if(descrambler.output() != input)
			{
				std::cout << "failed!" << std::endl;
				return 1;
			}
		}

		std::cout << "success." << std::endl;
	}

	std::cout << "Testing random continuous scrambling->descrambling cycle in GF2 with gr::Isatec::GuidedScrambling::Scrambler... ";
	{
		std::cout.flush();

		const std::vector<Symbol> divider({1,0,1,1,0,1,1});
		std::vector<Symbol> input;
		input.resize(32);

		Scrambler scrambler;
		scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 2);

		Descrambler_impl descrambler;
		descrambler.set_fieldSize(2);
		descrambler.set_augmentingLength(4);
		descrambler.set_codewordLength(input.size()+4);
		descrambler.set_continuous(true);
		descrambler.set_multiplier(divider);

		for(unsigned int i=0; i<64; ++i)
		{
			Word::randomize<GF2>(input);
			scrambler.scramble(divider, input);
			descrambler.descramble(scrambler.output());
			if(descrambler.output() != input)
			{
				std::cout << "failed!" << std::endl;
				return 1;
			}
		}

		std::cout << "success." << std::endl;
	}

	std::cout << "Testing random block scrambling->descrambling cycle in GF4 with gr::Isatec::GuidedScrambling::Scrambler... ";
	{
		std::cout.flush();

		const std::vector<Symbol> divider({2,3,1,1,3,3});
		std::vector<Symbol> input;
		input.resize(32);

		const std::vector<Symbol> remainder(divider.size()-1);

		Scrambler scrambler;
		scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 4);

		Descrambler_impl descrambler;
		descrambler.set_fieldSize(4);
		descrambler.set_augmentingLength(4);
		descrambler.set_codewordLength(input.size()+4);
		descrambler.set_continuous(false);
		descrambler.set_multiplier(divider);

		for(unsigned int i=0; i<64; ++i)
		{
			Word::randomize<GF4>(input);
			scrambler.setRemainder(remainder);
			scrambler.scramble(divider, input);
			descrambler.descramble(scrambler.output());
			if(descrambler.output() != input)
			{
				std::cout << "failed!" << std::endl;
				return 1;
			}
		}

		std::cout << "success." << std::endl;
	}

	std::cout << "Testing random continuous scrambling->descrambling cycle in GF4 with gr::Isatec::GuidedScrambling::Scrambler... ";
	{
		std::cout.flush();

		const std::vector<Symbol> divider({2,3,1,1,3,3});
		std::vector<Symbol> input;
		input.resize(32);

		Scrambler scrambler;
		scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 4);

		Descrambler_impl descrambler;
		descrambler.set_fieldSize(4);
		descrambler.set_augmentingLength(4);
		descrambler.set_codewordLength(input.size()+4);
		descrambler.set_continuous(true);
		descrambler.set_multiplier(divider);

		for(unsigned int i=0; i<64; ++i)
		{
			Word::randomize<GF4>(input);
			scrambler.scramble(divider, input);
			descrambler.descramble(scrambler.output());
			if(descrambler.output() != input)
			{
				std::cout << "failed!" << std::endl;
				return 1;
			}
		}

		std::cout << "success." << std::endl;
	}

	return 0;
}
