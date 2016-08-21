/*!
 * @file      GuidedScrambler.h
 * @brief     Declares the "Guided Scrambler" GNU Radio block
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

#ifndef GR_GS_GUIDEDSCRAMBLER_H
#define GR_GS_GUIDEDSCRAMBLER_H

#include "gr-gs/config.h"
#include <gnuradio/block.h>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! "Guided Scrambler" GNU Radio block
        /*!
         * @date   August 19, 2016
         * @author Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        class GS_API GuidedScrambler: virtual public gr::block
        {
        public:
            //! Access field size
            /*!
             * @return Field size as the *n* in GF(*n*)
             */
            virtual unsigned int fieldSize() const =0;

            //! Set fieldSize
            /*!
             * Note that not all field sizes are supported. Only powers of two
             * are supported within the closed range of 2 to
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

            //! Access constellation
            /*!
             * @return Constant reference to constellation vector
             */
            virtual const std::vector<std::complex<float>>&
            constellation() const =0;

            //! Set constellation
            /*!
             * @param  [in] constellation This is a direct mapping of symbols
             *                            (as vector indices) to constellation
             *                            points.
             */
            virtual void set_constellation(
                    const std::vector<std::complex<float>>& constellation) =0;

            //! Access selection method
            /*!
             * @return Selection Method
             */
            virtual const std::string& selectionMethod() const =0;

            //! List selection methods
            /*!
             * @return Vector of selection method names
             */
            static const std::vector<std::string>& selectionMethods();

            //! Set selection method
            /*!
             * @param  [in] method Desired selection method
             */
            virtual void set_selectionMethod(const std::string& method) =0;

            //! Access divider word
            /*!
             * @return Divider word (polynomial)
             */
            virtual const Word& divider() const =0;

            //! Set divider word
            /*!
             * @param  [in] divider Desired divider word (polynomial)
             */
            virtual void set_divider(const Word& divider) =0;

            //! Access number of concurrent scrambling threads
            /*!
             * @return Number of concurrent threads running in this block
             */
            virtual unsigned int threads() const =0;

            //! Set number of concurrent scrambling threads
            /*!
             * Use this function to control how many "child" threads are used
             * for the guided scrambling process. There can not be more threads
             * than there are concurrent scrambling operations to be executed.
             * This means that there is an upper bound to this regardless of
             * the value provided. If you'd like the block to decide an optimal
             * number of threads pass the value 0 (the default).
             *
             * @param  [in] number Desired number of concurrent scrambling
             *                     threads
             */
            virtual void set_threads(unsigned int number) =0;

            //! Access the framing tag name
            virtual const std::string& framingTag() const =0;

            //! Set the framing tag name
            /*!
             * This allows the codeword/sourceword starts to be aligned with a
             * frame stream tag. Use this to ensure alignment with the
             * Descrambler.
             *
             * @param [in] tag Desired string to use for the "key" of the tag
             *                 inserted at frame beginnings. Use an empty string
             *                 to disable framing.
             */
            virtual void set_framingTag(const std::string& tag) =0;

            //! Shared pointer to this
            typedef boost::shared_ptr<GuidedScrambler> sptr;

            //! Manufacture a guided scrambler
            /*!
             * @param [in] fieldSize See set_fieldSize()
             * @param [in] codewordLength See set_codewordLength()
             * @param [in] augmentingLength See set_augmentingLength()
             * @param [in] continuous See set_continuous()
             * @param [in] divider See set_divider()
             * @param [in] threads See set_threads()
             * @param [in] constellation See set_constellation()
             * @param [in] selectionMethod See set_selectionMethod()
             * @param [in] framingTag See set_framingTag()
             * @return Shared pointer to newly allocated guided scrambler
             */
            static sptr make(
                    const unsigned int fieldSize = 4,
                    const unsigned int codewordLength = 12,
                    const unsigned int augmentingLength = 3,
                    const bool continuous = true,
                    const Word& divider = {1,0,0,1},
                    const unsigned int threads = 0,
                    const std::vector<std::complex<float>>& constellation =
                        gr::gs::defaultConstellation(4),
                    const std::string& selectionMethod = "MSW",
                    const std::string& framingTag = "");
        };
    }
}

#endif
