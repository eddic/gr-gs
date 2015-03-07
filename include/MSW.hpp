/*!
 * @file       MSW.hpp
 * @brief      Declares the gr::Isatec::GuidedScrambling::MSW class
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 3, 2015
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

#ifndef MSW_HPP
#define MSW_HPP

#include <complex>

#include "Analyzer.hpp"

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations
		namespace GuidedScrambling
		{
			//! Performs %MSW (mean squared weight) analysis of a codeword
			/*!
			 * @date    March 3, 2015
			 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class MSW: public Analyzer
			{
			public:
				//! %Feedback mechanism for %MSW (mean squared weight) codeword analysis
				/*!
				 * Here is where we store and pass on the RDS (running digital sum) value
				 * remaining at the end of the analysis.
				 *
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				class Feedback: public Analyzer::Feedback
				{
				public:
					//! Clone %MSW feedback data
					/*!
					 * @return  Pointer to dynamically allocated %MSW Feedback copy.
					 * @date    March 3, 2015
					 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
					 */
					Analyzer::Feedback* clone() const;

					Feedback();

					std::complex<float> RDS; //!< RDS (running digital sum) value at end of codeword
				};

				//! Perform %MSW analysis on a codeword
				/*!
				 * This actually performs the %MSW analysis on the passed codeword using
				 * the feedback provided. After calling this function the feedback() and
				 * analysis() accessors become valid.
				 *
				 * @param   [in] codeword The codeword to perform analysis on.
				 * @param   [in] feedback The feedback from the winning codeword to start
				 *          the analysis with.
				 * @param   [in] constellation This is a direct mapping of symbols (as
				 *          vector indices) to constellation points.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void analyze(
						const std::vector<Symbol>& codeword,
						const Analyzer::Feedback& feedback,
						const std::vector<std::complex<float>>& constellation);

				//! Extract feedback from %MSW analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function.
				 *
				 * @return  Constant reference to internal Feedback object.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const Analyzer::Feedback& feedback() const;

				//! Extract result from %MSW analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function. In this context a lower analysis means a better
				 * codeword.
				 *
				 * @return  Floating point representation of the codeword's %MSW
				 *          analysis. Lower is better.
				 * @date    March 3, 2015
				 * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				float analysis() const;
			private:
				Feedback m_feedback; //!< %Feedback from %MSW analysis
				float m_analysis; //!< Actual result from %MSW analysis
			};
		}
	}
}

#endif
