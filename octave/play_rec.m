function y=play_rec(x,Fs)
  
  % Define sample rate is not supplied
  if nargin<2
    Fs=44100;
  endif
  
  % Re-arrange to columns per channel
  nrows=size(x,1);
  ncols=size(x,2);
  if(ncols>nrows)
    x=x';
  endif
  
  % No more than two channels
  ncols=size(x,2);
  if ncols>2
    x=x(:,1:2);
  endif
  
  if exist('duplex_audio.oct','file')
    y=duplex_audio(x,Fs);
  else
    % Use Octave's player and recorder. Based on code found at
    %  http://octave.1599824.n4.nabble.com/Simultaneous-play-record-of-audio-signals-td4679448.html
    p=audioplayer(x,Fs)
    r=audiorecorder(Fs,16)
    play(p)
    duration=length(x)/Fs;
    recordblocking(r,duration)
    y=getaudiodata(r);
  endif
endfunction