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

#include "Descrambler_impl.hpp"

typedef unsigned char Symbol;

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::Descrambler_impl unit tests ***\n\n";

    std::cout << "Testing Descrambler_impl::descrambler() in GF2... ";
    {
        std::cout.flush();
        const std::vector<Symbol> output({0,1,0,0,1,1,0,1,0,0,0,1});
        const std::vector<Symbol> product({1,0,0,1,0,1,0,0,1,1,0,1,0,0,0,1});
        const std::vector<Symbol> input({1,1,1,1,0,0,1,1,0,0,1,0,0,1,1,0});

        Descrambler_impl<Symbol> descrambler(
                2,
                input.size(),
                input.size()-output.size(),
                false,
                {1,1,0,1},
                "");
        descrambler.descramble(input);

        if(output != descrambler.output() || product != descrambler.product())
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing Descrambler_impl::descrambler() in GF4... ";
    {
        std::cout.flush();
        const std::vector<Symbol> output({3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
        const std::vector<Symbol> product({0,0,2,1,3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3});
        const std::vector<Symbol> input({0,0,1,1,3,2,3,1,2,0,3,0,1,2,0,1,3,1,3,1,1,2,2,1,2,1,2});

        Descrambler_impl<Symbol> descrambler(
                4,
                input.size(),
                input.size()-output.size(),
                false,
                {2,3,1,1,3,3},
                "");
        descrambler.descramble(input);
        descrambler.descramble(input);

        if(output != descrambler.output() || product != descrambler.product())
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    return 0;
}
