/*!
 * @file      ProbabilityMapper.hpp
 * @brief     Declares the ProbabilityMapper class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      November 22, 2018
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
             * @date   November 22, 2018
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class ProbabilityMapper
            {
            private:
                //! Our constellation pattern
                std::vector<ComplexInteger> m_constellation;

                //! Our collapsed constellation
                std::vector<int> m_collapsed;

                //! Maps constellation points to collapsed real points
                std::vector<Symbol> m_constellationToReal;

                //! Maps constellation points to collapsed imaginary points
                std::vector<Symbol> m_constellationToImag;

                //! Maps collapsed real/imag points constellation points
                std::vector<Symbol> m_collapsedToConstellation;

                //! Compute a Gaussian curve
                static inline double gaussian(
                        int value,
                        double mean,
                        double variance);

                //! Get the maximum RDS value
                static inline unsigned getMaxRDS(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength);

                //! Probabilities associated with all transitions
                std::vector<
                    std::vector<
                        std::vector<
                            double>>> m_probabilities;

                //! Nats of informations associated with all transitions
                std::vector<
                    std::vector<
                        std::vector<
                            double>>> m_nats;

                //! Bits of informations associated with all transitions
                std::vector<
                    std::vector<
                        std::vector<
                            double>>> m_bits;

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

                const std::vector<int>& collapsed() const
                {
                    return m_collapsed;
                }

                inline Symbol realConstellationPoint(const Symbol point) const
                {
                    return m_constellationToReal[point];
                }

                inline Symbol imagConstellationPoint(const Symbol point) const
                {
                    return m_constellationToImag[point];
                }

                inline Symbol decollapseConstellationPoint(
                        const Symbol real,
                        const Symbol imag) const
                {
                    return m_collapsedToConstellation[
                        real
                        +imag*m_collapsed.size()];
                }

                //! Probability associated with transition
                inline double probability(
                        const unsigned codewordPosition,
                        const int rds,
                        const Symbol symbol) const
                {
                    return m_probabilities
                        [codewordPosition]
                        [static_cast<unsigned>(rds+maxRDS)]
                        [symbol];
                }

                //! Nats of informations associated with transition
                inline double nats(
                        const unsigned codewordPosition,
                        const int rds,
                        const Symbol symbol) const
                {
                    return m_nats
                        [codewordPosition]
                        [static_cast<unsigned>(rds+maxRDS)]
                        [symbol];
                }

                //! Bits of informations associated with transition
                inline double bits(
                        const unsigned codewordPosition,
                        const int rds,
                        const Symbol symbol) const
                {
                    return m_bits
                        [codewordPosition]
                        [static_cast<unsigned>(rds+maxRDS)]
                        [symbol];
                }

                const unsigned maxRDS;
            };
        }
    }
}

#endif
