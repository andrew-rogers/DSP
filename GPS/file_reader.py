#!/usr/bin/env python3

"""Global Position System (GPS) file reader for captured IQ signal

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

class FileReader :

    # Data file available from https://sourceforge.net/projects/gnss-sdr/files/data/
    def __init__( self, filename='2013_04_04_GNSS_SIGNAL_at_CTTC_SPAIN/2013_04_04_GNSS_SIGNAL_at_CTTC_SPAIN.dat') :
        self.offset = 0
        self.filename = filename

    def read( self, num_samples ) :
        data=np.fromfile(self.filename, dtype=np.int16, offset=self.offset, count=num_samples*2)
        self.offset = self.offset + 2 * len(data)

        # Convert values to complex
        data=data.reshape(num_samples,2)
        data=np.matmul(data,[1,1j])

        return data

