/*!
 * @file       ScramblerGroup.cpp
 * @brief      Defines the gr::Isatec::GuidedScrambling::ScramblerGroup class
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 3, 2015
 * @copyright  Copyright &copy; 2015 %Isatec Inc.  This project is released
 *             under the GNU General Public License Version 3.
 */

/* Copyright (C) 2015 %Isatec Inc.
 *
 * This file is part of the %Isatec GNU Radio Module
 *
 * The %Isatec GNU Radio Module is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The %Isatec GNU Radio Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * The %Isatec GNU Radio Module.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScramblerGroup.hpp"

void gr::Isatec::GuidedScrambling::ScramblerGroup::handler(HandlerArguments& args, const HandlerConstArguments& cargs)
{
   std::unique_lock<std::mutex> countLock(args.countMutex, std::defer_lock);
   std::unique_lock<std::mutex> sleepLock(args.sleepMutex);
   while(cargs.sleep)
   {
      sleepLock.unlock();

      scramble(cargs.divider, *cargs.input, cargs.remainder, *cargs.feedback, cargs.constellation);

      countLock.lock();
      --args.count;
      args.countCV.notify_one();

      sleepLock.lock();
      countLock.unlock();
      args.sleepCV.wait(sleepLock);
   }
}

void gr::Isatec::GuidedScrambling::ScramblerGroup::configure(
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
      scrambler.configure(length, indexStart++, augmentingLength, remainderLength, method, fieldSize);
   m_winner = &m_scramblers.front();
}
