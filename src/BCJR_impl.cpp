/*!
 * @file      BCJR_impl.cpp
 * @brief     Defines the "Guided Scrambling BCJR" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 18, 2017
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

#include "BCJR_impl.hpp"
#include "gr-gs/Data.h"

#include <gnuradio/io_signature.h>
#include <algorithm>
#include <limits>
#include <numeric>

template<typename Symbol>
double gr::gs::Implementations::BCJR_impl<Symbol>::noisePower() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_noisePower;
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::set_noisePower(
        const double noise)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_noisePower = noise;
    m_realTrellis.set_noisePower(noise);
    m_imagTrellis.set_noisePower(noise);
}

template<typename Symbol>
int gr::gs::Implementations::BCJR_impl<Symbol>::general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const Complex*& input = reinterpret_cast<const Complex*&>(input_items[0]);
    Symbol*& output = reinterpret_cast<Symbol*&>(output_items[0]);

    unsigned outputted=0;

    m_realTrellis.insert(input, ninput_items[0]);
    m_imagTrellis.insert(input, ninput_items[0]);
    this->consume_each(ninput_items[0]);

    auto reals = m_realTrellis.output();
    auto real = reals.begin();
    auto imags = m_imagTrellis.output();
    auto imag = imags.begin();
    while(
            real != reals.end() &&
            imag != imags.end() &&
            outputted < static_cast<unsigned>(noutput_items))
    {
        *output++ = m_mapper.decollapseConstellation(*real++, *imag++);
        ++outputted;
    }

    reals.erase(
            reals.begin(),
            real);
    if(!reals.empty())
        m_realTrellis.putBack(std::move(reals));

    imags.erase(
            imags.begin(),
            imag);
    if(!imags.empty())
        m_imagTrellis.putBack(std::move(imags));

    return outputted;
}

template<typename Symbol>
unsigned gr::gs::Implementations::BCJR_impl<Symbol>::getBound(
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

    return std::max(maximum, std::abs(minimum));
}

template<typename Symbol>
gr::gs::Implementations::BCJR_impl<Symbol>::BCJR_impl(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const double noise):
    gr::block("Guided Scrambling BCJR",
        io_signature::make(1,1,sizeof(gr::gs::Complex)),
        io_signature::make(1,1,sizeof(Symbol))),
    m_noisePower(noise),
    m_codewordLength(codewordLength),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength,
            minCorrelation,
            1024,
            false),
    m_bound(getBound(fieldSize, codewordLength, augmentingLength)),
    m_realTrellis(true, m_mapper, codewordLength, noise, m_bound),
    m_imagTrellis(false, m_mapper, codewordLength, noise, m_bound)
{
    this->set_relative_rate(1);
    this->enable_update_rate(false);
}

template<typename Symbol>
typename gr::gs::BCJR<Symbol>::sptr gr::gs::BCJR<Symbol>::make(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double minCorrelation,
        const double noise)
{
    return gnuradio::get_initial_sptr(
            new Implementations::BCJR_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                minCorrelation,
                noise));
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::append(
        const double* distances)
{
    std::map<int, std::shared_ptr<Node>> head;

    std::shared_ptr<std::set<Node*>> set(new std::set<Node*>);

    std::vector<double> weightings(m_constellation.size());

    for(auto& source: m_head)
    {
        m_mapper.weightings(
                source.second->rds(),
                m_codewordPosition,
                weightings,
                m_real);
        const double sum = std::accumulate(
                    weightings.cbegin(),
                    weightings.cend(),
                    static_cast<double>(0));
        for(auto& weighting: weightings)
            weighting /= sum;

        for(
                Symbol symbol=0;
                symbol<m_constellation.size();
                ++symbol)
        {
            const int rds = source.second->m_rds + m_constellation[symbol];

            if(static_cast<unsigned>(std::abs(rds)) > m_bound)
                continue;

            auto& destination = head[rds];
            if(!destination)
                destination.reset(new Node(*this, rds, set));

            const double metric = source.second->m_metric
                + *(distances+symbol)
                - m_noisePower*std::log(weightings[symbol]);

            if(metric < destination->m_metric)
            {
                destination->m_metric = metric;
                destination->m_source = source.second;
                destination->m_symbol = symbol;
            }
        }
    }

    m_head.swap(head);

    double minMetric=std::numeric_limits<double>::max();
    for(const auto& node: m_head)
        if(node.second->m_metric < minMetric)
            minMetric = node.second->m_metric;

    for(auto node=m_head.begin(); node!=m_head.end();)
    {
        if(node->second->m_metric > minMetric+10)
            node = m_head.erase(node);
        else
        {
            node->second->m_metric -= minMetric;
            ++node;
        }
    }

    m_codewordPosition
        = (m_codewordPosition+1)%m_codewordLength;
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::Node::close(
        const unsigned depth)
{
    if(m_set->empty())
    {
        if(!m_trellis.m_outputBuffer.empty())
        {
            std::lock_guard<std::mutex> lock(m_trellis.m_mutex);
            m_trellis.m_output.splice(
                    m_trellis.m_output.end(),
                    m_trellis.m_outputBuffer);
        }
    }
    else
    {
        m_trellis.m_outputBuffer.push_front(m_symbol);
        m_set->clear();
    }

    if(m_source)
        m_source->close(depth+1);

    if(depth >= m_trellis.m_history)
        m_source.reset();
}

template<typename Symbol>
gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::Trellis(
        const bool real,
        const ProbabilityMapper<Symbol>& mapper,
        const unsigned codewordLength,
        const double noisePower,
        const unsigned bound):
    m_real(real),
    m_constellation(mapper.constellation(real)),
    m_mapper(mapper),
    m_history(mapper.history()),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_bound(bound),
    m_noisePower(noisePower)
{
    std::shared_ptr<std::set<Node*>> set(new std::set<Node*>);
    std::shared_ptr<Node> node(new Node(*this, 0, set));
    node->m_metric = 0;
    m_head[0] = node;
    for(unsigned i=0; i<m_mapper.history()-1; ++i)
    {
        std::shared_ptr<Node> newNode(new Node(*this, 0, set));
        newNode->m_metric=0;
        node->m_source = newNode;
        node = newNode;
    }
    set->clear();
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::insert(
        const Complex* input,
        size_t size)
{
    std::vector<double> distances(m_constellation.size());
    for(unsigned i=0; i<size; ++i)
    {
        const double value = m_real?input->real():input->imag();
        ++input;
        for(Symbol symbol=0; symbol < m_constellation.size(); ++symbol)
        {
            const double difference = value - m_constellation[symbol];
            distances[symbol] = difference*difference;
        }

        append(distances.data());
    }
}

template<typename Symbol>
gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::Node::Node(
        Trellis& trellis,
        int rds,
        std::shared_ptr<std::set<Node*>> set):
    m_trellis(trellis),
    m_set(set),
    m_rds(rds),
    m_metric(std::numeric_limits<double>::max())
{
    m_set->insert(this);
}

template<typename Symbol>
gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::Node::~Node()
{
    m_set->erase(this);
    if(m_set->size()==1)
        (*m_set->begin())->close(0);
}

template<typename Symbol>
typename gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::RDSiterator
gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::Node::rds() const
{
    return RDSiterator(this);
}

template<typename Symbol>
int gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::RDSiterator::operator*(
        ) const
{
    return m_node->m_rds;
}

template<typename Symbol>
typename gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::RDSiterator&
gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::RDSiterator::operator--()
{
    m_node = m_node->m_source.get();
    return *this;
}

template<typename Symbol>
gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::RDSiterator::RDSiterator(
        const Node* node):
    m_node(node)
{
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::set_noisePower(
        const double noisePower)
{
    m_noisePower = noisePower;
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::set_codewordPosition(
        const unsigned codewordPosition)
{
    m_codewordPosition = codewordPosition;
}

template<typename Symbol>
std::list<Symbol> gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::output()
{
    std::list<Symbol> output;
    std::lock_guard<std::mutex> lock(m_mutex);
    output.swap(m_output);
    return output;
}

template<typename Symbol>
void gr::gs::Implementations::BCJR_impl<Symbol>::Trellis::putBack(
        std::list<Symbol>&& output)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_output.splice(m_output.begin(), output);
}

template class gr::gs::BCJR<unsigned char>;
template class gr::gs::BCJR<unsigned short>;
template class gr::gs::BCJR<unsigned int>;
