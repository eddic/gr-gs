/*!
 * @file       audioReceiver.hpp
 * @brief      Declares the Audio Receiver demo application
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       May 12, 2015
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

#ifndef AUDIORECEIVER_HPP
#define AUDIORECEIVER_HPP

#include <QDialog>
#include <QTimer>
#include "ui_audioReceiver.h"

#include <gnuradio/top_block.h>

#include <gnuradio/qtgui/freq_sink_c.h>
#include <gnuradio/qtgui/freq_sink_f.h>
#include <gnuradio/qtgui/const_sink_c.h>
#include <gnuradio/qtgui/time_sink_c.h>
#include <gnuradio/qtgui/time_sink_f.h>
#include <gnuradio/qtgui/waterfall_sink_c.h>
#include <gnuradio/qtgui/waterfall_sink_f.h>

#include <gnuradio/uhd/usrp_source.h>

#include <gnuradio/filter/fft_filter_ccc.h>
#include <gnuradio/filter/fft_filter_ccf.h>
#include <gnuradio/blocks/multiply_const_cc.h>

#include <gnuradio/analog/pll_carriertracking_cc.h>
#include <gnuradio/digital/costas_loop_cc.h>

#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/filter/iir_filter_ffd.h>
#include <gnuradio/blocks/complex_to_arg.h>
#include <gnuradio/blocks/complex_to_real.h>
#include <gnuradio/blocks/complex_to_imag.h>
#include <gnuradio/blocks/complex_to_mag.h>

#include <gnuradio/filter/rational_resampler_base_fff.h>
#include <gnuradio/blocks/multiply_const_ff.h>

#include <gnuradio/audio/sink.h>
#include <gnuradio/blocks/wavfile_sink.h>
#include <gnuradio/blocks/null_sink.h>


//! GNU Radio Namespace
namespace gr
{
   //! Contains all blocks for the %Isatec GNU Radio Module
   namespace Isatec
   {
      //! Contains all full demo applications for the %Isatec GNU Radio Module
      namespace Applications
      {
         //! Generic Audio Receiver Application
         /*!
          * This application allows the operator to receive a wide variety of
          * different signal and both visualize and listen to them.
          *
          * @date    May 12, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         class AudioReceiver: public QDialog
         {
            Q_OBJECT

         private:
            Ui::AudioReceiver m_ui; //!< Main window user interface made in QT Designer

            gr::top_block_sptr m_top; //!< The GNU Radio top block

            class USRP
            {
            private:
               gr::uhd::usrp_source::sptr m_block; //!< Our physical input device
               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group

               //! Set actual value labels
               void setActuals();

               //! Get the current USRP gain
               double gain() const;

               //! Get the current USRP bandwidth
               double bandwidth() const;

            public:
               //! Configure the USRP device 
               void configure();
               
               //! Get the current USRP center frequency
               double frequency() const;

               //! Get the current USRP sample rate
               unsigned int sampleRate() const;

               //! Connect to, or disconnect from the USRP device
               bool connect();

               //! Shared pointer to actual block
               gr::basic_block_sptr block();

               USRP(Ui::AudioReceiver& ui):
                  m_ui(ui)
               {}
            };
            USRP m_usrp;

            class Passband
            {
            private:
               gr::filter::fft_filter_ccc::sptr m_complex; //!< Complex tap FIR filter (for SSB)
               gr::filter::fft_filter_ccf::sptr m_float; //!< Real tap FIR filter
               gr::blocks::multiply_const_cc::sptr m_mult; //!< Single tap filter
               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group
               USRP& m_usrp; //!< Reference to USRP device
               unsigned int m_decimation; //!< Currect decimation value
               int m_sideband; //!< Current sideband value
               gr::top_block_sptr& m_top;

            public:
               //! Configure the passband filter
               void configure();

               //! Initialize the passband filter
               void initialize();

               //! Get the current output sample rate
               unsigned int sampleRate() const;
               
               //! Do we need to teardown the flowgraph?
               bool disconnect() const;

               //! Connect the blocks
               void connect();

               //! Shared pointer to actual block
               gr::basic_block_sptr block();

               Passband(Ui::AudioReceiver& ui, USRP& usrp, gr::top_block_sptr& top):
                  m_ui(ui),
                  m_usrp(usrp),
                  m_decimation(0),
                  m_sideband(-1),
                  m_top(top)
               {}
            };
            Passband m_passband;

            class Carrier
            {
            private:
               gr::analog::pll_carriertracking_cc::sptr m_pll; //!< Phase locked loop
               gr::digital::costas_loop_cc::sptr m_costas; //!< Costas loop
               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group
               Passband& m_passband; //!< Reference to the upstream passband filter
               gr::top_block_sptr& m_top;

            public:
               //! Configure the carrier recovery block
               void configure();

               //! Initialize the carrier recovery block
               void initialize();

               //! Do we need to teardown the flowgraph?
               bool disconnect() const;

               //! Connect the blocks
               void connect();

               //! Get the current output sample rate
               unsigned int sampleRate() const;
               
               //! Shared pointer to actual block
               gr::basic_block_sptr block();

               Carrier(Ui::AudioReceiver& ui, Passband& passband, gr::top_block_sptr& top):
                  m_ui(ui),
                  m_passband(passband),
                  m_top(top)
               {}
            };
            Carrier m_carrier;

            class Detection
            {
            private:
               gr::blocks::complex_to_arg::sptr m_pm; //!< PM Demodulator
               gr::analog::quadrature_demod_cf::sptr m_fm; //!< FM Demodulator
               gr::blocks::complex_to_real::sptr m_inphase; //!< In Phase AM
               gr::blocks::complex_to_imag::sptr m_quadrature; //!< Quadrature AM
               gr::blocks::complex_to_mag::sptr m_magnitude; //!< Magnitude AM

               gr::filter::iir_filter_ffd::sptr m_iir; //!< IIR for deemphasis

               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group
               Carrier& m_carrier; //!< Reference to the upstream carrier detecter filter
               gr::top_block_sptr& m_top;

            public:
               //! Configure the detection filter
               void configure();

               //! Initialize the detection filter
               void initialize();

               //! Do we need to teardown the flowgraph?
               bool disconnect() const;

               //! Connect the blocks
               void connect();

               //! Get the current output sample rate
               unsigned int sampleRate() const;

               //! Shared pointer to actual block
               gr::basic_block_sptr block();

               Detection(Ui::AudioReceiver& ui, Carrier& carrier, gr::top_block_sptr& top):
                  m_ui(ui),
                  m_carrier(carrier),
                  m_top(top)
               {}
            };
            Detection m_detection;

            class Baseband
            {
            private:
               gr::filter::rational_resampler_base_fff::sptr m_fir; //!< FIR filter
               gr::blocks::multiply_const_ff::sptr m_mult; //!< Single tap filter
               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group
               Detection& m_detection; //!< Reference to the upstream detection filter
               gr::top_block_sptr& m_top;
               unsigned int m_decimation; //!< Currect decimation value
               unsigned int m_interpolation; //!< Currect interpolation value

            public:
               //! Configure the baseband filter
               void configure();

               //! Initialize the baseband filter
               void initialize();

               //! Shared pointer to actual block
               gr::basic_block_sptr block();

               //! Return output sample rate
               unsigned int sampleRate();

               //! Do we need to teardown the flowgraph?
               bool disconnect() const;

               //! Connect the blocks
               void connect();

               Baseband(Ui::AudioReceiver& ui, Detection& detection, gr::top_block_sptr& top):
                  m_ui(ui),
                  m_detection(detection),
                  m_top(top),
                  m_decimation(0),
                  m_interpolation(0)
               {}
            };
            Baseband m_baseband;

            class Audio
            {
            private:
               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group
               gr::audio::sink::sptr m_audio; //!< Audio output block
               gr::blocks::wavfile_sink::sptr m_wavfile; //!< Wavefile output
               gr::blocks::null_sink::sptr m_null; //!< Null output

               unsigned int m_sampleRate; //!< Current sample rate
               Baseband& m_baseband; //!< Reference to the upstream baseband filter
               gr::top_block_sptr& m_top;
               unsigned int m_seconds;

            public:
               //! Configure the baseband filter
               void configure();

               //! Initialize the baseband filter
               void initialize();

               //! Do we need to teardown the flowgraph?
               bool disconnect() const;

               //! Connect the blocks
               void connect();

               //! Start an audio recording
               void record();

               //! Second timer
               QTimer m_timer;

               //! Time clocker
               void timer();

               Audio(Ui::AudioReceiver& ui, Baseband& baseband, gr::top_block_sptr& top):
                  m_ui(ui),
                  m_sampleRate(0),
                  m_baseband(baseband),
                  m_top(top)
               {}
            };
            Audio m_audio;

            class Display
            {
            private:
               gr::qtgui::freq_sink_c::sptr m_fft_c; //!< Complex FFT display
               gr::qtgui::freq_sink_f::sptr m_fft_f; //!< Real FFT display
               gr::qtgui::const_sink_c::sptr m_constellation; //!< Constellation pattern display
               gr::qtgui::time_sink_f::sptr m_time; //!< Real time domain display
               gr::qtgui::waterfall_sink_c::sptr m_waterfall_c; //!< Complex waterfall display
               gr::qtgui::waterfall_sink_f::sptr m_waterfall_f; //!< Real waterfall display

               Ui::AudioReceiver& m_ui; //!< Reference to our Ui Group
               unsigned int m_probe; //!< Current probe connection
               USRP& m_usrp;
               Passband& m_passband;
               Carrier& m_carrier;
               Detection& m_detection;
               Baseband& m_baseband;
               gr::top_block_sptr& m_top;

            public:
               //! Do we need to teardown the flowgraph?
               bool disconnect() const;

               //! Configure the display filter
               void configure();

               //! Initialize the display filter
               void initialize(QWidget* parent);

               //! Connect the probe
               void connect();

               Display(
                     Ui::AudioReceiver& ui,
                     USRP& usrp,
                     Passband& passband,
                     Carrier& carrier,
                     Detection& detection,
                     Baseband& baseband,
                     gr::top_block_sptr& top):
                  m_ui(ui),
                  m_probe(0),
                  m_usrp(usrp),
                  m_passband(passband),
                  m_carrier(carrier),
                  m_detection(detection),
                  m_baseband(baseband),
                  m_top(top)
               {}
            };
            Display m_display;

            void disableSignals();
            void enableSignals();

            bool m_running;

         public:
            AudioReceiver();
            ~AudioReceiver();
            static unsigned int audioSampleRate(int index);

         private slots:
            void reconfigure();
            void connect();
            void folder();
            void record();
            void timer();
         };
      }
   }
}

#endif
