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
# deviates above and below the threshold of a 1-bit ADC. An analogue low-pass
# filter is used to shape the PRBS into a series of varying amplitude and width
# peaks. To avoid interference in the low frequency band of the measured signal,
# a short repeating sequence is used.
#
#  +---------------+     +---------+
#  | Random binary |---->| Lowpass |------.
#  | Sequence gen  |     |  filter |      |
#  +---------------+     +---------+      v    +-----------+
#                                        (+)-->| Threshold |---> bitstream
#            +---------------+            ^    +-----------+
#            | Signal source |            |
#            |   eg. voice   |------------'
#            +---------------+
#

import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

fs=192000*32; # I2S rate of Pi
duration=0.1;
N=int(fs*duration); # Number of sample in simulation

'''
# Repeating short Random binary sequence
dither=[0 0 1 1 0 1 0 0 0 1 1 0 1 1 1 1 1 1 0 0 1 0 0 0];
dither=[dither 1-dither];
dither=repmat(dither,1,ceil(N/length(dither)));
dither=dither(1:N)-0.5;
'''

def mls(poly) :

	# Start with all ones (size determined from poly)
	reg = poly
	start = 0
	while reg > 0 :
		start = start | reg
		reg = reg >> 1
	seq = [0]*start

	done = False
	reg = start
	l=0
	while done == False :
		if reg & 1 :
			seq[l] = 1
			reg = (reg >> 1) ^ poly
		else :
			reg = reg >> 1
		if reg == start :
			done = True
		l=l+1
	return seq[0:l]

dither=mls(77794)
print(len(dither),2**17)

dither = np.array(dither)
dither = np.repeat(dither, np.ceil(N/len(dither)))
dither = dither[0:N]

#b,a=signal.butter(2,[0.8,0.9],'bandpass')
b,a=signal.butter(2,0.9,'highpass')
dither = signal.lfilter(b,a,dither)

ft = np.abs(np.fft.fft(dither))
f = np.arange(0,N) * float(fs)/N/1000
plt.plot(f,ft)
plt.xlabel('Frequency (kHz)')
plt.show()

# Generate a sin signal with DC offset.
f=1000
sig=0.2+0.1*np.sin(2*np.pi*np.arange(0,N)*f/fs);

# Adder
y=sig+dither

# threshold or 1-bit ADC
y=2*(y>0)-1

ft = np.abs(np.fft.fft(y))
f = np.arange(0,int(N/300)) * float(fs)/N/1000
plt.plot(f,ft[0:len(f)])
plt.xlabel('Frequency (kHz)')
plt.show()

