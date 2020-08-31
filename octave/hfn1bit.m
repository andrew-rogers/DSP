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

# Repeating short Random binary sequence
dither=[0 0 1 1 0 1 0 0 0 1 1 0 1 1 1 1 1 1 0 0 1 0 0 0];
dither=[dither 1-dither];
dither=repmat(dither,1,ceil(fs/length(dither)));
dither=dither(1:fs)-0.5;

# Analogue RC highpass filter.
pkg load signal
[b,a]=butter(1,0.15,'high');
hf_noise=filter(b,a,dither);

# Generate a sin signal
f=1000
sig=0.1*sin(2*pi*(1:fs)*f/fs);

# Adder
y=sig+hf_noise;

# threshold
y=2*(y>0)-1;

figure
bode_plot(y,fs,0,10e3);

figure
bode_plot(y,fs,100,10000);

# Resample to audio
fsr=8000;
[b,a]=butter(4,fsr/fs);
y=filter(b,a,y);
yr=resample_fractional(y,length(y)*fsr/fs);
#soundsc(yr,fsr)

