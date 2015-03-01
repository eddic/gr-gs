#include "Analyzer.hpp"
#include "gr-isatec/Exceptions.hpp"

#include "MSW.hpp"

const std::vector<std::string> gr::Isatec::GuidedScrambling::Analyzer::names = { "MSW" };

gr::Isatec::GuidedScrambling::Analyzer* gr::Isatec::GuidedScrambling::manufactureAnalyzer(const unsigned int method)
{
	switch(method)
	{
		case 0:
			return new gr::Isatec::GuidedScrambling::MSW;
		default:
			throw Exceptions::BadSelectionMethod();
	}
}

gr::Isatec::GuidedScrambling::Analyzer::Feedback* gr::Isatec::GuidedScrambling::manufactureFeedback(const unsigned int method)
{
	switch(method)
	{
		case 0:
			return new gr::Isatec::GuidedScrambling::MSW::Feedback();
		default:
			throw Exceptions::BadSelectionMethod();
	}
}
