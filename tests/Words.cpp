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
#include "GF8.hpp"
#include "GF16.hpp"
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

    std::cout << "Testing Words::multiply<GF8>()... ";
    {
        std::cout.flush();

        const Word properProduct({6,5,5,5,5,3,5,1,1,2,6,1,1,1,3,4,6});
        const Word multiplicand({6,0,5,2,1,4,1,1,7,7,6,4,0,3,7,7,7});
        const Word multiplier({1,4,0,4,2,3});
        Word product(multiplicand.size());
        Word remainder(multiplier.size()-1);

        Words::multiply<GF8>(multiplicand, multiplier, product, remainder, false);

        if(product != properProduct)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Words::divide<GF8>()... ";
    {
        std::cout.flush();

        const Word dividend({6,0,5,2,1,4,1,1,7,7,6,4,0,3,7,7,7});
        const Word divisor({1,4,0,4,2,3});
        const Word properQuotient({6,5,7,6,1,1,1,4,6,7,0,1,5,5,3,0,7});
        const Word properRemainder({3,2,4,5,2});

        Word quotient(dividend.size());
        Word remainder(divisor.size()-1);

        Words::divide<GF8>(dividend, divisor, quotient, remainder);

        if(quotient != properQuotient && remainder != properRemainder)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Words::multiply<GF16>()... ";
    {
        std::cout.flush();

        const Word properProduct({13,1,8,11,6,8,0,4,10,12,7,6,2,12,4,8,15,5,0,0,9,12,9,5,9});
        const Word multiplicand({12,6,10,2,3,4,15,10,9,2,10,3,8,11,15,0,3,15,0,10,11,4,2,11,3});
        const Word multiplier({11,6,11,15,7,0,15});
        Word product(multiplicand.size());
        Word remainder(multiplier.size()-1);

        Words::multiply<GF16>(multiplicand, multiplier, product, remainder, false);

        if(product != properProduct)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Words::divide<GF16>()... ";
    {
        std::cout.flush();

        const Word dividend({12,6,10,2,3,4,15,10,9,2,10,3,8,11,15,0,3,15,0,10,11,4,2,11,3});
        const Word divisor({11,6,11,15,7,0,15});
        const Word properQuotient({9,2,4,10,8,7,7,0,10,4,7,6,4,1,12,7,7,5,11,13,10,1,3,2,3});
        const Word properRemainder({13,6,3,11,13,2});

        Word quotient(dividend.size());
        Word remainder(divisor.size()-1);

        Words::divide<GF16>(dividend, divisor, quotient, remainder);

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

    std::cout << "Testing random block Words::divide->Words::multiply cycle in GF8... ";
    {
        std::cout.flush();

        const Word scrambler({1,4,0,4,2,3});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1);
        Word multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF8>(input);
            Words::divide<GF8>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF8>(quotient, scrambler, output, multiplyRemainder, false);
            if(output != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random continuous Words::divide->Words::multiply cycle in GF8... ";
    {
        std::cout.flush();

        const Word scrambler({1,4,0,4,2,3});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1, 0);
        Word multiplyRemainder(scrambler.size()-1, 0);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF8>(input);
            Words::divide<GF8>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF8>(quotient, scrambler, output, multiplyRemainder);
            if(output != input)
            {
                std::cout << "failed!" << i << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random block Words::divide->Words::multiply cycle in GF16... ";
    {
        std::cout.flush();

        const Word scrambler({11,6,11,15,7,0,15});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1);
        Word multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF16>(input);
            Words::divide<GF16>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF16>(quotient, scrambler, output, multiplyRemainder, false);
            if(output != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random continuous Words::divide->Words::multiply cycle in GF16... ";
    {
        std::cout.flush();

        const Word scrambler({11,6,11,15,7,0,15});
        Word input;
        input.resize(32);

        Word quotient(input.size());
        Word output(input.size());
        Word divideRemainder(scrambler.size()-1, 0);
        Word multiplyRemainder(scrambler.size()-1, 0);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF16>(input);
            Words::divide<GF16>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF16>(quotient, scrambler, output, multiplyRemainder);
            if(output != input)
            {
                std::cout << "failed!" << i << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    /*
    std::cout << "Finding GF16 multiplicative inverses...\n";
    {
        for(Symbol i=0; i!=16; ++i)
        {
            for(Symbol j=0; j!=16; ++j)
            {
                if(j==0)
                    std::cout << 0;
                else
                    for(Symbol k=0; k!=16; ++k)
                        if(GF16(j)*GF16(k)==1)
                            std::cout << ',' << static_cast<unsigned>(GF16(i)*GF16(k));
            }
            std::cout << '\n';
        }
    }*/

    return 0;
}
