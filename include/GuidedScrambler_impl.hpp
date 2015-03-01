/*!
 * @file			GuidedScrambler_impl.hpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::GuidedScrambler_impl class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			February 27, 2015
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

#ifndef GUIDEDSCRAMBLER_IMPL_HPP
#define GUIDEDSCRAMBLER_IMPL_HPP

#include <mutex>
#include <thread>

#include "gr-isatec/GuidedScrambler.h"
#include "ScramblerGroup.hpp"

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations.
		namespace GuidedScrambling
		{
			//! Performs guided scrambling operations.
			/*!
			 * This class allows us to perform guided scrambling in an efficient multi
			 * threaded fashion
			 *
			 * @date		February 27, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class GuidedScrambler_impl: public GuidedScrambler
			{
			public:
				//! No copying allowed
				GuidedScrambler_impl(const GuidedScrambler_impl& x) = delete;
				//! No copying allowed
				GuidedScrambler_impl& operator=(const GuidedScrambler_impl& x) = delete;

				//! Access field size
				/*!
				 * @return	Field Size
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int fieldSize() const;

				//! Set fieldSize
				/*!
				 * @param	[in] size Desired field size
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_fieldSize(const unsigned int size);

				//! Access codeword length
				/*!
				 * @return	Codeword Length
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int codewordLength() const;

				//! Set codeword length
				/*!
				 * @param	[in] length Desired codeword length
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_codewordLength(const unsigned int length);

				//! Access augmenting length
				/*!
				 * @return	Augmenting Length
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int augmentingLength() const;

				//! Set augmenting length
				/*!
				 * @param	[in] length Desired augmenting length
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_augmentingLength(const unsigned int length);

				//! Access continuous encoding setting
				/*!
				 * @return	True if set to continuous encoding, false if block encoding
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				bool continuous() const;

				//! Set continuous or block enconding
				/*!
				 * @param	[in] continuous Set to true for continuous encoding, false for
				 *				block encoding
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_continuous(bool continuous);

				//! Access constellation
				/*!
				 * @return	Constant reference to constellation vector
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<std::complex<float>>& constellation() const;

				//! Set constellation
				/*!
				 * @param	[in] constellation This is a direct mapping of symbols (as
				 *				vector indices) to constellation points.
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_constellation(const std::vector<std::complex<float>>& constellation);

				//! Access selection method
				/*!
				 * @return	Selection Method
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::string& selectionMethod() const;

				//! List selection methods
				/*!
				 * @return	Selection Methods
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				static const std::vector<std::string>& selectionMethods();

				//! Set selection method
				/*!
				 * @param	[in] method Desired selection method
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_selectionMethod(const std::string& method);

				//! Access divider word
				/*!
				 * @return	Divisor Word
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& divider() const;

				//! Set divider polynomial
				/*!
				 * @param	[in] divider Desired divider polynomial
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_divider(const std::vector<Symbol>& divider);

				//! Access number of concurrent scrambling threads
				/*!
				 * @return	Number of concurrent scrambling threads
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int threads() const;

				//! Set maximum number of concurrent scrambling threads
				/*!
				 * @param	[in] number Desired number of concurrent scrambling threads
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_threads(unsigned int number=0);

				//! Perform guided scrambling on an input source word
				/*!
				 * @param	[in] input Input source word.
				 * @return	Code word.
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
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
				 *  - divider = 1 0 0 1
				 *  - field size = 4
				 *
				 * @date		February 27, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
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
				std::vector<Symbol>::const_iterator m_codewordIt; //!< Iterator to codeword symbol
				std::vector<Symbol> m_sourceword; //!< Buffered source word
				std::vector<Symbol>::iterator m_sourcewordIt; //!< Iterator to sourceword symbol
			};
		}
	}
}

#endif
