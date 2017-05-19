/*!
 * @file      Primitives.cpp
 * @brief     Declares the gr::gs::Primitives namespace.
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 19, 2017
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

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Tools for finding primitive polynomials
        namespace Primitives
        {
            template<typename Symbol>
            struct Irreducible
            {
                std::vector<Symbol> word;
                bool primitive;
            };

            //! An ordered set for words
            template<typename Symbol> using Irreducibles
                = std::vector<Irreducible<Symbol>>;

            //! Populate a set of irreducibles with 1-byte symbols
            GS_API void populateIrreducibles_b(
                    const unsigned fieldSize,
                    const unsigned length,
                    Irreducibles<unsigned char>& irreducibles);

            //! Populate a set of irreducibles with 2-byte symbols
            GS_API void populateIrreducibles_s(
                    const unsigned fieldSize,
                    const unsigned length,
                    Irreducibles<unsigned short>& irreducibles);

            //! Populate a set of irreducibles with 4-byte symbols
            GS_API void populateIrreducibles_i(
                    const unsigned fieldSize,
                    const unsigned length,
                    Irreducibles<unsigned int>& irreducibles);

            //! Comparison class needed for words
            template<typename Symbol>
            struct IrreducibleLess
            {
                bool operator()(
                        const Irreducible<Symbol>& x,
                        const Irreducible<Symbol>& y) const
                {
                    if(x.word.size() == y.word.size())
                    {
                        for(
                                typename std::vector<Symbol>::const_iterator
                                    i = x.word.cbegin(),
                                    j = y.word.cbegin();
                                i != x.word.cend();
                                ++i, ++j)
                            if(*i < *j)
                                return true;
                    }
                    else
                        return x.word.size() < y.word.size();
                    return false;
                }

                bool operator()(
                        const Irreducible<Symbol>& polynomial,
                        const unsigned length) const
                {
                    return polynomial.word.size() < length;
                }

                bool operator()(
                        const unsigned length,
                        const Irreducible<Symbol>& polynomial) const
                {
                    return length < polynomial.word.size();
                }
            };

            //! Populate a set of irreducibles
            template<typename Symbol>
            inline void populateIrreducibles(
                    const unsigned fieldSize,
                    const unsigned length,
                    Irreducibles<Symbol>& irreducibles);

            //! Find a set of primitives of a specific length
            template<typename Symbol>
            inline std::vector<std::vector<Symbol>> findPrimitives(
                    const unsigned fieldSize,
                    const unsigned length);

            //! Find a primitive of a specific length with the least terms
            template<typename Symbol>
            inline std::vector<Symbol> findPrimitive(
                    const unsigned fieldSize,
                    const unsigned length);
        }
    }
}

template<typename Symbol>
void gr::gs::Primitives::populateIrreducibles(
        const unsigned fieldSize,
        const unsigned length,
        Irreducibles<Symbol>& irreducibles)
{
    const std::function<void(
            const std::vector<Symbol>& dividend,
            const std::vector<Symbol>& divider,
            std::vector<Symbol>& quotient,
            std::vector<Symbol>& remainder)> divide
        = GuidedScrambling::Words::getDivide<Symbol>(fieldSize, true);

    const unsigned startingLength
        = irreducibles.empty()?2:irreducibles.crbegin()->word.size()+1;

    for(unsigned i=startingLength; i<=length; ++i)
    {
        Irreducibles<Symbol> newIrreducibles;

        std::vector<Symbol> polynomial(i, 0);
        polynomial.front() = 1;

        std::vector<Symbol> quotient(i, 0);

        while(polynomial.front() != 0)
        {
            // Try dividing all the lesser degree irreducibles
            bool foundFactor = false;
            for(const auto& divider: irreducibles)
            {
                std::vector<Symbol> remainder(divider.word.size()-1, 0);
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
                    foundFactor = true;
                    break;
                }
            }

            if(!foundFactor)
            {
                // It's irreducible, but is it primitive?
                bool isPrimitive = true;
                unsigned maxDegree = fieldSize;
                for(unsigned j=2; j<polynomial.size(); ++j)
                    maxDegree *= fieldSize;

                std::vector<Symbol> quotient(maxDegree);
                std::vector<Symbol> dividend(maxDegree);
                dividend.front()=1;
                dividend.back()=1;
                std::vector<Symbol> remainder(polynomial.size()-1, 0);
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
                                isPrimitive = false;
                                break;
                            }
                        }
                }
                else
                {
                    isPrimitive = false;
                }

                Irreducible<Symbol> newIrreducible;
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

void gr::gs::Primitives::populateIrreducibles_b(
        const unsigned fieldSize,
        const unsigned length,
        Irreducibles<unsigned char>& irreducibles)
{
    populateIrreducibles<unsigned char>(fieldSize, length, irreducibles);
}

void gr::gs::Primitives::populateIrreducibles_s(
        const unsigned fieldSize,
        const unsigned length,
        Irreducibles<unsigned short>& irreducibles)
{
    populateIrreducibles<unsigned short>(fieldSize, length, irreducibles);
}

void gr::gs::Primitives::populateIrreducibles_i(
        const unsigned fieldSize,
        const unsigned length,
        Irreducibles<unsigned int>& irreducibles)
{
    populateIrreducibles<unsigned int>(fieldSize, length, irreducibles);
}

template<typename Symbol>
std::vector<std::vector<Symbol>> gr::gs::Primitives::findPrimitives(
        const unsigned fieldSize,
        const unsigned length)
{
    Irreducibles<Symbol> irreducibles;
    populateIrreducibles(fieldSize, length, irreducibles);

    auto range = std::equal_range(
            irreducibles.cbegin(),
            irreducibles.cend(),
            length,
            IrreducibleLess<Symbol>());

    std::vector<std::vector<Symbol>> subset;
    subset.reserve(range.second-range.first);
    for(auto irreducible=range.first; irreducible!=range.second; ++irreducible)
        if(irreducible->primitive)
            subset.push_back(std::move(irreducible->word));

    return subset;
}

std::vector<std::vector<unsigned char>> gr::gs::Primitives::findPrimitives_b(
        const unsigned fieldSize,
        const unsigned length)
{
    return findPrimitives<unsigned char>(fieldSize, length);
}

std::vector<std::vector<unsigned short>> gr::gs::Primitives::findPrimitives_s(
        const unsigned fieldSize,
        const unsigned length)
{
    return findPrimitives<unsigned short>(fieldSize, length);
}

std::vector<std::vector<unsigned int>> gr::gs::Primitives::findPrimitives_i(
        const unsigned fieldSize,
        const unsigned length)
{
    return findPrimitives<unsigned int>(fieldSize, length);
}

template<typename Symbol>
std::vector<Symbol> gr::gs::Primitives::findPrimitive(
        const unsigned fieldSize,
        const unsigned length)
{
    Irreducibles<Symbol> irreducibles;
    populateIrreducibles(fieldSize, length, irreducibles);

    auto range = std::equal_range(
            irreducibles.cbegin(),
            irreducibles.cend(),
            length,
            IrreducibleLess<Symbol>());

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
        return std::vector<Symbol>();
}

std::vector<unsigned char> gr::gs::Primitives::findPrimitive_b(
        const unsigned fieldSize,
        const unsigned length)
{
    return findPrimitive<unsigned char>(fieldSize, length);
}

std::vector<unsigned short> gr::gs::Primitives::findPrimitive_s(
        const unsigned fieldSize,
        const unsigned length)
{
    return findPrimitive<unsigned short>(fieldSize, length);
}

std::vector<unsigned int> gr::gs::Primitives::findPrimitive_i(
        const unsigned fieldSize,
        const unsigned length)
{
    return findPrimitive<unsigned int>(fieldSize, length);
}
