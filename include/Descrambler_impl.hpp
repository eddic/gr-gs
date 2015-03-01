/*!
 * @file			Descrambler_impl.hpp
 * @brief		Defines the gr::Isatec::GuideScrambling::Descrambler_impl class
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

#ifndef DESCRAMBLER_IMPL_HPP
#define DESCRAMBLER_IMPL_HPP

#include <functional>
#include <mutex>

#include "gr-isatec/config.h"
#include "gr-isatec/Descrambler.h"

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations.
		namespace GuidedScrambling
		{
			//! Performs a single descramble operation.
			/*!
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class Descrambler_impl: public Descrambler
			{
			public:
				//! Initialize the descrambler with some default options
				/*!
				 * This initializes the scrambler with the following parameters:
				 *  - codeword length = 12
				 *  - augmenting length = 3
				 *  - multiplier = 1 0 0 1
				 *  - continuous encoding = true
				 *  - field size = 4
				 *
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				Descrambler_impl();

				//~Descrambler_impl();

				//! Access field size
				/*!
				 * @return	Field Size
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int fieldSize() const;

				//! Set field size
				/*!
				 * @param	[in] size Desired field size
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_fieldSize(const unsigned int size);

				//! Access codeword length
				/*!
				 * @return	Codeword Length
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int codewordLength() const;

				//! Set codeword length
				/*!
				 * @param	[in] length Desired codeword length
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_codewordLength(const unsigned int length);

				//! Access augmenting length
				/*!
				 * @return	Augmenting Length
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				unsigned int augmentingLength() const;

				//! Set augmenting length
				/*!
				 * @param	[in] length Desired augmenting length
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_augmentingLength(const unsigned int length);

				//! Access continuous encoding setting
				/*!
				 * @return	True if set to continuous encoding, false if block encoding
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				bool continuous() const;

				//! Set continuous or block enconding
				/*!
				 * @param	[in] continuous Set to true for continuous encoding, false for
				 *				block encoding
				 * @date		Jan 17, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_continuous(bool continuous);

				//! Access multiplier word
				/*!
				 * @return	Divisor Word
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& multiplier() const;

				//! Set multiplier polynomial
				/*!
				 * @param	[in] multiplier Desired multiplier polynomial
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void set_multiplier(const std::vector<Symbol>& multiplier);

				//! Perform the actual descramble operation
				/*!
				 * Note that in order to improve efficiency of operation this function
				 * does not perform length/bounds checks on the input Word parameters. It
				 * is up to the calling function to ensure the parameters are correctly
				 * sized.
				 *
				 * @param	[in] input Input to be descrambled. The size of this word
				 * 			\a must equal the codeword length (set in codeword()).
				 * @return	Result of descramble operation
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void descramble(const std::vector<Symbol>& input);

				//! Accessor for the descramble operation output
				/*!
				 * This becomes valid upon completion of a call to the descramble()
				 * function.
				 *
				 * Use this to see the result of a call to descramble().
				 *
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol> output() const;

				//! Accessor for the actual product of multiplication
				/*!
				 * This becomes valid upon completion of a call to the descramble()
				 * function.
				 *
				 * Use of this accessor is primarily for debug and purposes as it allows
				 * us to see the actual product word (including the augmenting portion)
				 * that resulted from the last call to descramble().
				 *
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const std::vector<Symbol>& product() const;

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
				mutable std::mutex m_mutex; //!< Always practice safe threading

				std::vector<Symbol> m_codeword; //!< The input codeword
				std::vector<Symbol>::iterator m_codewordIt; //!< Input codeword position
				std::vector<Symbol> m_product; //!< The actual product of multiplication
				std::vector<Symbol>::const_iterator m_productIt; //!< Output sourceword position
				std::vector<Symbol> m_remainder;	//!< The remainder

				unsigned int m_codewordLength; //!< The codeword size
				unsigned int m_augmentingLength; //!< The length of the augmenting portion of the codeword
				std::vector<Symbol> m_multiplier;	//!< The multiplier used to descramble
				bool m_continuous; //!< True if we're doing continuous multiplication

				bool m_valid; //!< Indicates whether or not we need to initialize everything
				unsigned int m_fieldSize; //!< Field size to descramble with

				std::function<void(
					const std::vector<Symbol>&,
					const std::vector<Symbol>&,
					std::vector<Symbol>&,
					std::vector<Symbol>&,
					bool)> m_multiply; //!< The actual multiplying function to use
			};
		}
	}
}

#endif
