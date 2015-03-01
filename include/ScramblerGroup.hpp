/*!
 * @file			ScramblerGroup.hpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::ScramblerGroup class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Jan 21, 2015
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

#ifndef SCRAMBLERGROUP_HPP
#define SCRAMBLERGROUP_HPP

#include <mutex>
#include <condition_variable>
#include <memory>

#include "Scrambler.hpp"
#include "Analyzer.hpp"

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations.
		namespace GuidedScrambling
		{
			//! Performs a group of scramble operations.
			/*!
			 * This class simply allows us to group numerous Scramble object into a
			 * single execution group. The intention is for each group to be given is own
			 * thread to allow for concurrent scrambling.
			 
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class ScramblerGroup
			{
			public:
				//! Configure the scrambler's parameters
				/*!
				 * This function allows the scrambler group to be dynamically
				 * re-configured at runtime. Note that calling this will reset all
				 * remainders to all zeroes; effectively resetting a continuous scrambling
				 * operation.
				 *
				 * The augmenting portion of the input words will be set using the index
				 * ranges passed as a parameters. For example, if you wanted a set of
				 * scramblers to used augmenting portions defined by 2,3,4 and 5, you
				 * would set indexStart=2 and indexEnd=6.
				 *
				 * @param	[in] length Length of codeword.
				 * @param	[in] indexStart Starting index of the scrambler group. This is
				 *				used to set the augmenting portion of the input words.
				 * @param	[in] indexEnd Ending index of the scrambler group. This is
				 *				used to set the augmenting portion of the input words.
				 * @param	[in] augmentingLength Length of augmenting segment of the
				 * 			codewords.
				 * @param	[in] remainderLength Length of the remainder words. This is
				 * 			really the length of the divider word minus one;
				 * @param	[in] method The desired analysis method to use.
				 * @param	[in] fieldSize Desired field size
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void configure(
						const unsigned int length,
						unsigned int indexStart,
						const unsigned int indexEnd,
						const unsigned int augmentingLength,
						const unsigned int remainderLength,
						const unsigned int method,
						const unsigned int fieldSize);

				//! Perform the actual scramble operation
				/*!
				 * This function allows you to set the remainder registers used during the
				 * scramble operation. In order to do block scrambling, this should be set
				 * to all zeros before each scrambling operation. If doing a single
				 * continuous scrambling operation, this can be ignored. If doing multiple
				 * competing continuous scrambling operations (as in guided scrambling),
				 * this should be used with the remainder() of the winning scrambler.
				 *
				 * Note that in order to improve efficiency of operation this function
				 * does not perform length/bounds checks on the input Word parameters. It
				 * is up to the calling function to ensure the parameters are correctly
				 * sized.
				 *
				 * @param	[in] divider Input dividing word. The size of this word
				 * 			\a must equal remainderLength (set in configure())+1.
				 * @param	[in] input Input to be scrambled. The size of this word
				 * 			\a must equal length-augmentingLength (both set in
				 * 			configure()).
				 * @param	[in] remainder Input remainder word. The size of this word
				 * 			\a must equal remainderLength (set in configure()). Note that
				 * 			this should be a \a copy of the winning remainder in the
				 * 			continuous case. If the winning remainder is used directly bad
				 * 			things will likely happen.
				 * @param	[in] feedback The feedback from the winning codeword to start
				 *				the analysis with.
				 * @param	[in] constellation This is a direct mapping of symbols (as
				 *				vector indices) to constellation points.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void scramble(
						const std::vector<Symbol>& divider,
						const std::vector<Symbol>& input,
						const std::vector<Symbol>& remainder,
						const Analyzer::Feedback& feedback,
						const std::vector<std::complex<float>>& constellation)
				{
					m_winner = &m_scramblers.front();
					for(auto& scrambler: m_scramblers)
					{
						scrambler.setRemainder(remainder);
						scrambler.scramble(divider, input);
						scrambler.analyze(feedback, constellation);
						if(scrambler.analysis() < winner()->analysis())
							m_winner = &scrambler;
					}
				}

				//! Arguments for handler()
				struct HandlerArguments
				{
					std::mutex countMutex; //!< Mutex for the count argument
					std::condition_variable countCV; //!< Condition Variable for the count argument
					unsigned int count; //!< Count for active scrambler group threads
					std::mutex sleepMutex; //!< Count for active scrambler group threads
					std::condition_variable sleepCV; //!< Mutex for the sleep argument
				};

				//! Constant arguments for handler()
				struct HandlerConstArguments
				{
					bool sleep; //!< Specifies whether or not the threads should sleep or terminate when it's done this round of scrambling. False would indicate terminate
					std::vector<Symbol> divider; //!< Input dividing word
					const std::vector<Symbol>* input; //!< Input to be scrambled
					std::vector<Symbol> remainder; //!< Input remainder word
					std::unique_ptr<const Analyzer::Feedback> feedback; //!< The feedback from the winning codeword to start the analysis with
					std::vector<std::complex<float>> constellation; //!< This is a direct mapping of symbols (as vector indices) to constellation points
				};

				//! Handler function for scrambler group thread
				/*!
				 * @param	[inout] args Collection of non-constant arguments
				 * @param	[in] cargs Collection of constant arguments
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void handler(HandlerArguments& args, const HandlerConstArguments& cargs);

				//! See winning scrambler
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * child ScramblerGroup::scramble() function.
				 *
				 * @return	Constant pointer to winning scrambler
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const Scrambler* winner() const
				{
					return m_winner;
				}


			private:
				std::vector<Scrambler> m_scramblers; //!< Vector of scramblers
				const Scrambler* m_winner; //!< Winning Scrambler
			};
		}
	}
}

#endif
