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
		int n_y=args(1).matrix_value()(0,0);
		int n_x=x.numel();

		double m_xy = (double)n_x/n_y; // Decimation factor

		// Create the output samples
		Matrix ret(1,n_y);
		for( int c_y=0; c_y<n_y; c_y++)
		{
			// Linear interpolation between two adjacent entries in input
			double fi_x = (c_y) * m_xy;
			int i_x = (int)fi_x;
			double frac = fi_x - i_x;
			ret(c_y)=x(i_x)*(1-frac)+x(i_x+1)*(frac);
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

