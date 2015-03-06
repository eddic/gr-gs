/*!
 * @file			GF2.hpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::GF2 class.
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 3, 2015
 * @copyright	Copyright &copy; 2015 %Isatec Inc.  This project is released
 *					under the GNU General Public License Version 3.
 */

/* Copyright (C) 2015 %Isatec Inc.
 * 
 * This file is part of the %Isatec GNU Radio Module
 *
 * The %Isatec GNU Radio Module is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The %Isatec GNU Radio Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *                                                                          
 * You should have received a copy of the GNU General Public License along with
 * The %Isatec GNU Radio Module.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GF2_HPP
#define GF2_HPP

#include <random>
#include <array>
#include <complex>

#include "gr-isatec/config.h"

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains elements needed for the GuidedScrambler and Descrambler blocks
		namespace GuidedScrambling
		{
			//! Handles Galois Field (2) symbols.
			/*!
			 * This class can contain a Galois Field (2) symbol.  The class allows for
			 * some basic arithmetic operations needed to perform scrambling.
			 *
			 * Be aware that no checks occur to ensure that the integer values actually
			 * represent valid symbols. Should they not, the behaviour is undefined.
			 *
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class GF2
			{
			public:
				//! Size of symbol
				static const unsigned int fieldSize=2;

				//! Initialize symbol from it's integer representation
				/*!
				 * @param	[in] x Source integer.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF2(const Symbol x):
					m_data(x)
				{}

				//! Perform Galois Field (2) addition
				/*!
				 * @param	[in] x Right hand summand.
				 * @return	Galois Field (2) sum of *this and argument.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF2 operator+(const GF2 x) const
				{
					return GF2(x.m_data ^ m_data);
				}

				//! Perform Galois Field (2) subtraction
				/*!
				 * @param	[in] x Subtrahend in subtraction operation.
				 * @return	Galois Field (2) difference of *this and the argument
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF2 operator-(const GF2 x) const
				{
					return *this + x;
				}

				//! Perform Galois Field (2) negation
				/*!
				 * @return	The Galois Field (2) additive inverse of *this
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF2 operator-() const
				{
					return *this;
				}

				//! Perform Galois Field (2) multiplication
				/*!
				 * @param	[in] x Multiplier in multiplication operation.
				 * @return	Galois Field (2) product of *this and the argument
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF2 operator*(const GF2 x) const
				{
					return GF2(x.m_data & m_data);
				}

				//! Perform Galois Field (2) division
				/*!
				 * @param	[in] x Divider in division operation.
				 * @return	Galois Field (2) quotient of *this and the argument
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GF2 operator/(const GF2 x) const
				{
					return *this * x;
				}

				//! Generate random symbol
				/*!
				 * This does not utilize any reproducible random number generation
				 * nor can it's statistics by controlled. It is intended entirely
				 * for debug and testing purposes.
				 *
				 * @return	Randomly generated Galois Field (2) symbol.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				static GF2 random()
				{
					std::random_device generator;
					std::uniform_int_distribution<Symbol> distribution(0,fieldSize-1);
					return GF2(distribution(generator));
				}

				//! Access internal Symbol
				/*!
				 * @return	Internal Symbol
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				operator Symbol() const
				{
					return m_data;
				}

			private:
				Symbol m_data;	//!> Internal symbol representation.
			};
		}
	}
}

#endif
