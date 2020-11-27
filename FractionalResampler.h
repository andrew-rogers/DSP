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

#ifndef FRACTIONAL_RESAMPLER_H
#define FRACTIONAL_RESAMPLER_H

#include <cstdint>

class FractionalResampler
{
public:
	FractionalResampler(uint32_t M, uint32_t N);
	uint32_t resample(double* input, double* output, uint32_t num_samples);
	uint32_t resample1(double* input, uint32_t num_samples, double* output);

private:
	uint32_t M,N;
	int32_t cnt;
	double prev;
};

#endif // FRACTIONAL_RESAMPLER_H

