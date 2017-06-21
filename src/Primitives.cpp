/*!
 * @file      Primitives.cpp
 * @brief     Defines some primitive polynomial functions
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      June 21, 2017
 * @copyright Copyright &copy; 2017 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
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

#include "gr-gs/Primitives.h"
#include "gr-gs/exceptions.h"

#include "Words.hpp"

#include <algorithm>
#include <cmath>

template
std::vector<unsigned char> gr::gs::findPrimitive(
        const unsigned fieldSize,
        const unsigned length);
template
std::vector<unsigned short> gr::gs::findPrimitive(
        const unsigned fieldSize,
        const unsigned length);
template
std::vector<unsigned int> gr::gs::findPrimitive(
        const unsigned fieldSize,
        const unsigned length);
template<typename Symbol>
std::vector<Symbol> gr::gs::findPrimitive(
        const unsigned fieldSize,
        const unsigned length)
{
    unsigned winnerTerms=length+1;
    std::vector<Symbol> winner(length, 0);

    std::vector<Symbol> polynomial(length, 0);
    polynomial.front() = 1;

    while(polynomial.front() != 0)
    {
        if(polynomial.back() != 0)
        {
            unsigned terms=0;
            for(const auto& symbol: polynomial)
                if(symbol != 0)
                    ++terms;

            if(terms < winnerTerms)
                if(verifyPrimitive<Symbol>(fieldSize, polynomial))
                {
                    winnerTerms = terms;
                    winner = polynomial;
                }
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

    return winner;
}

template
bool gr::gs::verifyPrimitive(
        const unsigned fieldSize,
        const std::vector<unsigned char>& word);
template
bool gr::gs::verifyPrimitive(
        const unsigned fieldSize,
        const std::vector<unsigned short>& word);
template
bool gr::gs::verifyPrimitive(
        const unsigned fieldSize,
        const std::vector<unsigned int>& word);
template<typename Symbol>
bool gr::gs::verifyPrimitive(
        const unsigned fieldSize,
        const std::vector<Symbol>& word)
{
    std::vector<Symbol> dividend(1, 1);
    std::vector<Symbol> quotient(1, 0);
    const std::function<void(
            const std::vector<Symbol>& dividend,
            const std::vector<Symbol>& divider,
            std::vector<Symbol>& quotient,
            std::vector<Symbol>& remainder)> divide
        = GuidedScrambling::Words::getDivide<Symbol>(fieldSize, false);
    const unsigned period =
        [fieldSize] (const unsigned exp)
        {
            unsigned answer=1;
            for(unsigned i=0; i<exp; ++i)
                answer *= fieldSize;
            return answer;
        }(word.size()-1);
    std::vector<Symbol> remainder(word.size()-1, 0);

    divide(dividend, word, quotient, remainder);
    const std::vector<Symbol> startingRemainder(remainder);
    dividend[0]=0;

    for(unsigned i=2; i<period; ++i)
    {
        divide(dividend, word, quotient, remainder);
        if(remainder == startingRemainder)
            return false;
    }
    return true;
}
