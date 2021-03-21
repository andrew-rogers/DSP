#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import subprocess

fs = 6144000

def runSinInt8() :
    cp=subprocess.run( ["sh", "-c", "./sin 0.002 | ./int8"], capture_output = True )
    bitstream = []
    for b in cp.stdout :
        for i in range(8) :
            bitstream.append( int(b&1) )
            b = b >> 1

    return bitstream

din = runSinInt8()

# Simulate simple RC filter, 40kHz cut-off
b,a=signal.butter(1,40000/fs,'lowpass')
sig = signal.lfilter(b,a,din)

t=np.arange(0,len(sig))*1000.0/fs
plt.plot(t,sig)
plt.xlabel('Time (ms)')
plt.show()

