/*!
 * @file      ProbabilityMapper.cpp
 * @brief     Defines the ProbabilityMapper class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 14, 2017
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

#include "ProbabilityMapper.hpp"
#include "gr-gs/Data.h"

#include <gsl/gsl_linalg.h>
#include <cmath>
#include <map>
#include <memory>
#include <algorithm>

#include <iostream>
//#include <iomanip>

template<typename Symbol>
gr::gs::Implementations::ProbabilityMapper<Symbol>::ProbabilityMapper(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const unsigned windowSize,
        const bool doubleEnded):
    m_taps(codewordLength),
    m_variances(codewordLength),
    m_codewordLength(codewordLength),
    m_history(0)
{
    // Setup constellation stuff
    {
        const auto constellation = gr::gs::defaultConstellation(fieldSize);
        m_constellation.reserve(constellation.size());
        m_constellation.assign(constellation.begin(), constellation.end());

        std::set<double> reals;
        std::set<double> imags;
        for(const auto& point: m_constellation)
        {
            reals.insert(point.real());
            imags.insert(point.imag());
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
                        std::complex<double>(
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

    // We need this to do modular subtraction
    const unsigned denegatizer
        =(autocovarianceDataLength/codewordLength+1)*codewordLength;

    for(unsigned position=0; position<codewordLength; ++position)
    {
        double& variance = m_variances[position];
        variance = (
                autocovariance[position].back()[0][0]
                +autocovariance[position].back()[1][1])/2;
        if(variance == 0)
            continue;

        // Since the autocovariance data we've got only looks in the past, we
        // need to do some fun stuff to get the future values
        std::vector<double> leftAutocovariance;
        leftAutocovariance.reserve(autocovarianceDataLength);
        std::vector<double> rightAutocovariance;
        rightAutocovariance.reserve(autocovarianceDataLength);

        //std::cout << "  Autocorrelation:\n";
        for(unsigned tau=0; tau<autocovarianceDataLength; ++tau)
        {
            // Our autocovariance matrix is tau-reversed
            const unsigned index = autocovarianceDataLength-1-tau;

            // Here we find our covariance values for the past
            const double leftPosition=(denegatizer+position-tau)%codewordLength;
            const double leftVariance = (
                    autocovariance[leftPosition].back()[0][0]
                    +autocovariance[leftPosition].back()[1][1])/2;
            const double leftCovariance = (
                    autocovariance[position][index][0][0]
                    +autocovariance[position][index][1][1])/2;
            const double leftCorrelation
                = leftCovariance/std::sqrt(variance*leftVariance);

            // Here we find our covariance values for the future
            const double rightPosition = (position+tau)%codewordLength;
            const double rightVariance = (
                    autocovariance[rightPosition].back()[0][0]
                    +autocovariance[rightPosition].back()[1][1])/2;
            const double rightCovariance = (
                    autocovariance[rightPosition][index][0][0]
                    +autocovariance[rightPosition][index][1][1])/2;
            const double rightCorrelation
                = rightCovariance/std::sqrt(variance*rightVariance);

            // Has the correlation decayed sufficiently?
            if(
                    rightCorrelation < minCorrelation &&
                    leftCorrelation < minCorrelation)
                break;

            leftAutocovariance.push_back(leftCovariance);
            rightAutocovariance.push_back(rightCovariance);
        }

        const unsigned tapLength = leftAutocovariance.size()-1;

        std::vector<double> CaaInv(tapLength*tapLength);
        {
            auto CaaInvView = gsl_matrix_view_array(
                    CaaInv.data(),
                    tapLength,
                    tapLength);
            auto Caa = gsl_matrix_alloc(tapLength, tapLength);
            for(unsigned row=0; row<tapLength; ++row)
                for(unsigned column=0; column<tapLength; ++column)
                {
                    const int tau =
                        static_cast<int>(column)-static_cast<int>(row);
                    const unsigned index = static_cast<unsigned>(std::abs(tau));
                    const double value(
                        tau>0?
                            rightAutocovariance[index]:
                            leftAutocovariance[index]);
                    gsl_matrix_set(Caa, row, column, value);
                }

            auto CaaP = gsl_permutation_alloc(tapLength);

            int s;
            gsl_linalg_LU_decomp(Caa, CaaP, &s);
            gsl_linalg_LU_invert(Caa, CaaP, &CaaInvView.matrix);
            gsl_permutation_free(CaaP);
            gsl_matrix_free(Caa);
        }

        std::vector<double>& taps = m_taps[position];
        taps.resize(tapLength);

        for(unsigned column=0; column<tapLength; ++column)
            for(unsigned row=0; row<tapLength; ++row)
                taps[column] += CaaInv[row*tapLength+column]
                    * leftAutocovariance[tapLength-row];

        for(unsigned index=0; index<tapLength; ++index)
            variance -=
                taps[index] * rightAutocovariance[tapLength-index];

        if(variance < 0)
            throw;
    }

    for(unsigned index=0; index<m_codewordLength; ++index)
    {
        unsigned newHistory = m_taps[index].size();
        if(newHistory > m_history)
            m_history=newHistory;
    }
    m_inputSize = windowSize+m_history;
    if(doubleEnded)
        m_inputSize += m_history;
    m_buffer.reset(new double[m_inputSize+1]);
}

template<typename Symbol>
float gr::gs::Implementations::ProbabilityMapper<Symbol>::map(
        const Symbol symbol,
        const double* rds,
        unsigned codewordPosition,
        const bool real) const
{
    double mean=0;
    const double* pastRDS = rds;

    for(
            auto tap = m_taps[codewordPosition].crbegin();
            tap != m_taps[codewordPosition].crend();
            ++tap)
    {
        mean += *tap * *pastRDS;
        --pastRDS;
    }

    return probability(
            *rds,
            symbol,
            mean,
            m_variances[codewordPosition],
            real);
}

template<typename Symbol>
void gr::gs::Implementations::ProbabilityMapper<Symbol>::map(
        const Symbol* const input,
        bool computeHistory,
        float* output,
        unsigned codewordPosition,
        const bool real) const
{
    const auto& constellation = this->constellation(real);
    {
        double* rds = m_buffer.get();
        const Symbol* const inputEnd = input+m_inputSize;

        if(!computeHistory)
        {
            double* const rdsHistoryEnd = rds+m_history+1;
            std::fill(rds, rdsHistoryEnd, 0);
            rds = rdsHistoryEnd;
            for(
                    const Symbol* symbol = input+m_history;
                    symbol < inputEnd;
                    ++symbol, ++rds)
                *rds = *(rds-1) + constellation[*symbol];
        }
        else
        {
            *rds++ = 0;

            double meanRDS = 0;

            for(
                    const Symbol* symbol = input; symbol < inputEnd; ++symbol)
            {
                *rds = *(rds-1) + constellation[*symbol];
                meanRDS += *rds;
                ++rds;
            }
            meanRDS /= m_inputSize;
            meanRDS = std::round(meanRDS);

            rds = m_buffer.get();
            *rds++ = -meanRDS;

            for(
                    const Symbol* symbol = input;
                    symbol < inputEnd;
                    ++symbol, ++rds)
                *rds = *(rds-1) + constellation[*symbol];
        }
    }

    const double* rds = m_buffer.get()+m_history;
    const double* const rdsEnd = m_buffer.get()+m_inputSize;

    const Symbol* inputIt = input+m_history;

    while(rds != rdsEnd)
    {
        *output++ = map(
                *inputIt++,
                rds++,
                codewordPosition++,
                real);

        if(codewordPosition >= m_codewordLength)
            codewordPosition = 0;
    }
}

template<typename Symbol>
float gr::gs::Implementations::ProbabilityMapper<Symbol>::probability(
        double rds,
        Symbol symbol,
        double mean,
        double variance,
        const bool real) const
{
    const auto& constellation = this->constellation(real);

    if(variance == 0)
    {
        if(rds+constellation[symbol] == mean)
            return 1.0;
        else
            return 0.0;
    }

    double numerator=0;
    double denominator=0;
    for(unsigned i=0; i<constellation.size(); ++i)
    {
        const double value = gaussian(rds+constellation[i], mean, variance);
        denominator += value;
        if(i == symbol)
            numerator = value;
    }

    return static_cast<float>(numerator/denominator);
}

template<typename Symbol>
double gr::gs::Implementations::ProbabilityMapper<Symbol>::gaussian(
        double value,
        double mean,
        double variance) const
{
    return std::exp(-std::pow(value-mean, 2)/(2*variance));
}

template class gr::gs::Implementations::ProbabilityMapper<unsigned char>;
template class gr::gs::Implementations::ProbabilityMapper<unsigned short>;
template class gr::gs::Implementations::ProbabilityMapper<unsigned int>;

template<typename Symbol>
void gr::gs::Implementations::ProbabilityMapper<Symbol>::collapseConstellation(
        Symbol* reals,
        Symbol* imags,
        const Symbol* points,
        const size_t length) const
{
    for(unsigned i=0; i<length; ++i)
    {
        reals[i] = m_constellationToReal[points[i]];
        imags[i] = m_constellationToImag[points[i]];
    }
}

template<typename Symbol>
void gr::gs::Implementations::ProbabilityMapper<Symbol>::decollapseConstellation(
        const Symbol* reals,
        const Symbol* imags,
        Symbol* points,
        const size_t length) const
{
    for(unsigned i=0; i<length; ++i)
        points[i] = decollapseConstellation(reals[i], imags[i]);
}
