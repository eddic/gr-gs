/*!
 * @file      ProbabilityMapper.cpp
 * @brief     Defines the ProbabilityMapper class
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 22, 2017
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
    {
        const auto constellation = gr::gs::defaultConstellation(fieldSize);
        m_constellation.reserve(constellation.size());
        m_constellation.assign(constellation.begin(), constellation.end());
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
        //std::cout << "**** Codeword Position: " << position << " ****\n";
        double& variance = m_variances[position];
        variance = (
                autocovariance[position].back()[0][0]
                +autocovariance[position].back()[1][1])/2;
        if(variance == 0)
        {
            //std::cout << "Got zero variance for " << position << std::endl;
            continue;
        }

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

            //std::cout << "    " << leftCovariance << ' ' << rightCovariance << '\n';

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

        /*std::cout << "  Taps:\n";
        for(const auto& tap: taps)
            std::cout << "    " << tap << '\n';*/

        if(variance < 0)
        {
            //std::cout << "Variance = " << variance << std::endl;
            throw;
        }
    }

    for(unsigned index=0; index<m_codewordLength; ++index)
    {
        unsigned newHistory = m_taps[index].size();
        if(newHistory > m_history)
            m_history=newHistory;
    }
    //std::cout << "History = " << m_history << '\n';
    m_inputSize = windowSize+m_history;
    if(doubleEnded)
        m_inputSize += m_history;
    m_buffer.reset(new std::complex<double>[m_inputSize+1]);
}

template<typename Symbol>
void gr::gs::Implementations::ProbabilityMapper<Symbol>::map(
        const Symbol* const input,
        bool computeHistory,
        float* output,
        unsigned codewordPosition) const
{
    {
        std::complex<double>* rds = m_buffer.get();
        const Symbol* const inputEnd = input+m_inputSize;

        if(!computeHistory)
        {
            std::complex<double>* const rdsHistoryEnd = rds+m_history+1;
            std::fill(rds, rdsHistoryEnd, std::complex<double>(0,0));
            rds = rdsHistoryEnd;
            for(
                    const Symbol* symbol = input+m_history;
                    symbol < inputEnd;
                    ++symbol, ++rds)
                *rds = *(rds-1) + m_constellation[*symbol];
        }
        else
        {
            *rds++ = std::complex<double>(0,0);

            std::complex<double> meanRDS = std::complex<double>(0,0);

            for(
                    const Symbol* symbol = input; symbol < inputEnd; ++symbol)
            {
                *rds = *(rds-1) + m_constellation[*symbol];
                meanRDS += *rds;
                ++rds;
            }
            meanRDS /= m_inputSize;
            meanRDS.real(std::round(meanRDS.real()));
            meanRDS.imag(std::round(meanRDS.imag()));

            rds = m_buffer.get();
            *rds++ = -meanRDS;

            for(
                    const Symbol* symbol = input;
                    symbol < inputEnd;
                    ++symbol, ++rds)
                *rds = *(rds-1) + m_constellation[*symbol];
        }
    }

    /*std::cout << "** History **\n";
    const Symbol* ourInput = input;
    for(const std::complex<double>* rds=m_buffer.get(); rds<m_buffer.get()+m_history+1; ++rds, ++ourInput)
    {
        std::cout << *rds << ' ' << static_cast<unsigned>(*ourInput) << ' ';
    }
    std::cout << '\n';*/

    const std::complex<double>* rds = m_buffer.get()+m_history;
    const std::complex<double>* const rdsEnd = m_buffer.get()+m_inputSize;

    //std::cout << "** RDSs **\n";
    const Symbol* inputIt = input+m_history;

    while(rds != rdsEnd)
    {
        const std::complex<double>* pastRDS = rds;
        std::complex<double> mean=0;

        /*std::cout << "Symbol = " << static_cast<unsigned>(*inputIt) << ' ' << m_constellation[*inputIt] << '\n';
        std::cout << "  Starting RDS = " << *rds << '\n';
        std::cout << "  Ending RDS = " << *(rds+1) << '\n';
        std::cout << "  Mean =";*/
        for(
                auto tap = m_taps[codewordPosition].crbegin();
                tap != m_taps[codewordPosition].crend();
                ++tap)
        {
            mean += *tap * *pastRDS;
            //std::cout << " + (" << *tap << '*' << *pastRDS << ')';
            --pastRDS;
        }
        /*std::cout << " = " << mean << '\n';
        std::cout << "  Variance = " << m_variances[codewordPosition] << '\n';*/
        *output = probability(
                *rds,
                *inputIt++,
                mean,
                m_variances[codewordPosition]);
        //std::cout << "  Probability = " << *output << '\n';

        ++rds;

        if(++codewordPosition >= m_codewordLength)
            codewordPosition = 0;

        ++output;
    }

    //std::cout << "Last Symbols = " << static_cast<unsigned>(input[symbols+m_history-3]) << ", " << static_cast<unsigned>(input[symbols+m_history-2]) << ", " << static_cast<unsigned>(input[symbols+m_history-1]) << '\n';
}

template<typename Symbol>
float gr::gs::Implementations::ProbabilityMapper<Symbol>::probability(
        std::complex<double> rds,
        Symbol symbol,
        std::complex<double> mean,
        double variance) const
{
    if(variance == 0)
    {
        if(rds+m_constellation[symbol] == mean)
            return 1.0;
        else
        {
            //std::cout << "Mean=" << mean << " vs RDS=" << rds+m_constellation[symbol] << std::endl;
            return 0.0;
        }
    }

    double numerator=0;
    double denominator=0;
    std::map<double, double> realProbabilities;
    std::map<double, double> imagProbabilities;
    for(unsigned i=0; i<m_constellation.size(); ++i)
    {
        const double& realPoint = m_constellation[i].real();
        auto realProbability = realProbabilities.find(realPoint);
        if(realProbability == realProbabilities.end())
            realProbability = realProbabilities.insert(
                    std::make_pair(
                        realPoint,
                        gaussian(
                            rds.real()+realPoint,
                            mean.real(),
                            variance))).first;

        const double& imagPoint = m_constellation[i].imag();
        auto imagProbability = imagProbabilities.find(imagPoint);
        if(imagProbability == imagProbabilities.end())
            imagProbability = imagProbabilities.insert(
                    std::make_pair(
                        imagPoint,
                        gaussian(
                            rds.imag()+imagPoint,
                            mean.imag(),
                            variance))).first;

        const double value = realProbability->second * imagProbability->second;

        denominator += value;
        if(i == symbol)
            numerator = value;
    }

    //auto probs = numerator/denominator;

    /*if(probs < 0.00001)
    {
        std::cout << "PMF mean=" << mean << " variance=" << variance << " rds=" << rds << " symbol=" << m_constellation[symbol] << '\n';
        for(const auto& constellationPoint: m_constellation)
        {
            const double value = realProbabilities[constellationPoint.real()]
                * imagProbabilities[constellationPoint.imag()];
            std::cout << "    " << constellationPoint << " = " << value/denominator << '\n';
        }
    }*/

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
