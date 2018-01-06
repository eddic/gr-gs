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

#include <complex>
#include <vector>
#include <limits>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        struct GS_API ComplexInteger
        {
            int64_t real;
            int64_t imag;

            ComplexInteger()
            {}

            ComplexInteger(int64_t x, int64_t y):
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

            ComplexInteger& operator+=(const ComplexInteger& z)
            {
                real += z.real;
                imag += z.imag;
                return *this;
            }

            ComplexInteger operator-(const ComplexInteger& z) const
            {
                return ComplexInteger(real-z.real, imag-z.imag);
            }

            ComplexInteger& operator-=(const ComplexInteger& z)
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

            ComplexInteger& operator*=(const ComplexInteger& z)
            {
                real = real*z.real - imag*z.imag;
                imag = real*z.imag + imag*z.real;
                return *this;
            }

            bool operator==(const ComplexInteger& z) const
            {
                return real == z.real && imag == z.imag;
            }

            uint64_t norm() const
            {
                return static_cast<uint64_t>(real*real + imag*imag);
            }

            bool overflow() const
            {
                return
                    real > std::numeric_limits<int32_t>::max() ||
                    real < std::numeric_limits<int32_t>::min() ||
                    imag > std::numeric_limits<int32_t>::max() ||
                    imag < std::numeric_limits<int32_t>::min();
            }

            template<typename T> operator std::complex<T>() const
            {
                return std::complex<T>(
                        static_cast<T>(real),
                        static_cast<T>(imag));
            }

            template<typename T>
            static std::vector<std::complex<T>> toStdComplex(
                    const std::vector<gr::gs::ComplexInteger>& source);
        };
    }
}

#endif
