/*!
 * @file			GuidedScrambler.h
 * @brief		Declares the GNU Radio Guided Scrambler
 * @author		Eddie Carle &lt;eddie@Isatec.ca&gt;
 * @date			February 10, 2015
 * @copyright	Copyright &copy; 2014 Eddie Carle &lt;eddie@Isatec.ca&gt;.
 * 				This project is released under the GNU General Public License
 * 				Version 3.
 */

/* Copyright (C) 2015 Eddie Carle <eddie@Isatec.ca>
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

#ifndef GUIDEDSCRAMBLER_H
#define GUIDEDSCRAMBLER_H

#include "gr-isatec/api.h"
#include "gr-isatec/config.h"
#include <gnuradio/block.h>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Performs guided scrambling operations in GNU radio applications.
		/*!
		 * @date		February 10, 2015
		 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
		 */
		class Isatec_API GuidedScrambler: virtual public gr::block
		{
		public:
			//! Access field size
			/*!
			 * @return	Field Size
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int fieldSize() const =0;

			//! Set fieldSize
			/*!
			 * @param	[in] size Desired field size
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_fieldSize(const unsigned int size) =0;

			//! Access codeword length
			/*!
			 * @return	Codeword Length
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int codewordLength() const =0;

			//! Set codeword length
			/*!
			 * @param	[in] length Desired codeword length
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_codewordLength(const unsigned int length) =0;

			//! Access augmenting length
			/*!
			 * @return	Augmenting Length
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int augmentingLength() const =0;

			//! Set augmenting length
			/*!
			 * @param	[in] length Desired augmenting length
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_augmentingLength(const unsigned int length) =0;

			//! Access continuous encoding setting
			/*!
			 * @return	True if set to continuous encoding, false if block encoding
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual bool continuous() const =0;

			//! Set continuous or block enconding
			/*!
			 * @param	[in] continuous Set to true for continuous encoding, false for
			 *				block encoding
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_continuous(bool continuous) =0;

			//! Access constellation
			/*!
			 * @return	Constant reference to constellation vector
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual const std::vector<std::complex<float>>& constellation() const =0;

			//! Set constellation
			/*!
			 * @param	[in] constellation This is a direct mapping of symbols (as
			 *				vector indices) to constellation points.
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_constellation(const std::vector<std::complex<float>>& constellation) =0;

			//! Access selection method
			/*!
			 * @return	Selection Method
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual const std::string& selectionMethod() const =0;

			//! List selection methods
			/*!
			 * @return	Selection Method
			 * @date		Feb 20, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			static const std::vector<std::string>& selectionMethods();

			//! Set selection method
			/*!
			 * @param	[in] method Desired selection method
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_selectionMethod(const std::string& method) =0;

			//! Access divider word
			/*!
			 * @return	Divisor Word
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual const std::vector<Symbol>& divider() const =0;

			//! Set divider polynomial
			/*!
			 * @param	[in] divider Desired divider polynomial
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_divider(const std::vector<Symbol>& divider) =0;

			//! Access number of concurrent scrambling threads
			/*!
			 * @return	Number of concurrent scrambling threads
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual unsigned int threads() const =0;

			//! Set maximum number of concurrent scrambling threads
			/*!
			 * @param	[in] number Desired number of concurrent scrambling threads
			 * @date		Jan 29, 2015
			 * @author	Eddie Carle &lt;eddie@Isatec.ca&gt;
			 */
			virtual void set_threads(unsigned int number) =0;

			typedef boost::shared_ptr<GuidedScrambler> sptr;
			static sptr make();
		};
	}
}

#endif
