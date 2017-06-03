#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Gs Stats
# Generated: Mon May 22 11:06:04 2017
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import gr_gs as gs
import numpy as np
import sys, time

class gs_stats(gr.top_block):
    def __init__(
            self,
            fieldSize = 4,
            codewordLength = 12,
            augmentingLength = 3,
            continuous = True,
            scramblerLength = 6,
            autocovarianceLength = 32,
            distributionWidth = 32,
            symbols = long(1e7),
            selectionMethod = 'MSW'):
        gr.top_block.__init__(self, "Guided Scrambling Statistical Simulation")

        ##################################################
        # Variables
        ##################################################
        self.scramblingPolynomial = scramblingPolynomial = gs.findPrimitive_b(
                fieldSize,
                6)
        self.constellation = constellation = gs.defaultConstellation(fieldSize)

        ##################################################
        # Blocks
        ##################################################
        self.symbolGenerator = gs.SymbolGenerator_b(([1] * fieldSize), '', 0)
        self.guidedScrambler = gs.GuidedScrambler_bb(
                fieldSize,
                codewordLength,
                augmentingLength,
                continuous,
                (scramblingPolynomial),
                0,
                (constellation),
                'MSW')
        self.terminator = gs.Terminator(1*gr.sizeof_char, symbols)
        self.symbolMapper = gs.SymbolMapper_bc((constellation))
        self.integrate = gs.Integrate_cc(1)
        self.streamToVector = blocks.stream_to_vector(
                gr.sizeof_gr_complex*1,
                codewordLength)
        self.vectorToStreams = blocks.vector_to_streams(
                gr.sizeof_gr_complex*1,
                codewordLength)

        self.autocovariances = []
        self.XXaverages = []
        self.XYaverages = []
        self.YXaverages = []
        self.YYaverages = []
        self.XXnullSinks = []
        self.XYnullSinks = []
        self.YXnullSinks = []
        self.YYnullSinks = []

        self.distributions = []
        self.distributionNullSinks = []

        for i in range(codewordLength):
            self.autocovariances.append(gs.Autocovariance_cf(
                autocovarianceLength,
                0,
                codewordLength,
                i))
            self.XXaverages.append(gs.Average_ff(autocovarianceLength, 2048))
            self.XYaverages.append(gs.Average_ff(autocovarianceLength, 2048))
            self.YXaverages.append(gs.Average_ff(autocovarianceLength, 2048))
            self.YYaverages.append(gs.Average_ff(autocovarianceLength, 2048))
            self.XXnullSinks.append(blocks.null_sink(
                gr.sizeof_float*autocovarianceLength))
            self.XYnullSinks.append(blocks.null_sink(
                gr.sizeof_float*autocovarianceLength))
            self.YXnullSinks.append(blocks.null_sink(
                gr.sizeof_float*autocovarianceLength))
            self.YYnullSinks.append(blocks.null_sink(
                gr.sizeof_float*autocovarianceLength))

            self.distributions.append(gs.Distribution_cf(
                    distributionWidth,
                    1,
                    -distributionWidth*(1+1j)/2,
                    1024))
            self.distributionNullSinks.append(blocks.null_sink(
                    gr.sizeof_float*distributionWidth))


        ##################################################
        # Connections
        ##################################################
        self.connect((self.symbolGenerator, 0), (self.guidedScrambler, 0))
        self.connect((self.guidedScrambler, 0), (self.symbolMapper, 0))
        self.connect((self.guidedScrambler, 0), (self.terminator, 0))
        self.connect((self.symbolMapper, 0), (self.integrate, 0))
        self.connect((self.integrate, 0), (self.streamToVector, 0))
        self.connect((self.streamToVector, 0), (self.vectorToStreams, 0))

        for i in range(codewordLength):
            self.connect((self.integrate, 0), (self.autocovariances[i], 0))    
            self.connect((self.autocovariances[i], 0), (self.XXaverages[i], 0))    
            self.connect((self.XXaverages[i], 0), (self.XXnullSinks[i], 0))    
            self.connect((self.autocovariances[i], 1), (self.XYaverages[i], 0))    
            self.connect((self.XYaverages[i], 0), (self.XYnullSinks[i], 0))    
            self.connect((self.autocovariances[i], 2), (self.YXaverages[i], 0))    
            self.connect((self.YXaverages[i], 0), (self.YXnullSinks[i], 0))    
            self.connect((self.autocovariances[i], 3), (self.YYaverages[i], 0))    
            self.connect((self.YYaverages[i], 0), (self.YYnullSinks[i], 0))    

            self.connect((self.vectorToStreams, i), (self.distributions[i], 0))    
            self.connect((self.distributions[i], 0), (self.distributionNullSinks[i], 0))    

    def symbols(self):
        return self.terminator.samples()

    def XX(self, index):
        return self.XXaverages[index].average()

    def XY(self, index):
        return self.XYaverages[index].average()

    def YX(self, index):
        return self.YXaverages[index].average()

    def YY(self, index):
        return self.YYaverages[index].average()

    def finished(self):
        return self.terminator.finished()

    def distribution(self, index):
        return self.distributions[index].distribution()

