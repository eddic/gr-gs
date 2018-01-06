/*!
 * @file      ComplexInteger.h
 * @brief     Declares a limited complex integer type
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

#ifndef GR_GS_COMPLEXINTEGER_H
#define GR_GS_COMPLEXINTEGER_H

#include "gr-gs/config.h"

#include <complex>
#include <vector>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        struct GS_API ComplexInteger
        {
            int32_t real;
            int32_t imag;

            ComplexInteger()
            {}

            ComplexInteger(int32_t x, int32_t y):
                real(x),
                imag(y)
            {}

            ComplexInteger(const ComplexInteger& z):
                real(z.real),
                imag(z.imag)
            {}

            ComplexInteger& operator=(const ComplexInteger& z)
            {
                real = z.real;
                imag = z.imag;
                return *this;
            }

            ComplexInteger operator+(const ComplexInteger& z) const
            {
                return ComplexInteger(real+z.real, imag+z.imag);
            }

            ComplexInteger operator+=(const ComplexInteger& z)
            {
                real += z.real;
                imag += z.imag;
                return *this;
            }

            ComplexInteger operator-(const ComplexInteger& z) const
            {
                return ComplexInteger(real-z.real, imag-z.imag);
            }

            ComplexInteger operator-=(const ComplexInteger& z)
            {
                real -= z.real;
                imag -= z.imag;
                return *this;
            }

            ComplexInteger operator*(const ComplexInteger& z) const
            {
                return ComplexInteger(
                        real*z.real - imag*z.imag,
                        real*z.imag + imag*z.real);
            }

            ComplexInteger operator*=(const ComplexInteger& z)
            {
                real = real*z.real - imag*z.imag;
                imag = real*z.imag + imag*z.real;
                return *this;
            }

            uint32_t norm() const
            {
                return static_cast<uint32_t>(real*real + imag*imag);
            }

            operator std::complex<float>() const
            {
                return std::complex<float>(
                        static_cast<float>(real),
                        static_cast<float>(imag));
            }

            operator std::complex<double>() const
            {
                return std::complex<double>(
                        static_cast<double>(real),
                        static_cast<double>(imag));
            }

            static std::vector<std::complex<float>> toFloat(
                    const std::vector<ComplexInteger>& source);

            static std::vector<std::complex<double>> toDouble(
                    const std::vector<ComplexInteger>& source);
        };
    }
}

#endif
