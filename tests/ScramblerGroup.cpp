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

#include "ScramblerGroup.hpp"
#include "Descrambler_impl.hpp"
#include "GF2.hpp"
#include "GF4.hpp"
#include "Words.hpp"

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::ScramblerGroup unit tests ***\n\n";

    std::cout << "Testing random block scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::ScramblerGroup... ";
    {
        std::cout.flush();

        const Word divider({1,0,1,1,0,1,1});
        Word input;
        input.resize(32);

        const Word remainder(divider.size()-1);

        const unsigned int indexStart=3;
        const unsigned int indexEnd=14;

        ScramblerGroup scramblers;
        scramblers.configure(input.size()+4,indexStart,indexEnd,4,divider.size()-1, 0, 2);

        Descrambler_impl descrambler(
                2,
                input.size()+4,
                4,
                false,
                divider,
                "");

        std::random_device generator;
        std::uniform_int_distribution<size_t> distribution(0, indexEnd-indexStart-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2>(input);
            scramblers.scramble(divider, input, remainder, scramblers.winner()->feedback(), defaultConstellation(2));
            descrambler.descramble(scramblers.winner()->output());
            if(descrambler.output() != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF2 with gr::gs::GuidedScrambling::ScramblerGroup... ";
    {
        std::cout.flush();

        const Word divider({1,0,1,1,0,1,1});
        Word remainder(divider.size()-1);
        Word input;
        input.resize(32);

        const unsigned int indexStart=3;
        const unsigned int indexEnd=14;

        ScramblerGroup scramblers;
        scramblers.configure(input.size()+4,indexStart,indexEnd,4,divider.size()-1, 0, 2);

        Descrambler_impl descrambler(
                2,
                input.size()+4,
                4,
                true,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF2>(input);
            scramblers.scramble(divider, input, remainder, scramblers.winner()->feedback(), defaultConstellation(2));
            remainder = scramblers.winner()->remainder();
            descrambler.descramble(scramblers.winner()->output());

            if(descrambler.output() != input)
            {
                std::cout << "failed on " << i << "!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random block scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::ScramblerGroup... ";
    {
        std::cout.flush();

        const Word divider({2,3,1,1,3,3});
        Word input;
        input.resize(32);

        const Word remainder(divider.size()-1);

        const unsigned int indexStart=3;
        const unsigned int indexEnd=14;

        ScramblerGroup scramblers;
        scramblers.configure(input.size()+4,indexStart,indexEnd,4,divider.size()-1, 0, 4);

        Descrambler_impl descrambler(
                4,
                input.size()+4,
                4,
                false,
                divider,
                "");

        std::random_device generator;
        std::uniform_int_distribution<size_t> distribution(0, indexEnd-indexStart-1);

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4>(input);
            scramblers.scramble(divider, input, remainder, scramblers.winner()->feedback(), defaultConstellation(2));
            descrambler.descramble(scramblers.winner()->output());
            if(descrambler.output() != input)
            {
                std::cout << "failed!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing random continuous scrambling->descrambling cycle in GF4 with gr::gs::GuidedScrambling::ScramblerGroup... ";
    {
        std::cout.flush();

        const Word divider({1,0,1,1,0,1,1});
        Word remainder(divider.size()-1,0);
        Word input;
        input.resize(32);

        const unsigned int indexStart=3;
        const unsigned int indexEnd=14;

        ScramblerGroup scramblers;
        scramblers.configure(input.size()+4,indexStart,indexEnd,4,divider.size()-1, 0, 4);

        Descrambler_impl descrambler(
                4,
                input.size()+4,
                4,
                true,
                divider,
                "");

        for(unsigned int i=0; i<64; ++i)
        {
            Words::randomize<GF4>(input);

            scramblers.scramble(divider, input, remainder, scramblers.winner()->feedback(), defaultConstellation(2));
            remainder = scramblers.winner()->remainder();
            descrambler.descramble(scramblers.winner()->output());

            if(descrambler.output() != input)
            {
                std::cout << "failed on " << i << "!" << std::endl;
                return 1;
            }
        }

        std::cout << "success." << std::endl;
    }

    return 0;
}
