/*!
 * @file      PulseGenerator_impl.hpp
 * @brief     Declares the "Pulse Generator" GNU Radio block implementation
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

#ifndef GR_GS_PULSEGENERATOR_IMPL_HPP
#define GR_GS_PULSEGENERATOR_IMPL_HPP

#include "gr-gs/PulseGenerator.h"

#include <deque>
#include <mutex>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            //! "Pulse Generator" GNU Radio block implementation
            /*!
             * Implements gr::gs::PulseGenerator
             *
             * @date    March 7, 2015
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class PulseGenerator_impl: public PulseGenerator
            {
            public:
                //! No copying allowed
                PulseGenerator_impl(const PulseGenerator_impl& x) = delete;
                //! No copying allowed
                PulseGenerator_impl& operator=(const PulseGenerator_impl& x)
                    = delete;

                const std::vector<std::complex<float>>& constellation() const;
                void set_constellation(
                        const std::vector<std::complex<float>>& constellation);

                double baudRate() const;
                void set_baudRate(const double baudRate);

                unsigned int numberOfTaps() const;
                void set_numberOfTaps(const unsigned int numberOfTaps);

                double alpha() const;
                void set_alpha(const double alpha);

                float amplitude() const;
                void set_amplitude(const float amplitude);

                unsigned int shape() const;
                void set_shape(const unsigned int shape);

                void enable_tags(const bool enable);

                float phase() const;
                void set_phase(const float phase);

                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Initialize the pulse generator with some default options
                /*!
                 * This initializes the pulse shaper with the following
                 * parameters:
                 *  - constellation = (1,0) (0,1) (0,-1) (-1,0)
                 *  - baud rate = 100 kBd
                 *  - number of taps = 1024
                 *  - excess bandwidth (alpha) = 0.5
                 *  - symbol phase shift = 0
                 *  - pulse shape = rectangular
                 *  - tagging = disabled
                 *  - amplitude = 0.8
                 *
                 * @param   samplesPerSymbol Amount of samples to generator per
                 *                           symbol
                 * @date    March 5, 2015
                 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                PulseGenerator_impl(unsigned int samplesPerSymbol);

            private:
                //! Always practice safe threading
                mutable std::mutex m_mutex;

                //! How many samples to output per symbol
                const unsigned int m_samplesPerSymbol;

                //! Constellation patter to map symbols
                std::vector<std::complex<float>> m_constellation;

                //! Baud rate
                double m_baudRate;

                //! Number of taps in the pulse shaping filter
                unsigned int m_numberOfTaps;

                //! Excess bandwidth in root-raised-cosine
                double m_alpha;

                //! How much to phase shift per symbol
                float m_phase;

                //! Current symbol phase offset
                float m_currentPhase;

                //! Current signal amplitude
                float m_amplitude;

                //! Index to current pulse shaping type
                unsigned int m_shape;

                //! Actual FIR taps for pulse shaping
                std::vector<float> m_taps;

                //! A queue of input values for the FIR
                std::deque<std::complex<float>> m_input;

                //! Are we fully configured and valid?
                bool m_valid;

                //! Are we generating scope trigger tags?
                bool m_tag;
            };
        }
    }
}

#endif
