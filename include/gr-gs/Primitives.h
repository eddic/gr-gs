/*!
 * @file      Primitives.hpp
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

#ifndef GR_GS_PRIMITIVES_HPP
#define GR_GS_PRIMITIVES_HPP

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
         * @date   August 21, 2016
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        namespace Primitives
        {
            //! Comparison class needed for words
            struct WordLess
            {
                bool operator()(const Word& x, const Word& y) const
                {
                    if(x.size() == y.size())
                    {
                        for(
                                Word::const_iterator i=x.cbegin(), j=y.cbegin();
                                i != x.cend();
                                ++i, ++j)
                            if(*i < *j)
                                return true;
                    }
                    else
                        return x.size() < y.size();
                    return false;
                }

                bool operator()(const Word& word, const unsigned length) const
                {
                    return word.size() < length;
                }

                bool operator()(const unsigned length, const Word& word) const
                {
                    return length < word.size();
                }
            };

            //! An ordered set for words
            typedef std::vector<Word> WordSet;

            GS_API void populate(
                    const unsigned fieldSize,
                    const unsigned length,
                    WordSet& wordSet);

            GS_API WordSet getPrimitives(
                    const unsigned fieldSize,
                    const unsigned length);

            GS_API Word getTrinomial(
                    const unsigned fieldSize,
                    const unsigned length);
        }
    }
}

#endif
