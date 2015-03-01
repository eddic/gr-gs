#include "gr-isatec/config.h"
#include "MSW.hpp"

gr::Isatec::GuidedScrambling::Analyzer::Feedback* gr::Isatec::GuidedScrambling::MSW::Feedback::clone() const
{
	return new Feedback(*this);
}

gr::Isatec::GuidedScrambling::MSW::Feedback::Feedback():
	RDS(std::complex<float>(STARTINGRDS))
{}

void gr::Isatec::GuidedScrambling::MSW::analyze(const std::vector<Symbol>& codeword, const Analyzer::Feedback& feedback, const std::vector<std::complex<float>>& constellation)
{
	m_feedback.RDS = static_cast<const Feedback&>(feedback).RDS;
	m_analysis = 0;

	for(const Symbol& symbol: codeword)
	{
		m_feedback.RDS += constellation[symbol];
		m_analysis += std::norm(m_feedback.RDS);
	}
}

const gr::Isatec::GuidedScrambling::Analyzer::Feedback& gr::Isatec::GuidedScrambling::MSW::feedback() const
{
	return m_feedback;
}

float gr::Isatec::GuidedScrambling::MSW::analysis() const
{
	return m_analysis;
}
