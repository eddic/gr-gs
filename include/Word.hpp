/*!
 * @file       Word.hpp
 * @brief      Defines the gr::Isatec::GuidedScrambling::Word class.
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 3, 2015
 * @copyright  Copyright &copy; 2015 %Isatec Inc.  This project is released
 *             under the GNU General Public License Version 3.
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

#ifndef WORD_HPP
#define WORD_HPP

#include <vector>
#include <map>
#include <string>
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
			//! Handles Galois Field words.
			/*!
			 * @date    March 3, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			namespace Word
			{
				//! Arithmetic multiplication of words
				/*!
				 * Note that the remainder and product words are not resized or set to
				 * zero before division occurs. This means that continuous multiplication
				 * can be accomplished by leaving the remainder word untouched between
				 * multiplication operations. This also means, however, that you must
				 * ensure:
				 * 1. The length of the product word is equal to the length of the
				 *    multiplicand word.
				 * 2. The length of the remainder word is equal to the length of the
				 *    multiplier word minus one.
				 * 3. The implementer must be conscious of the contents of the remainder
				 *    word before calling this function.
				 *
				 * Should the continuous argument be set true, the remainder word will be
				 * updated at the end of the multiplication operation.
				 *
				 * @tparam  GF Field Type.
				 * @param   [in] multiplicand Multiplicand in multiplication operation.
				 * @param   [in] multiplier Multiplier in multiplication operation.
				 * @param   [out] product Product of multiplication operation. This word
				 *          \a must be of the same size (length) as the multiplicand or
				 *          bad things will happen.
				 * @param   [inout] remainder Input and output remainder of
				 *          multiplication operation. This size (length) of this word
				 *          \a must equal the size of the multiplier minus one or bad
				 *          things will happen.
				 * @param   [in] continuous If set to true, the remainder word is updated
				 *          with the new remainder.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				template<typename Field>
				void multiply(
						const std::vector<Symbol>& multiplicand,
						const std::vector<Symbol>& multiplier,
						std::vector<Symbol>& product,
						std::vector<Symbol>& remainder,
						bool continuous=true);

				//! Arithmetic division of words
				/*!
				 * Note that the remainder and quotient words are not resized or set to
				 * zero before division occurs. This means that continuous division can
				 * be accomplished by leaving the remainder word untouched between
				 * division operations. This also means, however, that you must ensure:
				 * 1. The length of the quotient word is equal to the length of the
				 *    dividend word.
				 * 2. The length of the remainder word is equal to the length of the
				 *    divider word minus one.
				 * 3. The implementer must be conscious of the contents of the remainder
				 *    word before calling this function.
				 *
				 * @tparam  GF Field Type.
				 * @param   [in] dividend Dividend in division operation.
				 * @param   [in] divider Divider in division operation.
				 * @param   [out] quotient Quotient of division operation. This word
				 *          \a must be of the same size (length) as the dividend or
				 *          bad things will happen.
				 * @param   [inout] remainder Input and output remainder of division
				 *          operation. This size (length) of this word \a must equal the
				 *          size of the divider minus one or bad things will happen.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				template<typename Field>
				void divide(
						const std::vector<Symbol>& dividend,
						const std::vector<Symbol>& divider,
						std::vector<Symbol>& quotient,
						std::vector<Symbol>& remainder);

				//! Give string representation of the word
				/*!
				 * This is intended for debug and testing purposes.
				 *
				 * @param   [in] word Input word.
				 * @return  Double quote enclosed sequence of space separated numeric symbol
				 *          representations.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				inline std::string to_string(const std::vector<Symbol>& word)
				{
					std::string output("\"");
					for(unsigned int i=0; i<word.size(); ++i)
						output += std::to_string((unsigned int)(word[i])) + ' ';
					output.pop_back();
					output += '"';
					return output;
				}

				//! Randomize symbols in word
				/*!
				 * Calling this function will not affect the size (length) of the word. It
				 * merely assigns random values to all symbols in the word.
				 *
				 * This does not utilize any reproducible random generation nor can it's
				 * statistics by controlled. It is intended entirely for debug and
				 * testing purposes.
				 *
				 * @tparam  GF Field Type.
				 * @param   [in] word Input word.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				template<typename Field>
				void randomize(std::vector<Symbol>& word)
				{
					for(Symbol& symbol: word)
						symbol = Field::random();
				}
			}
		}
	}
}

#endif
