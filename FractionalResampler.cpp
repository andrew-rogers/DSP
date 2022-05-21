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
	this->M = M;
	this->N = N;
	invM = 1.0L / M;
	cnt = 0;
	prev = 0.0L;
}

uint32_t FractionalResampler::resample(double* input, uint32_t num_samples, double* output)
{
	int m = 0;
	for( int n=0; n<num_samples; n++)
	{
		double curr = input[n];

		while(cnt<M)
		{
			// Linear interpolation between two adjacent entries in input
			output[m]=prev + (curr-prev)*cnt*invM;
			m++;

			cnt = cnt + N;
		}
		cnt=cnt-M;

		prev = curr;
	}
	return m;
}

