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
from gnuradio import fft as gr_fft
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.fft import window
from optparse import OptionParser
import gr_gs as gs
import numpy as np
import sys, time, os, zlib

class gs_stats(gr.top_block):
    def __init__(
            self,
            scrambler,
            fieldSize = 4,
            codewordLength = 12,
            augmentingLength = 3,
            continuous = True,
            autocovarianceLength = 32,
            distributionWidth = 32,
            symbols = long(1e7),
            windowSize = 16384,
            selectionMethod = 'MSW'):
        gr.top_block.__init__(self, "Guided Scrambling Statistical Simulation")

        ##################################################
        # Variables
        ##################################################
        scramblingPolynomial = scrambler
        self.constellation = constellation = gs.defaultConstellation(fieldSize)
        self.windowSize = windowSize

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

        self.distributions = []

        for i in range(codewordLength):
            self.autocovariances.append(gs.Autocovariance_cf(
                autocovarianceLength,
                0,
                codewordLength,
                i))
            self.XXaverages.append(gs.Average_ff(autocovarianceLength))
            self.XYaverages.append(gs.Average_ff(autocovarianceLength))
            self.YXaverages.append(gs.Average_ff(autocovarianceLength))
            self.YYaverages.append(gs.Average_ff(autocovarianceLength))

            self.distributions.append(gs.Distribution_cf(
                    distributionWidth,
                    1,
                    -distributionWidth*(1+1j)/2,
                    False))

        self.fftStreamToVector = blocks.stream_to_vector(
                gr.sizeof_gr_complex,
                windowSize)
        self.fft = gr_fft.fft_vcc(
                windowSize,
                True,
                (window.rectangular(windowSize)),
                True,
                8)
        self.complexToMagSquared = blocks.complex_to_mag_squared(windowSize)
        self.fftAverage = gs.Average_ff(windowSize)

        self.powerComplexToMagSquared = blocks.complex_to_mag_squared(1)
        self.powerAverage = gs.Average_ff(1)


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
            self.connect((self.autocovariances[i], 1), (self.XYaverages[i], 0))    
            self.connect((self.autocovariances[i], 2), (self.YXaverages[i], 0))    
            self.connect((self.autocovariances[i], 3), (self.YYaverages[i], 0))    

            self.connect((self.vectorToStreams, i), (self.distributions[i], 0))    

        self.connect((self.symbolMapper, 0), (self.fftStreamToVector, 0))    
        self.connect((self.fftStreamToVector, 0), (self.fft, 0))    
        self.connect((self.fft, 0), (self.complexToMagSquared, 0))    
        self.connect((self.complexToMagSquared, 0), (self.fftAverage, 0))

        self.connect((self.symbolMapper, 0), (self.powerComplexToMagSquared, 0))    
        self.connect((self.powerComplexToMagSquared, 0), (self.powerAverage, 0))

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

    def psd(self):
        fft = self.fftAverage.average()
        data = np.empty([tb.windowSize/2+1,2], dtype=np.float64)

        for i in range(tb.windowSize/2+1):
            data[i,0] = float(i)/tb.windowSize
            data[i,1] = fft[tb.windowSize/2-i]/(
                    tb.windowSize*self.powerAverage.average()[0])
        return data

    def power(self):
        return self.powerAverage.average()[0]

dataPath = sys.argv[1]
symbolCount = float(sys.argv[2]) #1e8
autocovarianceLength = int(sys.argv[3]) #64
distributionWidth = int(sys.argv[4]) #128
fftSize = int(sys.argv[5]) #16384

fieldSizes = [2,4,16]
codewordLengths = range(2,25)
augmentingLengths = range(1,11)
maxRate = 0.5
maxScramblers = 4096
scramblerLength = 20

