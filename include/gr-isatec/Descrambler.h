/*!
 * @file			Descrambler.hpp
 * @brief		Defines the GNU Radio Descrambler (Guided)
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

#ifndef DESCRAMBLER_H
#define DESCRAMBLER_H

#include <algorithm>
#include <functional>

#include "gr-isatec/config.h"
#include "gr-isatec/api.h"
#include <gnuradio/block.h>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Performs a single descramble operation.
		/*!
		 * @date		Jan 21, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class Isatec_API Descrambler: virtual public gr::block
		{
		public:
			//! Access field size
			/*!
			 * @return	Field Size
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int fieldSize() const =0;

			//! Set field size
			/*!
			 * @param	[in] size Desired field size
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_fieldSize(const unsigned int size) =0;

			//! Access codeword length
			/*!
			 * @return	Codeword Length
			 * @date		Jan 17, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int codewordLength() const =0;

			//! Set codeword length
			/*!
			 * @param	[in] length Desired codeword length
			 * @date		Jan 17, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_codewordLength(const unsigned int length) =0;

			//! Access augmenting length
			/*!
			 * @return	Augmenting Length
			 * @date		Jan 17, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int augmentingLength() const =0;

			//! Set augmenting length
			/*!
			 * @param	[in] length Desired augmenting length
			 * @date		Jan 17, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_augmentingLength(const unsigned int length) =0;

			//! Access continuous encoding setting
			/*!
			 * @return	True if set to continuous encoding, false if block encoding
			 * @date		Jan 17, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual bool continuous() const =0;

			//! Set continuous or block enconding
			/*!
			 * @param	[in] continuous Set to true for continuous encoding, false for
			 *				block encoding
			 * @date		Jan 17, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_continuous(bool continuous) =0;

			//! Access multiplier word
			/*!
			 * @return	Divisor Word
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual const std::vector<Symbol>& multiplier() const =0;

			//! Set multiplier polynomial
			/*!
			 * @param	[in] multiplier Desired multiplier polynomial
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_multiplier(const std::vector<Symbol>& multiplier) =0;

			typedef boost::shared_ptr<Descrambler> sptr;
			static sptr make();
		};
	}
}

#endif
