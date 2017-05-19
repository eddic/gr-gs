/*!
 * @file      Words.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::Words namespace.
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 18, 2017
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

#include <algorithm>
#include <cmath>

#include "gr-gs/exceptions.h"
#include "Words.hpp"
#include "GF2.hpp"
#include "GF4.hpp"
#include "GF8.hpp"
#include "GF16.hpp"

std::vector<gr::gs::Complex> gr::gs::defaultConstellation(
        unsigned int fieldSize)
{
    static const Complex::value_type root2inv=1.0/std::sqrt(2);

    switch(fieldSize)
    {
        case 2:
            return {{-1,0},{1,0}};
        case 4:
            return {{1,0},{0,1},{0,-1},{-1,0}};
        case 8:
            return {
                {1,0},
                {root2inv,root2inv},
                {-root2inv,root2inv},
                {0,1},
                {root2inv,-root2inv},
                {0,-1},
                {-1,0},
                {-root2inv,-root2inv}
            };
        case 16:
            return {
                {-1,1},
                {-1,1.0/3},
                {-1,-1},
                {-1,-1.0/3},
                {-1.0/3,1},
                {-1.0/3,1.0/3},
                {-1.0/3,-1},
                {-1.0/3,-1.0/3},
                {1,1},
                {1,1.0/3},
                {1,-1},
                {1,-1.0/3},
                {1.0/3,1},
                {1.0/3,1.0/3},
                {1.0/3,-1},
                {1.0/3,-1.0/3}
            };
        default:
            return std::vector<Complex>();
    }
}

template<typename Field>
void gr::gs::GuidedScrambling::Words::divide(
        const std::vector<typename Field::Symbol>& dividend,
        const std::vector<typename Field::Symbol>& divider,
        std::vector<typename Field::Symbol>& quotient,
        std::vector<typename Field::Symbol>& remainder)
{
    for(unsigned int i=0; i<quotient.size(); ++i)
    {
        typename Field::Symbol& output=quotient[i];
        output =
            (
                Field(dividend[i])+Field(remainder.front())
            )/Field(divider.front());

        for(unsigned int j=1; j<remainder.size(); ++j)
            remainder[j-1] =
                Field(output) * (-Field(divider[j])) + Field(remainder[j]);
        remainder.back() = Field(output) * (-Field(divider.back()));
    }
}

template<typename Field>
void gr::gs::GuidedScrambling::Words::delayedDivide(
        const std::vector<typename Field::Symbol>& dividend,
        const std::vector<typename Field::Symbol>& divider,
        std::vector<typename Field::Symbol>& quotient,
        std::vector<typename Field::Symbol>& remainder)
{
    for(unsigned int i=0; i<quotient.size(); ++i)
    {
        typename Field::Symbol& output=quotient[i];
        output = Field(remainder.front())/Field(divider.front());

        for(unsigned int j=1; j<remainder.size(); ++j)
            remainder[j-1] =
                Field(output) * (-Field(divider[j])) + Field(remainder[j]);
        remainder.back() =
            Field(dividend[i])
            + Field(output) * (-Field(divider.back()));
    }
}

template
std::function<void(
    const std::vector<unsigned char>& dividend,
    const std::vector<unsigned char>& divider,
    std::vector<unsigned char>& quotient,
    std::vector<unsigned char>& remainder)>
gr::gs::GuidedScrambling::Words::getDivide<unsigned char>(
        unsigned fieldSize,
        bool delayed);
template
std::function<void(
    const std::vector<unsigned short>& dividend,
    const std::vector<unsigned short>& divider,
    std::vector<unsigned short>& quotient,
    std::vector<unsigned short>& remainder)>
gr::gs::GuidedScrambling::Words::getDivide<unsigned short>(
        unsigned fieldSize,
        bool delayed);
template
std::function<void(
    const std::vector<unsigned int>& dividend,
    const std::vector<unsigned int>& divider,
    std::vector<unsigned int>& quotient,
    std::vector<unsigned int>& remainder)>
gr::gs::GuidedScrambling::Words::getDivide<unsigned int>(
        unsigned fieldSize,
        bool delayed);
template<typename Symbol>
std::function<void(
    const std::vector<Symbol>& dividend,
    const std::vector<Symbol>& divider,
    std::vector<Symbol>& quotient,
    std::vector<Symbol>& remainder)>
gr::gs::GuidedScrambling::Words::getDivide(
        unsigned fieldSize,
        bool delayed)
{
    if(delayed)
        switch(fieldSize)
        {
            case 2:
                return Words::delayedDivide<GF2<Symbol>>;
            case 4:
                return Words::delayedDivide<GF4<Symbol>>;
            case 8:
                return Words::delayedDivide<GF8<Symbol>>;
            case 16:
                return Words::delayedDivide<GF16<Symbol>>;
            default:
                throw Exceptions::BadFieldSize();
        }
    else
        switch(fieldSize)
        {
            case 2:
                return Words::divide<GF2<Symbol>>;
            case 4:
                return Words::divide<GF4<Symbol>>;
            case 8:
                return Words::divide<GF8<Symbol>>;
            case 16:
                return Words::divide<GF16<Symbol>>;
            default:
                throw Exceptions::BadFieldSize();
        }
}

template<typename Field>
void gr::gs::GuidedScrambling::Words::multiply(
        const std::vector<typename Field::Symbol>& multiplicand,
        const std::vector<typename Field::Symbol>& multiplier,
        std::vector<typename Field::Symbol>& product,
        std::vector<typename Field::Symbol>& remainder,
        bool continuous)
{
    std::fill(product.begin(), product.end(), 0);
    const unsigned int productSize=product.size();
    const unsigned int remainderSize=remainder.size();
    for(unsigned int i=0; i<productSize; ++i)
    {
        typename Field::Symbol& output=product[i];
        for(unsigned int j=0; j<=remainderSize; ++j)
        {
            const unsigned int ijSum = i+j;
            const typename Field::Symbol& input =
                (ijSum<remainderSize)?
                remainder[ijSum]
                :multiplicand[ijSum-remainderSize];
            output = Field(output)
                +Field(input)*Field(multiplier[remainderSize-j]);
        }
    }
    if(continuous)
        std::copy(
                multiplicand.end()-remainderSize,
                multiplicand.end(),
                remainder.begin());
}

template
std::function<void(
    const std::vector<unsigned char>&,
    const std::vector<unsigned char>&,
    std::vector<unsigned char>&,
    std::vector<unsigned char>&,
    bool)>
gr::gs::GuidedScrambling::Words::getMultiply<unsigned char>(
        unsigned fieldSize);
template
std::function<void(
    const std::vector<unsigned short>&,
    const std::vector<unsigned short>&,
    std::vector<unsigned short>&,
    std::vector<unsigned short>&,
    bool)>
gr::gs::GuidedScrambling::Words::getMultiply<unsigned short>(
        unsigned fieldSize);
template
std::function<void(
    const std::vector<unsigned int>&,
    const std::vector<unsigned int>&,
    std::vector<unsigned int>&,
    std::vector<unsigned int>&,
    bool)>
gr::gs::GuidedScrambling::Words::getMultiply<unsigned int>(
        unsigned fieldSize);
template<typename Symbol>
std::function<void(
    const std::vector<Symbol>&,
    const std::vector<Symbol>&,
    std::vector<Symbol>&,
    std::vector<Symbol>&,
    bool)>
gr::gs::GuidedScrambling::Words::getMultiply(unsigned fieldSize)
{
    switch(fieldSize)
    {
        case 2:
            return Words::multiply<GF2<Symbol>>;
        case 4:
            return Words::multiply<GF4<Symbol>>;
        case 8:
            return Words::multiply<GF8<Symbol>>;
        case 16:
            return Words::multiply<GF16<Symbol>>;
        default:
            throw Exceptions::BadFieldSize();
    }
}
