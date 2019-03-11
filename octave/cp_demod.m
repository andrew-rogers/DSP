## Copyright (C) 2018 Andrew Rogers
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

## y = cp_demod(carrier, Fr, M)
##  continuous phase demodulator with sample rate decimation
##  carrier - the carrier signal
##  Fr - the normalised reference frequency
##  M - the sample rate decimation factor
##
## References:
##
##  <https://www.allaboutcircuits.com/textbook/radio-frequency-analysis-design/radio-frequency-demodulation/quadrature-frequency-and-phase-demodulation/>
##

function [phase_diff, phase]=cp_demod(carrier, ref_freq, M)
  
  % Normalised angular frequency
  w_ref=2*pi*ref_freq;
  
  % Quadrature demodulator
  N=length(carrier);
  isig=resample(cos((0:N-1)*w_ref).*carrier, 1, M);
  qsig=resample(-sin((0:N-1)*w_ref).*carrier, 1, M);

  % Get phase
  phase=atan2(qsig,isig);
  
  % Get the phase changes (differentiation)
  pkg load signal
  phase_diff=filter([1 -1],1,phase);

  % Remove 2*pi peaks by subtracting 2*pi
  idx=find(phase_diff>pi);
  phase_diff(idx)=phase_diff(idx)-2*pi;

  % Remove -2*pi peaks by adding 2*pi
  idx=find(phase_diff<-pi);
  phase_diff(idx)=phase_diff(idx)+2*pi;
endfunction
