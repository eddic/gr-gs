/*!
 * @file      Detector.h
 * @brief     Declares the "Guided Scrambling Detector" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 31, 2018
 * @copyright Copyright &copy; 2018 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2018 Eddie Carle
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

#ifndef GR_GS_DETECTOR_H
#define GR_GS_DETECTOR_H

#include "gr-gs/config.h"
#include <gnuradio/block.h>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Guided Scrambling Detector" GNU Radio block
        /*!
         * This block complex samples and makes hard decisions from both
         * euclidean distances to constellation points and the statistical
         * properties of the guided scrambling mechanism being used.
         *
         * See the members functions for further information on the parameters
         * and their meaning.
         *
         * @tparam Symbol Base type to use for symbol type. Can be unsigned
         *                char, unsigned short, or unsigned int.
         * @date   May 31, 2018
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename Symbol>
        class GS_API Detector: virtual public gr::block
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Detector> sptr;

            //! Manufacture a detector
            /*!
             * @param [in] fieldSize Field size of our symbols. Our default
             *                       constellation pattern is retrieved from
             *                       this.
             * @param [in] codewordLength Length of our codewords.
             * @param [in] augmentingLength How many augmenting symbols in the
             *                              codeword?
             * @param [in] noise This noise power level (or variance) is
             *                   required to perform accurate MAP detection.
             * @param [in] alignmentTag Desired string to use for the "key" of
             *                          the tag inserted at alignment. Use an
             *                          empty string to disable alignment.
             * @param [in] nodeDiscardMetric This defines how aggressively we
             *                               will discard trellis nodes. This
             *                               must be above zero. Larger numbers
             *                               mean more accurate detection but
             *                               make detection more computationally
             *                               intensive.
             * @return Shared pointer to newly allocated detector block
             */
            static sptr make(
                    const unsigned fieldSize,
                    const unsigned codewordLength,
                    const unsigned augmentingLength,
                    const double noise,
                    const std::string& alignmentTag = "",
                    const double nodeDiscardMetric = 10);


            //! Access noise power
            /*!
             * @return Noise power or variance
             */
            virtual double noisePower() const =0;

            //! Set noise power
            /*!
             * This noise power level (or variance) is required to perform
             * accurate MAP detection of the Guided Scrambled signal.
             *
             * @param  [in] power Current noise power level (variance)
             */
            virtual void set_noisePower(const double noise) =0;
        };

        typedef Detector<unsigned char> Detector_cb;
        typedef Detector<unsigned short> Detector_cs;
        typedef Detector<unsigned int> Detector_ci;
    }
}

#endif
