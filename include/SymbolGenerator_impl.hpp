/*!
 * @file			SymbolGenerator_impl.hpp
 * @brief		Declares the "Random Symbol Generator" GNU Radio block implementation
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 5, 2015
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

#include "gr-isatec/SymbolGenerator.h"

#include <random>

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! "Random Symbol Generator" GNU Radio block implementation
		/*!
		 * Implements gr::Isatec::SymbolGenerator
		 *
		 * @date		March 5, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class SymbolGenerator_impl: public SymbolGenerator
		{
		public:
			const std::vector<double>& weightings() const;
			void set_weightings(const std::vector<double>& weightings);

			//! GNU Radio work function
			int work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items);

			//! Initialize a random symbol generator with a default weightings
			/*!
			 * This symbol generator is initialized to a state where the "0" symbol
			 * is outputted with 100\% probability.
			 *
			 * @return	Shared pointer to newly allocated random symbol generator
			 * @date		March 5, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			SymbolGenerator_impl();

		private:
			std::vector<double> m_weightings; //!< Our symbol weightings
			std::default_random_engine m_generator; //!< Our random number generator
			std::discrete_distribution<Symbol> m_distribution; //!< Our distribution
		};
	}
}
