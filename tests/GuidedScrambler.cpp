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

#include "GuidedScrambler_impl.hpp"
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

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::GuidedScrambler_impl unit tests ***\n\n";

    std::cout << "Testing random block scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
    {
        std::cout.flush();

        GuidedScrambler_impl<Symbol> scrambler(
                2,
                12,
                3,
                false,
                {1,0,0,1},
                0,
                "MSW",
                "");

        Descrambler_impl<Symbol> descrambler(
                scrambler.fieldSize(),
                scrambler.codewordLength(),
                scrambler.augmentingLength(),
                scrambler.continuous(),
                scrambler.divider(),
                "");

        std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2<Symbol>>(input);
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
    {
        std::cout.flush();

        GuidedScrambler_impl<Symbol> scrambler(
                2,
                12,
                3,
                true,
                {1,0,0,1},
                0,
                "MSW",
                "");

        Descrambler_impl<Symbol> descrambler(
                scrambler.fieldSize(),
                scrambler.codewordLength(),
                scrambler.augmentingLength(),
                scrambler.continuous(),
                scrambler.divider(),
                "");

        std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2<Symbol>>(input);
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

    std::cout << "Testing random block scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
    {
        std::cout.flush();

        GuidedScrambler_impl<Symbol> scrambler(
                4,
                12,
                3,
                false,
                {1,0,0,1},
                0,
                "MSW",
                "");

        Descrambler_impl<Symbol> descrambler(
                scrambler.fieldSize(),
                scrambler.codewordLength(),
                scrambler.augmentingLength(),
                scrambler.continuous(),
                scrambler.divider(),
                "");

        std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4<Symbol>>(input);
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
    {
        std::cout.flush();

        GuidedScrambler_impl<Symbol> scrambler(
                4,
                12,
                3,
                true,
                {1,0,0,1},
                0,
                "MSW",
                "");

        Descrambler_impl<Symbol> descrambler(
                scrambler.fieldSize(),
                scrambler.codewordLength(),
                scrambler.augmentingLength(),
                scrambler.continuous(),
                scrambler.divider(),
                "");

        std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4<Symbol>>(input);
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF16 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
    {
        std::cout.flush();

        GuidedScrambler_impl<Symbol> scrambler(
                16,
                12,
                3,
                true,
                {1,0,0,1},
                0,
                "MSW",
                "");

        Descrambler_impl<Symbol> descrambler(
                scrambler.fieldSize(),
                scrambler.codewordLength(),
                scrambler.augmentingLength(),
                scrambler.continuous(),
                scrambler.divider(),
                "");

        std::vector<Symbol> input(scrambler.codewordLength()-scrambler.augmentingLength());

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF16<Symbol>>(input);
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
