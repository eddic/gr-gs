/*!
 * @file      Entropy_impl.hpp
 * @brief     Declares the "Entropy" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      June 19, 2017
 * @copyright Copyright &copy; 2017 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2017 Eddie Carle
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

#ifndef GR_GS_ENTROPY_IMPL_HPP
#define GR_GS_ENTROPY_IMPL_HPP

#include "gr-gs/Entropy.h"
#include "ProbabilityMapper.hpp"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! All block implementation too trivial for their own namespace
        namespace Implementations
        {
            //! Guided Scrambling "Entropy" GNU Radio block implementation
            /*!
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   June 19, 2017
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class Entropy_impl: public Entropy<Symbol>
            {
            public:
                //! Initialize the entropy
                /*!
                 * @param [in] fieldSize Desired field size as the *n* in
                 *                       GF(*n*)
                 * @param [in] codewordLength Desired codeword length
                 * @param [in] augmentingLength Desired augmenting Length
                 * @param [in] minCorrelation This decides how many taps we're
                 *                            going to need to calculate our
                 *                            means. Any autocorrelation data
                 *                            decays below this value will be
                 *                            truncated from our computations.
                 * @param [in] framingTag Desired string to use for the "key" of
                 *                        the tag inserted at frame beginnings.
                 *                        Use an empty string to disable
                 *                        framing.
                 */
                Entropy_impl(
                        const unsigned int fieldSize,
                        const unsigned int codewordLength,
                        const unsigned int augmentingLength,
                        const double minCorrelation,
                        const std::string& framingTag);

                //! No copying allowed
                Entropy_impl(const Entropy_impl& x) = delete;
                //! No copying allowed
                Entropy_impl& operator=(const Entropy_impl& x) = delete;

            private:
                //! GNU Radio work function
                int work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);

                //! Framing tag name/key
                std::string m_framingTag;

                //! PMT version of framing tag
                pmt::pmt_t m_framingTagPMT;

                //! The codeword length
                const unsigned int m_codewordLength;

                //! Where are we in our codeword?
                unsigned int m_codewordPosition;

                //! What is our RDS?
                std::complex<double> m_rds;

                //! The probability mapping object
                ProbabilityMapper<Symbol> m_mapper;

                //! Have we started mapping yet?
                bool m_started;
            };
        }
    }
}

#endif