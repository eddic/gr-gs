/*!
 * @file      Distribution_impl.hpp
 * @brief     Declares the "Distribution" GNU Radio block implementation
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
            //! "Distribution" for real values GNU Radio block implementation
            /*!
             * Implements gr::gs::Distribution_ff
             *
             * @date    June 12, 2017
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class Distribution_ff_impl: public Distribution_ff
            {
            public:
                //! No copying allowed
                Distribution_ff_impl(const Distribution_ff_impl& x)
                    = delete;
                //! No copying allowed
                Distribution_ff_impl& operator=(
                        const Distribution_ff_impl& x) = delete;

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
                 * @param [in] output Should we output the average or just store
                 *                    it?
                 * @param [in] decimation Should we decimate the output?
                 */
                inline Distribution_ff_impl(
                        const unsigned bins,
                        const double binSize,
                        const double leftBinCenter,
                        const bool output,
                        const unsigned decimation);

                virtual std::vector<double> distribution() const;
                virtual void reset();

            private:
                //! Are we outputting?
                const bool m_output;

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
            };

            //! "Distribution" for complex values GNU Radio block implementation
            /*!
             * Implements gr::gs::Distribution_cf
             *
             * @date    December 15, 2017
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class Distribution_cf_impl: public Distribution_cf
            {
            public:
                //! No copying allowed
                Distribution_cf_impl(const Distribution_cf_impl& x)
                    = delete;
                //! No copying allowed
                Distribution_cf_impl& operator=(
                        const Distribution_cf_impl& x) = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an distribution block
                /*!
                 * @param [in] bins Number of bins in the distribution
                 * @param [in] binSize Width of each bin
                 * @param [in] leastBinCenter The center point of the least
                 *                           (most negative) bin.
                 * @param [in] output Should we output the average or just store
                 *                    it?
                 * @param [in] decimation Should we decimate the output?
                 */
                inline Distribution_cf_impl(
                        const unsigned bins,
                        const double binSize,
                        const std::complex<double> leastBinCenter,
                        const bool output,
                        const unsigned decimation);

                virtual std::vector<std::vector<double>> distribution() const;
                virtual void reset();

            private:
                //! Are we outputting?
                const bool m_output;

                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! How many bins?
                const unsigned m_binCount;

                //! The bins themselves
                std::vector<std::vector<unsigned long long>> m_bins;

                //! Left edge of leftmost bin
                const std::complex<double> m_leastEdge;

                //! Size of bins
                const double m_binSize;

                //! Count of samples
                unsigned long long m_count;
            };
        }
    }
}

#endif
