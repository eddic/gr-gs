/*!
 * @file      InfiniteAverage_impl.hpp
 * @brief     Declares the "Infinite Average" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 11, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2016 Eddie Carle
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

#ifndef GR_GS_INFINITEAVERAGE_IMPL_HPP
#define GR_GS_INFINITEAVERAGE_IMPL_HPP

#include "gr-gs/InfiniteAverage.h"

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
            //! "Infinite Average" GNU Radio block implementation
            /*!
             * Implements gr::gs::InfiniteAverage
             *
             * @date    August 11, 2015
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class InfiniteAverage_impl: public InfiniteAverage
            {
            public:
                //! No copying allowed
                InfiniteAverage_impl(const InfiniteAverage_impl& x) = delete;
                //! No copying allowed
                InfiniteAverage_impl& operator=(const InfiniteAverage_impl& x)
                    = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an infinite average block
                /*!
                 * @param [in] vectorSize Number of elements in the vector
                 * @param [in] decimation Should we decimate the output?
                 * @return Shared pointer to newly allocated pulse generator
                 */
                inline InfiniteAverage_impl(
                        const unsigned vectorSize,
                        const unsigned decimation);

                virtual const std::vector<float>& average() const;
                virtual void reset();

            private:
                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! Sum of all vectors
                std::vector<double> m_sum;

                //! Vector count
                unsigned long long m_count;

                //! Current average
                std::vector<float> m_average;
            };
        }
    }
}

#endif
