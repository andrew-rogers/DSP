/*
    PiLightMeter - ALSA Duplex using non-blocking and poll
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

#include "ALSADuplex.h"

#include <stdio.h>
#include <poll.h>

/* TODO: Fix 8-bytes frames. Seems that some buffers aren't processed currently
   when 8-byte frames is used on Raspberry Pi. */
#define BYTES_PER_FRAME 4 // Each frames has 4 bytes, 2 * 16-bit
#define FRAMES_PER_PERIOD 1024 // Get poll event every 1024 frames
#define SAMPLE_RATE 192000 // Each frame 32 bits, 192000*32 = 6,144,000 bps

class PCMWriterStdin : public PCMWriter
{
public:
    int fillBuffer( char *buffer, int num_bytes )
    {
        int len = fread(buffer, 1, num_bytes, stdin);
        // TODO handle read errors

        return len;
    }
};

class PCMReaderStdout : public PCMReader
{
public:
    void processBuffer( char *buffer, int num_bytes )
    {
        fwrite(buffer, 1, num_bytes, stdout);
        // TODO handle write errors
    }
};

ALSADuplex* g_duplex = 0;

char g_devname[] = "hw:1";
#define PCM_DEVICE g_devname

int do_both()
{

    int err=0;
    struct device cdev;
    struct device pdev;
    g_duplex->setDevices( pdev, cdev );

	err = g_duplex->setupCaptureDevice();
	if (err < 0) return err;

	err = g_duplex->setupPlaybackDevice();
	if (err < 0) return err;

	int num_frames = 2*FRAMES_PER_PERIOD;
    char *cbuffer = static_cast<char*>( malloc(num_frames*BYTES_PER_FRAME) );
    char *pbuffer = static_cast<char*>( malloc(num_frames*BYTES_PER_FRAME) );

    struct pollfd fds[2];
    fds[0] = cdev.fd;
    fds[1] = pdev.fd;

    int nfr_total = 0;
    int nfr = g_duplex->capture( cbuffer, num_frames );
    if( nfr > 0 ) nfr_total = nfr;

    int nfw_total = 0;
    int nfw = g_duplex->playback( pbuffer, num_frames );
    if( nfw > 0 ) nfw_total = nfw;

    int done=0;
    while ( !done ) {
        poll(fds, 2, 1000);

        nfr = 0;
        if(fds[0].revents) {
            nfr = g_duplex->capture( cbuffer, num_frames );
            if( nfr > 0 ) nfr_total += nfr;
        }

        nfw = 0;
        if(fds[1].revents) {
            // nfw = playback(pdev.handle, pbuffer, num_frames);
            nfw = g_duplex->playback( pbuffer, num_frames );
            if( nfw > 0 ) nfw_total += nfw;
            else done=1;
        }
    }

    // Capture until we have as many frames as was played
    int remaining = nfw_total - nfr_total;
    while( remaining )
    {
        poll(fds, 1, 1000);

        nfr = 0;
        if(fds[0].revents) {
            nfr = g_duplex->capture( cbuffer, remaining > num_frames ? num_frames : remaining );
            if( nfr > 0 ) remaining -= nfr;
        }
    }

    free(cbuffer);
    free(pbuffer);

    snd_pcm_close(cdev.handle);
    snd_pcm_nonblock(pdev.handle,0);
    snd_pcm_drain(pdev.handle);
    snd_pcm_close(pdev.handle);

    return err;
}

int main (int argc, char *args[])
{
    PCMWriter* writer = new PCMWriterStdin();
    PCMReader* reader = new PCMReaderStdout();
    g_duplex = new ALSADuplex(*writer, *reader);
    int err = do_both();
    delete g_duplex;
    delete writer;
    delete reader;
    return err;
}
