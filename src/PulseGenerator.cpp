/*!
 * @file			PulseGenerator_gr.cpp
 * @brief		Defines the GNU Radio Symbol Generator
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

#include <gnuradio/io_signature.h>
#include <gnuradio/filter/firdes.h>

#include <deque>
#include <mutex>

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for performing guided scrambling operations.
	namespace Isatec
	{
		//! Performs guided scrambling operations in GNU radio applications.
		/*!
		 * @date		Feb 21, 2015
		 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
		 */
		class Isatec_API PulseGenerator_impl: public PulseGenerator
		{
		public:
			//! Access constellation
			/*!
			 * @return	Constellation pattern
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			const std::vector<std::complex<float>>& constellation() const
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				return m_constellation;
			}

			//! Set constellation
			/*!
			 * @param	[in] constellation Desired constellation pattern
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_constellation(const std::vector<std::complex<float>>& constellation)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_constellation = constellation;
			}

			//! Access baud rate
			/*!
			 * @return	Currently set baud rate
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			double baudRate() const
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				return m_baudRate;
			}

			//! Set baud rate
			/*!
			 * @param	[in] baudRate Desired baud rate
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_baudRate(const double baudRate)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_baudRate = baudRate;
				m_valid=false;
			}

			//! Access number of taps
			/*!
			 * @return	Currently set number of taps
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			unsigned int numberOfTaps() const
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				return m_numberOfTaps;
			}

			//! Set number of taps
			/*!
			 * @param	[in] numberOfTaps Desired number of taps
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_numberOfTaps(const unsigned int numberOfTaps)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_numberOfTaps = numberOfTaps;
				m_valid=false;
			}

			//! Access excess bandwidth factor
			/*!
			 * @return	Currently set excess bandwidth factor
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			double alpha() const
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				return m_alpha;
			}

			//! Set excess bandwidth factor
			/*!
			 * Note that this value should be in the range of [0,1]
			 *
			 * @param	[in] alpha Desired excess bandwidth factor
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_alpha(const double alpha)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_alpha = alpha;
				m_valid=false;
			}

			//! Access pulse type
			/*!
			 * @return	Currently set pulse type
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			unsigned int type() const
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				return m_type;
			}

			//! Set pulse type
			/*!
			 * @param	[in] type Desired pulse type
			 * @date		Feb 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_type(const unsigned int type)
			{ 
				std::lock_guard<std::mutex> lock(m_mutex);
				m_type=type;
			}

			//! Enable trigger tags
			/*!
			 * @param	[in] enable True to enable, false to disable
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void enable_tags(const bool enable)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_tag = enable;
			}

			//! Access symbol phase shift
			/*!
			 * @return	Currently set symbol phase shift
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			float phase() const
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				return m_phase;
			}

			//! Set symbol phase shift
			/*!
			 * @param	[in] phase Desired symbol phase shift
			 * @date		Feb 22, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			void set_phase(const float phase)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_phase = phase;
				m_currentPhase = 0;
			}

			int work(int noutput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if(!m_valid)
				{
					m_taps = gr::filter::firdes::root_raised_cosine(
							m_samplesPerSymbol,
							m_baudRate*m_samplesPerSymbol,
							m_baudRate,
							m_alpha,
							m_numberOfTaps);
					m_input.clear();
					m_input.resize(1+(m_numberOfTaps-1)/m_samplesPerSymbol, 0);
					m_taps.resize(m_input.size()*m_samplesPerSymbol, 0);
					m_valid=true;
				}

				const Symbol* const start = reinterpret_cast<const Symbol*>(input_items[0]);
				const Symbol* const end = reinterpret_cast<const Symbol*>(input_items[0])+noutput_items/m_samplesPerSymbol;

				std::complex<float>* sample = reinterpret_cast<std::complex<float>*>(output_items[0]);
				size_t sampleCount=0;

				switch(m_type)
				{
					case 1:
					{
						for(const Symbol* input=start; input!=end; ++input)
						{
							m_input.pop_back();
							m_input.push_front(m_constellation[*input]);
							if(m_phase != 0)
							{
								m_input.front() = std::polar(std::abs(m_input.front()), std::arg(m_input.front())+m_currentPhase);
								m_currentPhase += m_phase;
								if(m_currentPhase>=2*pi)
									m_currentPhase -= 2*pi;
								else if(m_currentPhase <= -2*pi)
									m_currentPhase += 2*pi;
							}
							for(unsigned int i=0; i<m_samplesPerSymbol; ++i)
							{
								*sample = 0;
								auto tap=m_taps.cbegin()+i;
								for(const auto& symbol: m_input)
								{
									*sample += symbol * (*tap);
									tap += m_samplesPerSymbol;
								}
								++sample;
								if(m_tag && i==0)
								{
									this->add_item_tag(
											0,
											this->nitems_written(0)+sampleCount,
											pmt::string_to_symbol("symbol"),
											pmt::from_bool(true));
									sampleCount += m_samplesPerSymbol;
								}
							}
						}

						break;
					}

					default:
					{
						for(const Symbol* input=start; input!=end; ++input)
							for(unsigned int i=0; i<m_samplesPerSymbol; ++sample, ++i)
							{
								*sample = m_constellation[*input];
								if(m_phase != 0)
								{
									*sample = std::polar(std::abs(*sample), std::arg(*sample)+m_currentPhase);
									m_currentPhase += m_phase;
									if(m_currentPhase >= 2*pi)
										m_currentPhase -= 2*pi;
									else if(m_currentPhase <= -2*pi)
										m_currentPhase += 2*pi;
								}
								if(m_tag && i==0)
								{
									this->add_item_tag(
											0,
											this->nitems_written(0)+sampleCount,
											pmt::string_to_symbol("symbol"),
											pmt::from_bool(true));
									sampleCount += m_samplesPerSymbol;
								}
							}
						break;
					}
				}

				return noutput_items;
			}

			PulseGenerator_impl(unsigned int samplesPerSymbol):
				gr::sync_interpolator("Pulse Generator",
					io_signature::make(1,1,sizeof(Symbol)),
					io_signature::make(1,1,sizeof(std::complex<float>)),
					samplesPerSymbol),
				m_samplesPerSymbol(samplesPerSymbol),
				m_phase(0),
				m_currentPhase(0),
				m_type(0),
				m_valid(false),
				m_tag(false)
			{
			}

		private:
			mutable std::mutex m_mutex;
			const unsigned int m_samplesPerSymbol;
			std::vector<std::complex<float>> m_constellation;
			double m_baudRate;
			unsigned int m_numberOfTaps;
			double m_alpha; 
			float m_phase;
			float m_currentPhase;

			unsigned int m_type;

			std::vector<float> m_taps;
			std::deque<std::complex<float>> m_input;
			bool m_valid;
			bool m_tag;
		};
	}
}

gr::Isatec::PulseGenerator::sptr gr::Isatec::PulseGenerator::make(unsigned int samplesPerSymbol)
{
	return gnuradio::get_initial_sptr(new PulseGenerator_impl(samplesPerSymbol));
}

const std::vector<std::string>& gr::Isatec::PulseGenerator::types()
{
	static const std::vector<std::string> output= { "Rectangular", "Root-Raised Cosine" };
	return output;
}
