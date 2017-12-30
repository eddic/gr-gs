/*!
 * @file      Entropy.h
 * @brief     Declares the "Entropy" GNU Radio block
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

#ifndef GR_GS_ENTROPY_H
#define GR_GS_ENTROPY_H

#include "gr-gs/config.h"
#include <gnuradio/sync_block.h>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Guided Scrambling "Entropy" GNU Radio block
        /*!
         * This class takes a sequence of symbols and maps it to a sequence of
         * probabilities associated with the symbols.
         *
         * @tparam Symbol Base type to use for symbol type. Can be unsigned
         *                char, unsigned short, or unsigned int.
         * @date   December 29, 2017
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename Symbol>
        class GS_API Entropy: virtual public gr::sync_block
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Entropy> sptr;

            //! Manufacture a entropy with some default options
            /*!
             * @param [in] fieldSize Desired field size as the *n* in GF(*n*)
             * @param [in] codewordLength Desired codeword length
             * @param [in] augmentingLength Desired augmenting Length
             * @param [in] minCorrelation This decides how many taps we're going
             *                            to need to calculate our means. Any
             *                            autocorrelation data decays below this
             *                            value will be truncated from our
             *                            computations.
             * @param [in] alignmentTag Desired string to use for the "key" of
             *                          the tag present at the alignment point.
             *                          Use an empty string to disable
             *                          alignment.
             * @return Shared pointer to newly allocated entropy block
             */
            static sptr make(
                    const unsigned int fieldSize,
                    const unsigned int codewordLength,
                    const unsigned int augmentingLength,
                    const double minCorrelation = 0.01,
                    const std::string& alignmentTag = "");
        };

        typedef Entropy<unsigned char> Entropy_bf;
        typedef Entropy<unsigned short> Entropy_sf;
        typedef Entropy<unsigned int> Entropy_if;
    }
}

#endif
