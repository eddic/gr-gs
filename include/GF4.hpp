/*!
 * @file      GF4.hpp
 * @brief     Declares the gr::gs::GuidedScrambling::GF4 class
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

#ifndef GR_GS_GF4_HPP
#define GR_GS_GF4_HPP

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
            //! Handles Galois Field (4) symbols.
            /*!
             * This class can contain a Galois Field (4) symbol. The class
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
            class GF4
            {
            public:
                typedef T Symbol;

                //! Size of symbol
                static const unsigned int fieldSize=4;

                //! Initialize symbol from it's integer representation
                /*!
                 * @param  [in] x Source integer.
                 */
                GF4(const Symbol x):
                    m_data(x)
                {}

                //! Perform Galois Field (4) addition
                /*!
                 * @param  [in] x Right hand summand.
                 * @return Galois Field (4) sum of *this and argument.
                 */
                GF4 operator+(const GF4 x) const
                {
                    static const Symbol table[fieldSize][fieldSize] =
                    {
                        {0, 1, 2, 3},
                        {1, 0, 3, 2},
                        {2, 3, 0, 1},
                        {3, 2, 1, 0}
                    };
                    return table[m_data][x.m_data];
                }

                //! Perform Galois Field (4) subtraction
                /*!
                 * @param  [in] x Subtrahend in subtraction operation.
                 * @return Galois Field (4) difference of *this and the argument
                 */
                GF4 operator-(const GF4 x) const
                {
                    return *this + x;
                }

                //! Perform Galois Field (4) negation
                /*!
                 * @return The Galois Field (4) additive inverse of *this
                 */
                GF4 operator-() const
                {
                    return *this;
                }

                //! Perform Galois Field (4) multiplication
                /*!
                 * @param  [in] x Multiplier in multiplication operation.
                 * @return Galois Field (4) product of *this and the argument
                 */
                GF4 operator*(const GF4 x) const
                {
                    static const Symbol table[fieldSize][fieldSize] =
                    {
                        {0, 0, 0, 0},
                        {0, 1, 2, 3},
                        {0, 2, 3, 1},
                        {0, 3, 1, 2}
                    };
                    return table[m_data][x.m_data];
                }

                //! Perform Galois Field (4) division
                /*!
                 * @param  [in] x Divider in division operation.
                 * @return Galois Field (4) quotient of *this and the argument
                 */
                GF4 operator/(const GF4 x) const
                {
                    static const Symbol table[fieldSize][fieldSize] =
                    {
                      // 0, 1, 2, 3
                        {0, 0, 0, 0}, // 0
                        {0, 1, 3, 2}, // 1
                        {0, 2, 1, 3}, // 2
                        {0, 3, 2, 1}  // 3
                    };
                    return table[m_data][x.m_data];
                }

                //! Generate random symbol
                /*!
                 * This does not utilize any reproducible random number
                 * generation nor can it's statistics be controlled. It is
                 * intended entirely for debug and testing purposes.
                 *
                 * @return Randomly generated Galois Field (4) symbol.
                 */
                static GF4 random()
                {
                    std::random_device generator;
                    std::uniform_int_distribution<Symbol> distribution(
                            0,
                            fieldSize-1);
                    return GF4(distribution(generator));
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
