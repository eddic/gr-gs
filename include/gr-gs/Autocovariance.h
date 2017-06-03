/*!
 * @file      Autocovariance.h
 * @brief     Declares the "Autocovariance" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      June 2, 2017
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

#ifndef GR_GS_AUTOCOVARIANCE_H
#define GR_GS_AUTOCOVARIANCE_H

#include "gr-gs/config.h"

#include <gnuradio/sync_decimator.h>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Autocovariance" GNU Radio block
        /*!
         * This block computes the autocovariance of a signal. It outputs a
         * vector of the length requested for every sample it receives. For the
         * floating point case, there is a single output port that gives you the
         * product of the current sample with past samples.
         *
         * The complex case is similar but with four output port corresponding
         * to the covariance matrix.
         * * output[0] = real(x(n))*real(x(n-k))
         * * output[1] = real(x(n))*imag(x(n-k))
         * * output[2] = imag(x(n))*real(x(n-k))
         * * output[3] = imag(x(n))*imag(x(n-k))
         *
         * @tparam T Base signal type. Can be float or std::complex<float>.
         * @date    June 2, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename T>
        class GS_API Autocovariance: virtual public gr::sync_decimator
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Autocovariance> sptr;

            //! Get the set mean value of the signal
            /*!
             * This does not calculate the signal mean. It simply returns the
             * mean you already supplied.
             */
            virtual T mean() const =0;

            //! Set the mean value of the signal
            /*!
             * This mean value is needed to accurately compute the
             * autocovariance.
             */
            virtual void set_mean(T mean) =0;

            //! Manufacture an integrate block
            /*!
             * @param [in] length This decides how far back we look to calculate
             *                    the autocovariance. This, of course, will
             *                    equal the lengths of the vectors being spit
             *                    out of this thing.
             * @param [in] mean The mean value of the signal.
             * @param [in] decimation Should we decimate the output?
             * @param [in] offset This should be a value between 0 and 
             *                    decimation. 0 means we're taking the
             *                    autocorrelation just for the first sample in
             *                    the chunk. You can use this to calculate
             *                    autocorrelations in cyclostationary signals.
             * @return Shared pointer to newly allocated integrate block
             */
            static sptr make(
                    unsigned length,
                    T mean = 0,
                    const unsigned decimation = 1,
                    const unsigned offset = 0);
        };

        typedef Autocovariance<float> Autocovariance_ff;
        typedef Autocovariance<std::complex<float>> Autocovariance_cf;
    }
}

#endif
