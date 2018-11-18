#!/usr/bin/python2
# -*- coding: utf-8 -*-

from gnuradio import analog
from gnuradio import digital
from gnuradio import blocks
from gnuradio import gr
from gnuradio import fft as gr_fft
import gr_gs as gs
import numpy as np
import sys, time, os, zlib, argparse

class gs_stats(gr.top_block):
    def __init__(
            self,
            scrambler,
            fieldSize = 4,
            codewordLength = 12,
            augmentingLength = 3,
            continuous = True,
            noisePower = 0.09,
            maxErrors = 10000,
            maxSymbols = long(1e7),
            selectionMethod = 'MSW'):
        gr.top_block.__init__(self, "Guided Scrambling Error Rate Simulation")

        ##################################################
        # Variables
        ##################################################
        scramblingPolynomial = scrambler
        constellation = gs.defaultConstellation_f(fieldSize)
        constellationObj = digital.constellation_calcdist((constellation), (range(fieldSize)), 0, 1).base()

        self.maxErrors = maxErrors
        self.maxSymbols = maxSymbols

        ##################################################
        # Blocks
        ##################################################
        self.symbolGenerator = gs.SymbolGenerator_b(([1] * fieldSize), '')

        self.bareSymbolMapper = gs.SymbolMapper_bc((constellation))
        self.bareChannelNoise = analog.noise_source_c(analog.GR_GAUSSIAN, np.sqrt(noisePower*(codewordLength-augmentingLength)/codewordLength), 0)
        self.bareNoiseAdder = blocks.add_vcc(1)
        self.bareDetectorMultiplier = blocks.multiply_const_vcc((constellationObj.points()[0].real/constellation[0].real, ))
        self.bareDetector = digital.constellation_decoder_cb(constellationObj)
        self.bareErrorRate = gs.ErrorCount_bf(False, '', 0, 0)

        self.guidedScrambler = gs.GuidedScrambler_bb(fieldSize, codewordLength, augmentingLength, continuous, (scramblingPolynomial), 0, 'MSW', '')
        self.gsSymbolMapper = gs.SymbolMapper_bc((constellation))
        self.gsChannelNoise = analog.noise_source_c(analog.GR_GAUSSIAN, np.sqrt(noisePower), 0)
        self.gsNoiseAdder = blocks.add_vcc(1)

        self.mlDetectorMultiplier = blocks.multiply_const_vcc((constellationObj.points()[0].real/constellation[0].real, ))
        self.mlDetector = digital.constellation_decoder_cb(constellationObj)
        self.mlErrorRate = gs.ErrorCount_bf(False, '', 0, 0)

        self.mapDetector = gs.Detector_cb(fieldSize, codewordLength, augmentingLength, noisePower, "", 20)
        self.mapErrorRate = gs.ErrorCount_bf(False, '', maxErrors, maxSymbols)

        self.descrambler = gs.Descrambler_bb(fieldSize, codewordLength, augmentingLength, continuous, (scramblingPolynomial), '')
        self.descrambledErrorRate = gs.ErrorCount_bf(False, '', 0, 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.symbolGenerator, 0), (self.guidedScrambler, 0))

        self.connect((self.symbolGenerator, 0), (self.bareErrorRate, 0))
        self.connect((self.symbolGenerator, 0), (self.bareSymbolMapper, 0))
        self.connect((self.bareSymbolMapper, 0), (self.bareNoiseAdder, 0))
        self.connect((self.bareChannelNoise, 0), (self.bareNoiseAdder, 1))
        self.connect((self.bareNoiseAdder, 0), (self.bareDetectorMultiplier, 0))
        self.connect((self.bareDetectorMultiplier, 0), (self.bareDetector, 0))
        self.connect((self.bareDetector, 0), (self.bareErrorRate, 1))

        self.connect((self.guidedScrambler, 0), (self.gsSymbolMapper, 0))
        self.connect((self.gsSymbolMapper, 0), (self.gsNoiseAdder, 0))
        self.connect((self.gsChannelNoise, 0), (self.gsNoiseAdder, 1))

        self.connect((self.guidedScrambler, 0), (self.mlErrorRate, 0))
        self.connect((self.gsNoiseAdder, 0), (self.mlDetectorMultiplier, 0))
        self.connect((self.mlDetectorMultiplier, 0), (self.mlDetector, 0))
        self.connect((self.mlDetector, 0), (self.mlErrorRate, 1))

        self.connect((self.guidedScrambler, 0), (self.mapErrorRate, 0))
        self.connect((self.gsNoiseAdder, 0), (self.mapDetector, 0))
        self.connect((self.mapDetector, 0), (self.mapErrorRate, 1))

        self.connect((self.symbolGenerator, 0), (self.descrambledErrorRate, 0))
        self.connect((self.mapDetector, 0), (self.descrambler, 0))
        self.connect((self.descrambler, 0), (self.descrambledErrorRate, 1))

    def MAPerrorRate(self):
        return self.mapErrorRate.rate()

    def MLerrorRate(self):
        return self.mlErrorRate.rate()

    def BAREerrorRate(self):
        return self.bareErrorRate.rate()

    def DESCRAMBLEDerrorRate(self):
        return self.descrambledErrorRate.rate()

    def completion(self):
        errorCompletion = float(self.mapErrorRate.errors())/self.maxErrors
        symbolCompletion = float(self.mapErrorRate.symbols())/self.maxSymbols
        return max([errorCompletion, symbolCompletion])*100

    def errors(self):
        return self.mapErrorRate.errors()

    def finished(self):
        return self.mapErrorRate.finished()

