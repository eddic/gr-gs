/*!
 * @file       Descrambler_impl.hpp
 * @brief      Declares the Guided Scrambling "Descrambler" GNU Radio block implementation
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 3, 2015
 * @copyright  Copyright &copy; 2015 %Isatec Inc.  This project is released
 *             under the GNU General Public License Version 3.
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

#ifndef DESCRAMBLER_IMPL_HPP
#define DESCRAMBLER_IMPL_HPP

#include <functional>
#include <mutex>

#include "gr-isatec/config.h"
#include "gr-isatec/Descrambler.h"

//! GNU Radio Namespace
namespace gr
{
   //! Contains all blocks for the %Isatec GNU Radio Module
   namespace Isatec
   {
      //! Contains elements needed for the GuidedScrambler and Descrambler blocks
      namespace GuidedScrambling
      {
         //! Guided Scrambling "Descrambler" GNU Radio block implementation
         /*!
          * @date    March 3, 2015
          * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
          */
         class Descrambler_impl: public Descrambler
         {
         public:
            //! Initialize the descrambler with some default options
            /*!
             * This initializes the scrambler with the following parameters:
             *  - codeword length = 12
             *  - augmenting length = 3
             *  - multiplier = 1 0 0 1 (\f$x^3+1\f$)
             *  - continuous encoding = true
             *  - field size = 4
             *
             * @date    March 3, 2015
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            Descrambler_impl();

            //! No copying allowed
            Descrambler_impl(const Descrambler_impl& x) = delete;
            //! No copying allowed
            Descrambler_impl& operator=(const Descrambler_impl& x) = delete;

            unsigned int fieldSize() const;
            void set_fieldSize(const unsigned int size);

            unsigned int codewordLength() const;
            void set_codewordLength(const unsigned int length);

            unsigned int augmentingLength() const;
            void set_augmentingLength(const unsigned int length);

            bool continuous() const;
            void set_continuous(bool continuous);

            const std::vector<Symbol>& multiplier() const;
            void set_multiplier(const std::vector<Symbol>& multiplier);

            //! Perform the actual descramble operation
            /*!
             * Note that in order to improve efficiency of operation this
             * function does not perform length/bounds checks on the input code
             * word. It is up to the calling function to ensure the parameters
             * are correctly sized. After calling this function the output() and
             * product() accessors become valid.
             *
             * @param   [in] input Input code word to be descrambled. The size of
             *                     this word \a must equal the codeword length.
             * @date    March 3, 2015
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            void descramble(const std::vector<Symbol>& input);

            const std::vector<Symbol> output() const;
            const std::vector<Symbol>& product() const;

            //! GNU Radio work function
            int general_work(int noutput_items,
                  gr_vector_int &ninput_items,
                  gr_vector_const_void_star &input_items,
                  gr_vector_void_star &output_items);

            //! GNU Radio forecast function
            void forecast(
                  int noutput_items,
                  gr_vector_int& ninput_items_required);


         private:
            mutable std::mutex m_mutex; //!< Always practice safe threading

            std::vector<Symbol> m_codeword; //!< Buffer to store up input codeword
            std::vector<Symbol>::iterator m_codewordIt; //!< Codeword buffer write position
            std::vector<Symbol> m_product; //!< The actual product of multiplication
            std::vector<Symbol>::const_iterator m_productIt; //!< Output sourceword read position
            std::vector<Symbol> m_remainder; //!< The multiplication remainder

            unsigned int m_codewordLength; //!< The codeword length
            unsigned int m_augmentingLength; //!< The length of the augmenting portion of the codeword
            std::vector<Symbol> m_multiplier;   //!< The multiplier used to descramble
            bool m_continuous; //!< True if we're doing continuous multiplication

            bool m_valid; //!< Indicates whether or not we need to initialize everything
            unsigned int m_fieldSize; //!< Field size to descramble with

            std::function<void(
               const std::vector<Symbol>&,
               const std::vector<Symbol>&,
               std::vector<Symbol>&,
               std::vector<Symbol>&,
               bool)> m_multiply; //!< The actual multiplying function to use
         };
      }
   }
}

#endif
