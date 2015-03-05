/*!
 * @file			test.hpp
 * @brief		Declares the unit test functions
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			Jan 21, 2015
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

//! GNU Radio Namespace
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Contains all classes for performing guided scrambling operations
		namespace GuidedScrambling
		{
			//! Contains all unit tests
			namespace Tests
			{
				//! Fully tests all components of GS::Word
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Word_test();

				//! Fully tests GS::Analyzer
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Analyzer_test();

				//! Fully tests GS::Descrambler
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Descrambler_test();

				//! Fully tests GS::Scrambler
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int Scrambler_test();

				//! Fully tests GS::ScramblerGroup
				/*!
				 * @return	Zero on success, non-zero on fail.
				 * @date		Jan 21, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int ScramblerGroup_test();

				//! Fully tests GS::GuidedScrambler
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
