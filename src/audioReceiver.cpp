/*!
 * @file       audioReceiver.cpp
 * @brief      Defines the Audio Receiver application
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

#include "audioReceiver.hpp"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   gr::Isatec::Applications::AudioReceiver gr_app;

   gr_app.show();

   return app.exec();
}

#include <sstream>
#include <ctime>
#include <cmath>
#include <gnuradio/filter/firdes.h>
#include <thread>

#include <QMessageBox>
#include <QFileDialog>

// gr::Isatec::Applications::AudioReceiver::USRP Functions

double gr::Isatec::Applications::AudioReceiver::USRP::gain() const
{
   if(m_block)
      return m_block->get_gain();
   else
      return m_ui.usrpGain->value();
}

double gr::Isatec::Applications::AudioReceiver::USRP::frequency() const
{
   if(m_block)
      return m_block->get_center_freq();
   else
      return m_ui.usrpFrequency->value()*1e6;
}

double gr::Isatec::Applications::AudioReceiver::USRP::bandwidth() const
{
   if(m_block)
      return m_block->get_bandwidth();
   else
      return m_ui.usrpBandwidth->value()*1e3;
}

unsigned int gr::Isatec::Applications::AudioReceiver::USRP::sampleRate() const
{
   if(m_block)
      return m_block->get_samp_rate();
   else
      return m_ui.usrpSampleRate->value()*1000;
}

bool gr::Isatec::Applications::AudioReceiver::USRP::connect()
{
   bool tearDown=true;

   if(m_ui.usrpConnect->isChecked() && !m_block)
   {
      try
      {
         m_block = gr::uhd::usrp_source::make(::uhd::device_addr_t(m_ui.usrpAddress->text().toStdString()), ::uhd::stream_args_t("fc32", "sc16"));
         m_block->set_subdev_spec(m_ui.usrpSubdevice->text().toStdString());
         m_block->set_antenna(m_ui.usrpAntenna->text().toStdString());

         m_ui.usrpGain->setEnabled(true);
         m_ui.usrpGain->setMinimum(m_block->get_gain_range().start());
         m_ui.usrpGain->setMaximum(m_block->get_gain_range().stop());
         m_ui.usrpGain->setValue(m_block->get_gain());

         m_ui.usrpFrequency->setEnabled(true);
         m_ui.usrpFrequency->setMinimum(m_block->get_freq_range().start()/1e6);
         m_ui.usrpFrequency->setMaximum(m_block->get_freq_range().stop()/1e6);
         m_ui.usrpFrequency->setValue(m_block->get_center_freq()/1e6);

         m_ui.usrpBandwidth->setEnabled(true);
         m_ui.usrpBandwidth->setMinimum(m_block->get_bandwidth_range().start()/1e3);
         m_ui.usrpBandwidth->setMaximum(m_block->get_bandwidth_range().stop()/1e3);
         m_ui.usrpBandwidth->setValue(m_block->get_bandwidth()/1e6);

         m_ui.usrpSampleRate->setEnabled(true);
         m_ui.usrpSampleRate->setMinimum(m_block->get_samp_rates().start()/1e3);
         m_ui.usrpSampleRate->setMaximum(m_block->get_samp_rates().stop()/1e3);
         m_ui.usrpSampleRate->setValue(m_block->get_samp_rate()/1e3);

         m_ui.usrpAddress->setEnabled(false);
         m_ui.usrpSubdevice->setEnabled(false);
         m_ui.usrpAntenna->setEnabled(false);

         tearDown=false;
      }
      catch(std::exception& e)
      {
         m_ui.usrpConnect->setChecked(false);
         QMessageBox::warning(nullptr, tr("Audio Receiver"), QString(e.what()), QMessageBox::Ok);
      }
      catch(...)
      {
         m_ui.usrpConnect->setChecked(false);
         QMessageBox::warning(nullptr, tr("Audio Receiver"), tr("Caught unknown exception while trying to connect."), QMessageBox::Ok);
      }
   }

   if(tearDown)
   {
      m_block.reset();

      m_ui.usrpAddress->setEnabled(true);
      m_ui.usrpSubdevice->setEnabled(true);
      m_ui.usrpAntenna->setEnabled(true);
      
      m_ui.usrpGain->setEnabled(false);
      m_ui.usrpGain->setMinimum(0);
      m_ui.usrpGain->setValue(0);
      
      m_ui.usrpFrequency->setEnabled(false);
      m_ui.usrpFrequency->setMinimum(0);
      m_ui.usrpFrequency->setValue(0);
      
      m_ui.usrpBandwidth->setEnabled(false);
      m_ui.usrpBandwidth->setMinimum(0);
      m_ui.usrpBandwidth->setValue(0);
      
      m_ui.usrpSampleRate->setEnabled(false);
      m_ui.usrpSampleRate->setMinimum(0);
      m_ui.usrpSampleRate->setValue(0);
   }

   setActuals();
   return !tearDown;
}

void gr::Isatec::Applications::AudioReceiver::USRP::setActuals()
{
   std::ostringstream ss;
   ss << std::setprecision(3) << std::fixed << gain();
   m_ui.usrpGainActual->setText(QString::fromStdString(ss.str()));

   ss.str(std::string());
   ss << std::setprecision(6) << std::fixed << frequency()/1e6;
   m_ui.usrpFrequencyActual->setText(QString::fromStdString(ss.str()));

   ss.str(std::string());
   ss << std::setprecision(3) << std::fixed << bandwidth()/1e3;
   m_ui.usrpBandwidthActual->setText(QString::fromStdString(ss.str()));

   ss.str(std::string());
   ss << std::setprecision(3) << std::fixed << sampleRate()/1e3;
   m_ui.usrpSampleRateActual->setText(QString::fromStdString(ss.str()));
}

void gr::Isatec::Applications::AudioReceiver::USRP::configure()
{
   m_block->set_gain(m_ui.usrpGain->value());
   m_block->set_center_freq(m_ui.usrpFrequency->value()*1e6);
   m_block->set_bandwidth(m_ui.usrpBandwidth->value()*1e3);
   m_block->set_samp_rate(m_ui.usrpSampleRate->value()*1e3);
   setActuals();
}

gr::basic_block_sptr gr::Isatec::Applications::AudioReceiver::USRP::block()
{
   return m_block;
}

// gr::Isatec::Applications::AudioReceiver::Passband Functions

unsigned int gr::Isatec::Applications::AudioReceiver::Passband::sampleRate() const
{
   return m_usrp.sampleRate()/(m_mult?1:m_ui.passbandDecimation->value());
}

void gr::Isatec::Applications::AudioReceiver::Passband::initialize()
{
   m_ui.passbandSideband->insertItem(0, QString("Both"));
   m_ui.passbandSideband->insertItem(1, QString("Upper"));
   m_ui.passbandSideband->insertItem(2, QString("Lower"));
   m_ui.passbandSideband->setCurrentIndex(0);
   
   m_ui.passbandType->insertItem(0, QString("Flat"));
   m_ui.passbandType->insertItem(1, QString("Hamming"));
   m_ui.passbandType->insertItem(2, QString("Hann"));
   m_ui.passbandType->insertItem(3, QString("Blackman"));
   m_ui.passbandType->insertItem(4, QString("Rectangular"));
   m_ui.passbandType->insertItem(5, QString("Kaiser"));
   m_ui.passbandType->insertItem(6, QString("Blackman-Harris"));
   m_ui.passbandType->setCurrentIndex(0);
}

gr::basic_block_sptr gr::Isatec::Applications::AudioReceiver::Passband::block()
{
   if(m_complex)
      return m_complex;
   else if(m_float)
      return m_float;
   else
      return m_mult;
}

bool gr::Isatec::Applications::AudioReceiver::Passband::disconnect() const
{
   if(m_ui.passbandType->currentIndex()>0)
   {
      if(m_mult || m_decimation != unsigned(m_ui.passbandDecimation->value()) || m_sideband != m_ui.passbandSideband->currentIndex())
         return true;
   }
   else if(!m_mult)
      return true;

   return false;
}

void gr::Isatec::Applications::AudioReceiver::Passband::configure()
{
   const double gain=std::pow(10, m_ui.passbandGain->value()/20);

   if(m_float)
   {
      const std::vector<float> taps = gr::filter::firdes::low_pass(
            gain,
            m_usrp.sampleRate(),
            m_ui.passbandBandwidth->value()*1e3,
            m_ui.passbandTransition->value(),
            gr::filter::firdes::win_type(m_ui.passbandType->currentIndex()-1),
            m_ui.passbandBeta->value());
      m_float->set_taps(taps);
      m_ui.passbandTaps->display(int(taps.size()));
   }
   else if(m_complex)
   {
      double lowCut, highCut;
      if(m_ui.passbandSideband->currentIndex() == 1)
      {
         lowCut = 0;
         highCut = m_ui.passbandBandwidth->value()*1e3;
      }
      else
      {
         highCut = 0;
         lowCut = -m_ui.passbandBandwidth->value()*1e3;
      }

      std::vector<std::complex<float>> taps = gr::filter::firdes::complex_band_pass(
            gain,
            m_usrp.sampleRate(),
            lowCut,
            highCut,
            m_ui.passbandTransition->value(),
            gr::filter::firdes::win_type(m_ui.passbandType->currentIndex()-1),
            m_ui.passbandBeta->value());
      m_complex->set_taps(taps);
      m_ui.passbandTaps->display(int(taps.size()));
   }
   else if(m_mult)
   {
      m_mult->set_k(gain);
      m_ui.passbandTaps->display(1);
   }
}

void gr::Isatec::Applications::AudioReceiver::Passband::connect()
{
   m_mult.reset();
   m_complex.reset();
   m_float.reset();

   m_decimation = m_ui.passbandDecimation->value();
   m_sideband = m_ui.passbandSideband->currentIndex();

   if(m_ui.passbandType->currentIndex()>0)
   {
      if(m_ui.passbandSideband->currentIndex() == 0)
      {
         const std::vector<float> taps(m_decimation, 1);
         m_float = gr::filter::fft_filter_ccf::make(
               m_decimation,
               taps,
               std::thread::hardware_concurrency());
      }
      else
      {
         const std::vector<std::complex<float>> taps(m_decimation, std::complex<float>(1,0));
         m_complex = gr::filter::fft_filter_ccc::make(
               m_decimation,
               taps,
               std::thread::hardware_concurrency());
      }
   }
   else
      m_mult = gr::blocks::multiply_const_cc::make(1);

   std::ostringstream ss;
   ss << std::setprecision(3) << std::fixed << sampleRate()/1e3;
   m_ui.passbandSampleRate->display(QString::fromStdString(ss.str()));

   m_top->connect(m_usrp.block(), 0, block(), 0);
}

// gr::Isatec::Applications::AudioReceiver::Carrier Functions

void gr::Isatec::Applications::AudioReceiver::Carrier::initialize()
{
   m_ui.carrierMethod->insertItem(0, QString("None"));
   m_ui.carrierMethod->insertItem(1, QString("PLL"));
   m_ui.carrierMethod->insertItem(2, QString("Costas Loop"));
   m_ui.carrierMethod->setCurrentIndex(0);
   m_ui.carrierOrder->insertItem(0, QString("2"));
   m_ui.carrierOrder->insertItem(1, QString("4"));
   m_ui.carrierOrder->insertItem(2, QString("8"));
   m_ui.carrierOrder->setCurrentIndex(0);
}

gr::basic_block_sptr gr::Isatec::Applications::AudioReceiver::Carrier::block()
{
   switch(m_ui.carrierMethod->currentIndex())
   {
      case 1:
         return m_pll;
      case 2:
         return m_costas;
      default:
         return m_passband.block();
   }
}

void gr::Isatec::Applications::AudioReceiver::Carrier::configure()
{
   if(m_pll)
   {
      m_pll->set_loop_bandwidth(m_ui.carrierBW->value()/m_passband.sampleRate());
      m_pll->set_max_freq(m_ui.carrierMaxFreq->value()/m_passband.sampleRate());
      m_pll->set_min_freq(m_ui.carrierMinFreq->value()/m_passband.sampleRate());
   }
   else if(m_costas)
   {
   }
}

bool gr::Isatec::Applications::AudioReceiver::Carrier::disconnect() const
{
   switch(m_ui.carrierMethod->currentIndex())
   {
      case 1:
         if(!m_pll)
            return true;
         break;

      case 2:
         if(!m_costas)
            return true;
         break;
   }
   return false;
}

void gr::Isatec::Applications::AudioReceiver::Carrier::connect()
{
   m_pll.reset();
   m_costas.reset();

   switch(m_ui.carrierMethod->currentIndex())
   {
      case 1:
         m_pll = gr::analog::pll_carriertracking_cc::make(
               m_ui.carrierBW->value()/m_passband.sampleRate(),
               m_ui.carrierMaxFreq->value()/m_passband.sampleRate(),
               m_ui.carrierMinFreq->value()/m_passband.sampleRate());
         m_top->connect(m_passband.block(), 0, m_pll, 0);
         break;

      case 2:
         m_costas = gr::digital::costas_loop_cc::make(
               m_ui.carrierBW->value()/m_passband.sampleRate(),
               std::pow(2, m_ui.carrierOrder->currentIndex()+1));
         m_top->connect(m_passband.block(), 0, m_costas, 0);
         break;

      default:
         break;
   }
}

unsigned int gr::Isatec::Applications::AudioReceiver::Carrier::sampleRate() const
{
   return m_passband.sampleRate();
}

// gr::Isatec::Applications::AudioReceiver::Detection Functions

void gr::Isatec::Applications::AudioReceiver::Detection::initialize()
{
   m_ui.detectionMethod->insertItem(0, QString("In-phase AM"));
   m_ui.detectionMethod->insertItem(1, QString("Quadrature AM"));
   m_ui.detectionMethod->insertItem(2, QString("Magnitude AM"));
   m_ui.detectionMethod->insertItem(3, QString("PM"));
   m_ui.detectionMethod->insertItem(4, QString("FM"));
   m_ui.detectionMethod->setCurrentIndex(4);
}

void gr::Isatec::Applications::AudioReceiver::Detection::configure()
{
   if(m_ui.detectionMethod->currentIndex() == 4 && m_ui.detectionDeemphasis->isChecked())
   {
        const double wp = 1e6/m_ui.detectionTau->value();
        const double wpp = std::tan(wp / (sampleRate() * 2));

        const double a = (wpp - 1)/(wpp + 1);
        const double b = wpp/(1 + wpp);

        const std::vector<double> btaps = {b, b};
        const std::vector<double> ataps = {1, a};
        m_iir->set_taps(btaps, ataps);
   }
}

bool gr::Isatec::Applications::AudioReceiver::Detection::disconnect() const
{
   switch(m_ui.detectionMethod->currentIndex())
   {
      case 0:
         if(!m_inphase)
            return true;
         break;
      
      case 1:
         if(!m_quadrature)
            return true;
         break;

      case 2:
         if(!m_magnitude)
            return true;
         break;

      case 3:
         if(!m_pm)
            return true;
         break;

      case 4:
         if(!m_fm)
            return true;
         if(m_ui.detectionDeemphasis->isChecked() && !m_iir)
            return true;
         if(!m_ui.detectionDeemphasis->isChecked() && m_iir)
            return true;
         break;
   }
   return false;
}

void gr::Isatec::Applications::AudioReceiver::Detection::connect()
{
   m_inphase.reset();
   m_quadrature.reset();
   m_magnitude.reset();
   m_pm.reset();
   m_fm.reset();
   m_iir.reset();
   switch(m_ui.detectionMethod->currentIndex())
   {
      case 0:
         m_inphase = gr::blocks::complex_to_real::make();
         m_top->connect(m_carrier.block(), 0, m_inphase, 0);
         break;

      case 1:
         m_quadrature = gr::blocks::complex_to_imag::make();
         m_top->connect(m_carrier.block(), 0, m_quadrature, 0);
         break;

      case 2:
         m_magnitude = gr::blocks::complex_to_mag::make();
         m_top->connect(m_carrier.block(), 0, m_magnitude, 0);
         break;

      case 3:
         m_pm = gr::blocks::complex_to_arg::make();
         m_top->connect(m_carrier.block(), 0, m_pm, 0);
         break;

      case 4:
         m_fm = gr::analog::quadrature_demod_cf::make(m_ui.detectionDeemphasis->isChecked()?10:1);
         m_top->connect(m_carrier.block(), 0, m_fm, 0);
         if(m_ui.detectionDeemphasis->isChecked())
         {
            const std::vector<double> ataps(1, 1);
            const std::vector<double> btaps(1, 1);
            m_iir = gr::filter::iir_filter_ffd::make(btaps, ataps);
            m_top->connect(m_fm, 0, m_iir, 0);
         }
         break;
   }
}

gr::basic_block_sptr gr::Isatec::Applications::AudioReceiver::Detection::block()
{
   switch(m_ui.detectionMethod->currentIndex())
   {
      case 1:
         return m_quadrature;
      case 2:
         return m_magnitude;
      case 3:
         return m_pm;
      case 4:
         if(m_ui.detectionDeemphasis->isChecked())
            return m_iir;
         else
            return m_fm;
      default:
         return m_inphase;
   }
}

unsigned int gr::Isatec::Applications::AudioReceiver::Detection::sampleRate() const
{
   return m_carrier.sampleRate();
}

// gr::Isatec::Applications::AudioReceiver::Baseband Functions

void gr::Isatec::Applications::AudioReceiver::Baseband::initialize()
{
   m_ui.basebandType->insertItem(0, QString("Flat"));
   m_ui.basebandType->insertItem(1, QString("Hamming"));
   m_ui.basebandType->insertItem(2, QString("Hann"));
   m_ui.basebandType->insertItem(3, QString("Blackman"));
   m_ui.basebandType->insertItem(4, QString("Rectangular"));
   m_ui.basebandType->insertItem(5, QString("Kaiser"));
   m_ui.basebandType->insertItem(6, QString("Blackman-Harris"));
   m_ui.basebandType->setCurrentIndex(0);
}

void gr::Isatec::Applications::AudioReceiver::Baseband::configure()
{
   const double gain=std::pow(10, m_ui.basebandGain->value()/20);

   if(m_ui.basebandType->currentIndex()>0)
   {
      const std::vector<float> taps = gr::filter::firdes::band_pass(
                  gain,
                  m_detection.sampleRate()*m_interpolation,
                  m_ui.basebandLowCutoff->value(),
                  m_ui.basebandHighCutoff->value()*1e3,
                  m_ui.basebandTransition->value(),
                  gr::filter::firdes::win_type(m_ui.basebandType->currentIndex()-1),
                  m_ui.basebandBeta->value());
      m_fir->set_taps(taps);
      m_ui.basebandTaps->display(int(taps.size()));
   }
   else
   {
      m_mult->set_k(gain);
      m_ui.basebandTaps->display(1);
   }
}

gr::basic_block_sptr gr::Isatec::Applications::AudioReceiver::Baseband::block()
{
   if(m_ui.basebandType->currentIndex()>0)
      return m_fir;
   return m_mult;
}

unsigned int gr::Isatec::Applications::AudioReceiver::Baseband::sampleRate()
{
   if(m_ui.basebandType->currentIndex()>0)
      return audioSampleRate(m_ui.audioSampleRate->currentIndex());
   else
      return m_detection.sampleRate();
}

bool gr::Isatec::Applications::AudioReceiver::Baseband::disconnect() const
{
   if(m_ui.basebandType->currentIndex()>0)
   {
      unsigned int x = m_detection.sampleRate();
      unsigned int y = audioSampleRate(m_ui.audioSampleRate->currentIndex());
      unsigned int remainder;
      while (x != 0)
      {
         remainder = y % x;
         y = x;
         x = remainder;
      }
      const unsigned int decimation = m_detection.sampleRate()/y;
      const unsigned int interpolation = audioSampleRate(m_ui.audioSampleRate->currentIndex())/y;

      if(m_decimation == decimation && m_interpolation == interpolation && m_fir)
         return false;
   }
   else if(m_mult)
      return false;
   return true;
}

void gr::Isatec::Applications::AudioReceiver::Baseband::connect()
{
   m_mult.reset();
   m_fir.reset();

   {
      unsigned int x = m_detection.sampleRate();
      unsigned int y = audioSampleRate(m_ui.audioSampleRate->currentIndex());
      unsigned int remainder;
      while (x != 0)
      {
         remainder = y % x;
         y = x;
         x = remainder;
      }
      m_decimation = m_detection.sampleRate()/y;
      m_interpolation = audioSampleRate(m_ui.audioSampleRate->currentIndex())/y;
   }

   if(m_ui.basebandType->currentIndex()>0)
   {
      const std::vector<float> taps(m_decimation, 1);
      m_fir = gr::filter::rational_resampler_base_fff::make(
            m_interpolation,
            m_decimation,
            taps);
      m_top->connect(m_detection.block(), 0, m_fir, 0);
      m_ui.basebandDecimation->display(int(m_decimation));
      m_ui.basebandInterpolation->display(int(m_interpolation));
   }
   else
   {
      m_mult = gr::blocks::multiply_const_ff::make(1);
      m_top->connect(m_detection.block(), 0, m_mult, 0);
      m_ui.basebandDecimation->display(1);
      m_ui.basebandInterpolation->display(1);
   }
}

// gr::Isatec::Applications::AudioReceiver::Audio Functions

void gr::Isatec::Applications::AudioReceiver::Audio::initialize()
{
   m_ui.audioSampleSize->insertItem(0, QString("8"));
   m_ui.audioSampleSize->insertItem(1, QString("16"));
   m_ui.audioSampleSize->setCurrentIndex(1);

   m_ui.audioSampleRate->insertItem(0, QString("8000"));
   m_ui.audioSampleRate->insertItem(1, QString("44100"));
   m_ui.audioSampleRate->insertItem(2, QString("48000"));
   m_ui.audioSampleRate->setCurrentIndex(2);
}

void gr::Isatec::Applications::AudioReceiver::Audio::configure()
{
   if(m_wavfile)
      m_wavfile->set_bits_per_sample(8+8*m_ui.audioSampleSize->currentIndex());
}

bool gr::Isatec::Applications::AudioReceiver::Audio::disconnect() const
{
   if(m_baseband.sampleRate() == m_sampleRate && !m_ui.audioBypass->isChecked())
   {
      if(m_audio && m_wavfile)
         return false;
   }
   else if(m_null)
      return false;
   return true;
}

void gr::Isatec::Applications::AudioReceiver::Audio::connect()
{
   m_audio.reset();
   m_wavfile.reset();
   m_null.reset();

   m_sampleRate = audioSampleRate(m_ui.audioSampleRate->currentIndex());

   if(m_baseband.sampleRate() == m_sampleRate && !m_ui.audioBypass->isChecked())
   {
      m_wavfile = gr::blocks::wavfile_sink::make("/dev/null", 1, m_sampleRate, 8+8*m_ui.audioSampleSize->currentIndex());
      m_wavfile->close();
      m_top->connect(m_baseband.block(), 0, m_wavfile, 0);
      m_audio = gr::audio::sink::make(m_sampleRate, "pulse", false);
      m_top->connect(m_baseband.block(), 0, m_audio, 0);
   }
   else
   {
      m_null = gr::blocks::null_sink::make(sizeof(float));
      m_top->connect(m_baseband.block(), 0, m_null, 0);
   }
}

void gr::Isatec::Applications::AudioReceiver::Audio::record()
{
   if(m_wavfile)
   {
      if(m_ui.audioRecord->isChecked())
      {
         char time_buf[20];
         std::time_t now = time(0);
         std::strftime(time_buf, 21, "%Y-%m-%d-%H:%S:%M", std::localtime(&now));
         m_wavfile->open((m_ui.audioFolder->text().toStdString() + '/' + time_buf + ".wav").c_str());
         m_seconds = 0;
         m_ui.audioRecordLength->display(int(m_seconds));
         m_timer.start(1000);
      }
      else
      {
         m_timer.stop();
         m_wavfile->close();
      }
   }
}

void gr::Isatec::Applications::AudioReceiver::Audio::timer()
{
   m_ui.audioRecordLength->display(int(++m_seconds));
   m_timer.start(1000);
}

// gr::Isatec::Applications::AudioReceiver::Display Functions

void gr::Isatec::Applications::AudioReceiver::Display::initialize(QWidget* parent)
{
   m_ui.probe->setId(m_ui.usrpProbe, 1);
   m_ui.probe->setId(m_ui.passbandProbe, 2);
   m_ui.probe->setId(m_ui.carrierProbe, 3);
   m_ui.probe->setId(m_ui.detectionProbe, 4);
   m_ui.probe->setId(m_ui.basebandProbe, 5);

   m_fft_c = gr::qtgui::freq_sink_c::make(
         4096,
         gr::filter::firdes::WIN_BLACKMAN_HARRIS,
         1,
         1,
         "Spectrum",
         1,
         parent);
   m_ui.plotsTop->addWidget(m_fft_c->qwidget());

   m_fft_f = gr::qtgui::freq_sink_f::make(
         4096,
         gr::filter::firdes::WIN_BLACKMAN_HARRIS,
         0,
         1,
         "Spectrum",
         1,
         parent);
   m_fft_f->set_plot_pos_half(true);
   m_ui.plotsTop->addWidget(m_fft_f->qwidget());
   m_fft_f->qwidget()->hide();

   m_constellation = gr::qtgui::const_sink_c::make(
         1024,
         "Constellation Pattern",
         1,
         parent);
   m_constellation->set_line_style(0, 1);
   m_constellation->set_line_marker(0, 0);
   m_ui.plotsBottom->addWidget(m_constellation->qwidget());

   m_time = gr::qtgui::time_sink_f::make(
         1024,
         1,
         "Baseband Time Domain",
         1,
         parent),
   m_time->set_trigger_mode(
         gr::qtgui::TRIG_MODE_FREE,
         gr::qtgui::TRIG_SLOPE_POS,
         0,
         0,
         0);
   m_ui.plotsBottom->addWidget(m_time->qwidget());
   m_time->qwidget()->hide();

   m_waterfall_c = gr::qtgui::waterfall_sink_c::make(
         4096,
         gr::filter::firdes::WIN_BLACKMAN_HARRIS,
         1,
         1,
         "Waterfall",
         1,
         parent);
   m_ui.plotsBottom->addWidget(m_waterfall_c->qwidget());

   m_waterfall_f = gr::qtgui::waterfall_sink_f::make(
         4096,
         gr::filter::firdes::WIN_BLACKMAN_HARRIS,
         0,
         1,
         "Waterfall",
         1,
         parent);
   m_waterfall_f->set_plot_pos_half(true);
   m_ui.plotsBottom->addWidget(m_waterfall_f->qwidget());
   m_waterfall_f->qwidget()->hide();
}

bool gr::Isatec::Applications::AudioReceiver::Display::disconnect() const
{
   return m_probe != unsigned(m_ui.probe->checkedId());
}

void gr::Isatec::Applications::AudioReceiver::Display::connect()
{
   m_fft_c->qwidget()->hide();
   m_fft_f->qwidget()->hide();
   m_constellation->qwidget()->hide();
   m_time->qwidget()->hide();
   m_waterfall_c->qwidget()->hide();
   m_waterfall_f->qwidget()->hide();

   switch(m_ui.probe->checkedId())
   {
      case 1:
         m_top->connect(m_usrp.block(), 0, m_fft_c, 0);
         m_fft_c->qwidget()->show();
         m_top->connect(m_usrp.block(), 0, m_waterfall_c, 0);
         m_waterfall_c->qwidget()->show();
         m_top->connect(m_usrp.block(), 0, m_constellation, 0);
         m_constellation->qwidget()->show();
         break;
      case 2:
         m_top->connect(m_passband.block(), 0, m_fft_c, 0);
         m_fft_c->qwidget()->show();
         m_top->connect(m_passband.block(), 0, m_waterfall_c, 0);
         m_waterfall_c->qwidget()->show();
         m_top->connect(m_passband.block(), 0, m_constellation, 0);
         m_constellation->qwidget()->show();
         break;
      case 3:
         m_top->connect(m_carrier.block(), 0, m_fft_c, 0);
         m_fft_c->qwidget()->show();
         m_top->connect(m_carrier.block(), 0, m_waterfall_c, 0);
         m_waterfall_c->qwidget()->show();
         m_top->connect(m_carrier.block(), 0, m_constellation, 0);
         m_constellation->qwidget()->show();
         break;
      case 4:
         m_top->connect(m_detection.block(), 0, m_fft_f, 0);
         m_fft_f->qwidget()->show();
         m_top->connect(m_detection.block(), 0, m_waterfall_f, 0);
         m_waterfall_f->qwidget()->show();
         m_top->connect(m_detection.block(), 0, m_time, 0);
         m_time->qwidget()->show();
         break;
      case 5:
         m_top->connect(m_baseband.block(), 0, m_fft_f, 0);
         m_fft_f->qwidget()->show();
         m_top->connect(m_baseband.block(), 0, m_waterfall_f, 0);
         m_waterfall_f->qwidget()->show();
         m_top->connect(m_baseband.block(), 0, m_time, 0);
         m_time->qwidget()->show();
         break;
   }
   m_probe = m_ui.probe->checkedId();
}

void gr::Isatec::Applications::AudioReceiver::Display::configure()
{
   double frequency=0;
   unsigned int sampleRate=0;
   const char* label = "INVALID";

   switch(m_ui.probe->checkedId())
   {
      case 1:
         frequency = m_usrp.frequency();
         sampleRate = m_usrp.sampleRate();
         label = "USRP";
         break;
      case 2:
         frequency = m_usrp.frequency();
         sampleRate = m_passband.sampleRate();
         label = "Passband FIR";
         break;
      case 3:
         frequency = m_usrp.frequency();
         sampleRate = m_carrier.sampleRate();
         label = "Carrier Recovery";
         break;
      case 4:
         frequency = 0;
         sampleRate = m_detection.sampleRate();
         label = "Detection";
         break;
      case 5:
         frequency = 0;
         sampleRate = m_baseband.sampleRate();
         label = "Baseband FIR";
         break;
   }

   m_fft_c->set_frequency_range(frequency, sampleRate);
   m_fft_c->set_line_label(0, label);
   m_fft_f->set_frequency_range(frequency, sampleRate);
   m_fft_f->set_line_label(0, label);
   m_waterfall_c->set_frequency_range(frequency, sampleRate);
   m_waterfall_c->set_line_label(0, label);
   m_waterfall_f->set_frequency_range(frequency, sampleRate);
   m_waterfall_f->set_line_label(0, label);
   m_constellation->set_line_label(0, label);
   m_time->set_line_label(0, label);
   m_time->set_samp_rate(sampleRate);
}

// gr::Isatec::Applications::AudioReceiver Functions

void gr::Isatec::Applications::AudioReceiver::reconfigure()
{
   disableSignals();
   if(m_running)
   {
      try
      {
         bool restarted=false;
         if(
               m_passband.disconnect() ||
               m_carrier.disconnect() ||
               m_detection.disconnect() ||
               m_baseband.disconnect() ||
               m_audio.disconnect() ||
               m_display.disconnect())
         {
            m_top->stop();
            m_top->wait();
            m_top->disconnect_all();
            m_passband.connect();
            m_carrier.connect();
            m_detection.connect();
            m_baseband.connect();
            m_audio.connect();
            m_display.connect();
            restarted=true;
         }
         m_usrp.configure();
         m_passband.configure();
         m_carrier.configure();
         m_detection.configure();
         m_baseband.configure();
         m_audio.configure();
         m_display.configure();

         if(restarted)
            m_top->start();
      }
      catch(std::exception& e)
      {
         QMessageBox::warning(nullptr, tr("Audio Receiver"), QString(e.what()), QMessageBox::Ok);
         m_ui.usrpConnect->setChecked(false);
         m_usrp.connect();
      }
   }
   else
   {
      m_ui.usrpConnect->setChecked(false);
      m_usrp.connect();
   }
   enableSignals();
}

void gr::Isatec::Applications::AudioReceiver::connect()
{
   disableSignals();
   m_running = m_usrp.connect();
   if(m_running)
      reconfigure();
   else
   {
      m_top->stop();
      m_top->wait();
      m_top->disconnect_all();
   }
   enableSignals();
}

gr::Isatec::Applications::AudioReceiver::AudioReceiver():
   m_top(gr::make_top_block("Audio Receiver")),
   m_usrp(m_ui),
   m_passband(m_ui, m_usrp, m_top),
   m_carrier(m_ui, m_passband, m_top),
   m_detection(m_ui, m_carrier, m_top),
   m_baseband(m_ui, m_detection, m_top),
   m_audio(m_ui, m_baseband, m_top),
   m_display(m_ui, m_usrp, m_passband, m_carrier, m_detection, m_baseband, m_top),
   m_running(false)
{
   // Setup the main UI
   {
      m_ui.setupUi(this);
      this->setWindowFlags(Qt::Window);
   }

   m_passband.initialize();
   m_carrier.initialize();
   m_detection.initialize();
   m_baseband.initialize();
   m_audio.initialize();
   m_display.initialize(this);

   // Connect Signals
   {
      QObject::connect(m_ui.usrpConnect, SIGNAL(clicked()), this, SLOT(connect()));
      QObject::connect(m_ui.usrpProbe, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.usrpGain, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.usrpFrequency, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.usrpBandwidth, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.usrpSampleRate, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));

      QObject::connect(m_ui.passbandProbe, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandGain, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandType, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandSideband, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandDecimation, SIGNAL(valueChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandBandwidth, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandTransition, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.passbandBeta, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));

      QObject::connect(m_ui.carrierProbe, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.carrierMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.carrierMinFreq, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.carrierMaxFreq, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.carrierBW, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.carrierOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));

      QObject::connect(m_ui.detectionProbe, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.detectionMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.detectionDeemphasis, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.detectionTau, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));

      QObject::connect(m_ui.basebandProbe, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.basebandGain, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.basebandType, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.basebandLowCutoff, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.basebandHighCutoff, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.basebandTransition, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.basebandBeta, SIGNAL(valueChanged(double)), this, SLOT(reconfigure()));

      QObject::connect(m_ui.audioBypass, SIGNAL(clicked()), this, SLOT(reconfigure()));
      QObject::connect(m_ui.audioSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.audioSampleSize, SIGNAL(currentIndexChanged(int)), this, SLOT(reconfigure()));
      QObject::connect(m_ui.audioFolderButton, SIGNAL(clicked()), this, SLOT(folder()));
      QObject::connect(m_ui.audioRecord, SIGNAL(clicked()), this, SLOT(record()));
      QObject::connect(&m_audio.m_timer, SIGNAL(timeout()), this, SLOT(timer()));
   }
}

gr::Isatec::Applications::AudioReceiver::~AudioReceiver()
{
   if(m_running)
   {
      m_top->stop();
      m_top->wait();
      m_running=false;
   }
}

unsigned int gr::Isatec::Applications::AudioReceiver::audioSampleRate(int index)
{
   switch(index)
   {
      case 0:
         return 8000;
      case 1:
         return 44100;
      case 2:
         return 48000;
   }
   return 0;
}

void gr::Isatec::Applications::AudioReceiver::disableSignals()
{
   m_ui.usrpConnect->blockSignals(true);
   m_ui.usrpProbe->blockSignals(true);
   m_ui.usrpGain->blockSignals(true);
   m_ui.usrpFrequency->blockSignals(true);
   m_ui.usrpBandwidth->blockSignals(true);
   m_ui.usrpSampleRate->blockSignals(true);

   m_ui.passbandProbe->blockSignals(true);
   m_ui.passbandGain->blockSignals(true);
   m_ui.passbandType->blockSignals(true);
   m_ui.passbandSideband->blockSignals(true);
   m_ui.passbandDecimation->blockSignals(true);
   m_ui.passbandBandwidth->blockSignals(true);
   m_ui.passbandTransition->blockSignals(true);
   m_ui.passbandBeta->blockSignals(true);

   m_ui.carrierProbe->blockSignals(true);
   m_ui.carrierMethod->blockSignals(true);
   m_ui.carrierMinFreq->blockSignals(true);
   m_ui.carrierMaxFreq->blockSignals(true);
   m_ui.carrierBW->blockSignals(true);
   m_ui.carrierOrder->blockSignals(true);

   m_ui.detectionProbe->blockSignals(true);
   m_ui.detectionMethod->blockSignals(true);
   m_ui.detectionDeemphasis->blockSignals(true);
   m_ui.detectionTau->blockSignals(true);

   m_ui.basebandProbe->blockSignals(true);
   m_ui.basebandGain->blockSignals(true);
   m_ui.basebandType->blockSignals(true);
   m_ui.basebandLowCutoff->blockSignals(true);
   m_ui.basebandHighCutoff->blockSignals(true);
   m_ui.basebandTransition->blockSignals(true);
   m_ui.basebandBeta->blockSignals(true);

   m_ui.audioBypass->blockSignals(true);
   m_ui.audioSampleRate->blockSignals(true);
   m_ui.audioSampleSize->blockSignals(true);
}

void gr::Isatec::Applications::AudioReceiver::enableSignals()
{
   m_ui.usrpConnect->blockSignals(false);
   m_ui.usrpProbe->blockSignals(false);
   m_ui.usrpGain->blockSignals(false);
   m_ui.usrpFrequency->blockSignals(false);
   m_ui.usrpBandwidth->blockSignals(false);
   m_ui.usrpSampleRate->blockSignals(false);

   m_ui.passbandProbe->blockSignals(false);
   m_ui.passbandGain->blockSignals(false);
   m_ui.passbandType->blockSignals(false);
   m_ui.passbandSideband->blockSignals(false);
   m_ui.passbandDecimation->blockSignals(false);
   m_ui.passbandBandwidth->blockSignals(false);
   m_ui.passbandTransition->blockSignals(false);
   m_ui.passbandBeta->blockSignals(false);

   m_ui.carrierProbe->blockSignals(false);
   m_ui.carrierMethod->blockSignals(false);
   m_ui.carrierMinFreq->blockSignals(false);
   m_ui.carrierMaxFreq->blockSignals(false);
   m_ui.carrierBW->blockSignals(false);
   m_ui.carrierOrder->blockSignals(false);

   m_ui.detectionProbe->blockSignals(false);
   m_ui.detectionMethod->blockSignals(false);
   m_ui.detectionDeemphasis->blockSignals(false);
   m_ui.detectionTau->blockSignals(false);

   m_ui.basebandProbe->blockSignals(false);
   m_ui.basebandGain->blockSignals(false);
   m_ui.basebandType->blockSignals(false);
   m_ui.basebandLowCutoff->blockSignals(false);
   m_ui.basebandHighCutoff->blockSignals(false);
   m_ui.basebandTransition->blockSignals(false);
   m_ui.basebandBeta->blockSignals(false);

   m_ui.audioBypass->blockSignals(false);
   m_ui.audioSampleRate->blockSignals(false);
   m_ui.audioSampleSize->blockSignals(false);
}

void gr::Isatec::Applications::AudioReceiver::folder()
{
   disableSignals();
   m_ui.audioFolder->setText(QFileDialog::getExistingDirectory(this, tr("Select Record Folder"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
   enableSignals();
}

void gr::Isatec::Applications::AudioReceiver::record()
{
   disableSignals();
   m_audio.record();
   enableSignals();
}

void gr::Isatec::Applications::AudioReceiver::timer()
{
   disableSignals();
   m_audio.timer();
   enableSignals();
}
