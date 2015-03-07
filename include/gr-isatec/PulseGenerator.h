/*!
 * @file       PulseGenerator.h
 * @brief      Declares the "Pulse Generator" GNU Radio block
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 8, 2015
 * @copyright  Copyright &copy; 2015 %Isatec Inc.  This project is released
 *             under the GNU General Public License Version 3.
 */

/* Copyright (C) 2015 %Isatec Inc.
 * 
 * This file is part of the %Isatec GNU Radio Module
 *
 * The %Isatec GNU Radio Module is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The %Isatec GNU Radio Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *                                                                          
 * You should have received a copy of the GNU General Public License along with
 * The %Isatec GNU Radio Module.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PULSEGENERATOR_H
#define PULSEGENERATOR_H

#include "gr-isatec/config.h"

#include <gnuradio/sync_interpolator.h>
#include <vector>
#include <complex>

//! GNU Radio Namespace
namespace gr
{
   //! Contains all blocks for the %Isatec GNU Radio Module
   namespace Isatec
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
       *   where the integer representation of the symbol is used as an index to
       *   said vector.
       * - An accumulating per-symbol phase shift can be applied to the
       *   constellation pattern for the purpose of spectral null shifting if it
       *   is so desired.
       * - A trigger tag can be enabled to the start of every pulse for scope
       *   triggering.
       *
       * See the members functions for further information on the parameters and
       * their meaning.
       *
       * @date    March 3, 2015
       * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
       */
      class Isatec_API PulseGenerator: virtual public gr::sync_interpolator
      {
      public:
         //! Access symbol to constellation mapping vector
         /*!
          * @return  Constant reference to symbol to constellation mapping
          *          vector
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual const std::vector<std::complex<float>>& constellation() const =0;

         //! Set symbol to constellation mapping vector
         /*!
          * @param   [in] constellation Desired symbol to constellation mapping vector
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void set_constellation(const std::vector<std::complex<float>>& constellation) =0;

         //! Access baud rate
         /*!
          * This value is only used for calculating the taps used in
          * root-raised-cosine pulse shaping. It has no effect on rectangular
          * pulses.
          *
          * @return  Currently set baud rate
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual double baudRate() const =0;

         //! Set baud rate
         /*!
          * This value is only used for calculating the taps used in
          * root-raised-cosine pulse shaping. It has no effect on rectangular
          * pulses.
          *
          * @param   [in] baudRate Desired baud rate
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void set_baudRate(const double baudRate) =0;

         //! Access number of taps
         /*!
          * This value is only used for calculating the taps used in
          * root-raised-cosine pulse shaping. It has no effect on rectangular
          * pulses.
          *
          * @return  Currently set number of taps
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual unsigned int numberOfTaps() const =0;

         //! Set number of taps
         /*!
          * This value is only used for calculating the taps used in
          * root-raised-cosine pulse shaping. It has no effect on rectangular
          * pulses.
          *
          * Obviously setting a higher value here will give you a frequency
          * responce that is much closer to the ideal but result in higher
          * latency and computing resource usage. A good start might be 1024 or
          * 2048.
          *
          * @param   [in] numberOfTaps Desired number of taps
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void set_numberOfTaps(const unsigned int numberOfTaps) =0;

         //! Access excess bandwidth factor
         /*!
          * This value is only used for calculating the taps used in
          * root-raised-cosine pulse shaping. It has no effect on rectangular
          * pulses.
          *
          * @return  Currently set excess bandwidth factor
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
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
          * @param   [in] alpha Desired excess bandwidth factor
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void set_alpha(const double alpha) =0;

         //! Access pulse shape
         /*!
          * Use this integer as an index in the vector returned by shapes() to
          * find a corresponding textual description of the pulse shape.
          *
          * @return  Currently set pulse shape
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual unsigned int shape() const =0;

         //! Set pulse shape
         /*!
          * The passed integer value should be the index that corresponds to the
          * textual description of the pulse shape in the vector returned by
          * shapes().
          *
          * @param   [in] shape Desired pulse shape
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void set_shape(const unsigned int shape) =0;

         //! Enable trigger tags
         /*!
          * This will enable a mechanism whereby tags are associated with the
          * output samples that correspond to the start of the pulses. The
          * intention is simply a mechanism to trigger a scope at predictable
          * spots for eye pattern viewing. Don't enable this if you don't need
          * it as it will add unnecessary, yet significant, overhead.
          *
          * @param   [in] enable True to enable, false to disable
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void enable_tags(const bool enable) =0;

         //! Access symbol phase shift
         /*!
          * This value indicates the amount that the constellation pattern
          * should be "rotated" every symbol.
          *
          * @return  Currently set symbol phase shift
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual float phase() const =0;

         //! Set symbol phase shift
         /*!
          * By setting this we force the constellation pattern to rotate a fixed
          * amount every symbol. This is here to allow spectral elements
          * resulting entirely from symbol statistics to be frequency shifted.
          * Thus we can use some sort of encoding technique to shape symbol
          * spectra while using this mechanism to move said spectra around.
          *
          * If you don't already know what this is you likely do not want this
          * to be any value except zero as it will likely mess things up.
          *
          * @param   [in] phase Desired symbol phase shift
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         virtual void set_phase(const float phase) =0;

         //! Shared pointer to this
         typedef boost::shared_ptr<PulseGenerator> sptr;

         //! Manufacture a pulse generator with some default options
         /*!
          * This initializes the pulse generator with the following parameters:
          *  - constellation = (1,0) (0,1) (0,-1) (-1,0)
          *  - baud rate = 100 kBd
          *  - number of taps = 1024
          *  - excess bandwidth (alpha) = 0.5
          *  - symbol phase shift = 0
          *  - pulse shape = rectangular
          *  - tagging = disabled
          *
          * @param   samplesPerSymbol Amount of samples to generator per symbol
          * @return  Shared pointer to newly allocated pulse generator
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         static sptr make(unsigned int samplesPerSymbol=1);

         //! Get a vector of pulse shape names
         /*!
          * @return  Reference to constant vector of pulse shape names.
          * @date    March 5, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         static const std::vector<std::string>& shapes();
      };
   }
}

#endif
