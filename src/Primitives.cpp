/*!
 * @file      Primitives.cpp
 * @brief     Declares the gr::gs::Primitives namespace.
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 23, 2016
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
#include <cmath>

void gr::gs::Primitives::populateIrreducibles(
        const unsigned fieldSize,
        const unsigned length,
        Irreducibles& irreducibles)
{
    //using namespace GuidedScrambling::Words;
    const std::function<void(
            const Word& dividend,
            const Word& divider,
            Word& quotient,
            Word& remainder)> divide
        = GuidedScrambling::Words::getDivide(fieldSize, true);

    const unsigned startingLength
        = irreducibles.empty()?2:irreducibles.crbegin()->word.size()+1;

    for(unsigned i=startingLength; i<=length; ++i)
    {
        Irreducibles newIrreducibles;

        Word polynomial(i, 0);
        polynomial.front() = 1;

        Word quotient(i, 0);

        while(polynomial.front() != 0)
        {
            //std::cout << "Trying " << to_string(polynomial);
            // Try dividing all the lesser degree irreducibles
            bool foundFactor = false;
            for(const auto& divider: irreducibles)
            {
                Word remainder(divider.word.size()-1, 0);
                divide(
                        polynomial,
                        divider.word,
                        quotient,
                        remainder);
                if(std::all_of(
                            remainder.cbegin(),
                            remainder.cend(),
                            [](const Symbol& x) { return x==0; }))
                {
                    //std::cout << " but found factor " << to_string(divider.word) << " with quotient " << to_string(quotient) << std::endl;;
                    foundFactor = true;
                    break;
                }
            }

            if(!foundFactor)
            {
                //std::cout << " and it is irreducible";
                // It's irreducible, but is it primitive?
                bool isPrimitive = true;
                unsigned maxDegree = fieldSize;
                for(unsigned j=2; j<polynomial.size(); ++j)
                    maxDegree *= fieldSize;

                Word quotient(maxDegree);
                Word dividend(maxDegree);
                dividend.front()=1;
                dividend.back()=1;
                Word remainder(polynomial.size()-1, 0);
                divide(
                        dividend,
                        polynomial,
                        quotient,
                        remainder);
                if(std::all_of(
                            remainder.cbegin(),
                            remainder.cend(),
                            [](const Symbol& x) { return x==0; }))
                {
                    maxDegree -= 1;
                    for(unsigned j=polynomial.size(); j<maxDegree; ++j)
                        if(maxDegree%j == 0)
                        {
                            quotient.resize(j+1);
                            dividend.resize(j+1);
                            dividend.front()=1;
                            dividend.back()=1;
                            std::fill(remainder.begin(), remainder.end(), 0);
                            divide(
                                    dividend,
                                    polynomial,
                                    quotient,
                                    remainder);
                            if(std::all_of(
                                        remainder.cbegin(),
                                        remainder.cend(),
                                        [](const Symbol& x) { return x==0; }))
                            {
                                //std::cout << " but a factor of " << to_string(dividend) << std::endl;
                                isPrimitive = false;
                                break;
                            }
                        }
                }
                else
                {
                    //std::cout << " but not a factor of " << to_string(dividend) << std::endl;
                    isPrimitive = false;
                }

                //if(isPrimitive)
                //    std::cout << " and primitive." << std::endl;

                Irreducible newIrreducible;
                newIrreducible.word = polynomial;
                newIrreducible.primitive = isPrimitive;
                newIrreducibles.push_back(std::move(newIrreducible));
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

        irreducibles.reserve(irreducibles.size()+newIrreducibles.size());
        for(auto& irreducible: newIrreducibles)
            irreducibles.push_back(std::move(irreducible));
    }
}

std::vector<gr::gs::Word> gr::gs::Primitives::findPrimitives(
        const unsigned fieldSize,
        const unsigned length)
{
    Irreducibles irreducibles;
    populateIrreducibles(fieldSize, length, irreducibles);

    auto range = std::equal_range(
            irreducibles.cbegin(),
            irreducibles.cend(),
            length,
            IrreducibleLess());

    std::vector<Word> subset;
    subset.reserve(range.second-range.first);
    for(auto irreducible=range.first; irreducible!=range.second; ++irreducible)
        if(irreducible->primitive)
            subset.push_back(std::move(irreducible->word));

    return subset;
}

gr::gs::Word gr::gs::Primitives::findPrimitive(
        const unsigned fieldSize,
        const unsigned length)
{
    Irreducibles irreducibles;
    populateIrreducibles(fieldSize, length, irreducibles);

    auto range = std::equal_range(
            irreducibles.cbegin(),
            irreducibles.cend(),
            length,
            IrreducibleLess());

    auto primitive=range.second;
    unsigned leastTerms=0xffffffffU;

    for(auto polynomial=range.first; polynomial!= range.second; ++polynomial)
    {
        if(polynomial->primitive)
        {
            unsigned terms = 0;
            for(const auto& symbol: polynomial->word)
                if(symbol != 0)
                    ++terms;
            if(terms < leastTerms)
            {
                leastTerms=terms;
                primitive=polynomial;
            }
        }
    }
    if(primitive != range.second)
        return std::move(primitive->word);
    else
        return Word();
}
