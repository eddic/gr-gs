/*!
 * @file      Terminator.h
 * @brief     Declares the "Terminator" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 20, 2017
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

#ifndef GR_GS_TERMINATOR_H
#define GR_GS_TERMINATOR_H

#include "gr-gs/config.h"

#include <gnuradio/sync_block.h>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Terminator" GNU Radio block
        /*!
         * This block simply acts as a null sink until the desired sample count
         * is reached. At this point an EOF is propagated into the flowgraph.
         *
         * @date    May 20, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API Terminator: virtual public gr::sync_block
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Terminator> sptr;

            //! Manufacture a terminator block
            /*!
             * @param [in] sampleSize Number of bytes per sample (vector
             *                        included).
             * @param [in] sampleCount Number of samples until we terminate
             * @return Shared pointer to newly allocated terminator
             */
            static sptr make(
                    const unsigned sampleSize,
                    const unsigned long long sampleCount);

            //! How many samples have we sunk?
            virtual unsigned long long samples() const =0;

            //! Check if we have reach our end point
            virtual bool finished() const =0;

            //! Reset so we can run it again
            virtual void reset() =0;
        };
    }
}

#endif
