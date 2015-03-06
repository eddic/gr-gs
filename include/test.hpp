/*!
 * @file			test.hpp
 * @brief		Declares the GuidedScramling unit test functions
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 6, 2015
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

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains elements needed for the GuidedScrambler and Descrambler blocks
		namespace GuidedScrambling
		{
			//! Contains all GuidedScrambling unit tests
			namespace Tests
			{
				//! Fully tests all components of gr::Isatec::GuidedScrambling::Word
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Word_test();

				//! Fully tests gr::Isatec::GuidedScrambling::Analyzer
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Analyzer_test();

				//! Fully tests gr::Isatec::GuidedScrambling::Descrambler_impl
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Descrambler_test();

				//! Fully tests gr::Isatec::GuidedScrambling::Scrambler
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Scrambler_test();

				//! Fully tests gr::Isatec::GuidedScrambling::ScramblerGroup
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int ScramblerGroup_test();

				//! Fully tests gr::Isatec::GuidedScrambling::GuidedScrambler_impl
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int GuidedScrambler_test();
			}
		}
	}
}
