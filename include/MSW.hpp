/*!
 * @file      MSW.hpp
 * @brief     Declares the gr::gs::GuidedScrambling::MSW class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      May 18, 2017
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

#ifndef GR_GS_MSW_HPP
#define GR_GS_MSW_HPP

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
            //! Performs %MSW (mean squared weight) analysis of a codeword
            /*!
             * This selection method minimizes the mean square of the running
             * digital sum as described in [1].
             *
             *  1. K. Immink and L. Patrovics, “Performance assessment of
             *     DC-free multi-mode codes,” *IEEE Transactions on
             *     Communications*, vol. 45, no. 3, pp. 293-299, 1997.
             *
             * @tparam Symbol Base type to use for symbol type. Can be unsigned
             *                char, unsigned short, or unsigned int.
             * @date   May 18, 2017
             * @author Eddie Carle &lt;eddie@isatec.ca&gt;
             */
            template<typename Symbol>
            class MSW: public Analyzer<Symbol>
            {
            public:
                //! %Feedback mechanism for %MSW codeword analysis
                /*!
                 * Here is where we store and pass on the RDS (running digital
                 * sum) value remaining at the end of the analysis.
                 *
                 * @date   May 18, 2017
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                class Feedback: public Analyzer<Symbol>::Feedback
                {
                public:
                    //! Clone %MSW feedback data
                    /*!
                     * @return Pointer to dynamically allocated %MSW Feedback
                     *         copy.
                     * @date   May 18, 2017
                     * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                     */
                    typename Analyzer<Symbol>::Feedback* clone() const;

                    Feedback();

                    //! RDS (running digital sum) value at end of codeword
                    std::complex<double> RDS;
                };

                //! Perform %MSW analysis on a codeword
                /*!
                 * This actually performs the %MSW analysis on the passed
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
                 * @date   May 18, 2017
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                void analyze(
                        const std::vector<Symbol>& codeword,
                        const typename Analyzer<Symbol>::Feedback& feedback,
                        const std::vector<Complex>& constellation);

                //! Extract feedback from %MSW analysis
                /*!
                 * The output of this function becomes valid after a completed
                 * call to the analyze() function.
                 *
                 * @return Constant reference to internal Feedback object.
                 * @date   May 18, 2017
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                const typename Analyzer<Symbol>::Feedback& feedback() const;

                //! Extract result from %MSW analysis
                /*!
                 * The output of this function becomes valid after a completed
                 * call to the analyze() function. In this context a lower
                 * analysis means a better codeword.
                 *
                 * @return Floating point representation of the codeword's %MSW
                 *         analysis. Lower is better.
                 * @date   May 18, 2017
                 * @author Eddie Carle &lt;eddie@isatec.ca&gt;
                 */
                double analysis() const;

            private:
                //! %Feedback from %MSW analysis
                Feedback m_feedback;

                //! Actual result from %MSW analysis
                double m_analysis;
            };
        }
    }
}

#endif
