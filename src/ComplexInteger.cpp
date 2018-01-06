/*!
 * @file      ComplexInteger.cpp
 * @brief     Defines a limited complex integer type
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      January 5, 2018
 * @copyright Copyright &copy; 2018 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2018 Eddie Carle
 *
 * This file is part of the Guided Scrambling GNU Radio Module
 *
 * The Guided Scrambling GNU Radio Module is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The Guided Scrambling GNU Radio Module is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * The Guided Scrambling GNU Radio Module.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "gr-gs/ComplexInteger.h"

std::vector<std::complex<float>> gr::gs::ComplexInteger::toFloat(
        const std::vector<ComplexInteger>& source)
{
    std::vector<std::complex<float>> destination;
    destination.reserve(source.size());
    for(const ComplexInteger& z: source)
        destination.push_back(z);
    return destination;
}

std::vector<std::complex<double>> gr::gs::ComplexInteger::toDouble(
        const std::vector<ComplexInteger>& source)
{
    std::vector<std::complex<double>> destination;
    destination.reserve(source.size());
    for(const ComplexInteger& z: source)
        destination.push_back(z);
    return destination;
}
