/*!
 * @file      ScramblerGroup.cpp
 * @brief     Defines the gr::gs::GuidedScrambling::ScramblerGroup class
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

#include "ScramblerGroup.hpp"

template<typename Symbol>
void gr::gs::GuidedScrambling::ScramblerGroup<Symbol>::handler(
        HandlerArguments& args,
        const HandlerConstArguments& cargs)
{
    std::unique_lock<std::mutex> countLock(args.countMutex, std::defer_lock);
    std::unique_lock<std::mutex> sleepLock(args.sleepMutex);
    while(cargs.sleep)
    {
        sleepLock.unlock();

        scramble(
                cargs.divider,
                *cargs.input,
                cargs.remainder,
                *cargs.feedback,
                cargs.constellation);

        countLock.lock();
        --args.count;
        args.countCV.notify_one();

        sleepLock.lock();
        countLock.unlock();
        args.sleepCV.wait(sleepLock);
    }
}

template<typename Symbol>
void gr::gs::GuidedScrambling::ScramblerGroup<Symbol>::configure(
        const unsigned int length,
        unsigned int indexStart,
        const unsigned int indexEnd,
        const unsigned int augmentingLength,
        const unsigned int remainderLength,
        const unsigned int method,
        const unsigned int fieldSize)
{
    m_scramblers.resize(indexEnd-indexStart);
    for(auto& scrambler: m_scramblers)
        scrambler.configure(
                length,
                indexStart++,
                augmentingLength,
                remainderLength,
                method,
                fieldSize);
    m_winner = &m_scramblers.front();
}

template class gr::gs::GuidedScrambling::ScramblerGroup<unsigned char>;
template class gr::gs::GuidedScrambling::ScramblerGroup<unsigned short>;
template class gr::gs::GuidedScrambling::ScramblerGroup<unsigned int>;
