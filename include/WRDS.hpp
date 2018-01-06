/*!
 * @file      WRDS.hpp
 * @brief     Declares the gr::gs::GuidedScrambling::WRDS class
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

#ifndef GR_GS_WRDS_HPP
#define GR_GS_WRDS_HPP

#include <complex>

#include "Analyzer.hpp"

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        //! Stuff needed for the GuidedScrambler and Descrambler blocks
        namespace GuidedScrambling
        {
            //! Performs %WRDS (word running digital sum) analysis of a codeword
            /*!
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date    January 6, 2018
             * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class WRDS: public Analyzer<Symbol>
            {
            public:
                //! %Feedback mechanism for %WRDS codeword analysis
                /*!
                 * Here is where we store and pass on the RDS (running digital
                 * sum) value remaining at the end of the analysis.
                 *
                 * @date January 6, 2018
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                class Feedback: public Analyzer<Symbol>::Feedback
                {
                public:
                    //! Clone %WRDS feedback data
                    /*!
                     * @return Pointer to dynamically allocated %WRDS Feedback
                     *         copy.
                     */
                    typename Analyzer<Symbol>::Feedback* clone() const;

                    Feedback();

                    //! RDS (running digital sum) value at end of codeword
                    ComplexInteger RDS;
                };

                //! Perform %WRDS analysis on a codeword
                /*!
                 * This actually performs the %WRDS analysis on the passed
                 * codeword using the feedback provided. After calling this
                 * function the feedback() and analysis() accessors become
                 * valid.
                 *
                 * @param [in] codeword The codeword to perform analysis on.
                 * @param [in] feedback The feedback from the winning codeword
                 *                      to start the analysis with.
                 * @param [in] constellation This is a direct mapping of
                 *                             symbols (as vector indices) to
                 *                             constellation points.
                 */
                void analyze(
                        const std::vector<Symbol>& codeword,
                        const typename Analyzer<Symbol>::Feedback& feedback,
                        const std::vector<ComplexInteger>& constellation);

                //! Extract feedback from %WRDS analysis
                /*!
                 * The output of this function becomes valid after a completed
                 * call to the analyze() function.
                 *
                 * @return Constant reference to internal Feedback object.
                 */
                const typename Analyzer<Symbol>::Feedback& feedback() const;

                //! Extract result from %WRDS analysis
                /*!
                 * The output of this function becomes valid after a completed
                 * call to the analyze() function. In this context a lower
                 * analysis means a better codeword.
                 *
                 * @return Floating point representation of the codeword's %WRDS
                 *         analysis. Lower is better.
                 */
                uint64_t analysis() const;

            private:
                //! %Feedback from %WRDS analysis
                Feedback m_feedback;
            };
        }
    }
}

#endif
