/*!
 * @file			SymbolGenerator_gr.cpp
 * @brief		Defines the GNU Radio Symbol Generator implementation
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Feb 18, 2015
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

#include "gr-isatec/SymbolGenerator.h"

#include <random>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for performing guided scrambling operations.
	namespace Isatec
	{
		//! Symbol generating implementation class for GNU Radio
		/*!
		 * @date		Feb 18, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class SymbolGenerator_impl: public SymbolGenerator
		{
		public:
			//! Access weightings
			/*!
			 * @return	Weighting weightings
			 * @date		Feb 18, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			const std::vector<double>& weightings() const;

			//! Set weightings
			/*!
			 * @param	[in] weightings Desired symbol weightings
			 * @date		Feb 18, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_weightings(const std::vector<double>& weightings);

			int work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items);

			SymbolGenerator_impl();

		private:
			std::vector<double> m_weightings;
			std::default_random_engine m_generator;
			std::discrete_distribution<Symbol> m_distribution;
		};
	}
}
