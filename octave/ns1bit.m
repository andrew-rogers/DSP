## Copyright (C) 2020 Andrew Rogers
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
# filter is used to shape the noise to the higher frequencies away from the low 
# sampled signal frequencies.
#
#  +---------------+     +----------+
#  | Random binary |---->| Highpass |-----.
#  | Sequence gen  |     |  filter  |     |
#  +---------------+     +----------+     v    +-----------+
#                                        (+)-->| Threshold |---> bitstream
#       +-------------------------+       ^    +-----------+
#       | Low freq. signal source |       |
#       |     eg. thermistor      |-------'
#       +-------------------------+
#

fs=1000000;

# Random binary sequence
dither=rand(1,fs);
dither=dither>0.5;

# Analogue RC highpass noise shaping
pkg load signal
[b,a]=butter(1,0.1,'high');
hf_noise=filter(b,a,dither);

# Generate an offset signal
sig=0.05*sin((1:fs)/105)+0.1*sin((1:fs)/250);
sig=0.5+sig; # sig never goes negative

# Adder
y=sig+hf_noise;

# threshold
y=2*(y>0)-1;


bode_plot(y,fs,0,10e3);

