/*
    ALSA Duplex Class
    Copyright (C) 2021  Andrew Rogers

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

#include "ALSADuplex.h"

/* TODO: Fix 8-bytes frames. Seems that some buffers aren't processed currently
   when 8-byte frames is used on Raspberry Pi. */
#define BYTES_PER_FRAME 4 // Each frames has 4 bytes, 2 * 16-bit
#define FRAMES_PER_PERIOD 1024 // Get poll event every 1024 frames
#define SAMPLE_RATE 192000 // Each frame 32 bits, 192000*32 = 6,144,000 bps

int ALSADuplex::playback( char* buffer, int num_frames )
{
    if( len==0 ) {
        len = m_writer->fillBuffer( buffer, num_frames*BYTES_PER_FRAME );
        ptr = buffer;
    }

    int nfw = snd_pcm_writei(m_phandle, ptr, len > FRAMES_PER_PERIOD*BYTES_PER_FRAME ? FRAMES_PER_PERIOD : len/BYTES_PER_FRAME);
    // TODO handle write errors
    
    if( nfw > 0 ) {
        len -= nfw * BYTES_PER_FRAME;
        ptr += nfw * BYTES_PER_FRAME;
    }

    return nfw;
}

int ALSADuplex::capture( char* buffer, int num_frames )
{
    int nfr = snd_pcm_readi (m_chandle, buffer, num_frames > FRAMES_PER_PERIOD ? FRAMES_PER_PERIOD : num_frames);
    // TODO handle read errors

    if (nfr > 0) {
        m_reader->processBuffer( buffer, nfr*BYTES_PER_FRAME );
    }

    return nfr;
}
