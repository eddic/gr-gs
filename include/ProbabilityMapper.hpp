/*!
 * @file      ProbabilityMapper.hpp
 * @brief     Declares the ProbabilityMapper class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 31, 2018
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

#ifndef GR_GS_PROBABILITYMAPPER_HPP
#define GR_GS_PROBABILITYMAPPER_HPP

#include "gr-gs/Data.h"

#include <vector>
#include <set>
#include <memory>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            //! For performing selection analysis of a codeword
            /*!
             * This class takes symbols and maps them to probabilities
             * associated with the symbols on either the real or imaginary axis.
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   May 31, 2018
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class ProbabilityMapper
            {
            private:
                //! Our constellation pattern
                std::vector<ComplexInteger> m_constellation;

                //! Our collapsed real constellation
                std::vector<int> m_realConstellation;

                //! Our collapsed imaginary constellation
                std::vector<int> m_imagConstellation;

                //! Maps constellation points to collapsed real points
                std::vector<Symbol> m_constellationToReal;

                //! Maps constellation points to collapsed imaginary points
                std::vector<Symbol> m_constellationToImag;

                //! Maps collapsed real/imag points constellation points
                std::vector<Symbol> m_collapsedToConstellation;

                //! Taps for mean RDS calculation
                std::vector<double> m_taps;

                //! Variances at each codeword position
                std::vector<double> m_variances;

                //! Our codeword length
                const unsigned m_codewordLength;

                //! Compute a Gaussian curve
                inline double gaussian(
                        int value,
                        double mean,
                        double variance) const;

            public:
                //! Build our probability mapper
                /*!
                 * @param [in] fieldSize Field size of our symbols. Our default
                 *                       constellation pattern is retrieved from
                 *                       this.
                 * @param [in] codewordLength Length of our codewords.
                 * @param [in] augmentingLength How many augmenting symbols in
                 *                              the codeword?
                 */
                ProbabilityMapper(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength);

                //! View the variance at a specific position
                const double& variance(unsigned position) const
                {
                    return m_variances.at(position);
                }

                //! View the mean calculating tap at a specific position
                const double& tap(unsigned position) const
                {
                    return m_taps.at(position);
                }

                const std::vector<ComplexInteger>& constellation() const
                {
                    return m_constellation;
                }

                const std::vector<int>& constellation(
                        const bool real) const
                {
                    if(real)
                        return m_realConstellation;
                    else
                        return m_imagConstellation;
                }

                inline void collapseConstellation(
                        Symbol& real,
                        Symbol& imag,
                        const Symbol point) const
                {
                    real = m_constellationToReal[point];
                    imag = m_constellationToImag[point];
                }

                inline Symbol decollapseConstellation(
                        const Symbol real,
                        const Symbol imag) const
                {
                    return m_collapsedToConstellation[
                        real
                        +imag*m_realConstellation.size()];
                }

                //! Calculate the probability weightings
                /*!
                 * @param [in] rds The previous RDS value
                 * @param [in] codewordPosition Symbol's position in the
                 *                              codeword.
                 * @param [out] Probability weightings by symbol
                 * @param [in] real Set to true if we're working on the real
                 *                  axis. False means we're working on the
                 *                  imaginary axis.
                 */
                void weightings(
                        const int rds,
                        const unsigned codewordPosition,
                        std::vector<double>& weightings,
                        const bool real) const;
            };
        }
    }
}

#endif
