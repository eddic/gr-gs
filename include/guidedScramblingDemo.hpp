/*!
 * @file			guidedScramblingDemo.hpp
 * @brief		Declares the Guided Scrambling demo application
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 6, 2015
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

#ifndef GUIDEDSCRAMBLINGDEMO_HPP
#define GUIDEDSCRAMBLINGDEMO_HPP

#include <QDialog>
#include "ui_guidedScramblingDemo.h"

#include <gnuradio/top_block.h>
#include <gnuradio/qtgui/freq_sink_c.h>
#include <gnuradio/qtgui/const_sink_c.h>
#include <gnuradio/qtgui/time_sink_c.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/blocks/add_cc.h>
#include <gnuradio/analog/noise_source_c.h>
#include "gr-isatec/GuidedScrambler.h"
#include "gr-isatec/SymbolGenerator.h"
#include "gr-isatec/PulseGenerator.h"


//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains all full demo applications for the %Isatec GNU Radio Module
		namespace Applications
		{
			//! Graphical demo of the Guided Scrambling method
			/*!
			 * This application demonstrates the spectral capabilities of the
			 * GuidedScrambler block.
			 *
			 * @date		March 6, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class GuidedScramblingDemo: public QDialog
			{
				Q_OBJECT

			private:
				Ui::GuidedScramblingDemo m_ui; //!< Main window user interface made in QT Designer

				gr::top_block_sptr m_top; //!< The GNU Radio top block
				gr::qtgui::freq_sink_c::sptr m_fft; //!< FFT display
				gr::qtgui::const_sink_c::sptr m_constellation; //!< Constellation pattern display
				gr::qtgui::time_sink_c::sptr m_eye; //!< Time domain signal view
				gr::blocks::throttle::sptr m_throttle; //!< Used to control the baud rate in the absence of a USRP
				gr::blocks::add_cc::sptr m_adder; //!< used to add the noise in
				gr::analog::noise_source_c::sptr m_noise; //!< Used to generate the noise
				gr::Isatec::GuidedScrambler::sptr m_guidedScrambler; //!< Our guided scrambler block
				gr::Isatec::SymbolGenerator::sptr m_symbolGenerator; //!< The random symbol generator block
				gr::Isatec::PulseGenerator::sptr m_pulseGenerator; //!< The pulse shaping block

				bool m_running; //!< True if the flow graph is running, false otherwise

				void pause(); //!< Pause the flow graph
				void unpause(); //!< Unpause the flow graph
				unsigned int m_pauses; //!< How many pause requests are "queued" up?
			public:
				GuidedScramblingDemo();
				~GuidedScramblingDemo();

			private slots:
				void start(); //!< Start/stop the flowgraph
				void fieldSize(); //!< Update the field (symbol) size
				void codewordLength(); //!< Update the codeword length used by the guided scrambler
				void augmentingLength(); //!< Update the augmenting length used by the guided scrambler
				void scrambler(); //!< Update the scrambling polynomial used by the guided scrambler
				void selectionMethod(); //!< Update the selection method used by the guided scrambler
				void blockEncoding(); //!< Set block or continuous encoding in the guided scrambler
				void bypass(); //!< Bypass/unbypass the guided scrambler block

				void constellation(); //!< Update the constellation pattern used by numerous blocks
				void baudRate(); //!< Update the baud rate (on the coded side of things)
				void samplesPerSymbol(); //!< Update the amount of samples per symbol to generate
				void frequency(); //!< Update the centre frequency
				void weightings(); //!< Update the random symbol probability weightings
				void pulseShape(); //!< Update the pulse shape used in pulse generation
				void excessBandwidth(); //!< Update the excess bandwidth used in pulse generation
				void taps(); //!< Update the amount of tapes used in pulse generation
				void noise(); //!< Update the noise amplitude
				void phase(); //!< Update the symbol phase shift applied in the pulse generator
			};
		}
	}
}

#endif
