/*!
 * @file			config.h
 * @brief		Defines some basic parameters for the module
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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>
#include <vector>
#include <complex>

#include "gr-isatec/api.h"

#define STARTINGRDS 1,0

const double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089;

//! GNU Radio Namespace
namespace gr
{
	//! Contains all GNU Radio classes for the Isatec out-of-tree module
	namespace Isatec
	{
		//! Get a default constellation pattern for a field size
		Isatec_API std::vector<std::complex<float>> defaultConstellation(unsigned int fieldSize);

		//! Type to use for representation of symbols
		typedef uint8_t Symbol;

		//! Contains all Guided Scrambling classes.
		namespace GuidedScrambling
		{
			//! Maximum field size defined
			const unsigned int maxFieldSize=4;
		}
	}
}

#endif
