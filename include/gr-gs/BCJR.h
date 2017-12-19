/*!
 * @file      BCJR.h
 * @brief     Declares the "Guided Scrambling BCJR" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 16, 2017
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

#ifndef GR_GS_BCJR_H
#define GR_GS_BCJR_H

#include "gr-gs/config.h"
#include <gnuradio/block.h>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Guided Scrambling BCJR" GNU Radio block
        /*!
         * This block is practically useless save to gauge the performance of
         * the actual Detector block.
         *
         * See the members functions for further information on the parameters
         * and their meaning.
         *
         * @tparam Symbol Base type to use for symbol type. Can be unsigned
         *                char, unsigned short, or unsigned int.
         * @date   December 16, 2017
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename Symbol>
        class GS_API BCJR: virtual public gr::block
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<BCJR> sptr;

            //! Manufacture a BCJR detector with some default options
            /*!
             *
             * @param [in] fieldSize Field size of our symbols. Our default
             *                       constellation pattern is retrieved from
             *                       this.
             * @param [in] codewordLength Length of our codewords.
             * @param [in] augmentingLength How many augmenting symbols in the
             *                              codeword?
             * @param [in] minCorrelation This decides how many taps we're
             *                            going to need to calculate our means.
             *                            Any autocorrelation data decays below
             *                            this value will be truncated from our
             *                            computations.
             * @param [in] noise This noise power level (or variance) is
             *                   required to perform accurate MAP detection.
             * @param [in] windowSize Sequence length to use for detection
             * @param [in] maxErrors Maximum number of errors to log before EOF.
             * @param [in] maxSymbols Maximum number of symbols to log before
             *                        EOF.
             * @return Shared pointer to newly allocated detector block
             */
            static sptr make(
                    const unsigned fieldSize,
                    const unsigned codewordLength,
                    const unsigned augmentingLength,
                    const double minCorrelation,
                    const double noise);

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

        typedef BCJR<unsigned char> BCJR_cb;
        typedef BCJR<unsigned short> BCJR_cs;
        typedef BCJR<unsigned int> BCJR_ci;
    }
}

#endif
