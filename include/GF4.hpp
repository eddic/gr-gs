/*!
 * @file			GF4.hpp
 * @brief		Defines gr::Isatec::GuidedScrambling::GF4 class.
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Jan 20, 2015
 * @copyright	Copyright &copy; 2014 Eddie Carle &lt;eddie@isatec.ca&gt;.
 * 				This project is released under the GNU General Public License
 * 				Version 3.
 */

/* Copyright (C) 2015 Eddie Carle <eddie@isatec.ca>
 * 
 * This file is part of The Guided Scrambling Simulator.
 *
 * The Guided Scrambling Simulator is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * The Guided Scrambling Simulator is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *                                                                           
 * You should have received a copy of the GNU General Public License along with
 * The Guided Scrambling Simulator.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GF4_HPP
#define GF4_HPP

#include <cstdint>
#include <random>
#include <array>
#include <complex>

#include "gr-isatec/config.h"

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all Guided Scrambling classes.
		namespace GuidedScrambling
		{
			//! Handling Galois Field (4) symbols.
			/*!
			 * This class can contain a Galois Field (4) symbol.  The class allows for
			 * some basic arithmetic operations need to perform scrambling.
			 *
			 * Be aware that no checks occur to ensure that the integer values actually
			 * represent valid symbols. Should they not, the behaviour is undefined.
			 *
			 * @date		Jan 20, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class GF4
			{
			public:
				//! Size of symbol
				static const unsigned int fieldSize=4;

				//! Initialize symbol it's integer representation
				/*!
				 * @param	[in] x Source integer.
				 * @date		Jan 18, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF4(const Symbol x):
					m_data(x)
				{}

				//! Perform Galois Field (4) addition
				/*!
				 * @param	[in] x Right hand summand.
				 * @return	Galois Field (4) sum of *this and argument.
				 * @date		Jan 20, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
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
				 * @param	[in] x Subtrahend in subtraction operation.
				 * @return	Galois Field (4) difference of *this and the argument
				 * @date		Jan 20, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF4 operator-(const GF4 x) const
				{
					return *this + x;
				}

				//! Perform Galois Field (4) negation
				/*!
				 * @return	The Galois Field (4) "negative" of *this
				 * @date		Jan 18, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF4 operator-() const
				{
					return *this;
				}

				//! Perform Galois Field (4) multiplication
				/*!
				 * @param	[in] x Multiplier in multiplication operation.
				 * @return	Galois Field (4) product of *this and the argument
				 * @date		Jan 20, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
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
				 * @param	[in] x Divider in division operation.
				 * @return	Galois Field (4) quotient of *this and the argument
				 * @date		Jan 20, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF4 operator/(const GF4 x) const
				{
					static const Symbol table[fieldSize][fieldSize] =
					{
						{0, 0, 0, 0},
						{0, 1, 3, 2},
						{0, 2, 1, 3},
						{0, 3, 2, 1}
					};
					return table[m_data][x.m_data];
				}

				//! Generate random symbol
				/*!
				 * This does not utilize any reproducible random generation nor can it's
				 * statistics by controlled. It is intended entirely for debug and
				 * testing purposes.
				 *
				 * @return	Randomly generated Galois Field (4) symbol.
				 * @date		Jan 18, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				static GF4 random()
				{
					std::random_device generator;
					std::uniform_int_distribution<Symbol> distribution(0,fieldSize-1);
					return GF4(distribution(generator));
				}

				//! Access internal Symbol
				/*!
				 * @return	Internal Symbol
				 * @date		Jan 20, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				operator Symbol() const
				{
					return m_data;
				}

			private:
				Symbol m_data;	//!> Internal integer representation.
			};
		}
	}
}

#endif
