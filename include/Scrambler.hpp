/*!
 * @file			Scrambler.hpp
 * @brief		Declares the gr::Isatec::GuidedScrambler::Scrambler class
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

#ifndef SCRAMBLER_HPP
#define SCRAMBLER_HPP

#include <memory>
#include <algorithm>
#include <vector>
#include <functional>

#include "gr-isatec/config.h"
#include "Analyzer.hpp"

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains elements needed for the GuidedScrambler and Descrambler blocks
		namespace GuidedScrambling
		{
			//! For performing a single scramble operation.
			/*!
			 * The intention is for these scramblers to be grouped into ScramblerGroup
			 * objects for the purpose of keeping them in execution groups.
			 *
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class Scrambler
			{
			public:
				//! Configure the scrambler's parameters
				/*!
				 * This function allows the scrambler to be dynamically re-configured at
				 * runtime. Note that calling this will reset the remainder to all zeroes;
				 * effectively resetting a continuous scrambling operation.
				 *
				 * The augmenting portion of the input words will be set using the index
				 * value passed as a parameter.
				 *
				 * @param	[in] length Length of codeword.
				 * @param	[in] index Index of scrambler in the full set (not the group).
				 * 			This is used to set the augmenting portion of the input word.
				 * @param	[in] augmentingLength Length of augmenting segment of the
				 * 			codeword.
				 * @param	[in] remainderLength Length of the remainder word. This is
				 * 			really the length of the divisor word minus one;
				 * @param	[in] method The desired analysis method to use.
				 * @param	[in] fieldSize Desired field size
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void configure(
						const unsigned int length,
						unsigned int index,
						const unsigned int augmentingLength,
						const unsigned int remainderLength,
						const unsigned int method,
						const unsigned int fieldSize);

				//! Perform the actual scramble operation
				/*!
				 * Note that in order to improve efficiency of operation this function
				 * does not perform length/bounds checks on the input word parameters. It
				 * is up to the calling function to ensure the parameters are correctly
				 * sized.
				 *
				 * @param	[in] divider Input dividing word. The size of this word
				 * 			\a must equal remainderLength (set in configure())+1.
				 * @param	[in] input Input to be scrambled. The size of this word
				 * 			\a must equal length-augmentingLength (both set in
				 * 			configure()).
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void scramble(
						const std::vector<Symbol>& divider,
						const std::vector<Symbol>& input)
				{
					std::copy(input.begin(), input.end(), m_dividend.end()-input.size());
					m_divide(
							m_dividend,
							divider,
							m_output,
							m_remainder);					
				}

				//! Set the remainder register
				/*!
				 * This sets the remainder register used during the scramble operation. In
				 * order to do block scrambling, this should be set to all zeros before
				 * each scrambling operation. If doing a single continuous scrambling
				 * operation, this can be ignored. If doing multiple competing continuous
				 * scrambling operations (as in guided scrambling), this should be used
				 * with the remainder() of the winning scrambler.
				 *
				 * Note that in order to improve efficiency of operation this function
				 * does not perform length/bounds checks on the input Word. It is up to
				 * the calling function to ensure the remainder is correctly sized.
				 *
				 * @param	[in] remainder Input remainder word. The size of this word
				 * 			\a must equal remainderLength (set in configure()).
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void setRemainder(const std::vector<Symbol>& remainder)
				{
					if(&m_remainder != &remainder)
						std::copy(remainder.begin(), remainder.end(), m_remainder.begin());
				}

				//! Accessor for the resulting remainder word
				/*!
				 * This becomes valid upon completion of a call to the scramble()
				 * function.
				 *
				 * Use this to see the resulting remainder from a scramble operation.
				 *
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& remainder() const
				{
					return m_remainder;
				}

				//! Accessor for the scramble operation output
				/*!
				 * This becomes valid upon completion of a call to the scramble()
				 * function.
				 *
				 * Use this to see the result of a call to scramble().
				 *
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& output() const
				{
					return m_output;
				}

				//! Accessor for the input dividend used 
				/*!
				 * This becomes valid upon completion of a call to the scramble()
				 * function.
				 *
				 * Use of this accessor is primarily for debug and purposes as it allows
				 * us to see the actual input word (including the augmenting portion) used
				 * in the last call to scramble().
				 *
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& dividend() const
				{
					return m_dividend;
				}

				//! Perform analysis on output codeword
				/*!
				 * @param	[in] feedback The feedback from the winning codeword to start
				 *				the analysis with.
				 * @param	[in] constellation This is a direct mapping of symbols (as
				 *				vector indices) to constellation points.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void analyze(
						const Analyzer::Feedback& feedback,
						const std::vector<std::complex<float>>& constellation)
				{
					m_analyzer->analyze(m_output, feedback, constellation);
				}

				//! Extract feedback from analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function.
				 *
				 * @return	Constant reference to Feedback object.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const Analyzer::Feedback& feedback() const
				{
					return m_analyzer->feedback();
				}

				//! Extract result from analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function. In this context a lower analysis means a better
				 * codeword.
				 *
				 * @return	Floating point representation of the codeword's selection
				 *				analysis. Lower is better.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				float analysis() const
				{
					return m_analyzer->analysis();
				}

			protected:
				std::vector<Symbol> m_dividend; //!< Actual input to division operation
				std::vector<Symbol> m_output; //!< Output resulting from division operation
				std::vector<Symbol> m_remainder; //!< Remainder word used in division operation
				std::unique_ptr<Analyzer> m_analyzer; //!< Analyzer object
				std::function<void(
						const std::vector<Symbol>& dividend,
						const std::vector<Symbol>& divider,
						std::vector<Symbol>& quotient,
						std::vector<Symbol>& remainder)> m_divide; //!< Divide function we'll be using
			};
		}
	}
}

#endif
