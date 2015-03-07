/*!
 * @file       GuidedScrambler_test.cpp
 * @brief      Defines the gr::Isatec::GuidedScrambling::GuidedScrambler_impl unit test function
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 6, 2015
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

#include <iostream>

#include "test.hpp"
#include "GuidedScrambler_impl.hpp"
#include "Descrambler_impl.hpp"
#include "GF2.hpp"
#include "GF4.hpp"
#include "Word.hpp"

int gr::Isatec::GuidedScrambling::Tests::GuidedScrambler_test()
{
   std::cout << "\n*** Initiating gr::Isatec::GuidedScrambling::GuidedScrambler_impl unit tests ***\n\n";

   std::cout << "Testing random block scrambling->descrambling cycle in GF2 with gr::Isatec::GuidedScrambling::GuidedScrambler_impl... ";
   {
      std::cout.flush();

      GuidedScrambler_impl scrambler;
      scrambler.set_continuous(false);
      scrambler.set_fieldSize(2);
      scrambler.set_constellation(defaultConstellation(2));

      Descrambler_impl descrambler;
      descrambler.set_fieldSize(scrambler.fieldSize());
      descrambler.set_augmentingLength(scrambler.augmentingLength());
      descrambler.set_codewordLength(scrambler.codewordLength());
      descrambler.set_continuous(scrambler.continuous());
      descrambler.set_multiplier(scrambler.divider());

      std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());
      Word::randomize<GF2>(input);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF2>(input);
         const std::vector<Symbol>& output = scrambler.scramble(input);
         descrambler.descramble(output);
         if(descrambler.output() != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing random continuous scrambling->descrambling cycle in GF2 with gr::Isatec::GuidedScrambling::GuidedScrambler_impl... ";
   {
      std::cout.flush();

      GuidedScrambler_impl scrambler;
      scrambler.set_continuous(true);
      scrambler.set_fieldSize(2);
      scrambler.set_constellation(defaultConstellation(2));

      Descrambler_impl descrambler;
      descrambler.set_fieldSize(scrambler.fieldSize());
      descrambler.set_augmentingLength(scrambler.augmentingLength());
      descrambler.set_codewordLength(scrambler.codewordLength());
      descrambler.set_continuous(scrambler.continuous());
      descrambler.set_multiplier(scrambler.divider());

      std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());
      Word::randomize<GF2>(input);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF2>(input);
         const std::vector<Symbol>& output = scrambler.scramble(input);
         descrambler.descramble(output);
         if(descrambler.output() != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success.\n";
   }

   std::cout << "Testing random block scrambling->descrambling cycle in GF4 with gr::Isatec::GuidedScrambling::GuidedScrambler_impl... ";
   {
      std::cout.flush();

      GuidedScrambler_impl scrambler;
      scrambler.set_continuous(false);
      scrambler.set_constellation(defaultConstellation(4));
      scrambler.set_fieldSize(4);

      Descrambler_impl descrambler;
      descrambler.set_fieldSize(scrambler.fieldSize());
      descrambler.set_augmentingLength(scrambler.augmentingLength());
      descrambler.set_codewordLength(scrambler.codewordLength());
      descrambler.set_continuous(scrambler.continuous());
      descrambler.set_multiplier(scrambler.divider());

      std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());
      Word::randomize<GF4>(input);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF4>(input);
         const std::vector<Symbol>& output = scrambler.scramble(input);
         descrambler.descramble(output);
         if(descrambler.output() != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success.\n";
   }

   std::cout << "Testing random continuous scrambling->descrambling cycle in GF4 with gr::Isatec::GuidedScrambling::GuidedScrambler_impl... ";
   {
      std::cout.flush();

      GuidedScrambler_impl scrambler;
      scrambler.set_continuous(true);
      scrambler.set_constellation(defaultConstellation(4));
      scrambler.set_fieldSize(4);

      Descrambler_impl descrambler;
      descrambler.set_fieldSize(scrambler.fieldSize());
      descrambler.set_augmentingLength(scrambler.augmentingLength());
      descrambler.set_codewordLength(scrambler.codewordLength());
      descrambler.set_continuous(scrambler.continuous());
      descrambler.set_multiplier(scrambler.divider());

      std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());
      Word::randomize<GF4>(input);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF4>(input);
         const std::vector<Symbol>& output = scrambler.scramble(input);
         descrambler.descramble(output);
         if(descrambler.output() != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success.\n";
   }

   return 0;
}
