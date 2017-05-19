/*!
 * @file      SymbolGenerator_impl.hpp
 * @brief     Declares the "Random Symbol Generator" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 19, 2017
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

#ifndef GR_GS_SYMBOLGENERATOR_IMPL_HPP
#define GR_GS_SYMBOLGENERATOR_IMPL_HPP

#include "gr-gs/SymbolGenerator.h"

#include <random>
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
            //! "Random Symbol Generator" GNU Radio block implementation
            /*!
             * Implements gr::gs::SymbolGenerator
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   August 19, 2016
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class SymbolGenerator_impl: public SymbolGenerator<Symbol>
            {
            public:
                const std::vector<double>& weightings() const;
                void set_weightings(const std::vector<double>& weightings);

                const std::string& framingTag() const;
                void set_framingTag(const std::string& tag);

                const unsigned int frameLength() const;
                void set_frameLength(const unsigned int length);

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize with default symbol weightings
                /*!
                 * @param  [in] weightings See set_weightings()
                 * @param [in] framingTag See set_framingTag()
                 * @param [in] frameLength See set_framingLength()
                 */
                inline SymbolGenerator_impl(
                        const std::vector<double>& weightings,
                        const std::string& framingTag,
                        const unsigned int frameLength);

            private:
                //! Always practice safe threading
                mutable std::mutex m_mutex;

                //! Our symbol weightings
                std::vector<double> m_weightings;

                //! Our random number generator
                std::default_random_engine m_generator;

                //! Our distribution
                std::discrete_distribution<Symbol> m_distribution;

                //! Framing tag
                std::string m_framingTag;

                //! Framing tag PMT
                pmt::pmt_t m_framingTagPMT;

                //! Frame length (for frame tag generation)
                unsigned int m_frameLength;

                //! Current symbol number (for framing)
                unsigned int m_symbolNumber;

                //! Current frame number
                uint64_t m_frameNumber;
            };
        }
    }
}

#endif
