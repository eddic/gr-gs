/*!
 * @file      Autocovariance_impl.hpp
 * @brief     Declares the "Autocovariance" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      January 6, 2018
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

#ifndef GR_GS_AUTOCOVARIANCE_IMPL_HPP
#define GR_GS_AUTOCOVARIANCE_IMPL_HPP

#include "gr-gs/Autocovariance.h"

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
            //! "Autocovariance" GNU Radio block implementation
            /*!
             * Implements gr::gs::Autocovariance
             *
             * @tparam T Base signal type. Can be float or std::complex<float>.
             * @date    January 6, 2018
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename T>
            class Autocovariance_impl: public Autocovariance<T>
            {
            public:
                //! No copying allowed
                Autocovariance_impl(const Autocovariance_impl& x)
                    = delete;
                //! No copying allowed
                Autocovariance_impl& operator=(
                        const Autocovariance_impl& x) = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an autocovariance block
                /*!
                 * @param [in] length This decides how far back we look to
                 *                    calculate the autocovariance. This, of
                 *                    course, will equal the lengths of the
                 *                    vectors being spit out of this thing.
                 * @param [in] mean The mean value of the signal.
                 * @param [in] decimation Should we decimate the output?
                 * @param [in] offset This should be a value between 0 and
                 *                    decimation. 0 means we're taking the
                 *                    autocorrelation just for the first sample
                 *                    in the chunk. You can use this to
                 *                    calculate autocorrelations in
                 *                    cyclostationary signals.
                 */
                inline Autocovariance_impl(
                        unsigned length,
                        T mean,
                        const unsigned decimation,
                        const unsigned offset);

            private:
                static inline T conj(T x);

                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! The signal mean
                const T m_mean;

                //! Vector length
                const unsigned m_length;

                //! Offset within decimation chunk
                const unsigned m_offset;
            };
        }
    }
}

#endif
