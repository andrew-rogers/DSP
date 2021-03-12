#!/usr/bin/env python3

"""Global Position System (GPS) Coarse Acquisition (C/A) code generator

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

# The C/A codes are Gold codes formed by X-OR two GF2^10 fields. Each GF2^10 is
# produced by a Lineaer Feedback Shift Register (LFSR). Whilst a set of 1023
# Gold codes could be produced, only a subset is used for GPS. The shift of the
# second generator output is done using two output taps, the positions of which
# determine the shift and hence, resulting Gold code.

# LFSR for G1 polynomial, X^10 + X^3 + 1
#
#             .----------------(+)<---------------------------------------.
#             |                 ^                                         |
#             |                 |                                         |
#             '->[]--->[]--->[]-'->[]--->[]--->[]--->[]--->[]--->[]--->[]-'-->
# Tap number: 0     1     2     3     4     5     6     7     8     9     10

# LFSR for G2 polynomial, X^10 + X^9 + X^8 + X^6 + X^3 + X^2 + 1
#
#             .----------(+)<--(+)<--------------(+)<--------(+)<--(+)<---.
#             |           ^     ^                 ^           ^     ^     |
#             |           |     |                 |           |     |     |
#             '->[]--->[]-'->[]-o->[]--->[]--->[]-'->[]--->[]-o->[]-'->[]-'
# Tap number: 0     1     2     |     4     5     6     7     |     9     10
#                               |                             v
# Shifted output   X^8 + X^3    '--------------------------->(+)------------->
# Other shifted outputs can be produced by different selections of two taps.

def lfsr(poly) :
    reg = [1]*10
    output = [0]*1023
    for i in range(1023) :
        output[i] = reg[9]

        # X-OR polynomial taps
        y = 0
        for p in poly:
            y = y ^ reg[p-1]

        # Shift the register
        for k in range(9) :
            reg[9-k] = reg[8-k]
        reg[0] = y
    return output

g1_poly = [ 10, 3 ]
g2_poly = [ 10, 9, 8, 6, 3, 2 ]

g1_seq=lfsr(g1_poly)
g2_seq=lfsr(g2_poly)

def gold(shift) :

    # Get the shifted G2 sequence
    code = g2_seq[1023-shift:1023]
    code.extend(g2_seq[0:1023-shift])

    # X-OR with G1 sequence
    for i in range(1023) :
        code[i] = code[i] ^ g1_seq[i] 

    return code

print('1440',gold(5))
print('1131',gold(139))

