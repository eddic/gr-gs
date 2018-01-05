/*!
 * @file      Data.h
 * @brief     Declares the Data namespace
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

#ifndef GR_GS_DATA_H
#define GR_GS_DATA_H

#include "gr-gs/config.h"

#include <array>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Retrieve simulation result data
        namespace Data
        {
            //! Type for RDS autocovariance data
            typedef std::vector<
                        std::array<
                            std::array<
                                std::array<double, 2>
                            , 2>
                        , autocovarianceDataLength>
                    > Autocovariance;

            //! Retrieve RDS autocovariance data
            GS_API Autocovariance autocovariance(
                    const unsigned fieldSize,
                    const unsigned codewordLength,
                    const unsigned augmentingLength);

            //! Type for RDS probability mass data
            typedef std::vector<
                        std::array<
                            std::array<double, distributionDataWidth>
                        , distributionDataWidth>
                    > Distribution;

            //! Retrieve RDS probability mass data
            GS_API Distribution distribution(
                    const unsigned fieldSize,
                    const unsigned codewordLength,
                    const unsigned augmentingLength);

            //! Type for power spectral density data
            typedef std::vector<std::array<double, 2>> PSD;

            //! Retrieve power spectral density data
            GS_API PSD psd(
                    const unsigned fieldSize,
                    const unsigned codewordLength,
                    const unsigned augmentingLength);
        }
    }
}

#endif
