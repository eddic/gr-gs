/*!
 * @file      exceptions.h
 * @brief     Defines the GNU Radio Guided Scrambling C++ exception
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      July 8, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2016 Eddie Carle
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

#ifndef GR_GS_EXCEPTIONS_H
#define GR_GS_EXCEPTIONS_H

#include <exception>

#include "gr-gs/config.h"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Stuff needed for the GuidedScrambler and Descrambler blocks
        namespace GuidedScrambling
        {
            //! Contains all exceptions related to guided scrambling
            namespace Exceptions
            {
                class GR_GS BadScrambleInput: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Got an input word"
                            " that is the wrong length";
                    }
                };

                class GR_GS AugmentingCodewordLengthMismatch: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Told to operate "
                            "with a codeword size not larger than the "
                            "augmenting size";
                    }
                };

                class GR_GS AugmentingLengthTooSmall: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Told to operate "
                            "with an augmenting size less than 1";
                    }
                };

                class GR_GS DivisorLengthTooSmall: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Told to operate "
                            "with a divider word of length less than 2";
                    }
                };

                class GR_GS BadSelectionMethod: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Told to operate "
                            "with a constellation size other than the field "
                            "size";
                    }
                };

                class GR_GS BadInputLength: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Told to operate "
                            "with an input word not of length codeword size - "
                            "augmenting size";
                    }
                };

                class GR_GS BadFieldSize: public std::exception
                {
                    const char* what() const throw()
                    {
                        return "gr::Isatec::GuidedScrambling: Told to operate "
                            "with an invalid field size";
                    }
                };
            }
        }
    }
}

#endif
