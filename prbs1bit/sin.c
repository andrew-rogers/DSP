/*
    8-level quantised sine wave
    Copyright (C) 2021  Andrew Rogers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <stdio.h>
#include <stdint.h>
#include <math.h>

int8_t deltaSigma( double x )
{
    static int8_t y=0;
    static double sum=0.0;
    double delta = x - y;
    sum = sum + delta;
    y = sum;
    if (y>8) y=8;
    if (y<0) y=0;
    return y;
}

int main( int argc, char *args[] )
{
    uint32_t fs = 768000;
    double f = 1000;
    int N=10*fs;
    if (argc > 1) {
        double duration;
        sscanf(args[1], "%lf", &duration);
        N = duration * fs;
    }
    double pi=acos(-1.0);
    for( int n=0; n<N; n++ ) {
        double theta = 2 * pi * f * n / fs;
        int8_t val = deltaSigma(4 * sin(theta) + 4);
        fwrite(&val, 1, 1, stdout);
    }
    return 0;
}

