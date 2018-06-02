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
    maxRDS(getMaxRDS(fieldSize, codewordLength, augmentingLength))
{
    std::vector<double> taps(codewordLength);
    std::vector<double> variances(codewordLength);

    // Setup constellation stuff
    {
        const auto constellation = gr::gs::defaultConstellation_i(fieldSize);
        m_constellation.reserve(constellation.size());
        m_constellation.assign(constellation.begin(), constellation.end());

        std::set<int> collapsed;
        for(const auto& point: m_constellation)
            collapsed.insert(point.real);
        m_collapsed.reserve(collapsed.size());
        m_collapsed.assign(collapsed.begin(), collapsed.end());

        m_collapsedToConstellation.resize(m_constellation.size());
        m_constellationToReal.resize(m_constellation.size());
        m_constellationToImag.resize(m_constellation.size());
        for(Symbol symbol=0; symbol<m_constellation.size(); ++symbol)
        {
            const auto& point = m_constellation[symbol];
            const Symbol realSymbol = std::find(
                    m_collapsed.cbegin(),
                    m_collapsed.cend(),
                    point.real)-m_collapsed.cbegin();
            const Symbol imagSymbol = std::find(
                    m_collapsed.cbegin(),
                    m_collapsed.cend(),
                    point.imag)-m_collapsed.cbegin();

            m_constellationToReal[symbol] = realSymbol;
            m_constellationToImag[symbol] = imagSymbol;
            m_collapsedToConstellation[
                realSymbol + imagSymbol*m_collapsed.size()] = symbol;
        }
    }

    const Data::Autocovariance autocovariance = Data::autocovariance(
            m_constellation.size(),
            codewordLength,
            augmentingLength);

    const unsigned covarianceIndex = autocovarianceDataLength-2;

    // Build taps and variances
    for(unsigned position=0; position<codewordLength; ++position)
    {
        double& variance = variances[position];
        double& tap = taps[position];

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

    // Build transition matrix
    m_probabilities.resize(codewordLength);
    for(unsigned position=0; position<codewordLength; ++position)
    {
        m_probabilities[position].resize(maxRDS*2-1);
        for(unsigned rdsIndex=0; rdsIndex < maxRDS*2-1; ++rdsIndex)
        {
            m_probabilities[position][rdsIndex].resize(fieldSize);

            const int rds = static_cast<int>(rdsIndex)-maxRDS;
            const double& variance = variances[position];
            const double mean = static_cast<double>(rds) * taps[position];

            if(variance == 0)
            {
                for(Symbol symbol=0; symbol<m_collapsed.size(); ++symbol)
                {
                    if(rds+m_collapsed[symbol] == mean)
                        m_probabilities[position][rdsIndex][symbol] = 1.0;
                    else
                        m_probabilities[position][rdsIndex][symbol] = 0.0;
                }
            }
            else
            {
                double sum=0;
                for(Symbol symbol=0; symbol<m_collapsed.size(); ++symbol)
                {
                    m_probabilities[position][rdsIndex][symbol] = gaussian(
                            rds+m_collapsed[symbol],
                            mean,
                            variance);
                    sum += m_probabilities[position][rdsIndex][symbol];
                }

                for(Symbol symbol=0; symbol<fieldSize; ++symbol)
                    m_probabilities[position][rdsIndex][symbol] /= sum;
            }
        }
    }
}

template<typename Symbol>
double gr::gs::Implementations::ProbabilityMapper<Symbol>::gaussian(
        int value,
        double mean,
        double variance)
{
    return std::exp(-std::pow(static_cast<double>(value)-mean, 2)/(2*variance));
}

template<typename Symbol>
unsigned gr::gs::Implementations::ProbabilityMapper<Symbol>::getMaxRDS(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength)
{
    const auto distribution = Data::distribution(
            fieldSize,
            codewordLength,
            augmentingLength);

    std::array<double, distributionDataWidth> collapsed;
    for(unsigned real=0; real<distributionDataWidth; ++real)
        for(unsigned position=0; position<codewordLength; ++position)
            for(unsigned imag=0; imag<distributionDataWidth; ++imag)
                collapsed[real] += distribution[position][real][imag];

    int minimum=0, maximum=0;
    for(unsigned i=0; i<distributionDataWidth; ++i)
    {
        if(minimum==0)
        {
            if(collapsed[i] > 1e-20)
                minimum=i;
        }
        else if(maximum==0 && collapsed[i] < 1e-20)
            maximum=i;
    }

    minimum -= distributionDataWidth/2;
    maximum -= distributionDataWidth/2;

    return std::max(maximum, std::abs(minimum))+1;
}

template class gr::gs::Implementations::ProbabilityMapper<unsigned char>;
template class gr::gs::Implementations::ProbabilityMapper<unsigned short>;
template class gr::gs::Implementations::ProbabilityMapper<unsigned int>;
