/*!
 * @file      MSW2.hpp
 * @brief     Declares the gr::gs::GuidedScrambling::MSW2 class
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

#ifndef GR_GS_MSW2_HPP
#define GR_GS_MSW2_HPP

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
            //! Performs %MSW2 (mean squared weight dc^2) analysis of a codeword
            /*!
             * @date   March 10, 2015
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            class MSW2: public Analyzer
            {
            public:
                //! %Feedback mechanism for %MSW2 codeword analysis
                /*!
                 * Here is where we store and pass on the RDS (running digital
                 * sum) and RDSS (running digital sum-sum) value remaining at
                 * the end of the analysis.
                 *
                 * @date   March 10, 2015
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                class Feedback: public Analyzer::Feedback
                {
                public:
                    //! Clone %MSW2 feedback data
                    /*!
                     * @return Pointer to dynamically allocated %MSW2 Feedback
                     *         copy.
                     * @date   March 10, 2015
                     * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                     */
                    Analyzer::Feedback* clone() const;

                    Feedback();

                    //! RDS (running digital sum) value at end of codeword
                    std::complex<float> RDS;

                    //! RDSS (running digital sum-sum) value at end of codeword
                    std::complex<float> RDSS;
                };

                //! Perform %MSW2 analysis on a codeword
                /*!
                 * This actually performs the %MSW2 analysis on the passed
                 * codeword using the feedback provided. After calling this
                 * function the feedback() and analysis() accessors become
                 * valid.
                 *
                 * @param  [in] codeword The codeword to perform analysis on.
                 * @param  [in] feedback The feedback from the winning codeword
                 *                       to start the analysis with.
                 * @param  [in] constellation This is a direct mapping of
                 *                            symbols (as vector indices) to
                 *                            constellation points.
                 * @date   March 10, 2015
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                void analyze(
                        const std::vector<Symbol>& codeword,
                        const Analyzer::Feedback& feedback,
                        const std::vector<std::complex<float>>& constellation);

                //! Extract feedback from %MSW2 analysis
                /*!
                 * The output of this function becomes valid after a completed
                 * call to the analyze() function.
                 *
                 * @return Constant reference to internal Feedback object.
                 * @date   March 10, 2015
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                const Analyzer::Feedback& feedback() const;

                //! Extract result from %MSW2 analysis
                /*!
                 * The output of this function becomes valid after a completed
                 * call to the analyze() function. In this context a lower
                 * analysis means a better codeword.
                 *
                 * @return Floating point representation of the codeword's
                 *         %MSW2 analysis. Lower is better.
                 *
                 * @date   March 10, 2015
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                float analysis() const;

            private:
                //! %Feedback from %MSW2 analysis
                Feedback m_feedback;

                //! Actual result from %MSW2 analysis
                float m_analysis;
            };
        }
    }
}

#endif