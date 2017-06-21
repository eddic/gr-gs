/*!
 * @file      Primitives.h
 * @brief     Declares some primitive polynomial functions
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

#ifndef GR_GS_PRIMITIVES_H
#define GR_GS_PRIMITIVES_H

#include "gr-gs/config.h"

#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Find a primitive of a specific length with the least terms
        template<typename Symbol>
        GS_API std::vector<Symbol> findPrimitive(
                const unsigned fieldSize,
                const unsigned length);

        //! Verify that a polynomial is primitive
        template<typename Symbol>
        GS_API bool verifyPrimitive(
                const unsigned fieldSize,
                const std::vector<Symbol>& word);
    }
}

#endif
