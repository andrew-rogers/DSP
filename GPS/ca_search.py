#!/usr/bin/env python3

"""Global Position System (GPS) Coarse Acquisition (C/A) search

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

import ca_code
from satellite import Satellite
import numpy as np

class CASearch :

    def __init__( self, fs, doppler=7500 ) : 
        self.fs = fs
        self.doppler = doppler
        self.block_size = 0
        self.satellites = []

    def processBlock( self, bb_IQ ) :

        # Check for change of block size
        if len(bb_IQ) != self.block_size :
            self.block_size = len(bb_IQ)

            # Instantiate satellites if not already instantiated.
            if len(self.satellites) == 0 :
                for i in range(1,33) :
                    sat = Satellite( i, self.fs, self.block_size )
                    self.satellites.append(sat)

            # Set the block size for satellites
            for s in self.satellites :
                s.setBlockSize( self.block_size )

        # Get the FFT of the baseband IQ signal and search over doppler shifts.
        bb_fft = np.fft.fft( bb_IQ )
        self._search( bb_fft, 0 )
        doppler = int(np.ceil(self.doppler*float(self.block_size)/self.fs))
        for d in range(1,doppler+1) :
            self._search( bb_fft, -d )
            self._search( bb_fft, d ) 

    def enable( self, sid, en = True ) :
        self.satellites[sid].setEnabled(en)

    def disable( self, sid ) :
        self.enable( sid, False )

    def _search( self, bb_fft, doppler ) :

        # Shift the FFT to compensate doppler shift
        bb_fft = np.roll( bb_fft, doppler )

        # Loop through enabled Satellite correlation peak detectors
        for s in self.satellites :
            if s.isEnabled() :
                i,p = s.getPeak( bb_fft )
                print(doppler, s.sid, i, p)

def test() :
    MHz = 1e6
    fs = 4 * MHz
    block_size = 1 * int(fs / 1000) # 1 millisecond blocks
    sat7 = Satellite( 7, fs, block_size )

    # Create a shifted transmit test signal
    tx_sig = np.roll(sat7.code_sig, 962)

    # Add noise to give -15dB SNR
    snr_db=-15
    w = 2 * np.pi * 3500 / fs # 3.5kHz doppler shift
    theta = w * np.arange(len(tx_sig))
    rx_sig = tx_sig * np.exp( -1j * theta ) + np.random.randn(len(tx_sig)) * 10**(-snr_db/20)

    ca = CASearch( fs )
    ca.processBlock( rx_sig )

if __name__ == "__main__":
    test()
