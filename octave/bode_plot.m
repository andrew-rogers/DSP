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

function [amp_r,phase_r,f_r]=bode_plot(y,Fs,Fl,Fh)
  N=length(y);
  
  % Get the start and stop bins
  kl=floor(Fl*N/Fs+0.5);
  kh=floor(Fh*N/Fs+0.5);
  
  % Get the selected bins of FFT
  ff=fft(y);
  ff=ff(kl+1:kh+1);
  
  % Calculate amplitude and phase
  amp=abs(ff);
  phase=arg(ff);
  
  % Create frequecy axis values
  f=(kl:kh)*Fs/N;
  
  if nargout<1
    semilogx(f,20*log10(amp));
    xlabel('Frequency (Hz)');
    ylabel('Amplitude (dB)');
  else
    amp_r=amp;
    phase_r=phase;
    f_r=f;
  endif
endfunction