/*!
 * @file      Words.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::Words namespace.
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

#include <algorithm>

#include "gr-gs/exceptions.h"
#include "Words.hpp"
#include "GF2.hpp"
#include "GF4.hpp"

std::vector<std::complex<float>> gr::gs::defaultConstellation(
        unsigned int fieldSize)
{
    switch(fieldSize)
    {
        case 2:
            return std::vector<std::complex<float>>(
                    {{-1,0},{1,0}});
        case 4:
            return std::vector<std::complex<float>>(
                    {{1,0},{0,1},{0,-1},{-1,0}});
        default:
            return std::vector<std::complex<float>>();
    }
}

template<typename Field>
void gr::gs::GuidedScrambling::Words::divide(
        const Word& dividend,
        const Word& divider,
        Word& quotient,
        Word& remainder)
{
    for(unsigned int i=0; i<quotient.size(); ++i)
    {
        Symbol& output=quotient[i];
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
        const Word& dividend,
        const Word& divider,
        Word& quotient,
        Word& remainder)
{
    for(unsigned int i=0; i<quotient.size(); ++i)
    {
        Symbol& output=quotient[i];
        output = Field(remainder.front())/Field(divider.front());

        for(unsigned int j=1; j<remainder.size(); ++j)
            remainder[j-1] =
                Field(output) * (-Field(divider[j])) + Field(remainder[j]);
        remainder.back() =
            Field(dividend[i])
            + Field(output) * (-Field(divider.back()));
    }
}

std::function<void(
    const gr::gs::Word& dividend,
    const gr::gs::Word& divider,
    gr::gs::Word& quotient,
    gr::gs::Word& remainder)>
gr::gs::GuidedScrambling::Words::getDivide(unsigned fieldSize, bool delayed)
{
    if(delayed)
        switch(fieldSize)
        {
            case 2:
                return Words::delayedDivide<GF2>;
                break;
            case 4:
                return Words::delayedDivide<GF4>;
                break;
            default:
                throw Exceptions::BadFieldSize();
        }
    else
        switch(fieldSize)
        {
            case 2:
                return Words::divide<GF2>;
                break;
            case 4:
                return Words::divide<GF4>;
                break;
            default:
                throw Exceptions::BadFieldSize();
        }
}

template<typename Field>
void gr::gs::GuidedScrambling::Words::multiply(
        const Word& multiplicand,
        const Word& multiplier,
        Word& product,
        Word& remainder,
        bool continuous)
{
    std::fill(product.begin(), product.end(), 0);
    const unsigned int productSize=product.size();
    const unsigned int remainderSize=remainder.size();
    for(unsigned int i=0; i<productSize; ++i)
    {
        Symbol& output=product[i];
        for(unsigned int j=0; j<=remainderSize; ++j)
        {
            const unsigned int ijSum = i+j;
            const Symbol& input =
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

std::function<void(
    const gr::gs::Word&,
    const gr::gs::Word&,
    gr::gs::Word&,
    gr::gs::Word&,
    bool)>
gr::gs::GuidedScrambling::Words::getMultiply(unsigned fieldSize)
{
    switch(fieldSize)
    {
        case 2:
            return Words::multiply<GF2>;
            break;
        case 4:
            return Words::multiply<GF4>;
            break;
        default:
            throw Exceptions::BadFieldSize();
    }
}
