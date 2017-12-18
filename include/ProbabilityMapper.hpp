/*!
 * @file      ProbabilityMapper.hpp
 * @brief     Declares the ProbabilityMapper class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 17, 2017
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
             * This class takes a sequence of symbols and maps it to a sequence
             * of probabilities associated with the symbols on either the real
             * or imaginary axis. The input sequence should contains a
             * history() of symbols.
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   December 17, 2017
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class ProbabilityMapper
            {
            private:
                //! How big is our input (including history)?
                unsigned m_inputSize;

                //! Buffer for our RDS values
                std::unique_ptr<int[]> m_buffer;

                //! Our constellation pattern
                std::vector<std::complex<double>> m_constellation;

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
                std::vector<std::vector<double>> m_taps;

                //! Variances at each codeword position
                std::vector<double> m_variances;

                //! Our codeword length
                const unsigned m_codewordLength;

                //! How many symbols are required in history before our first?
                unsigned m_history;

                //! Compute a Gaussian curve
                inline double gaussian(
                        int value,
                        double mean,
                        double variance) const;

                void weightings_impl(
                        const int rds,
                        const unsigned codewordPosition,
                        const double mean,
                        std::vector<double>& weightings,
                        const bool real) const;

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
                 * @param [in] windowSize This is the desired window size of the
                 *                        input data. History should exist in
                 *                        addition to this.
                 * @param [in] doubleEnded Typically history data is only
                 *                         prepended to the input data. If,
                 *                         however, you want to have non-causal
                 *                         history including at the end of the
                 *                         input data set this to true.
                 */
                ProbabilityMapper(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength,
                        const double minCorrelation,
                        const unsigned windowSize,
                        const bool doubleEnded=false);

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

                const std::vector<int>& constellation(
                        const bool real) const
                {
                    if(real)
                        return m_realConstellation;
                    else
                        return m_imagConstellation;
                }

                void collapseConstellation(
                        Symbol* reals,
                        Symbol* imags,
                        const Symbol* points,
                        const size_t length) const;

                void decollapseConstellation(
                        const Symbol* reals,
                        const Symbol* imags,
                        Symbol* points,
                        const size_t length) const;

                inline Symbol decollapseConstellation(
                        Symbol real,
                        Symbol imag) const
                {
                    return m_collapsedToConstellation[
                        real
                        +imag*m_realConstellation.size()];
                }

                //! Calculate the probability weightings
                /*!
                 * @param [in] rds Reversible iterator to the *last* value in
                 *                 the RDS history array. There should be
                 *                 history() rds values available in this array.
                 * @param [in] codewordPosition Symbol's (not including the
                 *                              history) position in the
                 *                              codeword.
                 * @param [out] Probability weightings by symbol
                 * @param [in] real Set to true if we're working on the real
                 *                  axis. False means we're working on the
                 *                  imaginary axis.
                 */
                template<typename InputIt>
                inline void weightings(
                        InputIt rds,
                        const unsigned codewordPosition,
                        std::vector<double>& weightings,
                        const bool real) const
                {
                    double mean=0;
                    const int currentRDS = *rds;

                    for(
                            auto tap = m_taps[codewordPosition].crbegin();
                            tap != m_taps[codewordPosition].crend();
                            ++tap)
                    {
                        mean += *tap * static_cast<double>(*rds);
                        --rds;
                    }

                    weightings_impl(
                            currentRDS,
                            codewordPosition,
                            mean,
                            weightings,
                            real);
                }

                //! Map a symbol sequence to a sequence of probabilities
                /*!
                 * The length of the input data array must be one of the
                 * following. In the case of double ended having been specified
                 * at construction the length should be the specified window
                 * size plus double the history. In the non double ended case,
                 * the length is simply the specified window size plus the
                 * history.
                 *
                 * @param [in] input Pointer to first symbol including the
                 *                   history.
                 * @param [in] computeHistory Should we compute the RDS history
                 *                            from our input symbol history or
                 *                            should we assume it is zero?
                 * @param [out] output Pointer to the first output probability
                 *                     value.
                 * @param [in] codewordPosition First symbol's (not including
                 *                              the history) position in the
                 *                              codeword.
                 * @param [in] real Set to true if we're working on the real
                 *                  axis. False means we're working on the
                 *                  imaginary axis.
                 */
                void map(
                        const Symbol* const input,
                        bool computeHistory,
                        float* output,
                        unsigned codewordPosition,
                        const bool real) const;

                unsigned inputSize() const
                {
                    return m_inputSize;
                }
            };
        }
    }
}

#endif
