/*!
 * @file      PulseGenerator.h
 * @brief     Declares the "Pulse Generator" GNU Radio block
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 12, 2016
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

#ifndef GR_GS_PULSEGENERATOR_H
#define GR_GS_PULSEGENERATOR_H

#include "gr-gs/config.h"

#include <gnuradio/sync_interpolator.h>
#include <vector>
#include <complex>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Pulse Generator" GNU Radio block
        /*!
         * This block generates a sequence of digital pulses from a sequence of
         * digital symbols. This is an interpolating block in that it produces a
         * fixed quantity of of samples per symbol. The block has the following
         * capabilities:
         * - Either rectangular or root-raised-cosine pulses.
         * - Multiple parameters in the root-raised cosine case can be
         *   configured.
         * - The constellation pattern can be set as a vector of complex values
         *   where the integer representation of the symbol is used as an index
         *   to said vector.
         * - An accumulating per-symbol phase shift can be applied to the
         *   constellation pattern for the purpose of spectral null shifting if
         *   it is so desired.
         * - Output signal amplitude.
         * - A trigger tag can be enabled to the start of every pulse for scope
         *   triggering.
         *
         * See the members functions for further information on the parameters
         * and their meaning.
         *
         * @date    August 12, 2016
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API PulseGenerator: virtual public gr::sync_interpolator
        {
        public:
            //! Access symbol to constellation mapping vector
            /*!
             * @return Constant reference to symbol to constellation mapping
             *         vector
             */
            virtual const std::vector<std::complex<float>>&
            constellation() const =0;

            //! Set symbol to constellation mapping vector
            /*!
             * @param  [in] constellation Desired symbol to constellation
             *              mapping vector
             */
            virtual void set_constellation(
                    const std::vector<std::complex<float>>& constellation) =0;

            //! Access baud rate
            /*!
             * This value is only used for calculating the taps used in
             * root-raised-cosine pulse shaping. It has no effect on rectangular
             * pulses.
             *
             * @return Currently set baud rate
             */
            virtual double baudRate() const =0;

            //! Set baud rate
            /*!
             * This value is only used for calculating the taps used in
             * root-raised-cosine pulse shaping. It has no effect on rectangular
             * pulses.
             *
             * @param  [in] baudRate Desired baud rate
             */
            virtual void set_baudRate(const double baudRate) =0;

            //! Access number of taps
            /*!
             * This value is only used for calculating the taps used in
             * root-raised-cosine pulse shaping. It has no effect on rectangular
             * pulses.
             *
             * @return Currently set number of taps
             */
            virtual unsigned int numberOfTaps() const =0;

            //! Set number of taps
            /*!
             * This value is only used for calculating the taps used in
             * root-raised-cosine pulse shaping. It has no effect on rectangular
             * pulses.
             *
             * Obviously setting a higher value here will give you a frequency
             * response that is much closer to the ideal but result in higher
             * latency and computing resource usage. A good start might be 1024
             * or 2048.
             *
             * @param  [in] numberOfTaps Desired number of taps
             */
            virtual void set_numberOfTaps(const unsigned int numberOfTaps) =0;

            //! Access excess bandwidth factor
            /*!
             * This value is only used for calculating the taps used in
             * root-raised-cosine pulse shaping. It has no effect on rectangular
             * pulses.
             *
             * @return Currently set excess bandwidth factor
             */
            virtual double alpha() const =0;

            //! Set excess bandwidth factor
            /*!
             * This value is only used for calculating the taps used in
             * root-raised-cosine pulse shaping. It has no effect on rectangular
             * pulses.
             *
             * Note that this value should be in the range of [0,1]
             *
             * @param  [in] alpha Desired excess bandwidth factor
             */
            virtual void set_alpha(const double alpha) =0;

            //! Access signal amplitude
            /*!
             * This value represents the absolute *maximum* value that the
             * output signal can possibly reach. This is relevant when ensuring
             * clipping does not occur downstream.
             *
             * @return Currently set signal amplitude
             */
            virtual float amplitude() const =0;

            //! Set signal amplitude
            /*!
             * This value represents the absolute *maximum* value that the
             * output signal can possibly reach. This is relevant when ensuring
             * clipping does not occur downstream.
             *
             * @param  [in] amplitude Desired output signal amplitude
             */
            virtual void set_amplitude(const float amplitude) =0;

            //! Access pulse shape
            /*!
             * Use this integer as an index in the vector returned by shapes()
             * to find a corresponding textual description of the pulse shape.
             *
             * @return Currently set pulse shape
             */
            virtual unsigned int shape() const =0;

            //! Set pulse shape
            /*!
             * The passed integer value should be the index that corresponds to
             * the textual description of the pulse shape in the vector
             * returned by shapes().
             *
             * @param  [in] shape Desired pulse shape
             */
            virtual void set_shape(const unsigned int shape) =0;

            //! Enable trigger tags
            /*!
             * This will enable a mechanism whereby tags are associated with
             * the output samples that correspond to the start of the pulses.
             * The intention is simply a mechanism to trigger a scope at
             * predictable spots for eye pattern viewing. Don't enable this if
             * you don't need it as it will add unnecessary, yet significant,
             * overhead.
             *
             * @param  [in] enable True to enable, false to disable
             */
            virtual void enable_tags(const bool enable) =0;

            //! Access symbol phase shift
            /*!
             * This value indicates the amount that the constellation pattern
             * should be "rotated" every symbol.
             *
             * @return  Currently set symbol phase shift
             */
            virtual float phase() const =0;

            //! Set symbol phase shift
            /*!
             * By setting this we force the constellation pattern to rotate a
             * fixed amount every symbol. This is here to allow spectral
             * elements resulting entirely from symbol statistics to be
             * frequency shifted.  Thus we can use some sort of encoding
             * technique to shape symbol spectra while using this mechanism to
             * move said spectra around.
             *
             * If you don't already know what this is you likely do not want
             * this to be any value except zero as it will likely mess things
             * up.
             *
             * @param  [in] phase Desired symbol phase shift
             */
            virtual void set_phase(const float phase) =0;

            //! See the actual taps used (for root raised cosine)
            virtual const std::vector<float>& taps() const =0;

            //! Shared pointer to this
            typedef boost::shared_ptr<PulseGenerator> sptr;

            //! Manufacture a pulse generator with some default options
            /*!
             *
             * @param [in] samplesPerSymbol Amount of samples to generate per
             *                              symbol
             * @param [in] constellation See set_constellation()
             * @param [in] baudRate See set_baudRate()
             * @param [in] numberOfTaps See set_numberOfTaps()
             * @param [in] alpha See set_alpha()
             * @param [in] amplitude See set_amplitude()
             * @param [in] shape See set_shape()
             * @param [in] tags See enable_tags()
             * @param [in] phase See set_phase()
             * @return Shared pointer to newly allocated pulse generator
             */
            static sptr make(
                    unsigned int samplesPerSymbol = 1,
                    const std::vector<std::complex<float>>& constellation
                        = gr::gs::defaultConstellation(4),
                    const double baudRate = 100e3,
                    const unsigned int numberOfTaps = 1024,
                    const double alpha = 0.5,
                    const double amplitude = 0.8,
                    const unsigned int shape = 0,
                    const bool tags = false,
                    const double phase = 0);

            //! Get a vector of pulse shape names
            /*!
             * @return Reference to constant vector of pulse shape names.
             */
            static const std::vector<std::string>& shapes();
        };
    }
}

#endif
