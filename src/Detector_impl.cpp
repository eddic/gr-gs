/*!
 * @file      Detector_impl.cpp
 * @brief     Defines the "Guided Scrambling Detector" GNU Radio block
 *            implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      November 21, 2018
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

#include "Detector_impl.hpp"
#include "gr-gs/Data.h"

#include <gnuradio/io_signature.h>
#include <algorithm>
#include <limits>

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
                m_alignmentTag);
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
        auto reals = m_realTrellis.output();
        auto real = reals.begin();

        if(m_fieldSize > 2)
        {
            m_imagTrellis.insert(input, inputSize);
            auto imags = m_imagTrellis.output();
            auto imag = imags.begin();
            while(
                    real != reals.end() &&
                    imag != imags.end() &&
                    outputted < static_cast<unsigned>(noutput_items))
            {
                *output++ = m_mapper.decollapseConstellationPoint(*real++, *imag++);
                ++outputted;
            }

            imags.erase(
                    imags.begin(),
                    imag);
            if(!imags.empty())
                m_imagTrellis.putBack(std::move(imags));
        }
        else
        {
            while(
                    real != reals.end() &&
                    outputted < static_cast<unsigned>(noutput_items))
            {
                *output++ = *real++;
                ++outputted;
            }
        }

        reals.erase(
                reals.begin(),
                real);
        if(!reals.empty())
            m_realTrellis.putBack(std::move(reals));
    }

    this->consume_each(ninput_items[0]);
    return outputted;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::forecast(
        int noutput_items,
        gr_vector_int& ninput_items_required)
{
    const unsigned outputSize = m_fieldSize>2?std::min(
            m_realTrellis.outputSize(),
            m_imagTrellis.outputSize()):m_realTrellis.outputSize();

    if(outputSize >= static_cast<unsigned>(noutput_items))
        ninput_items_required[0] = 0;
    else
        ninput_items_required[0] = noutput_items - outputSize;
}

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Detector_impl(
        const unsigned fieldSize,
        const unsigned codewordLength,
        const unsigned augmentingLength,
        const double noise,
        const std::string& alignmentTag,
        const double nodeDiscardMetric):
    gr::block("Guided Scrambling Detector",
        io_signature::make(1,1,sizeof(gr::gs::Complex)),
        io_signature::make(1,1,sizeof(Symbol))),
    m_fieldSize(fieldSize),
    m_noisePower(noise),
    m_alignmentTag(pmt::string_to_symbol(alignmentTag)),
    m_aligned(alignmentTag.empty()),
    m_codewordLength(codewordLength),
    m_mapper(
            fieldSize,
            codewordLength,
            augmentingLength),
    m_realTrellis(
            true,
            m_mapper,
            codewordLength,
            noise,
            nodeDiscardMetric),
    m_imagTrellis(
            false,
            m_mapper,
            codewordLength,
            noise,
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
        const std::string& alignmentTag,
        const double nodeDiscardMetric)
{
    return gnuradio::get_initial_sptr(
            new Implementations::Detector_impl<Symbol>(
                fieldSize,
                codewordLength,
                augmentingLength,
                noise,
                alignmentTag,
                nodeDiscardMetric));
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::append(
        const double* distances)
{
    std::map<int, std::shared_ptr<Node>> head;
    std::shared_ptr<std::set<Node*>> set(new std::set<Node*>);

    for(auto& source: m_head)
        for(
                Symbol symbol=0;
                symbol<m_constellation.size();
                ++symbol)
        {
            const int rds = source.second->m_rds + m_constellation[symbol];

            if(static_cast<unsigned>(std::abs(rds)) > m_mapper.maxRDS)
                continue;

            const double information = m_mapper.information(
                    m_codewordPosition,
                    source.second->m_rds,
                    symbol);
            if(std::isinf(information))
                continue;

            const double metric = source.second->m_metric
                + *(distances+symbol)
                + m_noisePower*information;

            auto& destination = head[rds];
            if(!destination)
                destination.reset(new Node(*this, rds, set));

            if(metric < destination->m_metric)
            {
                destination->m_metric = metric;
                destination->m_source = source.second;
                destination->m_symbol = symbol;
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

    if(depth >= 1)
        m_source.reset();
}

template<typename Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::Trellis(
        const bool real,
        const ProbabilityMapper<Symbol>& mapper,
        const unsigned codewordLength,
        const double noisePower,
        const double nodeDiscardMetric):
    m_real(real),
    m_constellation(mapper.collapsed()),
    m_mapper(mapper),
    m_codewordLength(codewordLength),
    m_codewordPosition(0),
    m_noisePower(noisePower),
    m_nodeDiscardMetric(nodeDiscardMetric)
{
    std::shared_ptr<std::set<Node*>> set(new std::set<Node*>);
    std::shared_ptr<Node> node(new Node(*this, 0, set));
    node->m_metric = 0;
    m_head[0] = node;
    set->clear();
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::insert(
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
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::set_noisePower(
        const double noisePower)
{
    m_noisePower = noisePower;
}

template<typename Symbol> std::list<Symbol>
gr::gs::Implementations::Detector_impl<Symbol>::Trellis::output()
{
    std::list<Symbol> output;
    std::lock_guard<std::mutex> lock(m_mutex);
    output.swap(m_output);
    return output;
}

template<typename Symbol>
void gr::gs::Implementations::Detector_impl<Symbol>::Trellis::putBack(
        std::list<Symbol>&& output)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_output.splice(m_output.begin(), output);
}

template class gr::gs::Detector<unsigned char>;
template class gr::gs::Detector<unsigned short>;
template class gr::gs::Detector<unsigned int>;
