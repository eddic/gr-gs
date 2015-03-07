/*!
 * @file       Exceptions.hpp
 * @brief      Defines the gr::Isatec::GuidedScrambling::Exceptions namespace
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       Jan 17, 2015
 * @copyright  Copyright &copy; 2014 Eddie Carle &lt;eddie@isatec.ca&gt;.
 *             This project is released under the GNU General Public License
 *             Version 3.
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

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

#include "gr-isatec/config.h"

//! GNU Radio Namespace
namespace gr
{
   //! Contains all GNU Radio classes for the Isatec out-of-tree module
   namespace Isatec
   {
      //! Contains all classes for performing guided scrambling operations.
      namespace GuidedScrambling
      {
         //! Contains all exceptions related to guided scrambling
         namespace Exceptions
         {
            class Isatec_API BadScrambleInput: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Got an input word that is the wrong length";
               }
            };

            class Isatec_API AugmentingCodewordLengthMismatch: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Told to operate with a codeword size not larger than the augmenting size";
               }
            };

            class Isatec_API AugmentingLengthTooSmall: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Told to operate with an augmenting size less than 1";
               }
            };

            class Isatec_API DivisorLengthTooSmall: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Told to operate with a divider word of length less than 2";
               }
            };

            class Isatec_API BadSelectionMethod: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Told to operate with a constellation size other than the field size";
               }
            };

            class Isatec_API BadInputLength: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Told to operate with an input word not of length codeword size - augmenting size";
               }
            };

            class Isatec_API BadFieldSize: public std::exception
            {
               const char* what() const throw()
               {
                  return "gr::Isatec::GuidedScrambling: Told to operate with an invalid field size";
               }
            };
         }
      }
   }
}

#endif
