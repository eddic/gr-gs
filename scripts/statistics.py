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
from scipy.optimize import curve_fit


class gs_stats(gr.top_block):

    def __init__(
            self,
            fieldSize = 4,
            codewordLength = 12,
            augmentingLength = 3,
            continuous = True,
            scramblerLength = 6,
            autocovarianceLength = 32,
            distributionWidth = 25,
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
        self.integrate = gs.Integrate(1)
        self.complexToReal = blocks.complex_to_real(1)
        self.streamToVector = blocks.stream_to_vector(
                gr.sizeof_float*1,
                codewordLength)
        self.vectorToStreams = blocks.vector_to_streams(
                gr.sizeof_float*1,
                codewordLength)

        self.autocovariances = []
        self.averages = []
        self.averageNullSinks = []

        self.distributions = []
        self.distributionNullSinks = []

        for i in range(codewordLength):
            self.autocovariances.append(gs.Autocovariance_ff(
                autocovarianceLength,
                0,
                codewordLength,
                i))
            self.averages.append(gs.Average(autocovarianceLength, 2048))
            self.averageNullSinks.append(blocks.null_sink(
                gr.sizeof_float*autocovarianceLength))

            self.distributions.append(gs.Distribution(
                    distributionWidth,
                    1,
                    -(distributionWidth-1)/2,
                    1024))
            self.distributionNullSinks.append(blocks.null_sink(
                    gr.sizeof_float*distributionWidth))


        ##################################################
        # Connections
        ##################################################
        self.connect((self.symbolGenerator, 0), (self.guidedScrambler, 0))
        self.connect((self.guidedScrambler, 0), (self.symbolMapper, 0))
        self.connect((self.guidedScrambler, 0), (self.terminator, 0))
        self.connect((self.symbolMapper, 0), (self.complexToReal, 0))
        self.connect((self.complexToReal, 0), (self.integrate, 0))
        self.connect((self.integrate, 0), (self.streamToVector, 0))
        self.connect((self.streamToVector, 0), (self.vectorToStreams, 0))

        for i in range(codewordLength):
            self.connect((self.integrate, 0), (self.autocovariances[i], 0))    
            self.connect((self.autocovariances[i], 0), (self.averages[i], 0))    
            self.connect((self.averages[i], 0), (self.averageNullSinks[i], 0))    
            self.connect((self.vectorToStreams, i), (self.distributions[i], 0))    
            self.connect((self.distributions[i], 0), (self.distributionNullSinks[i], 0))    

    def symbols(self):
        return self.terminator.samples()

    def autocovariance(self, index):
        return self.averages[index].average()

    def finished(self):
        return self.terminator.finished()

    def distribution(self, index):
        return self.distributions[index].distribution()

symbolCount = 1e9
distributionWidth = 129
autocovarianceLength = 64

fieldSizes = [2]
codewordLengths = [24]
augmentingLengths = range(1,10)
maxRate = 0.5
maxScramblers = 256

for fieldSize in fieldSizes:
    print("Computing set with field size = {:d}".format(fieldSize))
    for codewordLength in codewordLengths:
        print("  Computing subset with codeword length = {:d}".format(codewordLength))
        modelError = []

        for augmentingLength in augmentingLengths:
            rate = float(augmentingLength)/float(codewordLength)
            scramblers = fieldSize**augmentingLength
            if rate > maxRate or scramblers > maxScramblers:
                continue

            modelError.append([augmentingLength])
            
            oldStderr = sys.stderr
            sys.stderr = open("/dev/null", 'w')
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
            sys.stderr = oldStderr

            # First order statistics
            file = open("RDS PMF for field size = {:d}, codeword length = {:d}, augmenting length = {:d}, and symbol count = {:.0e}.dat".format(
                fieldSize,
                codewordLength,
                augmentingLength,
                symbolCount), 'w')
            file.write(" RDS ")
            for position in range(codewordLength):
                file.write("   position={:03d}     ".format(
                    position))
            for RDS in range(distributionWidth):
                file.write("\n{: 4d}".format(RDS-(distributionWidth-1)/2))
                for position in range(codewordLength):
                    file.write(" {:+.12e}".format(
                        tb.distribution(position)[RDS]))
            file.close()

            # Model error stuff
            xdata = (
                    np.array(range((distributionWidth-1)/2), dtype=float)*2
                        - (distributionWidth-1)/2+1,
                    np.array(range((distributionWidth-1)/2+1), dtype=float)*2
                        - (distributionWidth-1)/2)
            for position in range(codewordLength):
                ydata = np.array(tb.distribution(position))
                ydata = ydata[(position+1)%2::2]
                def gaussian(x, a, b):
                    return a*np.exp(-x**2/b)
                popt, pcov = curve_fit(gaussian, xdata[position%2], ydata)
                error = 0.0
                for i in range(len(ydata)):
                    error += (ydata[i] - gaussian(xdata[position%2][i], popt[0], popt[1]))**2
                error = np.sqrt(error)
                modelError[-1].append(error)

            # Second order statistics
            file = open("RDS autocovariances for field size = {:d}, codeword length = {:d}, augmenting length = {:d}, and symbol count = {:.0e}.dat".format(
                fieldSize,
                codewordLength,
                augmentingLength,
                symbolCount), 'w')
            file.write(" tau ")
            for position in range(codewordLength):
                file.write("   position={:03d}     ".format(
                    position))
            for tau in range(autocovarianceLength):
                file.write("\n{: 4d}".format(-tau))
                for position in range(codewordLength):
                    file.write(" {:+.12e}".format(
                        tb.autocovariance(position)[autocovarianceLength-1-tau]))
            file.close()
        if len(modelError) == 0:
            continue
        file = open("RDS Gaussian model RMS error for field size = {:d}, codeword length = {:d} and symbol count = {:.0e}.dat".format(fieldSize, codewordLength, symbolCount), 'w')
        file.write(" pos ")
        for index in range(len(modelError)):
            file.write(" augmentingLength={:.0f} ".format(modelError[index][0]))
        for position in range(codewordLength):
            file.write("\n{: 4d}".format(position))
            for index in range(len(modelError)):
                file.write(" {:+.12e}".format(modelError[index][position+1]))
        file.close()
