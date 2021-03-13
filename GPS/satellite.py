#!/usr/bin/env python3

"""Global Position System (GPS) Satellite Channel Information

 The Standard Positioning Service (SPS) spec can be found at 
 https://www.navcen.uscg.gov/pubs/gps/sigspec/gpssps1.pdf

"""

# Copyright (c) 2021 Andrew Rogers
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import numpy as np
import ca_code
import sys
sys.path.append("..")
import fractional_resampler as fr

# Use the FFT - IFFT method to get correlation
#
#           .-----.           .------.
# y(n) ---->| FFT |--->(x)--->| IFFT |---> m(n)
#           '-----'     ^     '------'
#                       |
#             .-----.   | C'(k)
#    c'(n)--->| FFT |---'
#             '-----'
#
#   Convolution in time domain is multiplication in frequency domain.
#   Correlation is the same as convolution but with one of the input vectors
#   flipped. The local Gold code signal is flipped and its FFT taken. This is
#   stored for later use to avoid recalculation.
#

class Satellite :

    def __init__( self, sid, fs, block_size ) :
        self.sid = sid
        self.fs = fs
        self.block_size = block_size
        self.code_phase = 0
        self.doppler = 0
        self.enabled = True
        self._genCodeSignals()

    def setBlockSize( self, block_size ) :
        self.block_size = block_size
        self._genCodeSignals()

    def isEnabled( self ) :
        return self.enabled

    def setEnabled( self, en ) :
        self.enabled = en

    def xcorr( self, bb_fft ) :
        M = np.multiply( bb_fft, self.code_fft )
        m = np.abs( np.fft.ifft( M ) )
        return m

    def getPeak( self, bb_fft ) :
        m = self.xcorr( bb_fft )
        i = np.argmax( m )
        return i,m[i]

    def _genCodeSignals( self ) :
        shift = ca_code.sv_info[self.sid]['shift']
        gold = np.repeat( ca_code.gold( shift ), 4)
        rate = self.fs / ( len(gold) * 1000.0)

        # Calculate number of sequence repeats from blocksize. A sequence is 1ms long.
        num_blocks = int(np.ceil(self.block_size * 1000.0 / self.fs))
        sig = []
        for i in range(num_blocks) :
            sig.extend( 2 * np.array(gold) - 1 )

        self.code_sig = fr.resample( sig, rate )[0:self.block_size]
        self.code_fft = np.fft.fft(np.flip(self.code_sig))

def test() :
    MHz = 1e6
    fs = 4 * MHz
    block_size = 1 * int(fs / 1000) # 1 millisecond blocks
    sat7 = Satellite( 7, fs, block_size )

    # Create a shifted transmit test signal
    tx_sig = np.roll(sat7.code_sig, 962)

    # Add noise to give -20dB SNR
    snr_db=-20
    rx_sig = tx_sig + np.random.randn(len(tx_sig)) * 10**(-snr_db/20)

    # Do the correlation
    sig_fft = np.fft.fft( rx_sig )
    xcorr = sat7.xcorr( sig_fft )

    import matplotlib.pyplot as plt
    plt.plot(sat7.code_sig)
    plt.plot(xcorr)
    plt.show()

if __name__ == "__main__":
    test()

