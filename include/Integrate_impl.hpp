/*!
 * @file      Integrate_impl.hpp
 * @brief     Declares the "Integrate" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 29, 2017
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

#ifndef GR_GS_INTEGRATE_IMPL_HPP
#define GR_GS_INTEGRATE_IMPL_HPP

#include "gr-gs/Integrate.h"

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
            //! "Integrate" GNU Radio block implementation
            /*!
             * Implements gr::gs::Integrate
             *
             * @tparam Internal Internal storage type (may be of higher
             *                  accuracy).
             * @tparam External External stype for talking with GNU Radio.
             * @date    May 29, 2017
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Internal, typename External>
            class Integrate_impl: public Integrate<Internal, External>
            {
            public:
                //! No copying allowed
                Integrate_impl(const Integrate_impl& x)
                    = delete;
                //! No copying allowed
                Integrate_impl& operator=(
                        const Integrate_impl& x) = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an integrate block
                /*!
                 * @param [in] decimation Should we decimate the output?
                 */
                inline Integrate_impl(const unsigned decimation);

                virtual void reset();

            private:
                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! Our current sum
                Internal m_sum;
            };
        }
    }
}

#endif
