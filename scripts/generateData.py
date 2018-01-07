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
            fieldSize,
            codewordLength,
            augmentingLength,
            autocovarianceLength,
            symbols,
            windowSize):
        gr.top_block.__init__(self, "Guided Scrambling Statistical Simulation")

        ##################################################
        # Variables
        ##################################################
        scramblingPolynomial = gs.defaultScrambler_b(
                    fieldSize,
                    codewordLength,
                    augmentingLength)
        self.constellation = constellation = gs.defaultConstellation_f(fieldSize)
        self.windowSize = windowSize

        ##################################################
        # Blocks
        ##################################################
        self.symbolGenerator = gs.SymbolGenerator_b(([1] * fieldSize), '')
        self.guidedScrambler = gs.GuidedScrambler_bb(
                fieldSize,
                codewordLength,
                augmentingLength,
                True,
                (scramblingPolynomial),
                0,
                'MSW2')
        self.terminator = gs.Terminator(1*gr.sizeof_char, symbols)
        self.symbolMapper = gs.SymbolMapper_bc((constellation))

        self.rdsStream = gs.Integrate_cc(1)
        self.rdsAutocovariances = []
        self.rdsAverages = []

        self.rdssStream = gs.Integrate_cc(1)
        self.rdssAutocovariances = []
        self.rdssAverages = []

        for i in range(codewordLength):
            self.rdsAutocovariances.append(gs.Autocovariance_cc(
                autocovarianceLength,
                0,
                codewordLength,
                i))
            self.rdsAverages.append(gs.Average_cc(autocovarianceLength))
            self.rdssAutocovariances.append(gs.Autocovariance_cc(
                autocovarianceLength,
                0,
                codewordLength,
                i))
            self.rdssAverages.append(gs.Average_cc(autocovarianceLength))

        self.fftStreamToVector = blocks.stream_to_vector(
                gr.sizeof_gr_complex,
                windowSize)

        fftWindow = window.hanning(windowSize)
        self.windowConstant = np.sum(np.array(fftWindow)**2)

        self.fft = gr_fft.fft_vcc(
                windowSize,
                True,
                (fftWindow),
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

        self.connect((self.symbolMapper, 0), (self.rdsStream, 0))
        self.connect((self.rdsStream, 0), (self.rdssStream, 0))

        for i in range(codewordLength):
            self.connect((self.rdsStream, 0), (self.rdsAutocovariances[i], 0))    
            self.connect((self.rdsAutocovariances[i], 0), (self.rdsAverages[i], 0))    
            self.connect((self.rdssStream, 0), (self.rdssAutocovariances[i], 0))    
            self.connect((self.rdssAutocovariances[i], 0), (self.rdssAverages[i], 0))    

        self.connect((self.symbolMapper, 0), (self.fftStreamToVector, 0))    
        self.connect((self.fftStreamToVector, 0), (self.fft, 0))    
        self.connect((self.fft, 0), (self.complexToMagSquared, 0))    
        self.connect((self.complexToMagSquared, 0), (self.fftAverage, 0))

        self.connect((self.symbolMapper, 0), (self.powerComplexToMagSquared, 0))    
        self.connect((self.powerComplexToMagSquared, 0), (self.powerAverage, 0))

    def symbols(self):
        return self.terminator.samples()

    def rdsAutocovariance(self, index):
        return self.rdsAverages[index].average()

    def rdssAutocovariance(self, index):
        return self.rdssAverages[index].average()

    def finished(self):
        return self.terminator.finished()

    def power(self):
        return self.powerAverage.average()[0]

    def psd(self):
        fft = self.fftAverage.average()
        data = np.empty([self.windowSize/2+1,2], dtype=np.float64)

        for i in range(self.windowSize/2+1):
            data[i,0] = float(i)/self.windowSize

        data[0,1] = fft[self.windowSize/2]/(
                self.windowConstant*self.power())
        data[self.windowSize/2,1] = (fft[0])/(
                self.windowConstant*self.power())

        for i in range(1,self.windowSize/2):
            data[i,1] = (fft[self.windowSize/2-i]+fft[self.windowSize/2+i])/(
                    2*self.windowConstant*self.power())
        return data

dataPath = sys.argv[1]
symbolCount = 2e8#2e9
autocovarianceLength = int(sys.argv[2])
fftSize = int(sys.argv[3])

fieldSizes = [4]#[2,4,16]
codewordLengths = [12]#range(2,25)
augmentingLengths = [3]#range(1,11)
maxScramblers = 4096

for fieldSize in fieldSizes:
    print("Computing set with field size = {:d}".format(fieldSize))

    fieldPath = os.path.join(dataPath, "{:02d}".format(fieldSize))
    if os.path.exists(fieldPath):
        if not os.path.isdir(fieldPath):
            print("{:s} exists but isn't a directory".format(fieldPath))
    else:
        os.mkdir(fieldPath)

    for codewordLength in codewordLengths:
        print("  Computing subset with codeword length = {:d}".format(codewordLength))

        codewordPath = os.path.join(fieldPath, "{:02d}".format(codewordLength))
        if os.path.exists(codewordPath):
            if not os.path.isdir(codewordPath):
                print("{:s} exists but isn't a directory".format(codewordPath))
        else:
            os.mkdir(codewordPath)

        for augmentingLength in augmentingLengths:
            scramblers = fieldSize**augmentingLength
            if augmentingLength >= codewordLength or scramblers > maxScramblers:
                continue

            metaPath = os.path.join(codewordPath, "{:02d}.txt".format(augmentingLength))
            if os.path.exists(metaPath):
                metaFile = open(metaPath, 'r')
                if float(metaFile.readlines()[0].split(' ')[-1]) >= symbolCount:
                    continue
            
            tb = gs_stats(
                    fieldSize = fieldSize,
                    codewordLength = codewordLength,
                    augmentingLength = augmentingLength,
                    autocovarianceLength = autocovarianceLength,
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

            # Second order statistics
            autocovariances = np.empty(
                    [codewordLength, 2, autocovarianceLength],
                    dtype=np.complex128)
            for position in range(codewordLength):
                for tau in range(autocovarianceLength):
                    autocovariances[position, 0, tau] = tb.rdsAutocovariance(position)[tau]
                    autocovariances[position, 1, tau] = tb.rdssAutocovariance(position)[tau]
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
            metaFile.write("Autocovariance CRC: 0x{:08x}\n".format(
                zlib.crc32(autocovariances.tobytes())&0xffffffff))
            metaFile.write("PSD CRC: 0x{:08x}\n".format(
                zlib.crc32(tb.psd().tobytes())&0xffffffff))
            metaFile.close()
