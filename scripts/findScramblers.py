#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Gs Stats
# Generated: Mon May 22 11:06:04 2017
##################################################

from gnuradio import blocks
from gnuradio import gr
import gr_gs as gs
import numpy as np
import sys, time, os, zlib, math

class gs_stats(gr.top_block):
    def __init__(
            self,
            scrambler,
            fieldSize,
            codewordLength ,
            augmentingLength,
            symbols):
        gr.top_block.__init__(self, "Optimal Scrambler Finder")

        ##################################################
        # Variables
        ##################################################
        self.scrambler = scrambler
        self.fieldSize = fieldSize
        self.codewordLength = codewordLength
        self.augmentingLength = augmentingLength
        self.constellation = constellation = gs.defaultConstellation_f(fieldSize)

        ##################################################
        # Blocks
        ##################################################
        self.symbolGenerator = gs.SymbolGenerator_b(([1] * fieldSize), '')
        self.guidedScrambler = gs.GuidedScrambler_bb(
                fieldSize,
                codewordLength,
                augmentingLength,
                True,
                (scrambler),
                0,
                'MSW',
                '')
        self.terminator = gs.Terminator(1*gr.sizeof_char, long(symbols))
        self.symbolMapper = gs.SymbolMapper_bc((constellation))
        self.integrator = gs.Integrate_cc(1)
        self.average = gs.Average_ff(1)
        self.blocks_multiply_xx_1 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vff(1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff((0.5, ))
        self.blocks_complex_to_float_0 = blocks.complex_to_float(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.blocks_complex_to_float_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_complex_to_float_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_complex_to_float_0, 1), (self.blocks_multiply_xx_1, 0))
        self.connect((self.blocks_complex_to_float_0, 1), (self.blocks_multiply_xx_1, 1))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.average, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_xx_1, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.integrator, 0), (self.blocks_complex_to_float_0, 0))
        self.connect((self.symbolMapper, 0), (self.integrator, 0))
        self.connect((self.guidedScrambler, 0), (self.symbolMapper, 0))
        self.connect((self.symbolGenerator, 0), (self.guidedScrambler, 0))
        self.connect((self.guidedScrambler, 0), (self.terminator, 0))

    def symbols(self):
        return self.terminator.samples()

    def finished(self):
        return self.terminator.finished()

    def variance(self):
        return self.average.average()[0]

dataPath = sys.argv[1]
symbolCount = 1e7

fieldSizes = [2,4,16]
codewordLengths = range(2,25)
augmentingLengths = range(1,11)
maxRate = 0.5
maxScramblers = 4096
maxPrimitiveBits = 32

for fieldSize in fieldSizes:
    print("Computing set with field size = {:d}".format(fieldSize))

    fieldPath = os.path.join(dataPath, "{:02d}".format(fieldSize))

    primitives = dict()

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

            print("    Computing subset with augmenting length = {:d}".format(augmentingLength))

            winner=2
            length = 3
            variances = [1e6, 1e6, 1e6]

            while(True):
                if(math.log10(fieldSize)/math.log10(2) * length > maxPrimitiveBits):
                    break

                sys.stdout.write("      Computing variance for scrambler length = {:d}... 00.0%".format(
                    length))
                sys.stdout.flush()

                scrambler = None

                if length in primitives:
                    scrambler = primitives[length]
                else:
                    scrambler = gs.findPrimitive_b(fieldSize, length)
                    primitives[length] = scrambler

                tb = gs_stats(
                        scrambler = scrambler,
                        fieldSize = fieldSize,
                        codewordLength = codewordLength,
                        augmentingLength = augmentingLength,
                        symbols = long(symbolCount))
                tb.start()

                while not tb.finished():
                    sys.stdout.write("\033[6D{: 5.1f}%".format(tb.symbols()/float(symbolCount) * 100))
                    sys.stdout.flush()
                    time.sleep(1)
                tb.wait()
                variance = tb.variance()
                print("\033[6D done. Variance = {:.3f}".format(variance))

                variances.append(variance)

                if variance < variances[winner]:
                    winner = length
                elif length - winner == 3:
                    if np.isclose(
                            variances[winner],
                            variances[length],
                            atol=0.001):
                        winner = length
                    elif np.isclose(
                            variances[winner],
                            variances[length-1],
                            atol=0.001):
                        winner = length-1
                    elif np.isclose(
                            variances[winner],
                            variances[length-2],
                            atol=0.001):
                        winner = length-2
                    break

                length += 1

            print("      Found optimal primitive of length = {:d} with variance = {:.3f}:".format(
                winner,
                variances[winner]))
            sys.stdout.write("        ")
            print(primitives[winner])
            scramblerPath = os.path.join(
                    codewordPath,
                    "{:02d}-scrambler.txt".format(augmentingLength))
            scramblerFile = open(scramblerPath, 'w')
            for i in range(winner):
                if i != 0:
                    scramblerFile.write(' ')
                scramblerFile.write("{:d}".format(primitives[winner][i]))
            scramblerFile.write('\n')
            scramblerFile.close()
