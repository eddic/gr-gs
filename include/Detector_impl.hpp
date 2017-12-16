/*!
 * @file      Detector_impl.hpp
 * @brief     Declares the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 28, 2017
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

#ifndef GR_GS_DETECTOR_IMPL_HPP
#define GR_GS_DETECTOR_IMPL_HPP

#include <mutex>
#include <memory>
#include <list>

#include "gr-gs/Detector.h"
#include "ProbabilityMapper.hpp"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            //! "Symbol Mapper" GNU Radio block implementation
            /*!
             * Implements gr::gs::Detector
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date    August 28, 2017
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class Detector_impl: public Detector<Symbol>
            {
            public:
                //! No copying allowed
                Detector_impl(const Detector_impl& x) = delete;
                //! No copying allowed
                Detector_impl& operator=(const Detector_impl& x)
                    = delete;

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize the pulse generator with some default options
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
                 * @param [in] noise This noise power level (or variance) is
                 *                   required to perform accurate MAP detection.
                 * @param [in] windowSize Sequence length to use for detection
                 * @param [in] framingTag Desired string to use for the "key" of
                 *                        the tag inserted at frame beginnings.
                 *                        Use an empty string to disable
                 *                        framing.
                 */
                inline Detector_impl(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength,
                        const double minCorrelation,
                        const double noise,
                        const unsigned windowSize,
                        const std::string& framingTag);

                double noisePower() const;
                void set_noisePower(const double noise);

            private:
                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! Our current noise power level (variance)
                double m_noisePower;

                //! Framing tag name/key
                std::string m_framingTag;

                //! PMT version of framing tag
                pmt::pmt_t m_framingTagPMT;

                //! The codeword length
                const unsigned int m_codewordLength;

                //! Where are we in our codeword?
                unsigned int m_codewordPosition;

                //! The probability mapping object
                ProbabilityMapper<Symbol> m_mapper;

                //! Have we started mapping yet?
                bool m_started;

                //! Our window size
                const unsigned m_windowSize;

                //! Buffer for detected real symbols
                std::unique_ptr<Symbol[]> m_realSymbols;

                //! Buffer for detected imaginary symbols
                std::unique_ptr<Symbol[]> m_imagSymbols;

                //! Buffer for distances
                std::unique_ptr<double[]> m_distances;

                //! Buffer for RDS probabilities
                std::unique_ptr<float[]> m_probabilities;

                //! Buffer for metrics
                std::unique_ptr<double[]> m_metrics;

                //! Perform single axis detection on a sequence
                void detect(
                        const Complex* input,
                        std::unique_ptr<Symbol[]>& symbols,
                        const bool real);

                struct Rank
                {
                    double score;
                    unsigned index;
                    struct Distance
                    {
                        double distance;
                        Symbol symbol;
                    };
                    typedef std::list<Distance> Distances;
                    Distances distances;

                    typename Distances::const_iterator winner;
                };
                typedef std::list<Rank> Ranks;

                static void sort(Ranks& ranks);
                static void sort(typename Rank::Distances& distances);
                static void increment(Rank& rank);
                static void decrement(Rank& rank);
            };
        }
    }
}

#endif
