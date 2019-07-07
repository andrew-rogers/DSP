## Copyright (C) 2019 Andrew Rogers
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

## -*- texinfo -*-
## @deftypefn  {Function File} {@var{y} =} resample (@var{x}, @var{Ny})
## Change the sample rate of @var{x} to produce an output vector of length @var{Ny}.
##
## @end deftypefn

function y=resample_fractional(x,Ny)
  
  % Interpolate by at least a factor of five.
  Nx = length(x);
  M = ceil(5*Ny/Nx);
  if M<5
    M = 5;
  endif
  x = resample(x,M,1);
  
  % Do the fractional decimation.
  y = decimate_fractional(x,5*Ny);
  
  % Decimate by five to perform smoothing.
  y = resample(y,1,5);
endfunction
