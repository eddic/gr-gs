/*!
 * @file      SymbolMapper_impl.hpp
 * @brief     Declares the "Symbol Mapper" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 17, 2016
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

#ifndef GR_GS_SYMBOLMAPPER_IMPL_HPP
#define GR_GS_SYMBOLMAPPER_IMPL_HPP

#include "gr-gs/SymbolMapper.h"

#include <deque>
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
            //! "Symbol Mapper" GNU Radio block implementation
            /*!
             * Implements gr::gs::SymbolMapper
             *
             * @date    August 12, 2016
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class SymbolMapper_impl: public SymbolMapper
            {
            public:
                //! No copying allowed
                SymbolMapper_impl(const SymbolMapper_impl& x) = delete;
                //! No copying allowed
                SymbolMapper_impl& operator=(const SymbolMapper_impl& x)
                    = delete;

                const std::vector<std::complex<float>>& constellation() const;
                void set_constellation(
                        const std::vector<std::complex<float>>& constellation);

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize the pulse generator with some default options
                /*!
                 * @param [in] constellation See set_constellation()
                 */
                inline SymbolMapper_impl(
                    const std::vector<std::complex<float>>& constellation);

            private:
                //! Always practice safe threading
                mutable std::mutex m_mutex;

                //! Constellation patter to map symbols
                std::vector<std::complex<float>> m_constellation;
            };
        }
    }
}

#endif