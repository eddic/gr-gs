/*!
 * @file      Detector_impl.cpp
 * @brief     Defines the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      December 19, 2017
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

#include "Detector_impl.hpp"
#include "gr-gs/Data.h"

#include <gnuradio/io_signature.h>
#include <algorithm>
#include <limits>
#include <numeric>

template<typename Symbol>
double gr::gs::Implementations::Detector_impl<Symbol>::noisePower() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_noisePower;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::set_noisePower(
        const double noise)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_noisePower = noise;
    m_realTrellis.set_noisePower(noise);
    m_imagTrellis.set_noisePower(noise);
}

template<typename Symbol>
int gr::gs::Implementations::Detector_impl<Symbol>::general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const Complex*& input = reinterpret_cast<const Complex*&>(input_items[0]);
    Symbol*& output = reinterpret_cast<Symbol*&>(output_items[0]);
    size_t inputSize = ninput_items[0];

    unsigned outputted=0;

    // Get our codeword lined up
    if(!m_aligned)
    {
        std::vector<gr::tag_t> tags;
        std::vector<gr::tag_t>::const_iterator tag;

        this->get_tags_in_range(
                tags,
                0,
                this->nitems_read(0),
                this->nitems_read(0)+noutput_items,
                m_framingTagPMT);
        tag = tags.cbegin();

        if(tag != tags.cend())
        {
            const size_t offset = tag->offset - this->nitems_read(0);
            input += offset;
            output += offset;
            inputSize -= offset;
            outputted += offset;
            m_aligned = true;
        }
        else
            outputted = noutput_items;
    }

    if(m_aligned)
    {
        m_realTrellis.insert(input, inputSize);
        m_imagTrellis.insert(input, inputSize);

        auto& reals = m_realTrellis.output();
        auto& imags = m_imagTrellis.output();
        auto real = reals.begin();
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
        imags.erase(
                imags.begin(),
                imag);
    }

    this->consume_each(ninput_items[0]);
    return outputted;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::forecast(
        int noutput_items,
        gr_vector_int& ninput_items_required)
{
    const unsigned outputSize = std::min(
            m_realTrellis.outputSize(),
            m_imagTrellis.outputSize());

    if(outputSize >= static_cast<unsigned>(noutput_items))
        ninput_items_required[0] = 0;
    else
        ninput_items_required[0] = noutput_items - outputSize;
}

template<typename Symbol>
unsigned gr::gs::Implementations::Detector_impl<Symbol>::getBound(
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

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Detector_impl(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double noise,
        const std::string& framingTag,
        const double minCorrelation,
        const double nodeDiscardMetric):
    gr::block("Guided Scrambling Detector",
        io_signature::make(1,1,sizeof(gr::gs::Complex)),
        io_signature::make(1,1,sizeof(Symbol))),
    m_noisePower(noise),
    m_framingTag(framingTag),
    m_framingTagPMT(pmt::string_to_symbol(framingTag)),
    m_aligned(framingTag.empty()),
    m_codewordLength(codewordLength),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength,
            minCorrelation,
            1024,
            false),
    m_bound(getBound(fieldSize, codewordLength, augmentingLength)),
    m_realTrellis(
            true,
            m_mapper,
            codewordLength,
            noise,
            m_bound,
            nodeDiscardMetric),
    m_imagTrellis(
            false,
            m_mapper,
            codewordLength,
            noise,
            m_bound,
            nodeDiscardMetric)
{
    this->set_relative_rate(1);
    this->enable_update_rate(false);
}

template<typename Symbol>
typename gr::gs::Detector<Symbol>::sptr gr::gs::Detector<Symbol>::make(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double noise,
        const std::string& framingTag,
        const double minCorrelation,
        const double nodeDiscardMetric)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Detector_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                noise,
                framingTag,
                minCorrelation,
                nodeDiscardMetric));
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::append(
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
        if(node->second->m_metric > minMetric+m_nodeDiscardMetric)
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
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::Node::close(
        const unsigned depth)
{
    if(m_set->empty())
    {
        if(!m_trellis.m_outputBuffer.empty())
        {
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
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::Trellis(
        const bool real,
        const ProbabilityMapper<Symbol>& mapper,
        const unsigned codewordLength,
        const double noisePower,
        const unsigned bound,
        const double nodeDiscardMetric):
    m_real(real),
    m_constellation(mapper.constellation(real)),
    m_mapper(mapper),
    m_history(mapper.history()),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_bound(bound),
    m_noisePower(noisePower),
    m_nodeDiscardMetric(nodeDiscardMetric),
    m_stop(false),
    m_thread(
            &::gr::gs::Implementations::Detector_impl<Symbol>::Trellis::handler,
            this)
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
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::~Trellis()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
        m_cv.notify_one();
    }
    m_thread.join();
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::insert(
        const Complex* input,
        size_t size)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_input = input;
    m_inputSize = size;
    m_cv.notify_one();
    m_cv.wait(lock);
}

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::Node::Node(
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
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::Node::~Node()
{
    m_set->erase(this);
    if(m_set->size()==1)
        (*m_set->begin())->close(0);
}

template<typename Symbol>
typename gr::gs::Implementations::Detector_impl<Symbol>::Trellis::RDSiterator
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::Node::rds() const
{
    return RDSiterator(this);
}

template<typename Symbol> int
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::RDSiterator::operator*(
        ) const
{
    return m_node->m_rds;
}

template<typename Symbol>
typename gr::gs::Implementations::Detector_impl<Symbol>::Trellis::RDSiterator&
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::RDSiterator::operator--()
{
    m_node = m_node->m_source.get();
    return *this;
}

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::RDSiterator::RDSiterator(
        const Node* node):
    m_node(node)
{
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::set_noisePower(
        const double noisePower)
{
    m_noisePower = noisePower;
}

template<typename Symbol> std::list<Symbol>&
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::output()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_output;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::handler()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(!m_stop)
    {
        m_cv.wait(lock);
        m_cv.notify_one();
        if(m_stop)
            break;

        std::vector<double> distances(m_constellation.size());
        for(unsigned i=0; !m_stop && i<m_inputSize; ++i)
        {
            const double value = m_real?m_input->real():m_input->imag();
            ++m_input;
            for(Symbol symbol=0; symbol < m_constellation.size(); ++symbol)
            {
                const double difference = value - m_constellation[symbol];
                distances[symbol] = difference*difference;
            }

            append(distances.data());
        }
    }
}

template class gr::gs::Detector<unsigned char>;
template class gr::gs::Detector<unsigned short>;
template class gr::gs::Detector<unsigned int>;