def processArguments():
    parser = argparse.ArgumentParser(description='Simulate GS detection error rates.')
    parser.add_argument('-o', '--output', required=False, help='output file')
    parser.add_argument('-f', '--fieldSize', required=True, type=int)
    parser.add_argument('-c', '--codewordLength', required=True, type=int)
    parser.add_argument('-a', '--augmentingLength', required=True, type=int)
    parser.add_argument('-e', '--maxErrors', type=float, default=1e4, help='default: 1e4')
    parser.add_argument('-s', '--maxSymbols', type=float, default=1e10, help='default: 1e10')
    parser.add_argument('-r', '--minErrorRate', type=float, default=1e-7, help='default: 1e-7')

    return parser.parse_args()

args = processArguments()
fieldSize = args.fieldSize
augmentingLength = args.augmentingLength
codewordLength = args.codewordLength
scrambler = gs.defaultScrambler_b(
        fieldSize,
        codewordLength,
        augmentingLength)
maxSymbols = int(args.maxSymbols)
maxErrors = int(args.maxErrors)

noiseExponent = 0

if args.output is not None:
    file = open(args.output, 'w')
    file.write("  Noise Power  | MAP Detection |  ML Detection |  Descrambled  |  Unscrambled\n")

print("┌───────────────┬───────────────┬───────────────┬───────────────┬───────────────┐")
print("│  Noise Power  │ MAP Detection │  ML Detection │  Descrambled  │  Unscrambled  │")
print("├───────────────┼───────────────┼───────────────┼───────────────┼───────────────┤")

while True:
    noisePower = 10.0**((10.0-noiseExponent)/20)
    noiseExponent += 1

    tb = gs_stats(
            fieldSize = fieldSize,
            scrambler = scrambler,
            codewordLength = codewordLength,
            augmentingLength = augmentingLength,
            noisePower = noisePower,
            maxSymbols = long(maxSymbols),
            maxErrors = maxErrors)
    tb.start()

    sys.stdout.write("│{:.9e}│   0.0%".format(
        noisePower))
    sys.stdout.flush()
    while not tb.finished():
        sys.stdout.write("\033[6D{: >5.1f}%".format(tb.completion()))
        sys.stdout.flush()
        time.sleep(1)
    tb.stop()
    print("\033[7D{:.9e}│{:.9e}│{:.9e}│{:.9e}│".format(
        tb.MAPerrorRate(),
        tb.MLerrorRate(),
        tb.DESCRAMBLEDerrorRate(),
        tb.BAREerrorRate()))

    if args.output is not None:
        file.write('{:.9e} {:.9e} {:.9e} {:.9e} {:.9e}\n'.format(
            noisePower,
            tb.MAPerrorRate(),
            tb.MLerrorRate(),
            tb.DESCRAMBLEDerrorRate(),
            tb.BAREerrorRate()))
        file.flush()

    if tb.MAPerrorRate() < args.minErrorRate:
        break

print("└───────────────┴───────────────┴───────────────┴───────────────┴───────────────┘")
