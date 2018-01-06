/*!
 * @file      SymbolMapper.h
 * @brief     Declares the "Symbol Mapper" GNU Radio block
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

#ifndef GR_GS_SYMBOLMAPPER_H
#define GR_GS_SYMBOLMAPPER_H

#include "gr-gs/config.h"

#include <gnuradio/sync_interpolator.h>
#include <vector>
#include <complex>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Symbol Mapper" GNU Radio block
        /*!
         * This block generates a sequence of constellation points from a
         * sequence of digital symbols.
         *
         * See the members functions for further information on the parameters
         * and their meaning.
         *
         * @tparam Symbol Base type to use for symbol type. Can be unsigned
         *                char, unsigned short, or unsigned int.
         * @date    January 6, 2018
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename Symbol>
        class GS_API SymbolMapper: virtual public gr::sync_block
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<SymbolMapper> sptr;

            //! Manufacture a symbol mapper block
            /*!
             * @param [in] constellation Desired symbol to constellation mapping
             *                           vector
             * @return Shared pointer to newly allocated symbol mapper block
             */
            static sptr make(const std::vector<Complex>& constellation);
        };

        typedef SymbolMapper<unsigned char> SymbolMapper_bc;
        typedef SymbolMapper<unsigned short> SymbolMapper_sc;
        typedef SymbolMapper<unsigned int> SymbolMapper_ic;
    }
}

#endif
