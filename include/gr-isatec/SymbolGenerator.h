/*!
 * @file			SymbolGenerator.h
 * @brief		Declares the GNU Radio Symbol Generator
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			February 18, 2015
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

#ifndef SYMBOLGENERATOR_H
#define SYMBOLGENERATOR_H

#include "gr-isatec/api.h"
#include "gr-isatec/config.h"
#include <gnuradio/sync_block.h>

#include <vector>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Generate a random symbol sequence
		/*!
		 * @date		February 18, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class Isatec_API SymbolGenerator: virtual public gr::sync_block
		{
		public:
			//! Access weightings
			/*!
			 * @return	Weighting weightings
			 * @date		Feb 18, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual const std::vector<double>& weightings() const =0;

			//! Set weightings
			/*!
			 * @param	[in] weightings Desired symbol weightings
			 * @date		Feb 18, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_weightings(const std::vector<double>& weightings) =0;

			typedef boost::shared_ptr<SymbolGenerator> sptr;
			static sptr make();
		};
	}
}

#endif
