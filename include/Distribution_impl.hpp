/*!
 * @file      Distribution_impl.hpp
 * @brief     Declares the "Distribution" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 18, 2017
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

#ifndef GR_GS_DISTRIBUTION_IMPL_HPP
#define GR_GS_DISTRIBUTION_IMPL_HPP

#include "gr-gs/Distribution.h"

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
            //! "Distribution" GNU Radio block implementation
            /*!
             * Implements gr::gs::Distribution
             *
             * @date    May 16, 2017
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class Distribution_impl: public Distribution
            {
            public:
                //! No copying allowed
                Distribution_impl(const Distribution_impl& x)
                    = delete;
                //! No copying allowed
                Distribution_impl& operator=(
                        const Distribution_impl& x) = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an distribution block
                /*!
                 * @param [in] bins Number of bins in the distribution
                 * @param [in] binSize Width of each bin
                 * @param [in] leftBinCenter The center point of the left most
                 *                           (most negative) bin.
                 * @param [in] decimation Should we decimate the output?
                 */
                inline Distribution_impl(
                        const unsigned bins,
                        const double binSize,
                        const double leftBinCenter,
                        const unsigned decimation);

                virtual const std::vector<float>& distribution() const;
                virtual void reset();

            private:
                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! The bins themselves
                std::vector<unsigned long long> m_bins;

                //! Left edge of leftmost bin
                const double m_leftEdge;

                //! Size of bins
                const double m_binSize;

                //! Count of samples
                unsigned long long m_count;

                //! Current distribution
                std::vector<float> m_distribution;
            };
        }
    }
}

#endif