for fieldSize in fieldSizes:
    print("Computing set with field size = {:d}".format(fieldSize))

    fieldPath = os.path.join(dataPath, "{:02d}".format(fieldSize))
    if os.path.exists(fieldPath):
        if not os.path.isdir(fieldPath):
            print("{:s} exists but isn't a directory".format(fieldPath))
    else:
        os.mkdir(fieldPath)

    scrambler = None
    scramblerPath = os.path.join(fieldPath, "scrambler.txt")
    if os.path.exists(scramblerPath):
        scramblerFile = open(scramblerPath, 'r')
        scrambler = scramblerFile.readlines()[0].split(' ')
        scrambler = map(int, scrambler)
        scramblerFile.close()
    else:
        sys.stdout.write("  Generating primitive scrambler... ")
        sys.stdout.flush()
        scrambler = gs.findPrimitive_b(
                fieldSize,
                scramblerLength/int(np.sqrt(fieldSize)))
        print("done")
        scramblerFile = open(scramblerPath, 'w')
        scramblerFile.write("{:d}".format(scrambler[0]))
        for i in range(1,len(scrambler)):
            scramblerFile.write(" {:d}".format(scrambler[i]))
        scramblerFile.close()

    for codewordLength in codewordLengths:
        print("  Computing subset with codeword length = {:d}".format(codewordLength))

        codewordPath = os.path.join(fieldPath, "{:02d}".format(codewordLength))
        if os.path.exists(codewordPath):
            if not os.path.isdir(codewordPath):
                print("{:s} exists but isn't a directory".format(codewordPath))
        else:
            os.mkdir(codewordPath)

        for augmentingLength in augmentingLengths:
            rate = float(augmentingLength)/float(codewordLength)
            scramblers = fieldSize**augmentingLength
            if rate > maxRate or scramblers > maxScramblers:
                continue

            metaPath = os.path.join(codewordPath, "{:02d}.txt".format(augmentingLength))
            if os.path.exists(metaPath):
                metaFile = open(metaPath, 'r')
                if float(metaFile.readlines()[0].split(' ')[-1]) >= symbolCount:
                    continue
            
            tb = gs_stats(
                    fieldSize = fieldSize,
                    scrambler = scrambler,
                    codewordLength = codewordLength,
                    augmentingLength = augmentingLength,
                    autocovarianceLength = autocovarianceLength,
                    distributionWidth = distributionWidth,
                    windowSize = fftSize,
                    symbols = long(symbolCount))
            tb.start()

            sys.stdout.write("    Computing statistics for augmenting length = {:d}... 00.0%".format(
                augmentingLength))
            sys.stdout.flush()
            while not tb.finished():
                sys.stdout.write("\033[6D{: 5.1f}%".format(tb.symbols()/float(symbolCount) * 100))
                sys.stdout.flush()
                time.sleep(1)
            tb.wait()
            print("\033[6D done   ")

            # First order statistics
            distributions = np.empty(
                    [codewordLength, distributionWidth, distributionWidth],
                    dtype=np.float64)
            for position in range(codewordLength):
                distribution = tb.distribution(position)
                for real in range(distributionWidth):
                    for imag in range(distributionWidth):
                        distributions[position, imag, real] = distribution[imag][real]
            distributions.tofile(
                    os.path.join(
                        codewordPath,
                        "{:02d}-distribution.dat".format(augmentingLength)))

            # Second order statistics
            autocovariances = np.empty(
                    [codewordLength, autocovarianceLength, 2, 2],
                    dtype=np.float64)
            for position in range(codewordLength):
                for tau in range(autocovarianceLength):
                    autocovariances[position, tau, 0, 0] = tb.XX(position)[tau]
                    autocovariances[position, tau, 0, 1] = tb.XY(position)[tau]
                    autocovariances[position, tau, 1, 0] = tb.YX(position)[tau]
                    autocovariances[position, tau, 1, 1] = tb.YY(position)[tau]
            autocovariances.tofile(
                    os.path.join(
                        codewordPath,
                        "{:02d}-autocovariance.dat".format(augmentingLength)))

            # Power Spectral Density
            tb.psd().tofile(
                    os.path.join(
                        codewordPath,
                        "{:02d}-psd.dat".format(augmentingLength)))

            # Meta data
            metaFile = open(metaPath, 'w')
            metaFile.write("Symbols: {:.0e}\n".format(symbolCount))
            metaFile.write("Date: {:s}\n".format(time.strftime("%B %d, %Y at %H:%M %Z")))
            metaFile.write("Selection Method: MSW\n")
            metaFile.write("Constellation Power: {:.16e}\n".format(tb.power()))
            metaFile.write("Distribution CRC: 0x{:08x}\n".format(
                zlib.crc32(distributions.tobytes())&0xffffffff))
            metaFile.write("Autocovariance CRC: 0x{:08x}\n".format(
                zlib.crc32(autocovariances.tobytes())&0xffffffff))
            metaFile.write("PSD CRC: 0x{:08x}\n".format(
                zlib.crc32(tb.psd().tobytes())&0xffffffff))
            metaFile.close()
