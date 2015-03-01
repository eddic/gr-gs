/*!
 * @file			PulseGenerator_impl.hpp
 * @brief		Defines the GNU Radio Symbol Generator Implementation
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Feb 21, 2015
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

#include "gr-isatec/PulseGenerator.h"

#include <deque>
#include <mutex>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for performing guided scrambling operations.
	namespace Isatec
	{
		//! Pulse generating implementation class for GNU Radio
		/*!
		 * @date		Feb 21, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class PulseGenerator_impl: public PulseGenerator
		{
		public:
			//! Access constellation
			/*!
			 * @return	Constellation pattern
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			const std::vector<std::complex<float>>& constellation() const;

			//! Set constellation
			/*!
			 * @param	[in] constellation Desired constellation pattern
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_constellation(const std::vector<std::complex<float>>& constellation);

			//! Access baud rate
			/*!
			 * @return	Currently set baud rate
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			double baudRate() const;

			//! Set baud rate
			/*!
			 * @param	[in] baudRate Desired baud rate
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_baudRate(const double baudRate);

			//! Access number of taps
			/*!
			 * @return	Currently set number of taps
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			unsigned int numberOfTaps() const;

			//! Set number of taps
			/*!
			 * @param	[in] numberOfTaps Desired number of taps
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_numberOfTaps(const unsigned int numberOfTaps);

			//! Access excess bandwidth factor
			/*!
			 * @return	Currently set excess bandwidth factor
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			double alpha() const;

			//! Set excess bandwidth factor
			/*!
			 * Note that this value should be in the range of [0,1]
			 *
			 * @param	[in] alpha Desired excess bandwidth factor
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_alpha(const double alpha);

			//! Access pulse shape
			/*!
			 * @return	Currently set pulse shape
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			unsigned int shape() const;

			//! Set pulse shape
			/*!
			 * @param	[in] shape Desired pulse shape
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_shape(const unsigned int shape);

			//! Enable trigger tags
			/*!
			 * @param	[in] enable True to enable, false to disable
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void enable_tags(const bool enable);

			//! Access symbol phase shift
			/*!
			 * @return	Currently set symbol phase shift
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			float phase() const;

			//! Set symbol phase shift
			/*!
			 * @param	[in] phase Desired symbol phase shift
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_phase(const float phase);

			int work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items);

			PulseGenerator_impl(unsigned int samplesPerSymbol);

		private:
			mutable std::mutex m_mutex;
			const unsigned int m_samplesPerSymbol;
			std::vector<std::complex<float>> m_constellation;
			double m_baudRate;
			unsigned int m_numberOfTaps;
			double m_alpha; 
			float m_phase;
			float m_currentPhase;

			unsigned int m_shape;

			std::vector<float> m_taps;
			std::deque<std::complex<float>> m_input;
			bool m_valid;
			bool m_tag;
		};
	}
}
