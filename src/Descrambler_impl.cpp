/*!
 * @file      Descrambler_impl.cpp
 * @brief     Defines the "Descrambler" GNU Radio block implementation
 * @author    Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date      August 11, 2016
 * @copyright Copyright &copy; 2016 Eddie Carle. This project is released under
 *            the GNU General Public License Version 3.
 */
/* Copyright (C) 2016 Eddie Carle
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

#include "Descrambler_impl.hpp"
#include "gr-gs/exceptions.h"

#include <algorithm>
#include "Word.hpp"
#include "GF2.hpp"
#include "GF4.hpp"

void gr::gs::GuidedScrambling::Descrambler_impl::setup()
{
    if(m_augmentingLength < 1)
        throw Exceptions::AugmentingLengthTooSmall();

    if(m_augmentingLength >= m_codewordLength)
        throw Exceptions::AugmentingCodewordLengthMismatch();

    if(m_multiplier.size() < 2)
        throw Exceptions::DivisorLengthTooSmall();

    switch(m_fieldSize)
    {
        case 2:
            m_multiply = Word::multiply<GF2>;
            break;
        case 4:
            m_multiply = Word::multiply<GF4>;
            break;
        default:
            throw Exceptions::BadFieldSize();
    }

    for(auto& symbol: m_multiplier)
        if(symbol >= m_fieldSize)
            symbol = m_fieldSize-1;

    m_codeword.resize(m_codewordLength);
    m_codewordIt = m_codeword.begin();
    m_product.resize(m_codewordLength);
    m_productIt = m_product.end();
    m_remainder.resize(m_multiplier.size()-1);
    std::fill(m_remainder.begin(), m_remainder.end(), 0);
    m_valid=true;
}

template<typename Field>
void gr::gs::GuidedScrambling::Word::multiply(
        const std::vector<Symbol>& multiplicand,
        const std::vector<Symbol>& multiplier,
        std::vector<Symbol>& product,
        std::vector<Symbol>& remainder,
        bool continuous)
{
    std::fill(product.begin(), product.end(), 0);
    const unsigned int productSize=product.size();
    const unsigned int remainderSize=remainder.size();
    for(unsigned int i=0; i<productSize; ++i)
    {
        Symbol& output=product[i];
        for(unsigned int j=0; j<=remainderSize; ++j)
        {
            const unsigned int ijSum = i+j;
            const Symbol& input =
                (ijSum<remainderSize)?
                remainder[ijSum]
                :multiplicand[ijSum-remainderSize];
            output = Field(output)
                +Field(input)*Field(multiplier[remainderSize-j]);
        }
    }
    if(continuous)
        std::copy(
                multiplicand.end()-remainderSize,
                multiplicand.end(),
                remainder.begin());
}

gr::gs::GuidedScrambling::Descrambler_impl::Descrambler_impl(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const bool continuous,
        const std::vector<Symbol>& multiplier,
        const std::string& framingTag,
        const FramingStyle framingStyle):
    gr::block("Guided Scrambling Descrambler",
        gr::io_signature::make(1,1,sizeof(Symbol)),
        gr::io_signature::make(1,1,sizeof(Symbol))),
    m_codewordLength(codewordLength),
    m_augmentingLength(augmentingLength),
    m_multiplier(multiplier),
    m_continuous(continuous),
    m_valid(false),
    m_fieldSize(fieldSize),
    m_framingTag(framingTag),
    m_framingTagPMT(pmt::string_to_symbol(framingTag)),
    m_framingStyle(framingStyle)
{
    this->set_relative_rate(
            double(codewordLength-augmentingLength)/codewordLength);
    set_tag_propagation_policy(gr::block::TPP_DONT);
}

void gr::gs::GuidedScrambling::Descrambler_impl::descramble(
        const std::vector<Symbol>& input)
{
    if(!m_valid)
        setup();

    m_multiply(
            input,
            m_multiplier,
            m_product,
            m_remainder,
            m_continuous);
}

unsigned int gr::gs::GuidedScrambling::Descrambler_impl::fieldSize() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_fieldSize;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_fieldSize(
        const unsigned int size)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_fieldSize = size;
    m_valid=false;
}

unsigned int gr::gs::GuidedScrambling::Descrambler_impl::codewordLength() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_codewordLength;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_codewordLength(
        const unsigned int length)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_codewordLength = length;
    this->set_relative_rate(
            double(m_codewordLength-m_augmentingLength)/m_codewordLength);
    m_valid=false;
}

unsigned int gr::gs::GuidedScrambling::Descrambler_impl::augmentingLength() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_augmentingLength;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_augmentingLength(
        const unsigned int length)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_augmentingLength = length;
    this->set_relative_rate(
            double(m_codewordLength-m_augmentingLength)/m_codewordLength);
    m_valid=false;
}

bool gr::gs::GuidedScrambling::Descrambler_impl::continuous() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_continuous;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_continuous(bool continuous)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_continuous = continuous;
    m_valid=false;
}

const std::vector<gr::gs::Symbol>&
gr::gs::GuidedScrambling::Descrambler_impl::multiplier() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_multiplier;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_multiplier(
        const std::vector<Symbol>& multiplier)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_multiplier = multiplier;
    m_valid=false;
}

const std::string&
gr::gs::GuidedScrambling::Descrambler_impl::framingTag() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_framingTag;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_framingTag(
        const std::string& tag)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_framingTag = tag;
    m_framingTagPMT = pmt::string_to_symbol(tag);
}

const gr::gs::FramingStyle
gr::gs::GuidedScrambling::Descrambler_impl::framingStyle() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_framingStyle;
}

void gr::gs::GuidedScrambling::Descrambler_impl::set_framingStyle(
        const FramingStyle style)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_framingStyle = style;
}

const std::vector<gr::gs::Symbol>
gr::gs::GuidedScrambling::Descrambler_impl::output() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Symbol> output;
    if(m_valid)
    {
        output.resize(m_codewordLength-m_augmentingLength);
        std::copy(
                m_product.begin()+m_augmentingLength,
                m_product.end(),
                output.begin());
    }
    return output;
}

const std::vector<gr::gs::Symbol>&
gr::gs::GuidedScrambling::Descrambler_impl::product() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_product;
}

int gr::gs::GuidedScrambling::Descrambler_impl::general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!m_valid)
        setup();

    const Symbol* const& inputStart = reinterpret_cast<const Symbol*>(input_items[0]);
    const Symbol* input = inputStart;
    unsigned int inputSize = ninput_items[0];

    Symbol* const& outputStart = reinterpret_cast<Symbol*>(output_items[0]);
    Symbol* output = outputStart;
    unsigned int outputSize = noutput_items;

    std::vector<gr::tag_t> tags;
    std::vector<gr::tag_t>::const_iterator tag;

    if(m_framingStyle == ReadFrameMarkers)
    {
        this->get_tags_in_range(
                tags,
                0,
                this->nitems_read(0),
                this->nitems_read(0)+ninput_items[0],
                m_framingTagPMT);
        tag = tags.cbegin();
    }

    while(true)
    {
        const unsigned int outputCopySize = std::min(
                outputSize,
                unsigned(m_product.end()-m_productIt));

        if(outputCopySize)
        {
            output = std::copy(
                    m_productIt,
                    m_productIt+outputCopySize,
                    output);
            outputSize -= outputCopySize;
            m_productIt += outputCopySize;
        }
        else
        {
            const unsigned int inputCopySize = std::min(
                    inputSize,
                    unsigned(m_codeword.end()-m_codewordIt));
            if(inputCopySize)
            {
                m_codewordIt = std::copy(
                        input,
                        input+inputCopySize,
                        m_codewordIt);
                inputSize -= inputCopySize;
                input += inputCopySize;
            }

            if(m_productIt==m_product.end() && m_codewordIt==m_codeword.end())
            {
                m_multiply(
                        m_codeword,
                        m_multiplier,
                        m_product,
                        m_remainder,
                        m_continuous);
                m_productIt = m_product.begin()+m_augmentingLength;
                m_codewordIt = m_codeword.begin();

                if(m_framingStyle == ReadFrameMarkers && tag != tags.cend())
                {
                    const size_t offset =
                        tag->offset
                        -this->nitems_read(0)
                        -(input-inputStart);

                    if(offset < m_codewordLength)
                    {
                        std::cout << "Found a frame at input sample #" << tag->offset;
                        if(offset != 0)
                        {
                            std::cout << " but we're on input sample " << tag->offset-offset << ". Dropping " << offset << " symbols.";
                            inputSize -= offset;
                            input += offset;
                        }
                        std::cout << "\nPassing frame to output sample #" << this->nitems_written(0)+uint64_t(output-outputStart) << '\n' << std::endl;
                        this->add_item_tag(
                                0,
                                this->nitems_written(0)
                                +uint64_t(output-outputStart),
                                tag->key,
                                tag->value);
                        ++tag;
                    }
                }
            }
            else
                break;
        }
    }

    this->consume_each(ninput_items[0]-inputSize);
    return noutput_items-outputSize;
}

void gr::gs::GuidedScrambling::Descrambler_impl::forecast(
        int noutput_items,
        gr_vector_int& ninput_items_required)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!m_valid)
        setup();

    noutput_items -= m_product.end()-m_productIt;
    if(noutput_items>0)
    {
        const int requiredSourcewords =
            (noutput_items+m_codewordLength-m_augmentingLength-1)
            /(m_codewordLength-m_augmentingLength);
        ninput_items_required[0] =
            requiredSourcewords*m_codewordLength
            -(m_codewordIt-m_codeword.begin());
    }
    else
        ninput_items_required[0] = 0;
}

gr::gs::Descrambler::sptr gr::gs::Descrambler::make(
        const unsigned int fieldSize,
        const unsigned int codewordLength,
        const unsigned int augmentingLength,
        const bool continuous,
        const std::vector<Symbol>& multiplier,
        const std::string& framingTag,
        const FramingStyle framingStyle)
{
    return gnuradio::get_initial_sptr(
            new ::gr::gs::GuidedScrambling::Descrambler_impl(
                fieldSize,
                codewordLength,
                augmentingLength,
                continuous,
                multiplier,
                framingTag,
                framingStyle));
}
