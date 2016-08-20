/*!
 * @file      SymbolGenerator.h
 * @brief     Declares the "Random Symbol Generator" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 19, 2016
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
         * @date   August 19, 2016
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
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

            //! Access the framing tag name
            virtual const std::string& framingTag() const =0;

            //! Set the framing tag name
            /*!
             * @param [in] tag Desired string to use for the "key" of the tag
             *                 inserted at frame starts.
             */
            virtual void set_framingTag(const std::string& tag) =0;

            //! Access the frame length in symbols
            virtual const unsigned int frameLength() const =0;

            //! Set the frame length in symbols
            /*!
             * This means a frame tag is generated every *length* symbols. Zero
             * means no tags are generated. If your feeding this into a guided
             * scrambler ensure the length is a multiple of the codeword length.
             *
             * @param [in] length Desired length of frame in symbols.
             */
            virtual void set_frameLength(const unsigned int length) =0;

            //! Shared pointer to this
            typedef boost::shared_ptr<SymbolGenerator> sptr;

            //! Manufacture a random symbol generator with a default weightings
            /*!
             * The default symbol generator is initialized to a state where the
             * "0" symbol is outputted with 100\% probability.
             *
             * @param [in] weightings See set_weightings()
             * @param [in] framingTag See set_framingTag()
             * @param [in] frameLength See set_framingLength()
             * @return Shared pointer to newly allocated random symbol generator
             */
            static sptr make(
                    const std::vector<double>& weightings = {1, 1, 1, 1},
                    const std::string& framingTag = "frame",
                    const unsigned int frameLength = 0);
        };
    }
}

#endif
