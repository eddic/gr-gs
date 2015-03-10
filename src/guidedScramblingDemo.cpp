/*!
 * @file       guidedScramblingDemo.cpp
 * @brief      Defines the Guided Scrambling demo application
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 10, 2015
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

#include "guidedScramblingDemo.hpp"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   gr::Isatec::Applications::GuidedScramblingDemo demo;

   demo.show();

   return app.exec();
}

#include <sstream>
#include <cmath>

gr::Isatec::Applications::GuidedScramblingDemo::GuidedScramblingDemo():
   m_top(gr::make_top_block("Guided Scrambler")),
   m_fft(gr::qtgui::freq_sink_c::make(
         4096,
         gr::filter::firdes::WIN_BLACKMAN_HARRIS,
         50e6,
         2*100e3*2,
         "Spectral Density",
         1,
         this)),
   m_constellation(gr::qtgui::const_sink_c::make(
         1024,
         "Constellation Pattern",
         1,
         this)),
   m_eye(gr::qtgui::time_sink_c::make(
         1024,
         2*100e3*2,
         "Baseband Time Domain",
         1,
         this)),
   m_throttle(gr::blocks::throttle::make(sizeof(std::complex<float>), 100e3)),
   m_adder(gr::blocks::add_cc::make()),
   m_noise(gr::analog::noise_source_c::make(gr::analog::GR_GAUSSIAN, 0, 1984)),
   m_guidedScrambler(gr::Isatec::GuidedScrambler::make()),
   m_symbolGenerator(gr::Isatec::SymbolGenerator::make()),
   m_pulseGenerator(gr::Isatec::PulseGenerator::make(2)),
   m_running(false),
   m_pauses(0)
{
   // Setup the main UI
   {
      m_ui.setupUi(this);
      this->setWindowFlags(Qt::Window);
   }

   // Setup the FFT block
   {
      m_fft->enable_menu(true);
      m_fft->set_line_label(0, "Modulated Signal");
      m_ui.spectralDensityLayout->addWidget(m_fft->qwidget());
   }

   // Setup the constellation block
   {
      m_constellation->enable_menu(true);
      m_constellation->set_line_style(0, 1);
      m_constellation->set_line_marker(0, 0);
      m_constellation->set_line_label(0, "Baseband Signal");
      m_constellation->set_y_axis(-1, 1);
      m_constellation->set_x_axis(-1, 1);
      m_ui.constellationPatternLayout->addWidget(m_constellation->qwidget());
   }

   // Setup the time sink lock
   {
      m_eye->enable_menu(true);
      m_eye->set_trigger_mode(
            gr::qtgui::TRIG_MODE_TAG,
            gr::qtgui::TRIG_SLOPE_POS,
            0,
            0,
            0,
            "symbol");
      m_eye->set_line_label(0, "In Phase");
      m_eye->set_line_label(1, "Quadrature");
      //m_eye->set_update_time(1);
      m_eye->enable_tags(0, false);
      m_eye->enable_tags(1, false);
      m_eye->set_y_axis(-1, 1);
      m_ui.eyePatternLayout->addWidget(m_eye->qwidget());
   }

   // Setup the selection combo box
   {
      const std::vector<std::string>& selectionMethods = gr::Isatec::GuidedScrambler::selectionMethods();
      for(size_t i=0; i<selectionMethods.size(); ++i)
         m_ui.selectionMethod->insertItem(i, QString::fromStdString(selectionMethods[i]));
   }

   // Setup the pulse shape radio buttons
   {
      m_ui.pulseShape->setId(m_ui.rectangular, 0);
      m_ui.pulseShape->setId(m_ui.rootRaisedCosine, 1);
   }

   // Setup the field size spinbox
   {
      unsigned int value=0;
      unsigned int power;
      do
         power=std::pow(2,value++);
      while(power < gr::Isatec::GuidedScrambling::maxFieldSize);
      --value;

      m_ui.fieldSize->setMinimum(1);
      m_ui.fieldSize->setMaximum(value);
   }

   // Setup the flowgraph
   {
      m_top->connect(m_symbolGenerator, 0, m_guidedScrambler, 0);
      m_top->connect(m_guidedScrambler, 0, m_pulseGenerator, 0);
      m_top->connect(m_pulseGenerator, 0, m_adder, 0);
      m_top->connect(m_noise, 0, m_adder, 1);
      m_top->connect(m_adder, 0, m_throttle, 0);
      m_top->connect(m_throttle, 0, m_fft, 0);
      m_top->connect(m_throttle, 0, m_constellation, 0);
      m_top->connect(m_throttle, 0, m_eye, 0);
   }

   // Initialize the blocks
   {
      fieldSize();
      codewordLength();
      augmentingLength();
      scrambler();
      selectionMethod();
      blockEncoding();
      constellation();
      baudRate();
      samplesPerSymbol();
      frequency();
      weightings();
      excessBandwidth();
      taps();
      pulseShape();
   }

   // Connect Signals
   {
      QObject::connect(m_ui.start, SIGNAL(clicked()), this, SLOT(start()));
      QObject::connect(m_ui.fieldSize, SIGNAL(valueChanged(int)), this, SLOT(fieldSize()));
      QObject::connect(m_ui.codewordLength, SIGNAL(valueChanged(int)), this, SLOT(codewordLength()));
      QObject::connect(m_ui.augmentingLength, SIGNAL(valueChanged(int)), this, SLOT(augmentingLength()));
      QObject::connect(m_ui.scrambler, SIGNAL(textChanged(const QString&)), this, SLOT(scrambler()));
      QObject::connect(m_ui.selectionMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionMethod()));
      QObject::connect(m_ui.blockEncoding, SIGNAL(clicked()), this, SLOT(blockEncoding()));
      QObject::connect(m_ui.constellation, SIGNAL(textChanged(const QString&)), this, SLOT(constellation()));
      QObject::connect(m_ui.baudRate, SIGNAL(valueChanged(double)), this, SLOT(baudRate()));
      QObject::connect(m_ui.samplesPerSymbol, SIGNAL(valueChanged(int)), this, SLOT(samplesPerSymbol()));
      QObject::connect(m_ui.frequency, SIGNAL(valueChanged(double)), this, SLOT(frequency()));
      QObject::connect(m_ui.weightings, SIGNAL(textChanged(const QString&)), this, SLOT(weightings()));
      QObject::connect(m_ui.bypass, SIGNAL(clicked()), this, SLOT(bypass()));
      QObject::connect(m_ui.pulseShape, SIGNAL(buttonClicked(int)), this, SLOT(pulseShape()));
      QObject::connect(m_ui.excessBandwidth, SIGNAL(valueChanged(double)), this, SLOT(excessBandwidth()));
      QObject::connect(m_ui.taps, SIGNAL(valueChanged(int)), this, SLOT(taps()));
      QObject::connect(m_ui.noise, SIGNAL(valueChanged(int)), this, SLOT(noise()));
      QObject::connect(m_ui.phase, SIGNAL(valueChanged(double)), this, SLOT(phase()));
      QObject::connect(m_ui.usrp, SIGNAL(clicked()), this, SLOT(usrp()));
      QObject::connect(m_ui.rxAntenna, SIGNAL(textChanged(const QString&)), this, SLOT(usrp()));
      QObject::connect(m_ui.txAntenna, SIGNAL(textChanged(const QString&)), this, SLOT(usrp()));
      QObject::connect(m_ui.subdevice, SIGNAL(textChanged(const QString&)), this, SLOT(usrp()));
      QObject::connect(m_ui.txGain, SIGNAL(valueChanged(double)), this, SLOT(txGain()));
      QObject::connect(m_ui.rxGain, SIGNAL(valueChanged(double)), this, SLOT(rxGain()));
   }
}

gr::Isatec::Applications::GuidedScramblingDemo::~GuidedScramblingDemo()
{
   if(m_running)
   {
      m_top->stop();
      m_top->wait();
      m_running=false;
   }
}

void gr::Isatec::Applications::GuidedScramblingDemo::start()
{
   if(m_running)
   {
      m_top->stop();
      m_top->wait();
      m_running=false;
      m_ui.start->setText("Start");
      m_ui.status->setText("Not currently running");
   }
   else
   {
      m_top->start();
      m_running=true;
      m_ui.start->setText("Stop");
      m_ui.status->setText("Running");
   }
}

void gr::Isatec::Applications::GuidedScramblingDemo::fieldSize()
{
   m_guidedScrambler->set_fieldSize(std::pow(2,m_ui.fieldSize->value()));
   scrambler();
   constellation();
   weightings();
}

void gr::Isatec::Applications::GuidedScramblingDemo::codewordLength()
{
   m_guidedScrambler->set_codewordLength(m_ui.codewordLength->value());
   if(m_guidedScrambler->codewordLength() <= m_guidedScrambler->augmentingLength())
      m_ui.augmentingLength->setValue(m_guidedScrambler->codewordLength()-1);
}

void gr::Isatec::Applications::GuidedScramblingDemo::augmentingLength()
{
   m_guidedScrambler->set_augmentingLength(m_ui.augmentingLength->value());
   if(m_guidedScrambler->codewordLength() <= m_guidedScrambler->augmentingLength())
      m_ui.codewordLength->setValue(m_guidedScrambler->augmentingLength()+1);
}

void gr::Isatec::Applications::GuidedScramblingDemo::scrambler()
{
   std::istringstream ss(m_ui.scrambler->text().toStdString());
   std::vector<gr::Isatec::Symbol> polynomial;
   unsigned short int symbol;
   const unsigned int maxSymbol = std::pow(2,m_ui.fieldSize->value());

   ss >> symbol;
   while(!ss.fail() && symbol <= maxSymbol)
   {
      polynomial.push_back(symbol);
      ss >> symbol;
   }

   if(ss.eof() && polynomial.size()>1 && symbol < maxSymbol)
   {
      m_guidedScrambler->set_divider(polynomial);
      m_ui.scramblerLabel->setStyleSheet("");
   }
   else
      m_ui.scramblerLabel->setStyleSheet("QLabel { color: red; }");
}

void gr::Isatec::Applications::GuidedScramblingDemo::selectionMethod()
{
   m_guidedScrambler->set_selectionMethod(m_ui.selectionMethod->currentText().toStdString());
}

void gr::Isatec::Applications::GuidedScramblingDemo::blockEncoding()
{
   m_guidedScrambler->set_continuous(!m_ui.blockEncoding->isChecked());
}

void gr::Isatec::Applications::GuidedScramblingDemo::constellation()
{
   std::istringstream ss(m_ui.constellation->text().toStdString());
   std::vector<std::complex<float>> array;
   std::complex<float> symbol;

   ss >> symbol;
   while(!ss.fail())
   {
      array.push_back(symbol);
      ss >> symbol;
   }

   if(array.size() == std::pow(2,unsigned(m_ui.fieldSize->value())))
   {
      m_guidedScrambler->set_constellation(array);
      m_ui.constellationLabel->setStyleSheet("");
      m_pulseGenerator->set_constellation(array);
   }
   else
      m_ui.constellationLabel->setStyleSheet("QLabel { color: red; }");
}

void gr::Isatec::Applications::GuidedScramblingDemo::baudRate()
{
   m_throttle->set_sample_rate(1e3*m_ui.baudRate->value()*m_ui.samplesPerSymbol->value());
   m_eye->set_samp_rate(1e3*m_ui.baudRate->value()*m_ui.samplesPerSymbol->value());
   m_eye->set_nsamps(m_ui.samplesPerSymbol->value()*16);
   m_pulseGenerator->set_baudRate(1e3*m_ui.baudRate->value());
   if(m_usrpSink)
      m_usrpSink->set_samp_rate(1e3*m_ui.baudRate->value()*m_ui.samplesPerSymbol->value());
   frequency();
   bandwidth();
}

void gr::Isatec::Applications::GuidedScramblingDemo::frequency()
{
   m_fft->set_frequency_range(1e6*m_ui.frequency->value(), 2e3*m_ui.baudRate->value()*m_ui.samplesPerSymbol->value());
   if(m_usrpSink)
      m_usrpSink->set_center_freq(1e6*m_ui.frequency->value());
}

void gr::Isatec::Applications::GuidedScramblingDemo::samplesPerSymbol()
{
   pause();

   // Rebuild the pulse generator
   if(!m_pulseGenerator.unique())
      m_top->disconnect(m_pulseGenerator);
   m_pulseGenerator = gr::Isatec::PulseGenerator::make(m_ui.samplesPerSymbol->value());
   m_pulseGenerator->enable_tags(true);
   m_top->connect(m_pulseGenerator, 0, m_adder, 0);

   bypass();
   constellation();
   baudRate();
   frequency();
   excessBandwidth();
   taps();
   pulseShape();
   unpause();
}

void gr::Isatec::Applications::GuidedScramblingDemo::weightings()
{
   std::istringstream ss(m_ui.weightings->text().toStdString());
   std::vector<double> array;
   double symbol;

   ss >> symbol;
   while(!ss.fail())
   {
      array.push_back(symbol);
      ss >> symbol;
   }

   if(array.size() <= std::pow(2,unsigned(m_ui.fieldSize->value())))
      m_ui.weightingsLabel->setStyleSheet("");
   else
   {
      m_ui.weightingsLabel->setStyleSheet("QLabel { color: red; }");
      array.resize(std::pow(2,unsigned(m_ui.fieldSize->value())));
   }
   m_symbolGenerator->set_weightings(array);
}

void gr::Isatec::Applications::GuidedScramblingDemo::pause()
{
   if(m_pauses==0)
   {
      m_ui.start->setEnabled(false);
      if(m_running)
      {
         m_top->stop();
         m_top->wait();
      }
   }
   ++m_pauses;
}

void gr::Isatec::Applications::GuidedScramblingDemo::unpause()
{
   if(m_pauses==1)
   {
      if(m_running)
      {
         m_top->start();
      }
      m_ui.start->setEnabled(true);
   }
   --m_pauses;
}

void gr::Isatec::Applications::GuidedScramblingDemo::bypass()
{
   pause();

   if(m_ui.bypass->isChecked())
   {
      if(!m_guidedScrambler.unique())
         m_top->disconnect(m_guidedScrambler);
      m_top->connect(m_symbolGenerator, 0, m_pulseGenerator, 0);
   }
   else
   {
      if(!m_symbolGenerator.unique())
         m_top->disconnect(m_symbolGenerator);
      m_top->connect(m_symbolGenerator, 0, m_guidedScrambler, 0);
      m_top->connect(m_guidedScrambler, 0, m_pulseGenerator, 0);
   }

   unpause();
}

void gr::Isatec::Applications::GuidedScramblingDemo::pulseShape()
{
   m_pulseGenerator->set_shape(m_ui.pulseShape->checkedId());
   if(m_pulseGenerator->shape()>0)
   {
      m_ui.excessBandwidth->setEnabled(true);
      m_ui.taps->setEnabled(true);
   }
   else
   {
      m_ui.excessBandwidth->setEnabled(false);
      m_ui.taps->setEnabled(false);
   }
   bandwidth();
}

void gr::Isatec::Applications::GuidedScramblingDemo::excessBandwidth()
{
   m_pulseGenerator->set_alpha(m_ui.excessBandwidth->value());
   bandwidth();
}

void gr::Isatec::Applications::GuidedScramblingDemo::taps()
{
   m_pulseGenerator->set_numberOfTaps(m_ui.taps->value());
}

void gr::Isatec::Applications::GuidedScramblingDemo::noise()
{
   const int& value = m_ui.noise->value();
   const int& maximum = m_ui.noise->maximum();
   m_noise->set_amplitude(value==0?0:5*std::pow(10,(-1+double(value)/maximum)*5));
}

void gr::Isatec::Applications::GuidedScramblingDemo::phase()
{
   m_pulseGenerator->set_phase(m_ui.phase->value()*pi/180.0);
}

void gr::Isatec::Applications::GuidedScramblingDemo::usrp()
{
   pause();
   if(m_usrpSink)
   {
      if(!m_usrpSink.unique())
         m_top->disconnect(m_usrpSink);
      m_usrpSink.reset();
   }
   if(m_usrpSource)
   {
      if(!m_usrpSource.unique())
         m_top->disconnect(m_usrpSource);
      m_usrpSource.reset();
   }
   if(!m_fft.unique())
      m_top->disconnect(m_fft);
   if(!m_constellation.unique())
      m_top->disconnect(m_constellation);
   if(!m_eye.unique())
      m_top->disconnect(m_eye);
   if(!m_throttle.unique())
      m_top->disconnect(m_throttle);

   bool throttle=true;
   if(m_ui.usrp->isChecked())
   {
      try
      {
         // Set up USRP Sink
         m_usrpSink = gr::uhd::usrp_sink::make(::uhd::device_addr_t(m_ui.address->text().toStdString()), ::uhd::stream_args_t("fc32", "sc16"));
         m_usrpSink->set_subdev_spec(m_ui.subdevice->text().toStdString());
         m_usrpSink->set_antenna(m_ui.txAntenna->text().toStdString());
         m_usrpSink->set_center_freq(1e6*m_ui.frequency->value());
         m_usrpSink->set_samp_rate(1e3*m_ui.baudRate->value()*m_ui.samplesPerSymbol->value());
         m_ui.txGain->setMinimum(m_usrpSink->get_gain_range().start());
         m_ui.txGain->setMaximum(m_usrpSink->get_gain_range().stop());
         m_usrpSink->set_gain(m_ui.txGain->value());

         // Set up USRP Source
         m_usrpSource = gr::uhd::usrp_source::make(::uhd::device_addr_t(m_ui.address->text().toStdString()), ::uhd::stream_args_t("fc32", "sc16"));
         m_usrpSource->set_subdev_spec(m_ui.subdevice->text().toStdString());
         m_usrpSource->set_antenna(m_ui.rxAntenna->text().toStdString());
         m_usrpSource->set_center_freq(1e6*m_ui.frequency->value());
         m_usrpSource->set_samp_rate(1e3*m_ui.baudRate->value()*m_ui.samplesPerSymbol->value());
         m_ui.rxGain->setMinimum(m_usrpSource->get_gain_range().start());
         m_ui.rxGain->setMaximum(m_usrpSource->get_gain_range().stop());
         m_usrpSource->set_gain(m_ui.rxGain->value());

         throttle = false;
      }
      catch(...)
      {
         m_usrpSink.reset();
         m_usrpSource.reset();
         m_ui.usrp->setChecked(false);
      }
      if(!throttle)
      {
         m_top->connect(m_adder, 0, m_usrpSink, 0);

         m_top->connect(m_usrpSource, 0, m_fft, 0);
         m_top->connect(m_usrpSource, 0, m_constellation, 0);
         m_top->connect(m_usrpSource, 0, m_eye, 0);
      }
   }

   if(throttle)
   {
      m_top->connect(m_adder, 0, m_throttle, 0);
      m_top->connect(m_throttle, 0, m_fft, 0);
      m_top->connect(m_throttle, 0, m_constellation, 0);
      m_top->connect(m_throttle, 0, m_eye, 0);
   }
   unpause();
}

void gr::Isatec::Applications::GuidedScramblingDemo::txGain()
{
   if(m_usrpSink)
      m_usrpSink->set_gain(m_ui.txGain->value());
}

void gr::Isatec::Applications::GuidedScramblingDemo::rxGain()
{
   if(m_usrpSource)
      m_usrpSource->set_gain(m_ui.rxGain->value());
}

void gr::Isatec::Applications::GuidedScramblingDemo::bandwidth()
{
   if(m_usrpSink)
   {
      double bandwidth;
      if(m_pulseGenerator->shape()>0)
         bandwidth=m_ui.baudRate->value()*(1+m_ui.excessBandwidth->value());
      else
         bandwidth=m_ui.baudRate->value()*m_ui.samplesPerSymbol->value();
      m_usrpSink->set_bandwidth(bandwidth);
   }
}
