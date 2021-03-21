#!/usr/bin/env python3

## Copyright (C) 2020,2021 Andrew Rogers
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; see the file COPYING. If not, see
## <https://www.gnu.org/licenses/>.

# A random binary sequence is used to dither an anologue signal such that it
# deviates above and below the threshold of a 1-bit ADC. An analogue high-pass
# filter is used to shape the PRBS into the high frequency part of the spectrum
# to avoid interference in the low frequency band of the measured signal.
#
#  +---------------+     +----------+
#  | Random binary |---->| Highpass |-----.
#  | Sequence gen  |     |  filter  |     |
#  +---------------+     +----------+     |
#                                         |
#  +---------------+     +----------+     v
#  |   DC offset   |---->| Lowpass  |--->(+)
#  | Sequence gen  |     |  filter  |     |
#  +---------------+     +----------+     v    +-----------+
#                                        (+)-->| Threshold |---> bitstream
#            +---------------+            ^    +-----------+
#            | Signal source |            |
#            |   eg. voice   |------------'
#            +---------------+
#
# DC offset can be adjusted by lowpass filtering a DC offset sequence. The DC
# offset is effectively a 1-bit DAC. In practice, the PRBS and DC sequence can
# be interleaved or produced by a biased random process. See the 1-bit DAC
# interpolator, int8.c, for an efficient implementation.

import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import subprocess

fs=192000*64; # I2S rate of Pi
duration=0.1;
N=int(fs*duration); # Number of sample in simulation

def DcInterpolate( N, dc ) :
    dc8 = int( dc * 8 )
    out = []
    sum = 0.0
    av = 1.0
    for i in range(int(np.ceil(N/8))) :
        y = dc8
        if( av < dc ):
            y = dc8 + 1
        out.append(y)
        sum += y / 8.0
        av = sum / (i + 1)
    return out[0:N]

def DcPlusPrbs( N, dc ) :
    input = bytearray( DcInterpolate( N, dc ) )
    cp=subprocess.run( ["./int8"], input = input, capture_output = True )
    bitstream = []
    for b in cp.stdout :
        for i in range(8) :
            bitstream.append( int(b&1) )
            b = b >> 1

    return bitstream[0:N]

dout = DcPlusPrbs( N, 0.6 )
print(f'mean={np.mean(dout)}')

# Simulate electronic filters
b,a=signal.butter(2,0.05,'highpass')
din = signal.lfilter(b,a,dout)

b,a=signal.butter(2,100/fs,'lowpass')
din += signal.lfilter(b,a,dout)

print(f'mean={np.mean(din)}')

ft = np.abs(np.fft.fft(din))
f = np.arange(0,N) * float(fs)/N/1000
plt.plot(f,ft)
plt.xlabel('Frequency (kHz)')
plt.show()

# Generate a sin signal with DC offset.
f=1000
sig=0.2+0.1*np.sin(2*np.pi*np.arange(0,N)*f/fs);

# Adder
din=sig+din

# threshold or 1-bit ADC
din=2*(din>0)-1

ft = np.abs(np.fft.fft(din))
f = np.arange(0,int(N/300)) * float(fs)/N/1000
plt.plot(f,ft[0:len(f)])
plt.xlabel('Frequency (kHz)')
plt.show()

