#!/usr/bin/env python3

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

def resample(x, rate) :
    x_cnt = 0.0 # The input counter is fractional to give interpolation phase
    y_cnt = 0
    x_prev = 0
    y = [0]*int(len(x)*rate+1)
    for s in x :
        while x_cnt >= 0 :
            y[y_cnt] = x_cnt*x_prev + (1-x_cnt)*s # Linear interpolate
            y_cnt = y_cnt + 1
            x_cnt = x_cnt - 1/rate
        x_prev = s
        x_cnt = x_cnt + 1
    return y[0:y_cnt]
    
if __name__ == "__main__":

    import numpy as np
    import matplotlib.pyplot as plt

    input=np.sin(np.arange(20)/3)
    rate=3/5
    y=resample(input,rate)
    print(len(y))

    plt.plot(input,'o-')
    plt.plot(np.arange(len(y))/rate,y,'+-')

    plt.show()

