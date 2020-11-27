/*
    Fractional Resampler - Performs a fractional change of sample rate
    Copyright (C) 2020  Andrew Rogers

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

#include "FractionalResampler.h"

FractionalResampler::FractionalResampler(uint32_t M, uint32_t N)
{
	this->M=M;
	this->N=N;
	cnt=0;
	prev=0.0L;
}

uint32_t FractionalResampler::resample(double* input, double* output, uint32_t num_samples)
{
	double invM = 1.0L / M;
	double curr = input[0];
	int n = 0;
	for( int m=0; m<num_samples; m++)
	{

		// Linear interpolation between two adjacent entries in input
		double frac = cnt * invM;
		output[m]=prev*(1-frac)+curr*(frac);

		cnt = cnt + N;

		while( cnt >= M )
		{
			cnt = cnt - M;
			n ++;
			prev = curr;
			curr = input[n];
		}

	}

	return n+1;
}

uint32_t FractionalResampler::resample(double* input, uint32_t num_samples, double* output)
{
	double invM = 1.0L / M;
	int m = 0;
	for( int n=0; n<num_samples; n++)
	{
		double curr = input[n];

		while(cnt<M)
		{
			// Linear interpolation between two adjacent entries in input
			double frac = cnt * invM;
			output[m]=prev*(1-frac)+curr*(frac);
			m++;

			cnt = cnt + N;
		}
		cnt=cnt-M;

		prev = curr;
	}
	return m;
}

