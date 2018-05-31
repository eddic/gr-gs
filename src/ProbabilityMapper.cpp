/*!
 * @file      ProbabilityMapper.cpp
 * @brief     Defines the ProbabilityMapper class
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

#include "ProbabilityMapper.hpp"
#include "gr-gs/Data.h"

#include <cmath>
#include <map>
#include <memory>
#include <algorithm>
#include <numeric>

template<typename Symbol>
gr::gs::Implementations::ProbabilityMapper<Symbol>::ProbabilityMapper(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength):
    m_taps(codewordLength),
    m_variances(codewordLength),
    m_codewordLength(codewordLength)
{
    // Setup constellation stuff
    {
        const auto constellation = gr::gs::defaultConstellation_i(fieldSize);
        m_constellation.reserve(constellation.size());
        m_constellation.assign(constellation.begin(), constellation.end());

        std::set<int> reals;
        std::set<int> imags;
        for(const auto& point: m_constellation)
        {
            reals.insert(point.real);
            imags.insert(point.imag);
        }
        m_realConstellation.reserve(reals.size());
        m_realConstellation.assign(reals.begin(), reals.end());
        m_imagConstellation.reserve(imags.size());
        m_imagConstellation.assign(imags.begin(), imags.end());

        m_constellationToReal.resize(m_constellation.size());
        m_constellationToImag.resize(m_constellation.size());
        m_collapsedToConstellation.resize(m_constellation.size());

        for(unsigned real=0; real<m_realConstellation.size(); ++real)
            for(unsigned imag=0; imag<m_imagConstellation.size(); ++imag)
            {
                const Symbol symbol = std::find(
                        m_constellation.cbegin(),
                        m_constellation.cend(),
                        ComplexInteger(
                            m_realConstellation[real],
                            m_imagConstellation[imag]))
                    -m_constellation.cbegin();

                if(symbol >= m_constellation.size())
                    throw;

                m_constellationToReal[symbol] = real;
                m_constellationToImag[symbol] = imag;
                m_collapsedToConstellation[
                    real + imag*m_realConstellation.size()] = symbol;
            }
    }

    const Data::Autocovariance autocovariance = Data::autocovariance(
            m_constellation.size(),
            codewordLength,
            augmentingLength);

    const unsigned covarianceIndex = autocovarianceDataLength-2;

    for(unsigned position=0; position<codewordLength; ++position)
    {
        double& variance = m_variances[position];
        double& tap = m_taps[position];

        variance = (
                autocovariance[position].back()[0][0]
                +autocovariance[position].back()[1][1])/2;
        if(variance == 0)
            continue;

        const double& covariance = (
                autocovariance[position][covarianceIndex][0][0]
                +autocovariance[position][covarianceIndex][1][1])/2;
        const unsigned pastPosition = (position==0?codewordLength:position)-1;
        const double& pastVariance = (
                autocovariance[pastPosition].back()[0][0]
                +autocovariance[pastPosition].back()[1][1])/2;

        tap = covariance / pastVariance;
        variance -= tap * covariance;

        if(variance < 0)
            throw;
    }
}

template<typename Symbol>
void gr::gs::Implementations::ProbabilityMapper<Symbol>::weightings(
        const int rds,
        const unsigned codewordPosition,
        std::vector<double>& weightings,
        const bool real) const
{
    const auto& constellation = this->constellation(real);
    const double& variance = m_variances[codewordPosition];
    const double mean = static_cast<double>(rds) * m_taps[codewordPosition];

    if(variance == 0)
    {
        for(Symbol symbol=0; symbol<constellation.size(); ++symbol)
        {
            if(rds+constellation[symbol] == mean)
                weightings[symbol] = 1.0;
            else
                weightings[symbol] = 0.0;
        }
    }
    else
        for(Symbol symbol=0; symbol<constellation.size(); ++symbol)
            weightings[symbol] = gaussian(
                    rds+constellation[symbol],
                    mean,
                    variance);
}

template<typename Symbol>
double gr::gs::Implementations::ProbabilityMapper<Symbol>::gaussian(
        int value,
        double mean,
        double variance) const
{
    return std::exp(-std::pow(static_cast<double>(value)-mean, 2)/(2*variance));
}

template class gr::gs::Implementations::ProbabilityMapper<unsigned char>;
template class gr::gs::Implementations::ProbabilityMapper<unsigned short>;
template class gr::gs::Implementations::ProbabilityMapper<unsigned int>;
