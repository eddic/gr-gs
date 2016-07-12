/*!
 * @file       SymbolGenerator.h
 * @brief      Declares the "Random Symbol Generator" GNU Radio block
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 8, 2015
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

#ifndef SYMBOLGENERATOR_H
#define SYMBOLGENERATOR_H

#include "gr-isatec/config.h"
#include <gnuradio/sync_block.h>

#include <vector>

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! "Random Symbol Generator" GNU Radio block
		/*!
		 * This block simply allows us to generate a random stream of uncorrelated
		 * symbols with discretely defined probability weightings. All we do is
		 * pass a vector of doubles where the double describes the weighting of
		 * the symbol as represented by the index of said vector. Thus a weighting
		 * vector of {1,1,1,2} would produce symbol values {0,1,2,3} with
		 * respective probabilities of {0.2, 0.2, 0.2, 0.4}.
		 *
		 * @date    March 8, 2015
		 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class Isatec_API SymbolGenerator: virtual public gr::sync_block
		{
		public:
			//! Access weightings
			/*!
			 * See the gr::Isatec::SymbolGenerator class description for more
			 * information on what the vector means.
			 *
			 * @return  Weighting weightings
			 * @date    March 5, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual const std::vector<double>& weightings() const =0;

			//! Set weightings
			/*!
			 * See the gr::Isatec::SymbolGenerator class description for more
			 * information on what the vector means.
			 *
			 * @param   [in] weightings Desired symbol weightings
			 * @date    Feb 18, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_weightings(const std::vector<double>& weightings) =0;

			//! Get symbol count
			/*!
			 * This function returns the total amount of symbols that have been
			 * generated since the last call to this method. This means that every
			 * time you call this function it resets the counter to zero.
			 *
			 * @return  Total amount of symbols since last call
			 * @date    March 8, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int count() =0;

			//! Shared pointer to this
			typedef boost::shared_ptr<SymbolGenerator> sptr;

			//! Manufacture a random symbol generator with a default weightings
			/*!
			 * This symbol generator is initialized to a state where the "0" symbol
			 * is outputted with 100\% probability.
			 *
			 * @return  Shared pointer to newly allocated random symbol generator
			 * @date    March 5, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			static sptr make();
		};
	}
}

#endif
