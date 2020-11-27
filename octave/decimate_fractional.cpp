/*
    decimate_fractional - Performs a slight reduction of sample rate
    Copyright (C) 2019  Andrew Rogers

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

#include <octave/oct.h>

DEFUN_DLD (decimate_fractional, args, nargout, "Performs a slight reduction of sample rate")
{
	int nargin = args.length ();

	octave_value_list retval(1);

	if(nargin >= 2){
		Matrix x=args(0).matrix_value();
		int M=args(1).matrix_value()(0,0);
		int N=x.numel();

		// Create the output samples
		Matrix ret(1,M);
		double invM = 1.0L / M;
		double prev = 0.0L;
		double curr = x(0);
		int cnt = 0;
		int n = 0;
		for( int m=0; m<M; m++)
		{

			// Linear interpolation between two adjacent entries in input
			double frac = cnt * invM;
			ret(m)=prev*(1-frac)+curr*(frac);

			cnt = cnt + N;

			while( cnt >= M )
			{
				cnt = cnt - M;
				n ++;
				prev = curr;
				curr = x(n);
			}

		}
		retval(0)=ret;
	}
	else
	{
		Matrix ret(0,0);
		retval(0)=ret;
	}

	return retval;
}

