/*!
 * @file			PulseGenerator_impl.cpp
 * @brief		Declares the "Pulse Generator" GNU Radio block implementation
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 5, 2015
 * @copyright	Copyright &copy; 2015 %Isatec Inc.  This project is released
 *					under the GNU General Public License Version 3.
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

#include "PulseGenerator_impl.hpp"

#include <gnuradio/io_signature.h>
#include <gnuradio/filter/firdes.h>

#include <deque>
#include <mutex>

const std::vector<std::complex<float>>& gr::Isatec::Implementations::PulseGenerator_impl::constellation() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_constellation;
}

void gr::Isatec::Implementations::PulseGenerator_impl::set_constellation(const std::vector<std::complex<float>>& constellation)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_constellation = constellation;
}

double gr::Isatec::Implementations::PulseGenerator_impl::baudRate() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_baudRate;
}

void gr::Isatec::Implementations::PulseGenerator_impl::set_baudRate(const double baudRate)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_baudRate = baudRate;
	m_valid=false;
}

unsigned int gr::Isatec::Implementations::PulseGenerator_impl::numberOfTaps() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_numberOfTaps;
}

void gr::Isatec::Implementations::PulseGenerator_impl::set_numberOfTaps(const unsigned int numberOfTaps)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_numberOfTaps = numberOfTaps;
	m_valid=false;
}

double gr::Isatec::Implementations::PulseGenerator_impl::alpha() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_alpha;
}

void gr::Isatec::Implementations::PulseGenerator_impl::set_alpha(const double alpha)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_alpha = alpha;
	m_valid=false;
}

unsigned int gr::Isatec::Implementations::PulseGenerator_impl::shape() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_shape;
}

void gr::Isatec::Implementations::PulseGenerator_impl::set_shape(const unsigned int shape)
{ 
	std::lock_guard<std::mutex> lock(m_mutex);
	m_shape=shape;
}

void gr::Isatec::Implementations::PulseGenerator_impl::enable_tags(const bool enable)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_tag = enable;
}

float gr::Isatec::Implementations::PulseGenerator_impl::phase() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_phase;
}

void gr::Isatec::Implementations::PulseGenerator_impl::set_phase(const float phase)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_phase = phase;
	m_currentPhase = 0;
}

int gr::Isatec::Implementations::PulseGenerator_impl::work(int noutput_items,
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

	switch(m_shape)
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

gr::Isatec::Implementations::PulseGenerator_impl::PulseGenerator_impl(unsigned int samplesPerSymbol):
	gr::sync_interpolator("Pulse Generator",
		io_signature::make(1,1,sizeof(Symbol)),
		io_signature::make(1,1,sizeof(std::complex<float>)),
		samplesPerSymbol),
	m_samplesPerSymbol(samplesPerSymbol),
	m_constellation(defaultConstellation(4)),
	m_baudRate(100e3),
	m_numberOfTaps(1024),
	m_alpha(0.5),
	m_phase(0),
	m_currentPhase(0),
	m_shape(0),
	m_valid(false),
	m_tag(false)
{
}

gr::Isatec::PulseGenerator::sptr gr::Isatec::PulseGenerator::make(unsigned int samplesPerSymbol)
{
	return gnuradio::get_initial_sptr(new Implementations::PulseGenerator_impl(samplesPerSymbol));
}

const std::vector<std::string>& gr::Isatec::PulseGenerator::shapes()
{
	static const std::vector<std::string> output= { "Rectangular", "Root-Raised Cosine" };
	return output;
}
