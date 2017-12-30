/*!
 * @file      SymbolGenerator.h
 * @brief     Declares the "Random Symbol Generator" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 29, 2017
 * @copyright Copyright &copy; 2017 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2017 Eddie Carle
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

#ifndef GR_GS_SYMBOLGENERATOR_H
#define GR_GS_SYMBOLGENERATOR_H

#include "gr-gs/config.h"
#include <gnuradio/sync_block.h>

#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Random Symbol Generator" GNU Radio block
        /*!
         * This block simply allows us to generate a random stream of
         * uncorrelated symbols with discretely defined probability weightings.
         * All we do is pass a vector of doubles where the double describes the
         * weighting of the symbol as represented by the index of said vector.
         * Thus a weighting vector of {1,1,1,2} would produce symbol values
         * {0,1,2,3} with respective probabilities of {0.2, 0.2, 0.2, 0.4}. This
         * block also has the ability to add framing tags at set intervals to
         * aid in alignment.
         *
         * @tparam Symbol Base type to use for symbol type. Can be unsigned
         *                char, unsigned short, or unsigned int.
         * @date   December 29, 2017
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename Symbol>
        class GS_API SymbolGenerator: virtual public gr::sync_block
        {
        public:
            //! Access weightings
            /*!
             * See the gr::Isatec::SymbolGenerator class description for more
             * information on what the vector means.
             *
             * @return Weighting weightings
             */
            virtual const std::vector<double>& weightings() const =0;

            //! Set weightings
            /*!
             * See the gr::Isatec::SymbolGenerator class description for more
             * information on what the vector means.
             *
             * @param  [in] weightings Desired symbol weightings
             */
            virtual void set_weightings(
                    const std::vector<double>& weightings) =0;

            //! Shared pointer to this
            typedef boost::shared_ptr<SymbolGenerator> sptr;

            //! Manufacture a random symbol generator with a default weightings
            /*!
             * The default symbol generator is initialized to a state where the
             * "0" symbol is outputted with 100\% probability.
             *
             * @param [in] weightings See set_weightings()
             * @param [in] alignmentTag Desired string to use for the "key" of
             *                          the tag present at the alignment point.
             *                          Use an empty string to disable
             *                          alignment.
             * @return Shared pointer to newly allocated random symbol generator
             */
            static sptr make(
                    const std::vector<double>& weightings = {1, 1, 1, 1},
                    const std::string& alignmentTag = "start");
        };

        typedef SymbolGenerator<unsigned char> SymbolGenerator_b;
        typedef SymbolGenerator<unsigned short> SymbolGenerator_s;
        typedef SymbolGenerator<unsigned int> SymbolGenerator_i;
    }
}

#endif
