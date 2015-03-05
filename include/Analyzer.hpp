/*!
 * @file			Analyzer.hpp
 * @brief		Declares the gr::Isatec::GuidedScrambling::Analyzer class
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 3, 2015
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

#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <vector>
#include <complex>
#include <string>

#include "gr-isatec/config.h"

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains elements needed for the GuidedScrambler and Descrambler blocks
		namespace GuidedScrambling
		{
			//! For performing selection analysis of a codeword
			/*!
			 * This class should be derived from to implement different codeword
			 * selection methods.
			 *
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class Analyzer
			{
			public:
				//! A vector of all the different selection method names
				static const std::vector<std::string> names;

				//! %Feedback mechanism for analysis
				/*!
				 * This class allows for the management of feedback data between
				 * analyzers by the guided scrambling system independent of the
				 * selection method being used. It is simply a way of wrapping
				 * whatever data needs to be passed from the previous winning
				 * analyzer to all the next ones. One simply needs to define the
				 * clone() function to to allow the feedback data to be copied.
				 *
				 * @date		March 3, 2015
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
					 * @date		March 3, 2015
					 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
					 */
					virtual Feedback* clone() const =0;

					virtual ~Feedback() {}
				};

				//! Perform analysis on a codeword
				/*!
				 * This actually performs the desired analysis on the passed
				 * codeword using the feedback required. After calling this function
				 * the feedback() and analysis() accessors become valid.
				 *
				 * Note that we perform no bounds checking on the constellation
				 * vector. This means that if a symbol in the codeword has a larger
				 * integer value than the size of the constellation vector we will
				 * have a segmentation fault.
				 *
				 * @param	[in] codeword The codeword to perform analysis on.
				 * @param	[in] feedback The feedback from the winning codeword to start
				 *				the analysis with.
				 * @param	[in] constellation This is a direct mapping of symbols (as
				 *				vector indices) to constellation points.
				 * @date		March 3, 2015
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
				 * @date		March 3, 2015
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
				 *				analysis. Lower value wins.
				 * @date		March 3, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				virtual float analysis() const =0;
			};

			//! Manufacture an Analyzer object
			/*!
			 * This simply allocates an Analyzer object of the desired type and
			 * returns a pointer to it. It is the responsibility of the caller to
			 * clean up when finished.
			 *
			 * @param	[in] method The desired analysis method to allocate
			 * @return	Pointer to newly allocated Analyzer object
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			Analyzer* manufactureAnalyzer(const unsigned int method);

			//! Manufacture a Feedback object
			/*!
			 * This simply allocates a Feedback object of the desired type and
			 * returns a pointer to it. It is the responsibility of the caller to
			 * clean up when finished.
			 *
			 * @param	[in] method The desired analysis method to base off of
			 * @return	Pointer Newly created Feedback object
			 * @date		March 3, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			Analyzer::Feedback* manufactureFeedback(const unsigned int method);
		}
	}
}

#endif
