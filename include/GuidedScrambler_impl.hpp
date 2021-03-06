/*!
 * @file      GuidedScrambler_impl.hpp
 * @brief     Declares the "Guided Scrambler" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      January 6, 2018
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

#ifndef GR_GS_GUIDEDSCRAMBLER_IMPL_HPP
#define GR_GS_GUIDEDSCRAMBLER_IMPL_HPP

#include <mutex>
#include <thread>

#include "gr-gs/GuidedScrambler.h"
#include "ScramblerGroup.hpp"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Stuff needed for the GuidedScrambler and Descrambler blocks
        namespace GuidedScrambling
        {
            //! "Guided Scrambler" GNU Radio block implementation
            /*!
             * This class allows us to perform guided scrambling in an
             * efficient multi threaded fashion
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   January 6, 2018
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class GuidedScrambler_impl: public GuidedScrambler<Symbol>
            {
            public:
                //! No copying allowed
                GuidedScrambler_impl(const GuidedScrambler_impl& x) = delete;
                //! No copying allowed
                GuidedScrambler_impl& operator=(
                        const GuidedScrambler_impl& x) = delete;

                unsigned int fieldSize() const;
                void set_fieldSize(const unsigned int size);

                unsigned int codewordLength() const;
                void set_codewordLength(const unsigned int length);

                unsigned int augmentingLength() const;
                void set_augmentingLength(const unsigned int length);

                bool continuous() const;
                void set_continuous(bool continuous);

                const std::string& selectionMethod() const;
                void set_selectionMethod(const std::string& method);

                const std::vector<Symbol>& divider() const;
                void set_divider(const std::vector<Symbol>& divider);

                unsigned int threads() const;
                void set_threads(unsigned int number=0);

                //! Perform guided scrambling on an input source word
                /*!
                 * Note that in order to improve efficiency of operation this
                 * function does not perform length/bounds checks on the input
                 * source word.  It is up to the calling function to ensure the
                 * parameters are correctly sized.
                 *
                 * @param  [in] input Input source word. The length of this
                 *                    source word \a must equal the codeword
                 *                    length minus the augmenting length.
                 * @return Constant reference code word.
                 */
                const std::vector<Symbol>& scramble(
                        const std::vector<Symbol>& input);

                //! Initialize the guided scrambler
                /*!
                 * @param [in] fieldSize See set_fieldSize()
                 * @param [in] codewordLength See set_codewordLength()
                 * @param [in] augmentingLength See set_augmentingLength()
                 * @param [in] continuous See set_continuous()
                 * @param [in] divider See set_divider()
                 * @param [in] threads See set_threads()
                 * @param [in] selectionMethod See set_selectionMethod()
                 * @param [in] alignmentTag Desired string to use for the "key"
                 *                          of the tag present at the alignment
                 *                          point. Use an empty string to
                 *                          disable alignment.
                 */
                GuidedScrambler_impl(
                        const unsigned int fieldSize,
                        const unsigned int codewordLength,
                        const unsigned int augmentingLength,
                        const bool continuous,
                        const std::vector<Symbol>& divider,
                        const unsigned int threads,
                        const std::string& selectionMethod,
                        const std::string& alignmentTag);

                //! Simply kills the active threads.
                ~GuidedScrambler_impl();

                //! GNU Radio work function
                int general_work(
                        int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! GNU Radio forecast function
                void forecast(
                        int noutput_items,
                        gr_vector_int& ninput_items_required);

            private:
                //! Kills all active threads
                void killThreads();

                //! Let's be thread safe
                mutable std::mutex m_mutex;

                //! The codeword size
                unsigned int m_codewordLength;

                //! The augmenting portion length
                unsigned int m_augmentingLength;

                //! The selection method used for analysis
                unsigned int m_selectionMethod;

                //! Number of concurrent threads/scrambling groups
                unsigned int m_groups;

                //! Field size to scramble with
                unsigned int m_fieldSize;

                //! Vector of active threads
                std::vector<std::thread> m_threads;

                //! Vector of active scrambler groups
                std::vector<ScramblerGroup<Symbol>> m_scramblerGroups;

                //! True if we're doing continuous encoding
                bool m_continuous;

                //! Arguments for ScramblerGroup::handler()
                typename ScramblerGroup<Symbol>::HandlerArguments m_args;

                //! Constant arguments for ScramblerGroup::handler()
                typename ScramblerGroup<Symbol>::HandlerConstArguments m_cargs;

                //! Pointer to current codeword
                const std::vector<Symbol>* m_codeword;

                //! Iterator to codeword read position
                typename std::vector<Symbol>::const_iterator m_codewordIt;

                //! Buffered source word
                std::vector<Symbol> m_sourceword;

                //! Iterator to sourceword write position
                typename std::vector<Symbol>::iterator m_sourcewordIt;

                //! Framing tag name/key
                pmt::pmt_t m_alignmentTag;

                //! Are we aligned?
                bool m_aligned;
            };
        }
    }
}

#endif
