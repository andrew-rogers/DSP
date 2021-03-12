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
#
# Other shifted outputs can be produced by different selections of two taps.
# The implementation below generates the G2 non-shifted sequence and stores it
# in memory. The shifted versions are then procuded by rotating the sequence.
# The two-tap method is therefore not implemented in practice.
#
# Table 2-1 in the SPS spec gives the G2 shift and the first 10 chips of the
# Gold code for each satellite.

sv_tab='''
ID	Shift	First 10 chips
1	5	1440
2	6	1620
3	7	1710
4	8	1744
5	17	1133
6	18	1455
7	139	1131
8	140	1454
9	141	1626
10	251	1504
11	252	1642
12	254	1750
13	255	1764
14	256	1772
15	257	1775
16	258	1776
17	469	1156
18	470	1467
19	471	1633
20	472	1715
21	473	1746
22	474	1763
23	509	1063
24	512	1706
25	513	1743
26	514	1761
27	515	1770
28	516	1774
29	859	1127
30	860	1453
31	861	1625
32	862	1712
'''

sv_info = [{'shift':0, 'chips':''}]

def init_sv_info() :
    for line in sv_tab.split('\n') :
        parts = line.split()
        if len(parts) == 3 :
            sv_info.append({'shift':int(parts[1]), 'chips':parts[2]})

init_sv_info()

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

def test() :
    for sid in range(1,33) :
        shift = sv_info[sid]['shift']
        expected_chips = sv_info[sid]['chips']

        # Get first ten chips from Gold generator
        chips = 0
        for c in gold(shift)[0:10] :
            chips = chips * 2 + c;
        chips = oct(chips)[2:]

        # Compare with expected chips
        msg = 'passed'
        if chips != expected_chips :
            msg = 'failed <---'

        print(sid, expected_chips, chips, msg)

if __name__ == "__main__":
    test()

