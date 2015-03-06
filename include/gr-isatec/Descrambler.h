/*!
 * @file			Descrambler.h
 * @brief		Declares the Guided Scrambling "Descrambler" GNU Radio block
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

#ifndef DESCRAMBLER_H
#define DESCRAMBLER_H

#include <algorithm>
#include <functional>

#include "gr-isatec/config.h"
#include "gr-isatec/api.h"
#include <gnuradio/block.h>

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Guided Scrambling "Descrambler" GNU Radio block
		/*!
		 * @date		March 3, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class Isatec_API Descrambler: virtual public gr::block
		{
		public:
			//! Access field size
			/*!
			 * @return	Field size as the <em>n</em> in GF(<em>n</em>)
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int fieldSize() const =0;

			//! Set field size
			/*!
			 * Note that not all field sizes are supported. Only powers of two are
			 * supported within the range of 2 to
			 * gr::Isatec::GuidedScrambling::maxFieldSize.
			 *
			 * @param	[in] size Desired field size as the <em>n</em> in
			 * 						 GF(<em>n</em>)
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_fieldSize(const unsigned int size) =0;

			//! Access codeword length
			/*!
			 * @return	Codeword Length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int codewordLength() const =0;

			//! Set codeword length
			/*!
			 * @param	[in] length Desired codeword length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_codewordLength(const unsigned int length) =0;

			//! Access augmenting length
			/*!
			 * @return	Augmenting Length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int augmentingLength() const =0;

			//! Set augmenting length
			/*!
			 * @param	[in] length Desired augmenting length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_augmentingLength(const unsigned int length) =0;

			//! Access continuous encoding setting
			/*!
			 * @return	True if set to continuous encoding, false if block encoding
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual bool continuous() const =0;

			//! Set continuous or block enconding
			/*!
			 * @param	[in] continuous Set to true for continuous encoding, false for
			 *				block encoding
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_continuous(bool continuous) =0;

			//! Access multiplier word
			/*!
			 * @return	Multiplier word (polynomial)
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual const std::vector<Symbol>& multiplier() const =0;

			//! Set multiplier polynomial
			/*!
			 * @param	[in] multiplier Desired multiplier word (polynomial)
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_multiplier(const std::vector<Symbol>& multiplier) =0;

			//! Shared pointer to this
			typedef boost::shared_ptr<Descrambler> sptr;

			//! Manufacture a descrambler with some default options
			/*!
			 * This initializes the descrambler with the following parameters:
			 *  - codeword length = 12
			 *  - augmenting length = 3
			 *  - multiplier = 1 0 0 1 (\f$x^3+1\f$)
			 *  - continuous encoding = true
			 *  - field size = 4
			 *
			 * @return	Shared pointer to newly allocated descrambler
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			static sptr make();
		};
	}
}

#endif
