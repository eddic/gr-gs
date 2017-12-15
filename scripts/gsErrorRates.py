#!/usr/bin/env python2

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
            windowSize = 16384,
            selectionMethod = 'MSW'):
        gr.top_block.__init__(self, "Guided Scrambling Error Rate Simulation")

        ##################################################
        # Variables
        ##################################################
        scramblingPolynomial = scrambler
        constellation = gs.defaultConstellation(fieldSize)
        constellationObj = digital.constellation_calcdist((constellation), (range(fieldSize)), 0, 1).base()

        self.maxErrors = maxErrors
        self.maxSymbols = maxSymbols

        ##################################################
        # Blocks
        ##################################################
        self.symbolMapper = gs.SymbolMapper_bc((constellation))
        self.symbolGenerator = gs.SymbolGenerator_b(([1] * fieldSize), '', 0)
        self.noiseAdder = blocks.add_vcc(1)
        self.mlErrorRate = gs.ErrorCount_bf(False, '', 0, 0)
        self.mapErrorRate = gs.ErrorCount_bf(False, '', maxErrors, maxSymbols)
        self.guidedScrambler = gs.GuidedScrambler_bb(fieldSize, codewordLength, augmentingLength, continuous, (scramblingPolynomial), 0, (constellation), 'MSW', '')
        self.detector = gs.Detector_cb(fieldSize, codewordLength, augmentingLength, 0.01, noisePower, windowSize, '')
        self.constellationDecoderMultiplier = blocks.multiply_const_vcc((constellationObj.points()[0].real/constellation[0].real, ))
        self.constellationDecoder = digital.constellation_decoder_cb(constellationObj)
        self.channelNoise = analog.noise_source_c(analog.GR_GAUSSIAN, np.sqrt(noisePower), 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.channelNoise, 0), (self.noiseAdder, 0))
        self.connect((self.constellationDecoder, 0), (self.mlErrorRate, 1))
        self.connect((self.constellationDecoderMultiplier, 0), (self.constellationDecoder, 0))
        self.connect((self.detector, 0), (self.mapErrorRate, 1))
        self.connect((self.guidedScrambler, 0), (self.mapErrorRate, 0))
        self.connect((self.guidedScrambler, 0), (self.mlErrorRate, 0))
        self.connect((self.guidedScrambler, 0), (self.symbolMapper, 0))
        self.connect((self.noiseAdder, 0), (self.constellationDecoderMultiplier, 0))
        self.connect((self.noiseAdder, 0), (self.detector, 0))
        self.connect((self.symbolGenerator, 0), (self.guidedScrambler, 0))
        self.connect((self.symbolMapper, 0), (self.noiseAdder, 1))

    def MAPerrorRate(self):
        return self.mapErrorRate.rate()

    def MLerrorRate(self):
        return self.mlErrorRate.rate()

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
	parser.add_argument('output', help='output file')
	parser.add_argument('-f', '--fieldSize', type=int)
	parser.add_argument('-c', '--codewordLength', type=int)
	parser.add_argument('-a', '--augmentingLength', type=int)
        parser.add_argument('-w', '--windowSize', type=int, default=128, help='default: 128')
        parser.add_argument('-e', '--maxErrors', type=float, default=1e4, help='default: 1e4')
        parser.add_argument('-s', '--maxSymbols', type=float, default=2e9, help='default: 2e9')

	return parser.parse_args()

args = processArguments()
fieldSize = args.fieldSize
augmentingLength = args.augmentingLength
codewordLength = args.codewordLength
scrambler = gs.defaultScrambler_b(
        fieldSize,
        codewordLength,
        augmentingLength)
windowSize = args.windowSize
maxSymbols = int(args.maxSymbols)
maxErrors = int(args.maxErrors)

noiseExponent = -30
#noiseExponent = 5

file = open(args.output, 'w')

while True:
    noisePower = 0.1875 * (7.0/8)**noiseExponent
    noiseExponent += 1

    tb = gs_stats(
            fieldSize = fieldSize,
            scrambler = scrambler,
            codewordLength = codewordLength,
            augmentingLength = augmentingLength,
            noisePower = noisePower,
            windowSize = windowSize,
            maxSymbols = long(maxSymbols),
            maxErrors = maxErrors)
    tb.start()

    sys.stdout.write("Computing error rate for noise power = {:g}... 00.0%".format(
        noisePower))
    sys.stdout.flush()
    while not tb.finished():
        sys.stdout.write("\033[6D{: 5.1f}%".format(tb.completion()))
        sys.stdout.flush()
        time.sleep(1)
    tb.stop()
    print("\033[6D done   ")

    file.write('{:.9e} {:.9e} {:.9e}\n'.format(
        noisePower,
        tb.MAPerrorRate(),
        tb.MLerrorRate()))
    file.flush()

    if tb.errors() < maxErrors:
        break
