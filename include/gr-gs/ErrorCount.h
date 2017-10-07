/*!
 * @file      ErrorCount.h
 * @brief     Declares the "Error Count" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      October 7, 2017
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

#ifndef GR_GS_ERRORCOUNT_H
#define GR_GS_ERRORCOUNT_H

#include "gr-gs/config.h"

#include <gnuradio/sync_block.h>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Error Count" GNU Radio block
        /*!
         * This block is used for calculating total error counts and rates for
         * framed symbol sequences over all time.
         *
         * @tparam Symbol Base type to use for symbol type. Can be unsigned
         *                char, unsigned short, or unsigned int.
         * @date   October 7, 2017
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<typename Symbol>
        class GS_API ErrorCount: virtual public gr::sync_block
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<ErrorCount> sptr;

            //! Manufacture an error count block
            /*!
             * @param [in] output Set to true for an output port.
             * @param [in] framingTag Desired string to use for the "key" of the
             *                        tag indicating frame starts. Use a zero
             *                        length string to indicate no framing is
             *                        used.
             * @param [in] maxErrors Maximum number of errors to log before EOF.
             * @param [in] maxSymbols Maximum number of symbols to log before
             *                        EOF.
             * @return Shared pointer to newly allocated pulse generator
             */
            static sptr make(
                    const bool output,
                    const std::string& framingTag = "",
                    const unsigned long long maxErrors = 0,
                    const unsigned long long maxSymbols = 0);

            //! See the total symbols counted
            virtual unsigned long long symbols() const =0;

            //! See the total errors
            virtual unsigned long long errors() const =0;

            //! See the current error rate [0,1]
            virtual double rate() const =0;

            //! Have we reached the end?
            virtual bool finished() const =0;

            //! Reset the block
            virtual void reset() =0;
        };

        typedef ErrorCount<unsigned char> ErrorCount_bf;
        typedef ErrorCount<unsigned short> ErrorCount_sf;
        typedef ErrorCount<unsigned int> ErrorCount_if;
    }
}

#endif
