/*!
 * @file			MSW.hpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::MSW class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Jan 20, 2015
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

#ifndef MSW_HPP
#define MSW_HPP

#include <complex>

#include "Analyzer.hpp"

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations
		namespace GuidedScrambling
		{
			//! Performs %MSW (mean squared weight) analysis of a codeword
			/*!
			 * @date		Jan 20, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class MSW: public Analyzer
			{
			public:
				//! %Feedback mechanism for %MSW (mean squared weight) codeword analysis
				/*!
				 * Here is where we store and pass on the RDS (running digital sum) value
				 * remaining at the end of the analysis.
				 *
				 * @date		Jan 16, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				class Feedback: public Analyzer::Feedback
				{
				public:
					//! Clone %MSW feedback data
					/*!
					 * @return	Pointer to dynamically allocated %MSW Feedback copy.
					 * @date		Jan 16, 2015
					 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
					 */
					Analyzer::Feedback* clone() const;

					Feedback();

					std::complex<float> RDS; //!< RDS (running digital sum) value at end of codeword
				};

				//! Perform %MSW analysis on a codeword
				/*!
				 * This actually performs the %MSW analysis on the passed codeword using
				 * the feedback required. After calling this function the feedback() and
				 * analysis() accessors become valid.
				 *
				 * @param	[in] codeword The codeword to perform analysis on.
				 * @param	[in] feedback The feedback from the winning codeword to start
				 *				the analysis with.
				 * @param	[in] constellation This is a direct mapping of symbols (as
				 *				vector indices) to constellation points.
				 * @date		Jan 20, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
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
				 * @return	Constant reference to internal Feedback object.
				 * @date		Jan 16, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				const Analyzer::Feedback& feedback() const;

				//! Extract result from %MSW analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function. In this context a lower analysis means a better
				 * codeword.
				 *
				 * @return	Floating point representation of the codeword's %MSW
				 *				analysis. Lower is better.
				 * @date		Jan 16, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
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
