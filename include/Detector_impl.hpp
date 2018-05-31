/*!
 * @file      Detector_impl.hpp
 * @brief     Declares the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 31, 2018
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

#ifndef GR_GS_DETECTOR_IMPL_HPP
#define GR_GS_DETECTOR_IMPL_HPP

#include <mutex>
#include <memory>
#include <list>
#include <numeric>

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
            //! "Guided Scrambling Detector" GNU Radio block implementation
            /*!
             * Implements gr::gs::Detector
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date    May 31, 2018
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

                //! GNU Radio work function
                int general_work(
                        int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! GNU Radio forecast function
                void forecast(
                        int noutput_items,
                        gr_vector_int& ninput_items_required);

                //! Initialize the detector
                /*!
                 * @param [in] fieldSize Field size of our symbols. Our default
                 *                       constellation pattern is retrieved from
                 *                       this.
                 * @param [in] codewordLength Length of our codewords.
                 * @param [in] augmentingLength How many augmenting symbols in
                 *                              the codeword?
                 * @param [in] noise This noise power level (or variance) is
                 *                   required to perform accurate MAP detection.
                 * @param [in] alignmentTag Desired string to use for the "key" of
                 *                          of the tag inserted at alignment.
                 *                          Use an empty string to disable
                 *                          alignment.
                 * @param [in] nodeDiscardMetric This defines how aggressively
                 *                               we will discard trellis nodes.
                 *                               This must be above zero.
                 *                               Larger numbers mean more
                 *                               accurate detection but make
                 *                               detection more computationally
                 *                               intensive.
                 */
                inline Detector_impl(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength,
                        const double noise,
                        const std::string& alignmentTag,
                        const double nodeDiscardMetric);

                double noisePower() const;
                void set_noisePower(const double noise);

            private:
                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! Our current noise power level (variance)
                double m_noisePower;

                //! Framing tag name/key
                pmt::pmt_t m_alignmentTag;

                //! Are we aligned yet?
                bool m_aligned;

                //! Codeword length
                const unsigned m_codewordLength;

                //! The probability mapping object
                const ProbabilityMapper<Symbol> m_mapper;

                //! RDS state bound
                const unsigned m_bound;

                //! Get the RDS bound of the code
                static inline unsigned getBound(
                        const unsigned fieldSize,
                        const unsigned codewordLength,
                        const unsigned augmentingLength);

                //! Trellis for guided scrambling detection
                class Trellis
                {
                public:
                    /*!
                     * @param [in] real True for real or in-phase componenent.
                     *                  False for imaginary or quadrature
                     *                  component.
                     * @param [in] mapper Our source symbol probability mapper.
                     * @param [in] codewordLength Length of our codewords.
                     * @param [in] noise This noise power level (or variance) is
                     *                   required to perform accurate MAP
                     *                   detection.
                     * @param [in] bound The upper bound of our RDS state.
                     * @param [in] nodeDiscardMetric This defines how
                     *                               aggressively we will
                     *                               discard trellis nodes.
                     *                               This must be above zero.
                     *                               Larger numbers mean more
                     *                               accurate detection but
                     *                               make detection more
                     *                               computationally intensive.
                     */
                    Trellis(
                            const bool real,
                            const ProbabilityMapper<Symbol>& mapper,
                            const unsigned codewordLength,
                            const double noisePower,
                            const unsigned bound,
                            const double nodeDiscardMetric);

                    void insert(const Complex* input, size_t size);

                    //! Get what available output data there is
                    std::list<Symbol> output();

                    //! Put back the output data we didn't use.
                    void putBack(std::list<Symbol>&& output);

                    //! Set our noise power
                    void set_noisePower(const double noise);

                    //! Get the output size for forecasting
                    unsigned outputSize() const
                    {
                        return m_output.size();
                    }

                private:
                    //! A node in the detection trellis
                    class Node
                    {
                    private:
                        //! The parent trellis
                        Trellis& m_trellis;

                        //! A set of all node in the current discrete time value
                        const std::shared_ptr<std::set<Node*>> m_set;

                    public:
                        //! RDS state value for this node
                        const int m_rds;

                        //! Symbol that transitioned to this RDS state
                        Symbol m_symbol;

                        //! Accumulated metric at this node
                        double m_metric;

                        //! Source node
                        std::shared_ptr<Node> m_source;

                        inline Node(
                                Trellis& trellis,
                                int rds,
                                std::shared_ptr<std::set<Node*>> set);

                        inline ~Node();

                        //! Close the trellis at this node
                        void close(const unsigned depth);
                    };

                    //! Append a new set of nodes to the trellis
                    void append(const double* distances);

                    //! Always practice safe threading
                    std::mutex m_mutex;

                    //! Available output symbols
                    std::list<Symbol> m_output;

                    //! Buffer for putting new output symbols
                    std::list<Symbol> m_outputBuffer;

                    //! True for real/in-phase component.
                    const bool m_real;

                    //! Associated constellation
                    const std::vector<int>& m_constellation;

                    //! Our probability mapper
                    const ProbabilityMapper<Symbol>& m_mapper;

                    //! The head end of the trellis
                    std::map<int, std::shared_ptr<Node>> m_head;

                    //! The codeword length
                    const unsigned m_codewordLength;

                    //! Our position in the codeword
                    unsigned m_codewordPosition;

                    //! Upper bound to the RDS states
                    const unsigned m_bound;

                    //! Current noise power level
                    double m_noisePower;

                    //! Node discard metric
                    const double m_nodeDiscardMetric;
                };

                //! The real/in-phase trellis
                Trellis m_realTrellis;

                //! The imaginary/quadrature trellis
                Trellis m_imagTrellis;
            };
        }
    }
}

#endif
