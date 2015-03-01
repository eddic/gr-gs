/*!
 * @file			PulseGenerator.h
 * @brief		Declares the GNU Radio Symbol Generator
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			February 21, 2015
 * @copyright	Copyright &copy; 2014 Eddie Carle &lt;eddie@isatec.ca&gt;.
 * 				This project is released under the GNU General Public License
 * 				Version 3.
 */

/* Copyright (C) 2015 Eddie Carle <eddie@isatec.ca>
 * 
 * This file is part of The Guided Scrambling Simulator.
 *
 * The Guided Scrambling Simulator is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * The Guided Scrambling Simulator is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *                                                                           
 * You should have received a copy of the GNU General Public License along with
 * The Guided Scrambling Simulator.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PULSEGENERATOR_H
#define PULSEGENERATOR_H

#include "gr-isatec/api.h"
#include "gr-isatec/config.h"
#include <gnuradio/sync_interpolator.h>

#include <vector>
#include <complex>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Generate a pulses from a symbol
		/*!
		 * @date		February 21, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class Isatec_API PulseGenerator: virtual public gr::sync_interpolator
		{
		public:
			//! Access constellation
			/*!
			 * @return	Constellation pattern
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual const std::vector<std::complex<float>>& constellation() const =0;

			//! Set constellation
			/*!
			 * @param	[in] constellation Desired constellation pattern
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_constellation(const std::vector<std::complex<float>>& constellation) =0;

			//! Access baud rate
			/*!
			 * @return	Currently set baud rate
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual double baudRate() const =0;

			//! Set baud rate
			/*!
			 * @param	[in] baudRate Desired baud rate
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_baudRate(const double baudRate) =0;

			//! Access number of taps
			/*!
			 * @return	Currently set number of taps
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int numberOfTaps() const =0;

			//! Set number of taps
			/*!
			 * @param	[in] numberOfTaps Desired number of taps
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_numberOfTaps(const unsigned int numberOfTaps) =0;

			//! Access excess bandwidth factor
			/*!
			 * @return	Currently set excess bandwidth factor
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual double alpha() const =0;

			//! Set excess bandwidth factor
			/*!
			 * Note that this value should be in the range of [0,1]
			 *
			 * @param	[in] alpha Desired excess bandwidth factor
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_alpha(const double alpha) =0;

			//! Access pulse shape
			/*!
			 * @return	Currently set pulse shape
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual unsigned int shape() const =0;

			//! Set pulse shape
			/*!
			 * @param	[in] shape Desired pulse shape
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_shape(const unsigned int shape) =0;

			//! Enable trigger tags
			/*!
			 * @param	[in] enable True to enable, false to disable
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void enable_tags(const bool enable) =0;

			//! Access symbol phase shift
			/*!
			 * @return	Currently set symbol phase shift
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual float phase() const =0;

			//! Set symbol phase shift
			/*!
			 * @param	[in] phase Desired symbol phase shift
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			virtual void set_phase(const float phase) =0;

			typedef boost::shared_ptr<PulseGenerator> sptr;
			static sptr make(unsigned int samplesPerSymbol=1);
			static const std::vector<std::string>& shapes();
		};
	}
}

#endif
