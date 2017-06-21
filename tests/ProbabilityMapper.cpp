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
#include <complex>

#include "ProbabilityMapper.hpp"

int main()
{
    gr::gs::Implementations::ProbabilityMapper<unsigned char> mapper(4,12,3,0.0001);

    //std::cout << "History = " << mapper.history() << '\n';

    std::vector<unsigned char> input
    {
0,
1,
3,
2,
0,
2,
1,
2,
1,
1,
1,
1,
0,
2,
2,
0,
2,
0,
0,
2,
2,
2,
0,
3,
3,
3,
0,
2,
0,
1,
3,
1,
2,
1,
2,
0,
0,
2,
2,
3,
2,
2,
2,
2,
1,
3,
0,
2,
0,
0,
1,
3,
2,
2,
1,
1,
3,
1,
0,
0,
3,
0,
1,
2,
1,
2,
1,
1,
1,
1,
1,
3,
2,
3,
3,
0,
0,
2,
3,
3,
3,
0,
1,
1,
0,
2,
1,
2,
2,
1,
0,
0,
1,
3,
2,
1,
0,
1,
3,
2,
1,
2,
1,
3,
1,
0,
3,
1,
0,
2,
3,
3,
3,
0,
3,
3,
0,
3,
1,
3,
0,
0,
1,
1,
3,
1,
1,
2,
1
    };

    std::vector<float> output(input.size()-mapper.history());

    mapper.map(input.data(), false, output.data(), {0,0}, output.size());

    return 0;
}
