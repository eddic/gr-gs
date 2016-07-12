/*!
 * @file      config.h
 * @brief     Defines some basic parameters for the module
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      July 8, 2016
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

#ifndef GR_GS_CONFIG_HPP
#define GR_GS_CONFIG_HPP

#include <cstdint>
#include <vector>
#include <complex>

#include <gnuradio/attributes.h>

#ifdef gnuradio_gs_EXPORTS
#  define GS_API __GR_ATTR_EXPORT
#else
#  define GS_API __GR_ATTR_IMPORT
#endif

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Pi
        const double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089;

        //! Get a default constellation pattern for a field size
        gs_API std::vector<std::complex<float>> defaultConstellation(
                unsigned int fieldSize);

        //! Type to use for representation of symbols
        typedef uint8_t Symbol;

        //! Stuff needed for the GuidedScrambler and Descrambler blocks
        namespace GuidedScrambling
        {
            //! Starting RDS for codeword analysis
            const std::complex<float> startingRDS={1,0};

            //! Maximum field size defined
            const unsigned int maxFieldSize=4;
        }
    }
}

#endif