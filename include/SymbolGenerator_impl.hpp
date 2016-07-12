/*!
 * @file      SymbolGenerator_impl.hpp
 * @brief     Declares the "Random Symbol Generator" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      July 8, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2016 Eddie Carle
 *
 * This file is part of the Guided Scrambling GNU Radio Module
 *
 * The Guided Scrambling GNU Radio Module is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The Guided Scrambling GNU Radio Module is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * The Guided Scrambling GNU Radio Module.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "gr-gs/SymbolGenerator.h"

#include <random>
#include <mutex>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            //! "Random Symbol Generator" GNU Radio block implementation
            /*!
             * Implements gr::gs::SymbolGenerator
             *
             * @date   March 8, 2015
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class SymbolGenerator_impl: public SymbolGenerator
            {
            public:
                const std::vector<double> weightings() const;
                void set_weightings(const std::vector<double>& weightings);
                unsigned int count();

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize with default symbol weightings
                /*!
                 * This symbol generator is initialized to a state where the
                 * "0" symbol is outputted with 100\% probability.
                 *
                 * @date   March 5, 2015
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                SymbolGenerator_impl();

            private:
                //! Always practice safe threading
                mutable std::mutex m_mutex;

                //! How many symbols have we generated?
                unsigned int m_count;

                //! Our symbol weightings
                std::vector<double> m_weightings;

                //! Our random number generator
                std::default_random_engine m_generator;

                //! Our distribution
                std::discrete_distribution<Symbol> m_distribution;
            };
        }
    }
}
