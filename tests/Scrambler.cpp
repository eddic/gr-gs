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

#include "Scrambler.hpp"
#include "Descrambler_impl.hpp"
#include "GF2.hpp"
#include "GF4.hpp"
#include "GF16.hpp"
#include "Words.hpp"

typedef unsigned char Symbol;

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::Scrambler unit tests ***\n\n";

    std::cout << "Testing Scrambler::scrambler() in GF2... ";
    {
        std::cout.flush();
        const std::vector<Symbol> input({0,1,0,0,1,1,0,1,0,0,0,1});
        const std::vector<Symbol> dividend({1,0,0,1,0,1,0,0,1,1,0,1,0,0,0,1});
        const std::vector<Symbol> divider({1,1,0,1});
        const std::vector<Symbol> output({1,1,1,1,0,0,1,1,0,0,1,0,0,1,1,0});

        Scrambler<Symbol> scrambler;
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

        Scrambler<Symbol> scrambler;
        scrambler.configure(input.size()+4,9,4,divider.size()-1,0,4);
        scrambler.scramble(divider, input);

        if(output != scrambler.output() || dividend != scrambler.dividend())
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random block scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::Scrambler... ";
    {
        std::cout.flush();

        const std::vector<Symbol> divider({1,0,1,1,0,1,1});
        std::vector<Symbol> input;
        input.resize(32);

        const std::vector<Symbol> remainder(divider.size()-1);

        Scrambler<Symbol> scrambler;
        scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 2);

        Descrambler_impl<Symbol> descrambler(
                2,
                input.size()+4,
                4,
                false,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2<Symbol>>(input);
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::Scrambler... ";
    {
        std::cout.flush();

        const std::vector<Symbol> divider({1,0,1,1,0,1,1});
        std::vector<Symbol> input;
        input.resize(32);

        Scrambler<Symbol> scrambler;
        scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 2);

        Descrambler_impl<Symbol> descrambler(
                2,
                input.size()+4,
                4,
                true,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2<Symbol>>(input);
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

    std::cout << "Testing random block scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::Scrambler... ";
    {
        std::cout.flush();

        const std::vector<Symbol> divider({2,3,1,1,3,3});
        std::vector<Symbol> input;
        input.resize(32);

        const std::vector<Symbol> remainder(divider.size()-1);

        Scrambler<Symbol> scrambler;
        scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 4);

        Descrambler_impl<Symbol> descrambler(
                4,
                input.size()+4,
                4,
                false,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4<Symbol>>(input);
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::Scrambler... ";
    {
        std::cout.flush();

        const std::vector<Symbol> divider({2,3,1,1,3,3});
        std::vector<Symbol> input;
        input.resize(32);

        Scrambler<Symbol> scrambler;
        scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 4);

        Descrambler_impl<Symbol> descrambler(
                4,
                input.size()+4,
                4,
                true,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4<Symbol>>(input);
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF16 with gr::gs::GuidedScrambling::Scrambler... ";
    {
        std::cout.flush();

        const std::vector<Symbol> divider({1,0,0,0,2,4});
        std::vector<Symbol> input;
        input.resize(32);

        Scrambler<Symbol> scrambler;
        scrambler.configure(input.size()+4,11,4,divider.size()-1, 0, 16);

        Descrambler_impl<Symbol> descrambler(
                16,
                input.size()+4,
                4,
                true,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF16<Symbol>>(input);
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
