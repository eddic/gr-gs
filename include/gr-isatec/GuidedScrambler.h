/*!
 * @file			GuidedScrambler.h
 * @brief		Defines the gr::Isatec::Descrambler GNU Radio block
 * @author		Eddie Carle &lt;eddie@Isatec.ca&gt;
 * @date			March 8, 2015
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

#ifndef GUIDEDSCRAMBLER_H
#define GUIDEDSCRAMBLER_H

#include "gr-isatec/api.h"
#include "gr-isatec/config.h"
#include <gnuradio/block.h>

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! "Guided Scrambler" GNU Radio block
		/*!
		 * @date		March 3, 2015
		 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
		 */
		class Isatec_API GuidedScrambler: virtual public gr::block
		{
		public:
			//! Access field size
			/*!
			 * @return	Field size as the <em>n</em> in GF(<em>n</em>)
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int fieldSize() const =0;

			//! Set fieldSize
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
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int codewordLength() const =0;

			//! Set codeword length
			/*!
			 * @param	[in] length Desired codeword length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_codewordLength(const unsigned int length) =0;

			//! Access augmenting length
			/*!
			 * @return	Augmenting Length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int augmentingLength() const =0;

			//! Set augmenting length
			/*!
			 * @param	[in] length Desired augmenting length
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_augmentingLength(const unsigned int length) =0;

			//! Access continuous encoding setting
			/*!
			 * @return	True if set to continuous encoding, false if block encoding
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual bool continuous() const =0;

			//! Set continuous or block enconding
			/*!
			 * @param	[in] continuous Set to true for continuous encoding, false for
			 *				block encoding
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_continuous(bool continuous) =0;

			//! Access constellation
			/*!
			 * @return	Constant reference to constellation vector
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual const std::vector<std::complex<float>>& constellation() const =0;

			//! Set constellation
			/*!
			 * @param	[in] constellation This is a direct mapping of symbols (as
			 *				vector indices) to constellation points.
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_constellation(const std::vector<std::complex<float>>& constellation) =0;

			//! Access selection method
			/*!
			 * @return	Selection Method
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual const std::string& selectionMethod() const =0;

			//! List selection methods
			/*!
			 * @return	Vector of selection method names
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			static const std::vector<std::string>& selectionMethods();

			//! Set selection method
			/*!
			 * @param	[in] method Desired selection method
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_selectionMethod(const std::string& method) =0;

			//! Access divider word
			/*!
			 * @return	Divider word (polynomial)
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual const std::vector<Symbol>& divider() const =0;

			//! Set divider word
			/*!
			 * @param	[in] divider Desired divider word (polynomial)
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_divider(const std::vector<Symbol>& divider) =0;

			//! Access number of concurrent scrambling threads
			/*!
			 * @return	Number of concurrent threads running in this block
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int threads() const =0;

			//! Set number of concurrent scrambling threads
			/*!
			 * Use this function to control how many "child" threads are used for
			 * the guided scrambling process. There can not be more threads than
			 * there are concurrent scrambling operations to be executed. This
			 * means that there is an upper bound to this regardless of the value
			 * provided. If you'd like the block to decide an optimal number of
			 * threads pass the value 0.
			 *
			 * @param	[in] number Desired number of concurrent scrambling threads
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_threads(unsigned int number) =0;

			//! Shared pointer to this
			typedef boost::shared_ptr<GuidedScrambler> sptr;

			//! Manufacture a guided scrambler with some default options
			/*!
			 * This initializes the guided scrambler with the following parameters:
			 *  - codeword length = 12
			 *  - augmenting length = 3
			 *  - selection method = MSW
			 *  - continuous encoding = true
			 *  - maximum threads = optimal
			 *  - divider = 1 0 0 1 (\f$x^3+1\f$)
			 *  - field size = 4
			 *  - constellation = (1,0) (0,1) (0,-1) (-1,0)
			 *
			 * @return	Shared pointer to newly allocated guided scrambler
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			static sptr make();
		};
	}
}

#endif