symbolCount = 1e7
distributionWidth = 128
autocovarianceLength = 128

fieldSizes = [2,4]
codewordLengths = [24]
augmentingLengths = [4]#range(1,10)
maxRate = 0.5
maxScramblers = 512

cppFile = open("RDSautocovariances.hpp", 'w')
cppFile.write('{')

for fieldSize in fieldSizes:
    print("Computing set with field size = {:d}".format(fieldSize))

    if fieldSize != fieldSizes[0]:
        cppFile.write(',')
    cppFile.write("\n    {{{0:d}, // Field size = {0:d}".format(fieldSize))
    cppFile.write("\n        {")

    for codewordLength in codewordLengths:
        print("  Computing subset with codeword length = {:d}".format(codewordLength))

        if codewordLength != codewordLengths[0]:
            cppFile.write(',')
        cppFile.write("\n            {{{0:d}, // Codeword length = {0:d}".format(codewordLength))
        cppFile.write("\n                {")

        for augmentingLength in augmentingLengths:
            rate = float(augmentingLength)/float(codewordLength)
            scramblers = fieldSize**augmentingLength
            if rate > maxRate or scramblers > maxScramblers:
                continue
            
            tb = gs_stats(
                    fieldSize = fieldSize,
                    codewordLength = codewordLength,
                    augmentingLength = augmentingLength,
                    autocovarianceLength = autocovarianceLength,
                    distributionWidth = distributionWidth,
                    symbols = long(symbolCount))
            tb.start()

            sys.stdout.write("    Computing statistics for augmenting length = {:d}... 00.0%".format(
                augmentingLength))
            sys.stdout.flush()
            while not tb.finished():
                sys.stdout.write("\033[5D{:04.1f}%".format(tb.symbols()/float(symbolCount) * 100))
                sys.stdout.flush()
                time.sleep(1)
            tb.wait()
            print("\033[5Ddone  ")

            # Second order statistics
            if augmentingLength != augmentingLengths[0]:
                cppFile.write(',')
            cppFile.write("\n                    {{{0:d}, // Augmenting symbols = {0:d}".format(augmentingLength))
            cppFile.write("\n                        {")
            for position in range(codewordLength):
                if position != 0:
                    cppFile.write(',')
                cppFile.write("\n                            {{ // Codeword Position = {:d}".format(position))
                for tau in range(autocovarianceLength):
                    cppFile.write("\n                                {{{{ // Tau = {:d}".format(-tau))
                    cppFile.write("\n                                    {{{{{:.16e}, {:.16e}}}}},".format(
                        tb.XX(position)[autocovarianceLength-1-tau],
                        tb.XY(position)[autocovarianceLength-1-tau]))
                    cppFile.write("\n                                    {{{{{:.16e}, {:.16e}}}}}".format(
                        tb.YX(position)[autocovarianceLength-1-tau],
                        tb.YY(position)[autocovarianceLength-1-tau]))
                    cppFile.write("\n                                }}")
                    if tau < autocovarianceLength-1:
                        cppFile.write(',')
                cppFile.write("\n                            }")
            cppFile.write("\n                        }")
            cppFile.write("\n                    }")

        cppFile.write("\n                }")
        cppFile.write("\n            }")

    cppFile.write("\n        }")
    cppFile.write("\n    }")

cppFile.write('\n}\n')
