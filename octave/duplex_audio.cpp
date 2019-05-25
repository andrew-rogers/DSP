/*
    DuplexAudio - Simultaneous record and playback
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
#include "../DuplexAudio/DuplexAudio.h"

DEFUN_DLD (duplex_audio, args, nargout, "Duplex Audio")
{
  int nargin = args.length ();
  
  octave_value_list retval(1);
  
  if(nargin >= 1){
    Matrix playback=args(0).matrix_value();
    DuplexAudio da;
    int nFrames=playback.numel();
    
    // Create playback buffer
    int nPlaybackChannels=da.getNumOutputChannels();
    sample_t* playback_buf=new sample_t[nFrames*nPlaybackChannels];
    for( int f=0; f<nFrames; f++)
    {
      for(int ch=0; ch<nPlaybackChannels;ch++)
      {
        playback_buf[f*nPlaybackChannels+ch]=playback(0,f);
      }
    }
    
    // Create record buffer
    int nRecordChannels=da.getNumInputChannels();
    sample_t* record_buf=new sample_t[nFrames*nRecordChannels];
    
    da.playRecordWait(playback_buf, record_buf, nFrames);
    
    // Copy the recording to return matrix
    Matrix ret(nFrames,nRecordChannels);
    for( int f=0; f<nFrames; f++)
    {
      for(int ch=0; ch<nRecordChannels;ch++)
      {
        ret(f,ch)=record_buf[f*nRecordChannels+ch];
      }
    }
    
    delete [] playback_buf;
    delete [] record_buf;
    
    retval(0)=ret;
  }
  else
  {
    Matrix ret(0,0);
    retval(0)=ret;
  }

  return retval;
}