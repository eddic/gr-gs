/*!
 * @file      Terminator_impl.hpp
 * @brief     Declares the "Terminator" GNU Radio block implementation
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

#ifndef GR_GS_TERMINATOR_IMPL_HPP
#define GR_GS_TERMINATOR_IMPL_HPP

#include "gr-gs/Terminator.h"

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
            //! "Terminator" GNU Radio block implementation
            /*!
             * Implements gr::gs::Terminator
             *
             * @date    May 20, 2017
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class Terminator_impl: public Terminator
            {
            public:
                //! No copying allowed
                Terminator_impl(const Terminator_impl& x) = delete;
                //! No copying allowed
                Terminator_impl& operator=(const Terminator_impl& x)
                    = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an average block
                /*!
                 * @param [in] sampleSize Number of bytes per sample (vector
                 *                        included).
                 * @param [in] sampleCount Number of samples until we terminate
                 */
                inline Terminator_impl(
                        const unsigned sampleSize,
                        const unsigned long long sampleCount);

                //! How many samples have we sunk?
                unsigned long long samples() const;

            private:
                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! End point for sample count
                const unsigned long long m_end;

                //! Current sample count
                unsigned long long m_samples;
            };
        }
    }
}

#endif
