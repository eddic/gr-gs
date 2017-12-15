/*!
 * @file      Distribution.h
 * @brief     Declares the "Distribution" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 15, 2017
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

#ifndef GR_GS_DISTRIBUTION_H
#define GR_GS_DISTRIBUTION_H

#include "gr-gs/config.h"

#include <gnuradio/sync_decimator.h>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Distribution" GNU Radio block for real values
        /*!
         * This block takes in a sequence of real numbers and assigns then to
         * bins in a probability distribution. It basically creates the
         * necessary output vector to be used as a histogram.
         *
         * @date    June 12, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API Distribution_ff: virtual public gr::sync_decimator
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Distribution_ff> sptr;

            //! Manufacture an distribution block
            /*!
             * @param [in] bins Number of bins in the distribution
             * @param [in] binSize Width of each bin
             * @param [in] leftBinCenter The center point of the left most (most
             *                           negative) bin.
             * @param [in] output Should we output the average or just store it?
             * @param [in] decimation Should we decimate the output?
             * @return Shared pointer to newly allocated distribution
             */
            static sptr make(
                    const unsigned bins,
                    const double binSize,
                    const double leftBinCenter,
                    const bool output = false,
                    const unsigned decimation = 1);

            //! See the current distribution
            virtual std::vector<double> distribution() const =0;

            //! Reset the current distribution
            virtual void reset() =0;
        };

        //! "Distribution" GNU Radio block for complex values
        /*!
         * This block takes in a sequence of complex numbers and assigns then to
         * bins in a probability distribution. The 2-dimensional distribution
         * can be accessed with the distribution() function while the vector
         * that gets outputted portwise is a just the PMF of the real values.
         *
         * @date    December 15, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API Distribution_cf: virtual public gr::sync_decimator
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Distribution_cf> sptr;

            //! Manufacture an distribution block
            /*!
             * @param [in] bins Number of bins in the distribution
             * @param [in] binSize Width of each bin
             * @param [in] leastBinCenter The center point of the least (most
             *                            negative) bin.
             * @param [in] output Should we output the average or just store it?
             * @param [in] decimation Should we decimate the output?
             * @return Shared pointer to newly allocated distribution
             */
            static sptr make(
                    const unsigned bins,
                    const double binSize,
                    const std::complex<double> leastBinCenter,
                    const bool output = false,
                    const unsigned decimation = 1);

            //! See the current distribution
            virtual std::vector<std::vector<double>> distribution() const =0;

            //! Reset the current distribution
            virtual void reset() =0;
        };
    }
}

#endif
