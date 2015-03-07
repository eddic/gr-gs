/*!
 * @file       Word_test.cpp
 * @brief      Defines the gr::Isatec::GuidedScrambling::Word unit test function
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
#include "GF2.hpp"
#include "GF4.hpp"
#include "Word.hpp"

int gr::Isatec::GuidedScrambling::Tests::Word_test()
{
   std::cout << "\n*** Initiating gr::Isatec::GuidedScrambling::Word unit tests ***\n\n";

   std::cout << "Testing Word::multiply<GF2>()... ";
   {
      std::cout.flush();

      const std::vector<Symbol> multiplicand({1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0});
      const std::vector<Symbol> multiplier({1,0,1,1,0,1,1});
      const std::vector<Symbol> properProduct({1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,0,1,0,0});
      const std::vector<Symbol> properRemainder({0,1,0,1,0,0});
      std::vector<Symbol> product(multiplicand.size());
      std::vector<Symbol> remainder(multiplier.size()-1);

      Word::multiply<GF2>(multiplicand, multiplier, product, remainder, false);

      if(product != properProduct && remainder != properRemainder)
      {
         std::cout << "failed!" << std::endl;
         return 1;
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing Word::divide<GF2>()... ";
   {
      std::cout.flush();

      const std::vector<Symbol> dividend({1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,0,1,0,0});
      const std::vector<Symbol> divisor({1,0,1,1,0,1,1});
      const std::vector<Symbol> properQuotient({1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0});
      const std::vector<Symbol> properRemainder({0,1,0,0,1,0});

      std::vector<Symbol> quotient(dividend.size());
      std::vector<Symbol> remainder(divisor.size()-1);

      Word::divide<GF2>(dividend, divisor, quotient, remainder);

      if(quotient != properQuotient && remainder != properRemainder)
      {
         std::cout << "failed!" << std::endl;
         return 1;
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing Word::multiply<GF4>()... ";
   {
      std::cout.flush();

      const std::vector<Symbol> properProduct({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
      const std::vector<Symbol> multiplier({2,3,1,1,3,3});
      const std::vector<Symbol> multiplicand({2,0,2,1,0,1,1,3,1,0,0,2,2,2,0,0,0,2,2,1,2,3,1});
      std::vector<Symbol> product(multiplicand.size());
      std::vector<Symbol> remainder(multiplier.size()-1);

      Word::multiply<GF4>(multiplicand, multiplier, product, remainder, false);

      if(product != properProduct)
      {
         std::cout << "failed!" << std::endl;
         return 1;
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing Word::divide<GF4>()... ";
   {
      std::cout.flush();

      const std::vector<Symbol> dividend({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
      const std::vector<Symbol> divisor({2,3,1,1,3,3});
      const std::vector<Symbol> properQuotient({2,0,2,1,0,1,1,3,1,0,0,2,2,2,0,0,0,2,2,1,2,3,1});
      const std::vector<Symbol> properRemainder({0,0,2,1,3});

      std::vector<Symbol> quotient(dividend.size());
      std::vector<Symbol> remainder(divisor.size()-1);

      Word::divide<GF4>(dividend, divisor, quotient, remainder);

      if(quotient != properQuotient && remainder != properRemainder)
      {
         std::cout << "failed!" << std::endl;
         return 1;
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing random block Word::divide->Word::multiply cycle in GF2... ";
   {
      std::cout.flush();

      const std::vector<Symbol> scrambler({1,0,1,1,0,1,1});
      std::vector<Symbol> input;
      input.resize(32);

      std::vector<Symbol> quotient(input.size());
      std::vector<Symbol> output(input.size());
      std::vector<Symbol> divideRemainder(scrambler.size()-1);
      std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF2>(input);
         Word::divide<GF2>(input, scrambler, quotient, divideRemainder);
         std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
         Word::multiply<GF2>(quotient, scrambler, output, multiplyRemainder, false);
         if(output != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing random continuous Word::divide->Word::multiply cycle in GF2... ";
   {
      std::cout.flush();

      const std::vector<Symbol> scrambler({1,0,1,1,0,1,1});
      std::vector<Symbol> input;
      input.resize(32);

      std::vector<Symbol> quotient(input.size());
      std::vector<Symbol> output(input.size());
      std::vector<Symbol> divideRemainder(scrambler.size()-1);
      std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF2>(input);
         Word::divide<GF2>(input, scrambler, quotient, divideRemainder);
         Word::multiply<GF2>(quotient, scrambler, output, multiplyRemainder);
         if(output != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing random block Word::divide->Word::multiply cycle in GF4... ";
   {
      std::cout.flush();

      const std::vector<Symbol> scrambler({2,3,1,1,3,3});
      std::vector<Symbol> input;
      input.resize(32);

      std::vector<Symbol> quotient(input.size());
      std::vector<Symbol> output(input.size());
      std::vector<Symbol> divideRemainder(scrambler.size()-1);
      std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF4>(input);
         Word::divide<GF4>(input, scrambler, quotient, divideRemainder);
         std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
         Word::multiply<GF4>(quotient, scrambler, output, multiplyRemainder, false);
         if(output != input)
         {
            std::cout << "failed!" << std::endl;
            return 1;
         }
      }

      std::cout << "success." << std::endl;
   }

   std::cout << "Testing random continuous Word::divide->Word::multiply cycle in GF4... ";
   {
      std::cout.flush();

      const std::vector<Symbol> scrambler({2,3,1,1,3,3});
      std::vector<Symbol> input;
      input.resize(32);

      std::vector<Symbol> quotient(input.size());
      std::vector<Symbol> output(input.size());
      std::vector<Symbol> divideRemainder(scrambler.size()-1, 0);
      std::vector<Symbol> multiplyRemainder(scrambler.size()-1, 0);

      for(unsigned int i=0; i<64; ++i)
      {
         Word::randomize<GF4>(input);
         Word::divide<GF4>(input, scrambler, quotient, divideRemainder);
         Word::multiply<GF4>(quotient, scrambler, output, multiplyRemainder);
         if(output != input)
         {
            std::cout << "failed!" << i << std::endl;
            return 1;
         }
      }

      std::cout << "success." << std::endl;
   }

   return 0;
}
