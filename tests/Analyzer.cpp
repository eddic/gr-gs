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
#include <memory>

#include "Analyzer.hpp"
#include "MSW.hpp"
#include "GF2.hpp"
#include "GF4.hpp"

typedef unsigned char Symbol;

int main()
{
    using namespace gr::gs;
    using namespace gr::gs::GuidedScrambling;

    std::cout << "\n*** Initiating gr::gs::GuidedScrambling::Analyzer unit tests ***\n\n";

    std::cout << "Testing GF2 MSW analysis... ";
    {
        std::cout.flush();

        const std::vector<Symbol> codeword{1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0};

        std::unique_ptr<Analyzer<Symbol>> analyzer(manufactureAnalyzer<Symbol>(0));
        analyzer->analyze(codeword, analyzer->feedback(), defaultConstellation(2));

        if(analyzer->analysis() != 481)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    std::cout << "Testing GF4 MSW analysis... ";
    {
        std::cout.flush();

        const std::vector<Symbol> codeword{3,1,1,1,0,0,1,1,3,2,2,3,3,3,1,1,2,3,3,1,0,2,3};
        std::unique_ptr<Analyzer<Symbol>> analyzer(manufactureAnalyzer<Symbol>(0));
        analyzer->analyze(codeword, analyzer->feedback(), defaultConstellation(4));

        if(analyzer->analysis() != 429)
        {
            std::cout << "failed!" << std::endl;
            return 1;
        }

        std::cout << "success." << std::endl;
    }

    return 0;
}
