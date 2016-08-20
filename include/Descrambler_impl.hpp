/*!
 * @file      Descrambler_impl.hpp
 * @brief     Declares the "Descrambler" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 19, 2016
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

#ifndef GR_GS_DESCRAMBLER_IMPL_HPP
#define GR_GS_DESCRAMBLER_IMPL_HPP

#include <functional>
#include <mutex>

#include "gr-gs/config.h"
#include "gr-gs/Descrambler.h"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Stuff needed for the GuidedScrambler and Descrambler blocks
        namespace GuidedScrambling
        {
            //! Guided Scrambling "Descrambler" GNU Radio block implementation
            /*!
             * @date   August 19, 2016
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class Descrambler_impl: public Descrambler
            {
            public:
                //! Initialize the descrambler
                /*!
                 * @param [in] fieldSize See set_fieldSize()
                 * @param [in] codewordLength See set_codewordLength()
                 * @param [in] augmentingLength See set_augmentingLength()
                 * @param [in] continuous See set_continuous()
                 * @param [in] multiplier See set_multiplier()
                 * @param [in] framingTag See set_framingTag()
                 */
                Descrambler_impl(
                    const unsigned int fieldSize,
                    const unsigned int codewordLength,
                    const unsigned int augmentingLength,
                    const bool continuous,
                    const std::vector<Symbol>& multiplier,
                    const std::string& framingTag);

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

                const std::string& framingTag() const;
                void set_framingTag(const std::string& tag);

                //! Perform the actual descramble operation
                /*!
                 * Note that in order to improve efficiency of operation this
                 * function does not perform length/bounds checks on the input
                 * code word. It is up to the calling function to ensure the
                 * parameters are correctly sized. After calling this function
                 * the output() and product() accessors become valid.
                 *
                 * @param [in] input Input code word to be descrambled. The size
                 *                   of this word \a must equal the codeword
                 *                   length.
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
                //! Always practice safe threading
                mutable std::mutex m_mutex;

                //! Buffer to store up input codeword
                std::vector<Symbol> m_codeword;

                //! Codeword buffer write position
                std::vector<Symbol>::iterator m_codewordIt;

                //! The actual product of multiplication
                std::vector<Symbol> m_product;

                //! Output sourceword read position
                std::vector<Symbol>::const_iterator m_productIt;

                //! The multiplication remainder
                std::vector<Symbol> m_remainder;

                //! The codeword length
                unsigned int m_codewordLength;

                //! The length of the augmenting portion of the codeword
                unsigned int m_augmentingLength;

                //! The multiplier used to descramble
                std::vector<Symbol> m_multiplier;

                //! True if we're doing continuous multiplication
                bool m_continuous;

                //! Indicates whether or not we need to initialize everything
                bool m_valid;

                //! Field size to descramble with
                unsigned int m_fieldSize;

                //! Framing tag name/key
                std::string m_framingTag;

                //! PMT version of framing tag
                pmt::pmt_t m_framingTagPMT;

                //! The actual multiplying function to use
                std::function<void(
                    const std::vector<Symbol>&,
                    const std::vector<Symbol>&,
                    std::vector<Symbol>&,
                    std::vector<Symbol>&,
                    bool)> m_multiply;

                //! Setup the descrambler
                void setup();
            };
        }
    }
}

#endif
