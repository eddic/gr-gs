/*!
 * @file      Descrambler.h
 * @brief     Declares the "Descrambler" GNU Radio block
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

#ifndef GR_GS_DESCRAMBLER_H
#define GR_GS_DESCRAMBLER_H

#include <algorithm>
#include <functional>

#include "gr-gs/config.h"
#include <gnuradio/block.h>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Guided Scrambling "Descrambler" GNU Radio block
        /*!
         * This block is a fully functional implementation of the recieve side
         * of Guided Scrambling channel coding technique first described by Dr.
         * Ivan Fair at The University of Alberta in [1].
         *
         *  1. Ivan J. Fair, Wayne D. Grover, Witold A. Krymien, and R. Ian
         *     MacDonald, “Guided scrambling: A new line coding technique for
         *     high bit rate fiber optic transmission systems,” *IEEE
         *     Transaction on Communications*, vol. 39, no. 2, pp. 289-297,
         *     February 1991.
         *
         * @date   August 19, 2016
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API Descrambler: virtual public gr::block
        {
        public:
            //! Access field size
            /*!
             * @return Field size as the *n* in GF(*n*)
             */
            virtual unsigned int fieldSize() const =0;

            //! Set field size
            /*!
             * Note that not all field sizes are supported. Only powers of two
             * are supported within the closed interval of 2 to
             * gr::gs::GuidedScrambling::maxFieldSize.
             *
             * @param  [in] size Desired field size as the *n* in GF(*n*)
             */
            virtual void set_fieldSize(const unsigned int size) =0;

            //! Access codeword length
            /*!
             * @return Codeword Length
             */
            virtual unsigned int codewordLength() const =0;

            //! Set codeword length
            /*!
             * @param  [in] length Desired codeword length
             */
            virtual void set_codewordLength(const unsigned int length) =0;

            //! Access augmenting length
            /*!
             * @return Augmenting Length
             */
            virtual unsigned int augmentingLength() const =0;

            //! Set augmenting length
            /*!
             * @param  [in] length Desired augmenting length
             */
            virtual void set_augmentingLength(const unsigned int length) =0;

            //! Access continuous encoding setting
            /*!
             * @return True if set to continuous encoding, false if block
             *         encoding
             */
            virtual bool continuous() const =0;

            //! Set continuous or block enconding
            /*!
             * @param  [in] continuous Set to true for continuous encoding,
             *                         false for block encoding
             */
            virtual void set_continuous(bool continuous) =0;

            //! Access multiplier word
            /*!
             * @return Multiplier word (polynomial)
             */
            virtual const Word& multiplier() const =0;

            //! Set multiplier polynomial
            /*!
             * @param  [in] multiplier Desired multiplier word (polynomial)
             */
            virtual void set_multiplier(
                    const Word& multiplier) =0;

            //! Access the framing tag name
            virtual const std::string& framingTag() const =0;

            //! Set the framing tag name
            /*!
             * This allows the codeword/sourceword starts to be aligned with a
             * frame stream tag. Use this to ensure alignment with the
             * GuidedScrambler.
             *
             * @param [in] tag Desired string to use for the "key" of the tag
             *                 inserted at frame beginnings. Use an empty string
             *                 to disable framing.
             */
            virtual void set_framingTag(const std::string& tag) =0;

            //! Shared pointer to this
            typedef boost::shared_ptr<Descrambler> sptr;

            //! Manufacture a descrambler with some default options
            /*!
             * @param [in] fieldSize See set_fieldSize()
             * @param [in] codewordLength See set_codewordLength()
             * @param [in] augmentingLength See set_augmentingLength()
             * @param [in] continuous See set_continuous()
             * @param [in] multiplier See set_multiplier()
             * @param [in] framingTag See set_framingTag()
             * @return Shared pointer to newly allocated descrambler
             */
            static sptr make(
                    const unsigned int fieldSize = 4,
                    const unsigned int codewordLength = 12,
                    const unsigned int augmentingLength = 3,
                    const bool continuous = true,
                    const Word& multiplier = {1,0,0,1},
                    const std::string& framingTag = "");
        };
    }
}

#endif
