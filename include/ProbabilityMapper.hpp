/*!
 * @file      ProbabilityMapper.hpp
 * @brief     Declares the ProbabilityMapper class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 7, 2017
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

#ifndef GR_GS_PROBABILITYMAPPER_HPP
#define GR_GS_PROBABILITYMAPPER_HPP

#include "gr-gs/Data.h"

#include <vector>
#include <set>

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
             * This class takes a sequence of symbols and maps it to a sequence
             * of probabilities associated with the symbols. The input sequence
             * should contains a history() of symbols and a starting RDS value
             * that indicates the RDS at the end of the history; not the
             * beginning.
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   August 7, 2017
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class ProbabilityMapper
            {
            private:
                //! Our constellation pattern
                std::vector<std::complex<double>> m_constellation;

                //! Taps for mean RDS calculation
                std::vector<std::vector<double>> m_taps;

                //! Variances at each codeword position
                std::vector<double> m_variances;

                //! Our codeword length
                const unsigned m_codewordLength;

                //! How many symbols are required in history before our first?
                unsigned m_history;

                //! Calculate the probability of a symbol arriving
                /*!
                 * @param [in] rds The RDS value at the end of the last symbol
                 * @param [in] symbol The symbol value we're interested in
                 * @param [in] mean The Gaussian mean of our PMF.
                 * @param [in] variance The Gaussian variance of our PMF
                 * @return The probability of symbol arriving
                 */
                inline float probability(
                        std::complex<double> rds,
                        Symbol symbol,
                        std::complex<double> mean,
                        double variance) const;

                //! Compute a Gaussian curve
                inline double gaussian(
                        double value,
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
                 * @param [in] minCorrelation This decides how many taps we're
                 *                            going to need to calculate our
                 *                            means. Any autocorrelation data
                 *                            decays below this value will be
                 *                            truncated from our computations.
                 */
                ProbabilityMapper(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength,
                        const double minCorrelation);

                //! View the variance at a specific position
                const double& variance(unsigned position) const
                {
                    return m_variances.at(position);
                }

                //! View the mean calculating taps at a specific position
                const std::vector<double>& taps(unsigned position) const
                {
                    return m_taps.at(position);
                }

                //! How many symbols of history need to be provided?
                unsigned history() const
                {
                    return m_history;
                }

                const std::vector<std::complex<double>>& constellation() const
                {
                    return m_constellation;
                }

                //! Map a symbol sequence to a sequence of probabilities
                /*!
                 * @param [in] input Pointer to first symbol including the
                 *                   history.
                 * @param [in] computeHistory Should we computer the RDS history
                 *                            from our input symbol history or
                 *                            should we assume it is zero?
                 * @param [out] output Pointer to the first output probability
                 *                     value.
                 * @param [in] startingRDS The RDS state at the end of the last
                 *                         symbol in the history. All
                 *                         pre/proceding RDS values are
                 *                         computing as this as a seed.
                 * @param [in] codewordPosition First symbol's (not including
                 *                              the history) position in the
                 *                              codeword.
                 * @return Closing RDS state value at the end of the input
                 *         symbol sequence.
                 */
                std::complex<double> map(
                        const Symbol* input,
                        bool computeHistory,
                        float* output,
                        const std::complex<double>& startingRDS,
                        const unsigned symbols,
                        unsigned codewordPosition=0) const;
            };
        }
    }
}

#endif
