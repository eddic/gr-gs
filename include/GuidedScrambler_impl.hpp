/*!
 * @file       GuidedScrambler_impl.hpp
 * @brief      Declares the "Guided Scrambler" GNU Radio block implementation
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

#ifndef GUIDEDSCRAMBLER_IMPL_HPP
#define GUIDEDSCRAMBLER_IMPL_HPP

#include <mutex>
#include <thread>

#include "gr-isatec/GuidedScrambler.h"
#include "ScramblerGroup.hpp"

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains elements needed for the GuidedScrambler and Descrambler blocks
		namespace GuidedScrambling
		{
			//! "Guided Scrambler" GNU Radio block implementation
			/*!
			 * This class allows us to perform guided scrambling in an efficient multi
			 * threaded fashion
			 *
			 * @date    March 3, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class GuidedScrambler_impl: public GuidedScrambler
			{
			public:
				//! No copying allowed
				GuidedScrambler_impl(const GuidedScrambler_impl& x) = delete;
				//! No copying allowed
				GuidedScrambler_impl& operator=(const GuidedScrambler_impl& x) = delete;

				unsigned int fieldSize() const;
				void set_fieldSize(const unsigned int size);

				unsigned int codewordLength() const;
				void set_codewordLength(const unsigned int length);

				unsigned int augmentingLength() const;
				void set_augmentingLength(const unsigned int length);

				bool continuous() const;
				void set_continuous(bool continuous);

				const std::vector<std::complex<float>>& constellation() const;
				void set_constellation(const std::vector<std::complex<float>>& constellation);

				const std::string& selectionMethod() const;
				void set_selectionMethod(const std::string& method);

				const std::vector<Symbol>& divider() const;
				void set_divider(const std::vector<Symbol>& divider);

				unsigned int threads() const;
				void set_threads(unsigned int number=0);

				//! Perform guided scrambling on an input source word
				/*!
				 * Note that in order to improve efficiency of operation this
				 * function does not perform length/bounds checks on the input
				 * source word.  It is up to the calling function to ensure the
				 * parameters are correctly sized.
				 *
				 * @param   [in] input Input source word. The length of this source
				 *                     word \a must equal the codeword length minus
				 *                     the augmenting length.
				 * @return  Constant reference code word.
				 * @date    March 8, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& scramble(const std::vector<Symbol>& input);

				//! Initialize the guided scrambler with some default options
				/*!
				 * This initializes the scrambler with the following parameters:
				 *  - codeword length = 12
				 *  - augmenting length = 3
				 *  - selection method = MSW
				 *  - continuous encoding = true
				 *  - maximum threads = hardware maximum
				 *  - divider = 1 0 0 1 (\f$x^3+1\f$)
				 *  - field size = 4
				 *  - constellation = (1,0) (0,1) (0,-1) (-1,0)
				 *
				 * @date    March 5, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				GuidedScrambler_impl();

				//! Simply kills the active threads.
				~GuidedScrambler_impl();

				//! GNU Radio work function
				int general_work(int noutput_items,
						gr_vector_int &ninput_items,
						gr_vector_const_void_star &input_items,
						gr_vector_void_star &output_items);

				//! GNU Radio forecast function
				void forecast(
						int noutput_items,
						gr_vector_int& ninput_items_required);

			private:
				//! Kills all active threads
				void killThreads();

				mutable std::mutex m_mutex; //!< Let's be thread safe
				unsigned int m_codewordLength; //!< The codeword size
				unsigned int m_augmentingLength; //!< The augmenting portion length
				unsigned int m_selectionMethod; //!< The selection method used for analysis
				unsigned int m_groups; //!< Number of concurrent threads/scrambling groups
				unsigned int m_fieldSize; //!< Field size to scramble with
				std::vector<std::thread> m_threads; //!< Vector of active threads
				std::vector<ScramblerGroup> m_scramblerGroups; //!< Vector of active scrambler groups
				bool m_continuous; //!< True if we're doing continuous encoding

				ScramblerGroup::HandlerArguments m_args; //!< Arguments for ScramblerGroup::handler()
				ScramblerGroup::HandlerConstArguments m_cargs; //!< Constant arguments for ScramblerGroup::handler()

				const std::vector<Symbol>* m_codeword; //!< Pointer to current codeword
				std::vector<Symbol>::const_iterator m_codewordIt; //!< Iterator to codeword read position
				std::vector<Symbol> m_sourceword; //!< Buffered source word
				std::vector<Symbol>::iterator m_sourcewordIt; //!< Iterator to sourceword write position
			};
		}
	}
}

#endif
