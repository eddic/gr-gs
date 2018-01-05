/*!
 * @file      ErrorCount_impl.hpp
 * @brief     Declares the "Error Count" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      January 4, 2018
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

#ifndef GR_GS_ERRORCOUNT_IMPL_HPP
#define GR_GS_ERRORCOUNT_IMPL_HPP

#include "gr-gs/ErrorCount.h"

#include <mutex>
#include <array>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            //! "Error Count" GNU Radio block implementation
            /*!
             * Implements gr::gs::ErrorCount
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   January 4, 2018
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class ErrorCount_impl: public ErrorCount<Symbol>
            {
            public:
                //! No copying allowed
                ErrorCount_impl(const ErrorCount_impl& x) = delete;
                //! No copying allowed
                ErrorCount_impl& operator=(const ErrorCount_impl& x)
                    = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize an error count block
                /*!
                 * @param [in] output Set to true for an output port.
                 * @param [in] alignmentTag Desired string to use for the "key" of
                 *                          of the tag inserted at alignment.
                 *                          Use an empty string to disable
                 *                          alignment.
                 * @param [in] maxErrors Maximum number of errors to log before
                 *                       EOF.
                 * @param [in] maxSymbols Maximum number of symbols to log
                 *                        before EOF.
                 * @return Shared pointer to newly allocated error count block
                 */
                inline ErrorCount_impl(
                        const bool output,
                        const std::string& alignmentTag,
                        const unsigned long long maxErrors,
                        const unsigned long long maxSymbols);

                unsigned long long symbols() const;
                unsigned long long errors() const;
                double rate() const;
                bool finished() const;
                void reset();

            private:
                //! Should we output our error rate?
                const bool m_output;

                //! Max errors before EOF
                const unsigned long long m_maxErrors;

                //! Max symbols before EOF
                const unsigned long long m_maxSymbols;

                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! Symbol count
                unsigned long long m_symbols;

                //! Error count
                unsigned long long m_errors;

                //! Current error rate
                double m_rate;

                //! Framing tag
                pmt::pmt_t m_alignmentTag;

                //! True if we've established the delay
                bool m_aligned;
            };
        }
    }
}

#endif
