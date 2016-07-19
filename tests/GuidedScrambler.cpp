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

#include "GuidedScrambler_impl.hpp"
#include "Descrambler_impl.hpp"
#include "GF2.hpp"
#include "GF4.hpp"
#include "Word.hpp"

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::GuidedScrambler_impl unit tests ***\n\n";

    std::cout << "Testing random block scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
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

    std::cout << "Testing random block scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
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

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::GuidedScrambler_impl... ";
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
