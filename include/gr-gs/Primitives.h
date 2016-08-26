/*!
 * @file      Primitives.h
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

#ifndef GR_GS_PRIMITIVES_H
#define GR_GS_PRIMITIVES_H

#include "gr-gs/config.h"

#include <functional>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Tools for finding primitive polynomials
        /*!
         * @date   August 23, 2016
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        namespace Primitives
        {
            struct Irreducible
            {
                Word word;
                bool primitive;
            };

            //! Comparison class needed for words
            struct IrreducibleLess
            {
                bool operator()(
                        const Irreducible& x,
                        const Irreducible& y) const
                {
                    if(x.word.size() == y.word.size())
                    {
                        for(
                                Word::const_iterator
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
                        const Irreducible& polynomial,
                        const unsigned length) const
                {
                    return polynomial.word.size() < length;
                }

                bool operator()(
                        const unsigned length,
                        const Irreducible& polynomial) const
                {
                    return length < polynomial.word.size();
                }
            };

            //! An ordered set for words
            typedef std::vector<Irreducible> Irreducibles;

            //! Populate a set of irreducibles
            GS_API void populateIrreducibles(
                    const unsigned fieldSize,
                    const unsigned length,
                    Irreducibles& irreducibles);

            //! Find a set of primitives of a specific length
            GS_API std::vector<Word> findPrimitives(
                    const unsigned fieldSize,
                    const unsigned length);

            //! Find a primitive of a specific length with the least terms
            GS_API Word findPrimitive(
                    const unsigned fieldSize,
                    const unsigned length);
        }
    }
}

#endif
