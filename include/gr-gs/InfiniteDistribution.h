/*!
 * @file      InfiniteDistribution.h
 * @brief     Declares the "Infinite Distribution" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 16, 2017
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

#ifndef GR_GS_INFINITEDISTRIBUTION_H
#define GR_GS_INFINITEDISTRIBUTION_H

#include "gr-gs/config.h"

#include <gnuradio/sync_decimator.h>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Infinite Distribution" GNU Radio block
        /*!
         * This block takes in a sequence of real numbers and assigns then to
         * bins in a probability distribution. It basically creates the
         * necessary output vector to be used as a histogram.
         *
         * @date    May 16, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API InfiniteDistribution: virtual public gr::sync_decimator
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<InfiniteDistribution> sptr;

            //! Manufacture an infinite average block
            /*!
             * @param [in] bins Number of bins in the distribution
             * @param [in] binSize Width of each bin
             * @param [in] leftBinCenter The center point of the left most (most
             *                           negative) bin.
             * @param [in] decimation Should we decimate the output?
             * @return Shared pointer to newly allocated infinite distribution
             */
            static sptr make(
                    const unsigned bins,
                    const double binSize,
                    const double leftBinCenter,
                    const unsigned decimation = 1);

            //! See the current distribution
            virtual const std::vector<float>& distribution() const =0;

            //! Reset the current average
            virtual void reset() =0;
        };
    }
}

#endif
