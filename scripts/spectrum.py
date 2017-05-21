#!/usr/bin/env python2

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import fft as gr_fft
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from optparse import OptionParser
import gr_gs as gs
import math, time, sys
import numpy as np


class gs_sim(gr.top_block):
    def __init__(
            self,
            fieldSize = 4,
            codewordLength = 12,
            augmentingLength = 3,
            continuous = True,
            scramblerLength = 6,
            windowSize = 16384,
            symbols = long(1e7),
            selectionMethod = 'MSW'):
        gr.top_block.__init__(self, "Guided Scrambling Spectral Simulation")

        ##################################################
        # Variables
        ##################################################
        scramblingPolynomial = gs.findPrimitive_b(fieldSize, 6)
        constellation = gs.defaultConstellation(fieldSize)
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
        self.terminator = gs.Terminator(gr.sizeof_char, symbols)
        self.symbolMapper = gs.SymbolMapper_bc((constellation))
        self.average = gs.Average(windowSize, 16)
        self.fftBlock = gr_fft.fft_vcc(
                windowSize,
                True,
                (window.rectangular(windowSize)),
                True,
                8)
        self.streamToVector = blocks.stream_to_vector(
                gr.sizeof_gr_complex,
                windowSize)
        self.nullSink = blocks.null_sink(gr.sizeof_float*windowSize)
        self.complexToMagSquared = blocks.complex_to_mag_squared(windowSize)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.complexToMagSquared, 0), (self.average, 0))    
        self.connect((self.streamToVector, 0), (self.fftBlock, 0))    
        self.connect((self.fftBlock, 0), (self.complexToMagSquared, 0))    
        self.connect((self.average, 0), (self.nullSink, 0))    
        self.connect((self.symbolMapper, 0), (self.streamToVector, 0))    
        self.connect((self.guidedScrambler, 0), (self.symbolMapper, 0))    
        self.connect((self.guidedScrambler, 0), (self.terminator, 0))    
        self.connect((self.symbolGenerator, 0), (self.guidedScrambler, 0))    

    def symbols(self):
        return self.terminator.samples()

    def fft(self):
        return self.average.average()

    def finished(self):
        return self.terminator.finished()

symbolCount = long(1e7)
versions = [
        [4, 12, 3], #fieldsize=4, codewordLength=12, augmentingLength=3
        [2,12,3]]   #fieldsize=2, codewordLength=12, augmentingLength=3

for version in versions:
    tb = gs_sim(
            fieldSize = version[0],
            codewordLength = version[1],
            augmentingLength = version[2],
            symbols = symbolCount)
    tb.start()

    sys.stdout.write("Computing F{:d}C{:d}A{:d} 00.0%".format(
        version[0],
        version[1],
        version[2]))
    sys.stdout.flush()
    while not tb.finished():
        sys.stdout.write("\033[5D{:04.1f}%".format(tb.symbols()/float(symbolCount) * 100))
        sys.stdout.flush()
        time.sleep(1)
    tb.wait()
    print("\033[5Ddone  ")

    fft = tb.fft()
    file = open("F{:d}C{:d}A{:d}_spectrum.dat".format(
        version[0],
        version[1],
        version[2]), 'w')

    for i in reversed(range(len(fft)/2+1)):
        file.write("{:.12e} {:.12e}\n".format(
            abs(float(i)/tb.windowSize - 0.5),
            fft[i]/tb.windowSize))
