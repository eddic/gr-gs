/*!
 * @file      Data.cpp
 * @brief     Defines the Data namespace
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      June 23, 2017
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

#include "gr-gs/Data.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <zlib.h>
#include <string>

//! GNU Radio Namespace
namespace gr
{
    //! Contains all blocks for the Guided Scrambling GNU Radio Module
    namespace gs
    {
        namespace Data
        {
            void raw(
                    const unsigned fieldSize,
                    const unsigned codewordLength,
                    const unsigned augmentingLength,
                    const std::string& key,
                    char* data,
                    size_t dataSize);
        }
    }
}

void gr::gs::Data::raw(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const std::string& key,
        char* data,
        size_t dataSize)
{
    {
        std::ostringstream path;
        path << dataPath << '/' << std::setfill('0') << std::right
             << std::setw(2) << fieldSize << '/'
             << std::setw(2) << codewordLength << '/'
             << std::setw(2) << augmentingLength << '-' << key << ".dat";

        std::ifstream file;
        file.exceptions(
                std::ifstream::failbit
                | std::ifstream::eofbit
                | std::ifstream::badbit);
        file.open(path.str());
        file.read(data, dataSize);
    }

    unsigned long hash;
    std::ostringstream path;
    path << dataPath << '/' << std::setfill('0') << std::right
         << std::setw(2) << fieldSize << '/'
         << std::setw(2) << codewordLength << '/'
         << std::setw(2) << augmentingLength << ".txt";

    std::ifstream file;
    file.exceptions(
            std::ifstream::failbit
            | std::ifstream::badbit);
    file.open(path.str());
    enum { LOOKING, FIRSTWORD, HASH } state = LOOKING;
    std::string token;
    while(!file.eof())
    {
        if(state == HASH)
        {
            file >> std::showbase >> std::hex >> hash;
            break;
        }

        file >> token;
        if(state == LOOKING)
        {
            for(auto& character: token)
                character |= 0x20;
            if(token == key)
                state = FIRSTWORD;
        }
        else if(token == "CRC:")
            state = HASH;
    }

    if(hash != crc32(0, reinterpret_cast<unsigned char*>(data), dataSize))
        throw;
}

gr::gs::Data::Autocovariance gr::gs::Data::autocovariance(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength)
{
    Autocovariance data(codewordLength);
    const size_t dataSize = 
        sizeof(double)
        *2*2
        *autocovarianceDataLength
        *codewordLength;

    raw(
            fieldSize,
            codewordLength,
            augmentingLength,
            "autocovariance",
            reinterpret_cast<char*>(data.data()),
            dataSize);

    return data;
}

gr::gs::Data::Distribution gr::gs::Data::distribution(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength)
{
    Distribution data(codewordLength);
    const size_t dataSize = 
        sizeof(double)
        *distributionDataWidth
        *distributionDataWidth
        *codewordLength;

    raw(
            fieldSize,
            codewordLength,
            augmentingLength,
            "distribution",
            reinterpret_cast<char*>(data.data()),
            dataSize);

    return data;
}

gr::gs::Data::PSD gr::gs::Data::psd(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength)
{
    PSD data(psdDataWidth);
    const size_t dataSize = sizeof(double)*2*psdDataWidth;

    raw(
            fieldSize,
            codewordLength,
            augmentingLength,
            "psd",
            reinterpret_cast<char*>(data.data()),
            dataSize);

    return data;
}

template
std::vector<unsigned char> gr::gs::defaultScrambler(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength);
template
std::vector<unsigned short> gr::gs::defaultScrambler(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength);
template
std::vector<unsigned int> gr::gs::defaultScrambler(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength);
template<typename Symbol>
std::vector<Symbol> gr::gs::defaultScrambler(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength)
{
    std::ostringstream path;
    path << dataPath << std::setfill('0') << std::right
         << '/' << std::setw(2) << fieldSize
         << '/' << std::setw(2) << codewordLength
         << '/' << std::setw(2) << augmentingLength
         << "-scrambler.txt";

    std::ifstream file;
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    file.open(path.str());
    file.exceptions(std::ifstream::badbit);

    std::vector<Symbol> scrambler;

    while(true)
    {
        unsigned int value;
        file >> value;
        if(file.eof())
            break;
        if(file.fail())
        {
            file.clear();
            file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
            file >> value;
        }
        scrambler.push_back(static_cast<Symbol>(value));
    }

    return scrambler;
}
