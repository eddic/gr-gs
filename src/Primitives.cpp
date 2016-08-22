/*!
 * @file      Primitives.cpp
 * @brief     Declares the gr::gs::Primitives namespace.
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 21, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
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

#include "gr-gs/Primitives.h"
#include "gr-gs/exceptions.h"

#include "Words.hpp"

#include <algorithm>
#include <iostream>

void gr::gs::Primitives::populate(
        const unsigned fieldSize,
        const unsigned length,
        WordSet& wordSet)
{
    using namespace GuidedScrambling::Words;
    const std::function<void(
            const Word& dividend,
            const Word& divider,
            Word& quotient,
            Word& remainder)> divide
        = GuidedScrambling::Words::getDivide(fieldSize);

    const unsigned startingLength 
        = wordSet.empty()?2:wordSet.crbegin()->size()+1;

    for(unsigned i=startingLength; i<=length; ++i)
    {
        std::vector<Word> newPrimitives;
        
        Word polynomial(i, 0);
        polynomial.front() = 1;
        polynomial.back() = 1;

        Word quotient(i, 0);

        while(polynomial.front() != 0)
        {
            std::cout << "Trying " << to_string(polynomial);
            // Try dividing all the lesser degree polynomials
            bool foundFactor = false;
            for(const auto& divider: wordSet)
            {
                Word remainder(divider.size()-1, 0);
                divide(
                        polynomial,
                        divider,
                        quotient,
                        remainder);
                std::fill(quotient.begin(), quotient.end(), 0);
                if(!std::all_of(
                            remainder.cbegin(),
                            remainder.cend(),
                            [](const Symbol& x) { return x==0; }))
                {
                    std::cout << " and found factor " << to_string(divider) << std::endl;
                    foundFactor = true;
                    break;
                }
            }

            if(!foundFactor)
            {
                newPrimitives.push_back(polynomial);
                std::cout << " and found no factor" << std::endl;
            }

            // Increment the polynomial
            for(
                    auto symbol = polynomial.rbegin();
                    symbol != polynomial.rend();
                    ++symbol)
            {
                if(*symbol == fieldSize-1)
                    *symbol = 0;
                else
                {
                    *symbol += 1;
                    break;
                }
            }
        }

        wordSet.reserve(wordSet.size()+newPrimitives.size());
        for(auto& primitive: newPrimitives)
            wordSet.push_back(std::move(primitive));
    }
}

gr::gs::Primitives::WordSet gr::gs::Primitives::getPrimitives(
        const unsigned fieldSize,
        const unsigned length)
{
    WordSet primitives;
    populate(fieldSize, length, primitives);
    return primitives;
}

gr::gs::Word gr::gs::Primitives::getTrinomial(
        const unsigned fieldSize,
        const unsigned length)
{
    WordSet primitives;
    populate(fieldSize, length, primitives);
    
    auto range = std::equal_range(
            primitives.cbegin(),
            primitives.cend(),
            length,
            WordLess());

    for(auto polynomial=range.first; polynomial!= range.second; ++polynomial)
    {
        unsigned terms = 0;
        for(const auto& symbol: *polynomial)
            if(symbol != 0)
                ++terms;
        if(terms == 3)
            return std::move(*polynomial);
    }
    return Word();
}
