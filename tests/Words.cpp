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

#include <iostream>

#include "GF2.hpp"
#include "GF4.hpp"
#include "GF16.hpp"
#include "Words.hpp"

typedef unsigned char Symbol;

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::Words unit tests ***\n\n";

    std::cout << "Testing Words::multiply<GF2>()... ";
    {
        std::cout.flush();

        const std::vector<Symbol> multiplicand({1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0});
        const std::vector<Symbol> multiplier({1,0,1,1,0,1,1});
        const std::vector<Symbol> properProduct({1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,0,1,0,0});
        const std::vector<Symbol> properRemainder({0,1,0,1,0,0});
        std::vector<Symbol> product(multiplicand.size());
        std::vector<Symbol> remainder(multiplier.size()-1);

        Words::multiply<GF2<Symbol>>(multiplicand, multiplier, product, remainder, false);

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

        const std::vector<Symbol> dividend({1,1,1,1,1,1,0,1,1,0,0,1,0,0,1,0,1,0,0});
        const std::vector<Symbol> divisor({1,0,1,1,0,1,1});
        const std::vector<Symbol> properQuotient({1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0});
        const std::vector<Symbol> properRemainder({0,1,0,0,1,0});

        std::vector<Symbol> quotient(dividend.size());
        std::vector<Symbol> remainder(divisor.size()-1);

        Words::divide<GF2<Symbol>>(dividend, divisor, quotient, remainder);

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

        const std::vector<Symbol> properProduct({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
        const std::vector<Symbol> multiplier({2,3,1,1,3,3});
        const std::vector<Symbol> multiplicand({2,0,2,1,0,1,1,3,1,0,0,2,2,2,0,0,0,2,2,1,2,3,1});
        std::vector<Symbol> product(multiplicand.size());
        std::vector<Symbol> remainder(multiplier.size()-1);

        Words::multiply<GF4<Symbol>>(multiplicand, multiplier, product, remainder, false);

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

        const std::vector<Symbol> dividend({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
        const std::vector<Symbol> divisor({2,3,1,1,3,3});
        const std::vector<Symbol> properQuotient({2,0,2,1,0,1,1,3,1,0,0,2,2,2,0,0,0,2,2,1,2,3,1});
        const std::vector<Symbol> properRemainder({0,0,2,1,3});

        std::vector<Symbol> quotient(dividend.size());
        std::vector<Symbol> remainder(divisor.size()-1);

        Words::divide<GF4<Symbol>>(dividend, divisor, quotient, remainder);

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

        const std::vector<Symbol> properProduct({13,1,8,11,6,8,0,4,10,12,7,6,2,12,4,8,15,5,0,0,9,12,9,5,9});
        const std::vector<Symbol> multiplicand({12,6,10,2,3,4,15,10,9,2,10,3,8,11,15,0,3,15,0,10,11,4,2,11,3});
        const std::vector<Symbol> multiplier({11,6,11,15,7,0,15});
        std::vector<Symbol> product(multiplicand.size());
        std::vector<Symbol> remainder(multiplier.size()-1);

        Words::multiply<GF16<Symbol>>(multiplicand, multiplier, product, remainder, false);

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

        const std::vector<Symbol> dividend({12,6,10,2,3,4,15,10,9,2,10,3,8,11,15,0,3,15,0,10,11,4,2,11,3});
        const std::vector<Symbol> divisor({11,6,11,15,7,0,15});
        const std::vector<Symbol> properQuotient({9,2,4,10,8,7,7,0,10,4,7,6,4,1,12,7,7,5,11,13,10,1,3,2,3});
        const std::vector<Symbol> properRemainder({13,6,3,11,13,2});

        std::vector<Symbol> quotient(dividend.size());
        std::vector<Symbol> remainder(divisor.size()-1);

        Words::divide<GF16<Symbol>>(dividend, divisor, quotient, remainder);

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

        const std::vector<Symbol> scrambler({1,0,1,1,0,1,1});
        std::vector<Symbol> input;
        input.resize(32);

        std::vector<Symbol> quotient(input.size());
        std::vector<Symbol> output(input.size());
        std::vector<Symbol> divideRemainder(scrambler.size()-1);
        std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2<Symbol>>(input);
            Words::divide<GF2<Symbol>>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF2<Symbol>>(quotient, scrambler, output, multiplyRemainder, false);
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

        const std::vector<Symbol> scrambler({1,0,1,1,0,1,1});
        std::vector<Symbol> input;
        input.resize(32);

        std::vector<Symbol> quotient(input.size());
        std::vector<Symbol> output(input.size());
        std::vector<Symbol> divideRemainder(scrambler.size()-1);
        std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2<Symbol>>(input);
            Words::divide<GF2<Symbol>>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF2<Symbol>>(quotient, scrambler, output, multiplyRemainder);
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

        const std::vector<Symbol> scrambler({2,3,1,1,3,3});
        std::vector<Symbol> input;
        input.resize(32);

        std::vector<Symbol> quotient(input.size());
        std::vector<Symbol> output(input.size());
        std::vector<Symbol> divideRemainder(scrambler.size()-1);
        std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4<Symbol>>(input);
            Words::divide<GF4<Symbol>>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF4<Symbol>>(quotient, scrambler, output, multiplyRemainder, false);
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

        const std::vector<Symbol> scrambler({2,3,1,1,3,3});
        std::vector<Symbol> input;
        input.resize(32);

        std::vector<Symbol> quotient(input.size());
        std::vector<Symbol> output(input.size());
        std::vector<Symbol> divideRemainder(scrambler.size()-1, 0);
        std::vector<Symbol> multiplyRemainder(scrambler.size()-1, 0);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4<Symbol>>(input);
            Words::divide<GF4<Symbol>>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF4<Symbol>>(quotient, scrambler, output, multiplyRemainder);
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

        const std::vector<Symbol> scrambler({11,6,11,15,7,0,15});
        std::vector<Symbol> input;
        input.resize(32);

        std::vector<Symbol> quotient(input.size());
        std::vector<Symbol> output(input.size());
        std::vector<Symbol> divideRemainder(scrambler.size()-1);
        std::vector<Symbol> multiplyRemainder(scrambler.size()-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF16<Symbol>>(input);
            Words::divide<GF16<Symbol>>(input, scrambler, quotient, divideRemainder);
            std::fill(divideRemainder.begin(), divideRemainder.end(), 0);
            Words::multiply<GF16<Symbol>>(quotient, scrambler, output, multiplyRemainder, false);
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

        const std::vector<Symbol> scrambler({11,6,11,15,7,0,15});
        std::vector<Symbol> input;
        input.resize(32);

        std::vector<Symbol> quotient(input.size());
        std::vector<Symbol> output(input.size());
        std::vector<Symbol> divideRemainder(scrambler.size()-1, 0);
        std::vector<Symbol> multiplyRemainder(scrambler.size()-1, 0);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF16<Symbol>>(input);
            Words::divide<GF16<Symbol>>(input, scrambler, quotient, divideRemainder);
            Words::multiply<GF16<Symbol>>(quotient, scrambler, output, multiplyRemainder);
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
