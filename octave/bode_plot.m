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