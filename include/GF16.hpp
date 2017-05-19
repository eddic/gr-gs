/*!
 * @file      GF16.hpp
 * @brief     Declares the gr::gs::GuidedScrambling::GF16 class
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

#ifndef GR_GS_GF16_HPP
#define GR_GS_GF16_HPP

#include <random>

#include "gr-gs/config.h"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Stuff needed for the GuidedScrambler and Descrambler blocks
        namespace GuidedScrambling
        {
            //! Handles Galois Field (16) symbols.
            /*!
             * This class can contain a Galois Field (16) symbol. The class
             * allows for some basic arithmetic operations needed to perform
             * scrambling.
             *
             * Be aware that no checks occur to ensure that the integer values
             * actually represent valid symbols. Should they not, the behaviour
             * is undefined.
             *
             * @tparam T Base type to use for symbol type. Can be unsigned char,
             *           unsigned short, or unsigned int.
             * @date   May 18, 2017
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename T>
            class GF16
            {
            public:
                typedef T Symbol;

                //! Size of symbol
                static const unsigned int fieldSize=16;

                //! Initialize symbol from it's integer representation
                /*!
                 * @param  [in] x Source integer.
                 */
                GF16(const Symbol x):
                    m_data(x)
                {}

                //! Perform Galois Field (16) addition
                /*!
                 * @param  [in] x Right hand summand.
                 * @return Galois Field (16) sum of *this and argument.
                 */
                GF16 operator+(const GF16 x) const
                {
                    static const Symbol table[fieldSize][fieldSize] =
                    {
                        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
                        {1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14},
                        {2,3,0,1,6,7,4,5,10,11,8,9,14,15,12,13},
                        {3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12},
                        {4,5,6,7,0,1,2,3,12,13,14,15,8,9,10,11},
                        {5,4,7,6,1,0,3,2,13,12,15,14,9,8,11,10},
                        {6,7,4,5,2,3,0,1,14,15,12,13,10,11,8,9},
                        {7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8},
                        {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
                        {9,8,11,10,13,12,15,14,1,0,3,2,5,4,7,6},
                        {10,11,8,9,14,15,12,13,2,3,0,1,6,7,4,5},
                        {11,10,9,8,15,14,13,12,3,2,1,0,7,6,5,4},
                        {12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3},
                        {13,12,15,14,9,8,11,10,5,4,7,6,1,0,3,2},
                        {14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1},
                        {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0}
                    };
                    return table[m_data][x.m_data];
                }

                //! Perform Galois Field (16) subtraction
                /*!
                 * @param  [in] x Subtrahend in subtraction operation.
                 * @return Galois Field (16) difference of *this and the argument
                 */
                GF16 operator-(const GF16 x) const
                {
                    return *this + x;
                }

                //! Perform Galois Field (16) negation
                /*!
                 * @return The Galois Field (16) additive inverse of *this
                 */
                GF16 operator-() const
                {
                    return *this;
                }

                //! Perform Galois Field (16) multiplication
                /*!
                 * @param  [in] x Multiplier in multiplication operation.
                 * @return Galois Field (16) product of *this and the argument
                 */
                GF16 operator*(const GF16 x) const
                {
                    static const Symbol table[fieldSize][fieldSize] =
                    {
                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
                        {0,2,4,6,8,10,12,14,3,1,7,5,11,9,15,13},
                        {0,3,6,5,12,15,10,9,11,8,13,14,7,4,1,2},
                        {0,4,8,12,3,7,11,15,6,2,14,10,5,1,13,9},
                        {0,5,10,15,7,2,13,8,14,11,4,1,9,12,3,6},
                        {0,6,12,10,11,13,7,1,5,3,9,15,14,8,2,4},
                        {0,7,14,9,15,8,1,6,13,10,3,4,2,5,12,11},
                        {0,8,3,11,6,14,5,13,12,4,15,7,10,2,9,1},
                        {0,9,1,8,2,11,3,10,4,13,5,12,6,15,7,14},
                        {0,10,7,13,14,4,9,3,15,5,8,2,1,11,6,12},
                        {0,11,5,14,10,1,15,4,7,12,2,9,13,6,8,3},
                        {0,12,11,7,5,9,14,2,10,6,1,13,15,3,4,8},
                        {0,13,9,4,1,12,8,5,2,15,11,6,3,14,10,7},
                        {0,14,15,1,13,3,2,12,9,7,6,8,4,10,11,5},
                        {0,15,13,2,9,6,4,11,1,14,12,3,8,7,5,10}
                    };
                    return table[m_data][x.m_data];
                }

                //! Perform Galois Field (16) division
                /*!
                 * @param  [in] x Divider in division operation.
                 * @return Galois Field (16) quotient of *this and the argument
                 */
                GF16 operator/(const GF16 x) const
                {
                    static const Symbol table[fieldSize][fieldSize] =
                    {
                        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                        {0,1,9,14,13,11,7,6,15,2,12,5,10,4,3,8},
                        {0,2,1,15,9,5,14,12,13,4,11,10,7,8,6,3},
                        {0,3,8,1,4,14,9,10,2,6,7,15,13,12,5,11},
                        {0,4,2,13,1,10,15,11,9,8,5,7,14,3,12,6},
                        {0,5,11,3,12,1,8,13,6,10,9,2,4,7,15,14},
                        {0,6,3,2,8,15,1,7,4,12,14,13,9,11,10,5},
                        {0,7,10,12,5,4,6,1,11,14,2,8,3,15,9,13},
                        {0,8,4,9,2,7,13,5,1,3,10,14,15,6,11,12},
                        {0,9,13,7,15,12,10,3,14,1,6,11,5,2,8,4},
                        {0,10,5,6,11,2,3,9,12,7,1,4,8,14,13,15},
                        {0,11,12,8,6,9,4,15,3,5,13,1,2,10,14,7},
                        {0,12,6,4,3,13,2,14,8,11,15,9,1,5,7,10},
                        {0,13,15,10,14,6,5,8,7,9,3,12,11,1,4,2},
                        {0,14,7,11,10,8,12,2,5,15,4,3,6,13,1,9},
                        {0,15,14,5,7,3,11,4,10,13,8,6,12,9,2,1}
                    };
                    return table[m_data][x.m_data];
                }

                //! Generate random symbol
                /*!
                 * This does not utilize any reproducible random number
                 * generation nor can it's statistics be controlled. It is
                 * intended entirely for debug and testing purposes.
                 *
                 * @return Randomly generated Galois Field (16) symbol.
                 */
                static GF16 random()
                {
                    std::random_device generator;
                    std::uniform_int_distribution<Symbol> distribution(
                            0,
                            fieldSize-1);
                    return GF16(distribution(generator));
                }

                //! Access internal Symbol
                /*!
                 * @return Internal Symbol
                 */
                operator Symbol() const
                {
                    return m_data;
                }

            private:
                //! Internal symbol representation.
                Symbol m_data;
            };
        }
    }
}

#endif
