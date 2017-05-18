/*!
 * @file      Integrate.h
 * @brief     Declares the "Integrate" GNU Radio block
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

#ifndef GR_GS_INTEGRATE_H
#define GR_GS_INTEGRATE_H

#include "gr-gs/config.h"

#include <gnuradio/sync_decimator.h>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Integrate" GNU Radio block
        /*!
         * This block takes in a sequence of numbers and continually integrates
         * them without reset.
         *
         * @date    May 16, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API Integrate: virtual public gr::sync_decimator
        {
        public:
            //! Shared pointer to this
            typedef boost::shared_ptr<Integrate> sptr;

            //! Manufacture an integrate block
            /*!
             * @param [in] decimation Should we decimate the output?
             * @return Shared pointer to newly allocated integrate block
             */
            static sptr make(const unsigned decimation = 1);

            //! Reset the current integration
            virtual void reset() =0;
        };
    }
}

#endif
