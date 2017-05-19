/*!
 * @file      Analyzer.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::Analyzer class
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

#include "Analyzer.hpp"
#include "gr-gs/exceptions.h"

#include "MSW.hpp"
#include "WRDS.hpp"
#include "MSW2.hpp"

const std::vector<std::string> gr::gs::GuidedScrambling::selectionMethods
{
     "MSW",
     "WRDS",
     "MSW2"
};

template gr::gs::GuidedScrambling::Analyzer<unsigned char>*
gr::gs::GuidedScrambling::manufactureAnalyzer<unsigned char>(
        const unsigned int method);
template gr::gs::GuidedScrambling::Analyzer<unsigned short>*
gr::gs::GuidedScrambling::manufactureAnalyzer<unsigned short>(
        const unsigned int method);
template gr::gs::GuidedScrambling::Analyzer<unsigned int>*
gr::gs::GuidedScrambling::manufactureAnalyzer<unsigned int>(
        const unsigned int method);
template<typename Symbol> gr::gs::GuidedScrambling::Analyzer<Symbol>*
gr::gs::GuidedScrambling::manufactureAnalyzer(const unsigned int method)
{
    switch(method)
    {
        case 0:
            return new gr::gs::GuidedScrambling::MSW<Symbol>;
        case 1:
            return new gr::gs::GuidedScrambling::WRDS<Symbol>;
        case 2:
            return new gr::gs::GuidedScrambling::MSW2<Symbol>;
        default:
            throw Exceptions::BadSelectionMethod();
    }
}

template typename gr::gs::GuidedScrambling::Analyzer<unsigned char>::Feedback*
gr::gs::GuidedScrambling::manufactureFeedback<unsigned char>(
        const unsigned int method);
template typename gr::gs::GuidedScrambling::Analyzer<unsigned short>::Feedback*
gr::gs::GuidedScrambling::manufactureFeedback<unsigned short>(
        const unsigned int method);
template typename gr::gs::GuidedScrambling::Analyzer<unsigned int>::Feedback*
gr::gs::GuidedScrambling::manufactureFeedback<unsigned int>(
        const unsigned int method);
template<typename Symbol>
typename gr::gs::GuidedScrambling::Analyzer<Symbol>::Feedback*
gr::gs::GuidedScrambling::manufactureFeedback(const unsigned int method)
{
    switch(method)
    {
        case 0:
            return new typename
                gr::gs::GuidedScrambling::MSW<Symbol>::Feedback;
        case 1:
            return new typename
                gr::gs::GuidedScrambling::WRDS<Symbol>::Feedback;
        case 2:
            return new typename
                gr::gs::GuidedScrambling::MSW2<Symbol>::Feedback;
        default:
            throw Exceptions::BadSelectionMethod();
    }
}
