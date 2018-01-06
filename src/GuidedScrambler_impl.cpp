/*!
 * @file      GuidedScrambler_impl.cpp
 * @brief     Defines the "Guided Scrambler" GNU Radio block implementation
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

#include <condition_variable>
#include <cmath>
#include <algorithm>
#include <memory>
#include <gnuradio/io_signature.h>

#include "GuidedScrambler_impl.hpp"
#include "gr-gs/exceptions.h"
#include "Words.hpp"

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::killThreads()
{
    if(m_threads.size())
    {
        {
            std::lock_guard<std::mutex> lock(m_args.sleepMutex);
            m_cargs.sleep=false;
            m_args.sleepCV.notify_all();
        }
        for(auto thread=m_threads.begin(); thread!=m_threads.end(); ++thread)
            thread->join();
        m_threads.clear();
    }
}

template<typename Symbol> const std::vector<Symbol>&
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::scramble(
        const std::vector<Symbol>& input)
{
    std::unique_lock<std::mutex> sleepLock(m_args.sleepMutex);
    if(!m_threads.size())
    {
        if(m_augmentingLength < 1)
            throw Exceptions::AugmentingLengthTooSmall();

        if(m_augmentingLength >= m_codewordLength)
            throw Exceptions::AugmentingCodewordLengthMismatch();

        if(m_cargs.divider.size() < 2)
            throw Exceptions::DivisorLengthTooSmall();

        const unsigned int totalScramblers = std::pow(
                m_fieldSize,
                m_augmentingLength);
        m_groups = std::min(m_groups, totalScramblers);
        m_scramblerGroups.resize(m_groups);
        m_threads.resize(m_groups);
        m_cargs.remainder.resize(m_cargs.divider.size()-1);
        std::fill(m_cargs.remainder.begin(), m_cargs.remainder.end(), 0);
        m_cargs.sleep=true;
        m_cargs.feedback.reset(manufactureFeedback<Symbol>(m_selectionMethod));

        const unsigned int groupSize = (totalScramblers-1)/m_groups+1;
        for(unsigned int i=0; i<m_groups; ++i)
        {
            m_scramblerGroups[i].configure(
                    m_codewordLength,
                    i*groupSize,
                    std::min(totalScramblers,(i+1)*groupSize),
                    m_augmentingLength,
                    m_cargs.divider.size()-1,
                    m_selectionMethod,
                    m_fieldSize);
            m_threads[i] = std::thread(
                    &::gr::gs::GuidedScrambling::ScramblerGroup<Symbol>::handler,
                    &m_scramblerGroups[i],
                    std::ref(m_args),
                    std::cref(m_cargs));
        }
    }
    else
        m_args.sleepCV.notify_all();

    if(input.size() != m_codewordLength-m_augmentingLength)
        throw Exceptions::BadInputLength();

    m_cargs.input = &input;
    m_args.count = m_groups;
    std::unique_lock<std::mutex> countLock(m_args.countMutex);
    sleepLock.unlock();
    do
        m_args.countCV.wait(countLock);
    while(m_args.count > 0);

    auto winner = m_scramblerGroups.front().winner();
    for(auto const& scramblerGroup: m_scramblerGroups)
        if(scramblerGroup.winner()->analysis() < winner->analysis())
            winner = scramblerGroup.winner();

    m_cargs.feedback.reset(winner->feedback().clone());
    if(m_continuous)
        std::copy(
                winner->remainder().begin(),
                winner->remainder().end(),
                m_cargs.remainder.begin());

    return winner->output();
}

template<typename Symbol>
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::GuidedScrambler_impl(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const bool continuous,
        const std::vector<Symbol>& divider,
        const unsigned int threads,
        const std::string& selectionMethod,
        const std::string& alignmentTag):
    gr::block("Guided Scrambler",
        gr::io_signature::make(1,1,sizeof(Symbol)),
        gr::io_signature::make(1,1,sizeof(Symbol))),
    m_codewordLength(codewordLength),
    m_augmentingLength(augmentingLength),
    m_selectionMethod(std::find(
            selectionMethods.cbegin(),
            selectionMethods.cend(),
            selectionMethod) - selectionMethods.cbegin()),
    m_groups(threads==0?std::thread::hardware_concurrency():threads),
    m_fieldSize(fieldSize),
    m_continuous(continuous),
    m_codeword(nullptr),
    m_sourceword(m_codewordLength-m_augmentingLength),
    m_sourcewordIt(m_sourceword.begin()),
    m_alignmentTag(pmt::string_to_symbol(alignmentTag)),
    m_aligned(alignmentTag.empty())
{
    this->set_relative_rate(
            double(codewordLength)/(codewordLength-augmentingLength));
    this->set_tag_propagation_policy(gr::block::TPP_DONT);
    set_divider(divider);
    m_cargs.constellation = defaultConstellation_i(m_fieldSize);
}

template<typename Symbol> const std::string&
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::selectionMethod() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return selectionMethods[m_selectionMethod];
}

template<typename Symbol> void
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_selectionMethod(
        const std::string& method)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_codeword = nullptr;
    auto result = std::find(
            selectionMethods.begin(),
            selectionMethods.end(),
            method);
    m_selectionMethod = result - selectionMethods.begin();
    killThreads();
}

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_fieldSize(
        const unsigned int size)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_codeword = nullptr;
    m_fieldSize = size;
    m_cargs.constellation = defaultConstellation_i(m_fieldSize);
    for(auto& symbol: m_cargs.divider)
        if(symbol >= size)
            symbol = size-1;
    killThreads();
}

template<typename Symbol> unsigned int
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::fieldSize() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_fieldSize;
}

template<typename Symbol> unsigned int
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::codewordLength() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_codewordLength;
}

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_codewordLength(
        const unsigned int length)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sourceword.resize(length-m_augmentingLength);
    m_sourcewordIt = m_sourceword.begin();
    m_codeword = nullptr;
    m_codewordLength = length;
    this->set_relative_rate(
            double(m_codewordLength)/(m_codewordLength-m_augmentingLength));
    killThreads();
}

template<typename Symbol> unsigned int
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::augmentingLength() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_augmentingLength;
}

template<typename Symbol> void
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_augmentingLength(
        const unsigned int length)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sourceword.resize(m_codewordLength-length);
    m_sourcewordIt = m_sourceword.begin();
    m_codeword = nullptr;
    m_augmentingLength = length;
    this->set_relative_rate(
            double(m_codewordLength)/(m_codewordLength-m_augmentingLength));
    killThreads();
}

template<typename Symbol>
bool gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::continuous() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_continuous;
}

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_continuous(
        bool continuous)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_codeword = nullptr;
    m_continuous = continuous;
    killThreads();
}

template<typename Symbol> const std::vector<Symbol>&
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::divider() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cargs.divider;
}

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_divider(
        const std::vector<Symbol>& divider)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_codeword = nullptr;
    m_cargs.divider = divider;
    killThreads();
}

template<typename Symbol> unsigned int
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::threads() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groups;
}

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::set_threads(
        unsigned int number)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_codeword = nullptr;
    if(number>0)
        m_groups = number;
    else
        m_groups = std::thread::hardware_concurrency();
    killThreads();
}

template<typename Symbol>
gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::~GuidedScrambler_impl()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    killThreads();
}

template<typename Symbol>
int gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const Symbol* const& inputStart = reinterpret_cast<const Symbol*>(input_items[0]);
    const Symbol* input = inputStart;
    unsigned int inputSize = ninput_items[0];

    Symbol* const& outputStart = reinterpret_cast<Symbol*>(output_items[0]);
    Symbol* output = outputStart;
    unsigned int outputSize = noutput_items;

    if(!m_aligned)
    {
        std::vector<gr::tag_t> tags;
        this->get_tags_in_range(
                tags,
                0,
                this->nitems_read(0),
                this->nitems_read(0)+ninput_items[0],
                m_alignmentTag);
        const std::vector<gr::tag_t>::const_iterator tag = tags.cbegin();

        if(tag != tags.cend())
        {
            const size_t offset = tag->offset-this->nitems_read(0);
            const unsigned outputOffset = std::min(
                    offset*m_codewordLength
                        /(m_codewordLength-m_augmentingLength),
                    static_cast<unsigned long>(outputSize));
            if(offset != 0)
            {
                inputSize -= offset;
                input += offset;
                outputSize -= outputOffset;
                output += outputOffset;
            }
            this->add_item_tag(
                    0,
                    this->nitems_written(0) + outputOffset,
                    tag->key,
                    tag->value);
            m_aligned = true;
        }
        else
        {
            this->consume_each(ninput_items[0]);
            return std::min(
                    ninput_items[0]*m_codewordLength
                        /(m_codewordLength-m_augmentingLength),
                    outputSize);
        }
    }

    while(true)
    {
        const unsigned int outputCopySize = m_codeword==nullptr?0:std::min(
                outputSize,
                unsigned(m_codeword->end()-m_codewordIt));
        if(outputCopySize)
        {
            output = std::copy(
                    m_codewordIt,
                    m_codewordIt+outputCopySize,
                    output);
            outputSize -= outputCopySize;
            m_codewordIt += outputCopySize;
            if(m_codewordIt == m_codeword->end())
                m_codeword = nullptr;
        }
        else
        {
            const unsigned int inputCopySize = std::min(
                    inputSize,
                    unsigned(m_sourceword.end()-m_sourcewordIt));
            if(inputCopySize)
            {
                m_sourcewordIt = std::copy(
                        input,
                        input+inputCopySize,
                        m_sourcewordIt);
                inputSize -= inputCopySize;
                input += inputCopySize;
            }

            if(!m_codeword && m_sourcewordIt == m_sourceword.end())
            {
                m_codeword = &scramble(m_sourceword);
                m_codewordIt = m_codeword->begin();
                m_sourcewordIt = m_sourceword.begin();
            }
            else
                break;
        }
    }

    this->consume_each(ninput_items[0]-inputSize);
    return noutput_items-outputSize;
}

template<typename Symbol>
void gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>::forecast(
        int noutput_items,
        gr_vector_int& ninput_items_required)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_codeword != nullptr)
        noutput_items -= m_codeword->end()-m_codewordIt;
    if(noutput_items>0)
    {
        const int requiredCodewords =
            (noutput_items+m_codewordLength-1) / m_codewordLength;
        ninput_items_required[0] =
            requiredCodewords*(m_codewordLength-m_augmentingLength)
            -(m_sourcewordIt-m_sourceword.begin());
    }
    else
        ninput_items_required[0] = 0;
}

template<typename Symbol> typename gr::gs::GuidedScrambler<Symbol>::sptr
gr::gs::GuidedScrambler<Symbol>::make(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const bool continuous,
        const std::vector<Symbol>& divider,
        const unsigned int threads,
        const std::string& selectionMethod,
        const std::string& alignmentTag)
{
    return gnuradio::get_initial_sptr(
            new ::gr::gs::GuidedScrambling::GuidedScrambler_impl<Symbol>(
                    fieldSize,
                    codewordLength,
                    augmentingLength,
                    continuous,
                    divider,
                    threads,
                    selectionMethod,
                    alignmentTag));
}

template<typename Symbol> const std::vector<std::string>&
gr::gs::GuidedScrambler<Symbol>::selectionMethods()
{
    return gr::gs::GuidedScrambling::selectionMethods;
}

template class gr::gs::GuidedScrambler<unsigned char>;
template class gr::gs::GuidedScrambling::GuidedScrambler_impl<unsigned char>;
template class gr::gs::GuidedScrambler<unsigned short>;
template class gr::gs::GuidedScrambling::GuidedScrambler_impl<unsigned short>;
template class gr::gs::GuidedScrambler<unsigned int>;
template class gr::gs::GuidedScrambling::GuidedScrambler_impl<unsigned int>;
