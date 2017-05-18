#!/usr/bin/env python2

from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from optparse import OptionParser
import gr_gs as gs
import math
import sip
import sys
import argparse

def parseArgs():
    parser = argparse.ArgumentParser(
            description='Guided Scrambling Simulator.',
            prefix_chars='+')
    parser.add_argument(
            '+f',
            '++field-size',
            nargs=1,
            default=4,
            type=int,
            choices=[2,4,8,16])
    parser.add_argument(
            '+l',
            '++codeword-length',
            nargs=1,
            default=12,
            type=int)
    parser.add_argument(
            '+a',
            '++augmenting-length',
            nargs=1,
            default=3,
            type=int)
    parser.add_argument(
            '+m',
            '++selection-method',
            nargs=1,
            default='MSW',
            choices=['MSW','WRDS','MSW2'])
    parser.add_argument(
            '+c',
            '++constellation',
            nargs='+',
            default=None,
            type=complex)
    parser.add_argument(
            '+r',
            '++rotation',
            nargs=1,
            default=0.0,
            type=float)
    parser.add_argument(
            '+w',
            '++window-size',
            nargs=1,
            default=8192,
            type=int)
    parser.add_argument(
            '+t',
            '++taps',
            nargs=1,
            default=4096,
            type=int,
            help='Taps in RRC filters')
    parser.add_argument(
            '+p',
            '++scrambler',
            nargs='+',
            default=None,
            type=int,
            help='Scrambling Polynomial')
    parser.add_argument(
            '+b',
            '++block',
            action='store_true',
            help='Use block encoding')
    parser.add_argument(
            '+e',
            '++errors',
            action='store_true',
            help='Measure error rate')
    parser.add_argument(
            '+n',
            '++noise',
            nargs=1,
            default=0.0,
            type=float,
            help='Noise Level')
    parser.add_argument(
            '+s',
            '++symbol-weightings',
            nargs='+',
            default=None,
            type=int,
            help='Symbol Probability Weightings')
    parser.add_argument(
            '+x',
            '++alpha',
            nargs=1,
            default=0.1,
            type=float,
            help='Excess Bandwidth factor in RRC filters')
    parser.add_argument(
            '+d',
            '++words',
            nargs=1,
            default=10000,
            type=int,
            help='Total codewords to process')
    parser.add_argument(
            '+i',
            '++samples-per-symbol',
            nargs=1,
            default=2,
            type=int,
            help='Interpolation level')
    arguments = parser.parse_args()
    return arguments

args = parseArgs()
dir(args)

if args.constellation is None:
    args.constellation = gs.defaultConstellation(args.field_size)
if args.scrambler is None:
    args.scrambler = gs.findPrimitive(args.field_size, 5)
rrcTaps = firdes.root_raised_cosine(
        2,
        args.samples_per_symbol,
        1,
        args.alpha,
        args.taps)
frameLength = 0
frameTag = ''
if args.errors:
    frameLength = 10000
    frametag = 'frame'
constellationObj = digital.constellation_calcdist(
        args.constellation,
        range(args.field_size),
        4,
        1).base()

symbolGenerator = gs.SymbolGenerator(
        ([1] * args.field_size),
        frameTag,
        frameLength*(args.codeword_length-args.augmenting_length))
noiseAdder = blocks.add_vcc(1)
transmitFilter = filter.interp_fir_filter_ccc(
        args.samples_per_symbol,
        rrcTaps)
transmitFilter.declare_sample_delay(args.taps/(args.samples_per_symbol*2))
guidedScrambler = gs.GuidedScrambler(
        args.field_size,
        args.codeword_length,
        args.augmenting_length,
        not args.block,
        args.scrambler,
        0,
        args.constellation,
        args.selection_method,
        frameTag)
psdFFT = fft.fft_vcc(
        args.window_size,
        True,
        window.blackmanharris(args.window_size),
        True,
        8)
receiveFilter = filter.fft_filter_ccc(args.samples_per_symbol, rrcTaps, 8)
receiveFilter.declare_sample_delay(args.taps/2)
constellationDecoder = digital.constellation_decoder_cb(constellationObj)
channelNoise = analog.noise_source_c(analog.GR_GAUSSIAN, args.noise, 0)
vectorizer = blocks.stream_to_vector(gr.sizeof_gr_complex*1, args.window_size)
transmitRotator = blocks.rotator_cc(args.rotation)
receiveRotator = blocks.rotator_cc(-args.rotation)
log10 = blocks.nlog10_ff(10, args.window_size, -10*math.log10(args.samples_per_symbol).real)
complex_to_mag_squared = blocks.complex_to_mag_squared(args.window_size)
symbolMapper = gs.SymbolMapper(args.constellation)
average = gs.Average(args.window_size, 16)
errorCount = gs.ErrorCount(frameTag)
descrambler = gs.Descrambler(
        args.field_size,
        args.codeword_length,
        args.augmenting_length,
        not args.block,
        args.scrambler,
        frameTag)

top = gr.top_block()

# Transmit part
top.connect((symbolGenerator, 0), (guidedScrambler, 0))
top.connect((guidedScrambler, 0), (symbolMapper, 0))
if args.rotation == 0:
    top.connect((symbolMapper, 0), (transmitFilter, 0))
else:
    top.connect((symbolMapper, 0), (transmitRotator, 0))
    top.connect((transmitRotator, 0), (transmitFilter, 0))
transmitEnd = transmitFilter
if args.noise != 0:
    top.connect((transmitFilter, 0), (noiseAdder, 0))
    top.connect((channelNoise, 0), (noiseAdder, 0))
    transmitEnd = noiseAdder

# Spectral analysis part
top.connect((transmitEnd, 0), (vectorizer, 0))
top.connect((vectorizer, 0), (psdFFT, 0))
top.connect((psdFFT, 0), (complex_to_mag_squared, 0))
top.connect((complex_to_mag_squared, 0), (average, 0))
