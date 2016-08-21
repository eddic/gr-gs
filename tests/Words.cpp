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

#include <iostream>

#include "GF2.hpp"
#include "GF4.hpp"
#include "Words.hpp"

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::Words unit tests ***\n\n";

    std::cout << "Testing Words::multiply<GF2>()... ";
    {
        std::cout.flush();

        const Word multiplicand({1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0});
        const Word multiplier({1,0,1,1,0,1,1});
        const Word properProduct({1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,0,1,0,0});
        const Word properRemainder({0,1,0,1,0,0});
        Word product(multiplicand.size());
        Word remainder(multiplier.size()-1);

        Words::multiply<GF2>(multiplicand, multiplier, product, remainder, false);

        if(product != properProduct && remainder != properRemainder)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Words::divide<GF2>()... ";
    {
        std::cout.flush();

        const Word dividend({1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,0,1,0,0});
        const Word divisor({1,0,1,1,0,1,1});
        const Word properQuotient({1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0});
        const Word properRemainder({0,1,0,0,1,0});

        Word quotient(dividend.size());
        Word remainder(divisor.size()-1);

        Words::divide<GF2>(dividend, divisor, quotient, remainder);

        if(quotient != properQuotient && remainder != properRemainder)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Words::multiply<GF4>()... ";
    {
        std::cout.flush();

        const Word properProduct({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
        const Word multiplier({2,3,1,1,3,3});
        const Word multiplicand({2,0,2,1,0,1,1,3,1,0,0,2,2,2,0,0,0,2,2,1,2,3,1});
        Word product(multiplicand.size());
        Word remainder(multiplier.size()-1);

        Words::multiply<GF4>(multiplicand, multiplier, product, remainder, false);

        if(product != properProduct)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Words::divide<GF4>()... ";
    {
        std::cout.flush();

        const Word dividend({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
        const Word divisor({2,3,1,1,3,3});
        const Word properQuotient({2,0,2,1,0,1,1,3,1,0,0,2,2,2,0,0,0,2,2,1,2,3,1});
        const Word properRemainder({0,0,2,1,3});

        Word quotient(dividend.size());
        Word remainder(divisor.size()-1);

        Words::divide<GF4>(dividend, divisor, quotient, remainder);

        if(quotient != properQuotient && remainder != properRemainder)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random block Words::divide->Words::multiply cycle in GF2... ";
    {
        std::cout.flush();

        const Word scrambler({1,0,1,1,0,1,1});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1);
        Word multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2>(input);
            Words::divide<GF2>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF2>(quotient, scrambler, output, multiplyRemainder, false);
            if(output != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random continuous Words::divide->Words::multiply cycle in GF2... ";
    {
        std::cout.flush();

        const Word scrambler({1,0,1,1,0,1,1});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1);
        Word multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2>(input);
            Words::divide<GF2>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF2>(quotient, scrambler, output, multiplyRemainder);
            if(output != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random block Words::divide->Words::multiply cycle in GF4... ";
    {
        std::cout.flush();

        const Word scrambler({2,3,1,1,3,3});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1);
        Word multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4>(input);
            Words::divide<GF4>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF4>(quotient, scrambler, output, multiplyRemainder, false);
            if(output != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random continuous Words::divide->Words::multiply cycle in GF4... ";
    {
        std::cout.flush();

        const Word scrambler({2,3,1,1,3,3});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1, 0);
        Word multiplyRemainder(scrambler.size()-1, 0);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4>(input);
            Words::divide<GF4>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF4>(quotient, scrambler, output, multiplyRemainder);
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
