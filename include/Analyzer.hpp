/*!
 * @file			Analyzer.hpp
 * @brief		Defines the gr::Isatec::GuidedScrambling::Analyzer class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Feb 20, 2015
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

#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <vector>
#include <complex>
#include <string>

#include "gr-isatec/config.h"

//! GNU Radio Stuff
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations
		namespace GuidedScrambling
		{
			//! Performs selection analysis of a codeword
			/*!
			 * This is a purely virtual class for the purpose of defining selection
			 * methods.
			 *
			 * @date		Feb 20, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class Analyzer
			{
			public:
				//! Method Names
				static const std::vector<std::string> names;

				//! %Feedback mechanism for analysis
				/*!
				 * This purely virtual class allows for the management of feedback data
				 * between analyzers by the guided scrambling system independent of the
				 * selection method being used. It is simply a way of storing whatever
				 * data needs to be passed from the previous winning analyzer to all the
				 * next ones. One simply needs to define the clone() function to to allow
				 * the feedback data to be copied.
				 *
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				class Feedback
				{
				public:
					//! Clone feedback data
					/*!
					 * This should allow the caller to make a fully independent copy of the
					 * feedback data. The returned value should simply be a pointer to a
					 * dynamically allocated child object. The caller shall be responsible
					 * for it's disposal of course.
					 *
					 * @return	Pointer to dynamically allocated Feedback copy child.
					 * @date		Jan 21, 2015
					 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
					 */
					virtual Feedback* clone() const =0;

					//! Virtual destructor is critical
					virtual ~Feedback() {}
				};

				//! Perform analysis on a codeword
				/*!
				 * This actually performs the desired analysis on the passed codeword
				 * using the feedback required. After calling this function the feedback()
				 * and analysis() accessors become valid.
				 *
				 * @param	[in] codeword The codeword to perform analysis on.
				 * @param	[in] feedback The feedback from the winning codeword to start
				 *				the analysis with.
				 * @param	[in] constellation This is a direct mapping of symbols (as
				 *				vector indices) to constellation points.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				virtual void analyze(
						const std::vector<Symbol>& codeword,
						const Feedback& feedback,
						const std::vector<std::complex<float>>& constellation) =0;

				//! Extract feedback from analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function.
				 *
				 * @return	Constant reference to internal Feedback object.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				virtual const Feedback& feedback() const =0;

				//! Extract result from analysis
				/*!
				 * The output of this function becomes valid after a completed call to the
				 * analyze() function. In this context a lower analysis means a better
				 * codeword.
				 *
				 * @return	Floating point representation of the codeword's selection
				 *				analysis. Lower is better.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				virtual float analysis() const =0;
			};

			//! Manufacture an Analyzer object
			/*!
			 * @param	[in] method The desired analysis method to make
			 * @return	Newly created Analyzer object
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			Analyzer* manufactureAnalyzer(const unsigned int method);

			//! Manufacture a Feedback object
			/*!
			 * @param	[in] method The desired analysis method to base off
			 * @return	Newly created Feedback object
			 * @date		Jan 21, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			Analyzer::Feedback* manufactureFeedback(const unsigned int method);
		}
	}
}

#endif
