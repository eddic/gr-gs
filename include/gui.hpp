#ifndef GUI_HPP
#define GUI_HPP

#include <QDialog>
#include "ui_gs.h"

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

class Gui: public QDialog
{
	Q_OBJECT

private:
	Ui::Gui m_ui;

	gr::top_block_sptr m_top;
	gr::qtgui::freq_sink_c::sptr m_fft;
	gr::qtgui::const_sink_c::sptr m_constellation;
	gr::qtgui::time_sink_c::sptr m_eye;
	gr::blocks::throttle::sptr m_throttle;
	gr::blocks::add_cc::sptr m_adder;
	gr::analog::noise_source_c::sptr m_noise;
	gr::Isatec::GuidedScrambler::sptr m_guidedScrambler;
	gr::Isatec::SymbolGenerator::sptr m_symbolGenerator;
	gr::Isatec::PulseGenerator::sptr m_pulseGenerator;

	bool m_running;

	void pause();
	void unpause();
	unsigned int m_pauses;
public:
	Gui();
	~Gui();

private slots:
	void start();
	void fieldSize();
	void codewordLength();
	void augmentingLength();
	void scrambler();
	void selectionMethod();
	void blockEncoding();
	void bypass();

	void constellation();
	void baudRate();
	void samplesPerSymbol();
	void frequency();
	void weightings();
	void pulseShape();
	void excessBandwidth();
	void taps();
	void noise();
	void phase();
};

#endif
