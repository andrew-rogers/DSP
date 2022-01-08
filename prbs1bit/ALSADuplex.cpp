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

namespace
{
    const int BytesPerFrame = 8;      // Each frames has 8 bytes, 2 * 32-bit
    const int FramesPerPeriod = 1024;
    const int SampleRate = 192000;    // Each frame 64 bits, 192000*64 = 12,288,000 bps

    int setup_device(struct device* dev, char* dev_name, snd_pcm_stream_t stream)
    {
        int err=0;
        unsigned int rate = SampleRate;
        snd_pcm_format_t format = SND_PCM_FORMAT_S32_LE;

        //snd_pcm_hw_params_t *hw_params;
        snd_pcm_hw_params_alloca(&(dev->hw_params));

        if ((err = snd_pcm_open(&(dev->handle), dev_name, stream, 0)) < 0) {
            fprintf (stderr, "Cannot open device %s (%s)\n", dev_name, snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_hw_params_any (dev->handle, dev->hw_params)) < 0) {
            fprintf (stderr, "Canot set parameters (%s)\n", snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_hw_params_set_access (dev->handle, dev->hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
            fprintf (stderr, "Cannot set interleaved (%s)\n", snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_hw_params_set_rate_near (dev->handle, dev->hw_params, &rate, 0)) < 0) {
            fprintf (stderr, "Cannot set rate (%s)\n", snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_hw_params_set_format (dev->handle, dev->hw_params, format)) < 0) {
            fprintf (stderr, "Cannot set format (%s)\n", snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_hw_params_set_channels (dev->handle, dev->hw_params, 2)) < 0) {
            fprintf (stderr, "Cannot set stereo (%s)\n", snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_hw_params (dev->handle, dev->hw_params)) < 0) {
            fprintf (stderr, "Cannot apply parameters (%s)\n", snd_strerror (err));
            return (err);
        }

        if ((err = snd_pcm_prepare (dev->handle)) < 0) {
            fprintf (stderr, "Cannot prepare  (%s)\n", snd_strerror (err));
            return (err);
        }

        return err;
    }
}

int ALSADuplex::setupCaptureDevice()
{
    int err = 0;
    if((err = setup_device(&m_cdev, m_devname, SND_PCM_STREAM_CAPTURE)) < 0) {
	    fprintf (stderr, "Cannot setup capture device (%s)\n", snd_strerror (err));
	}
	return err;
}

int ALSADuplex::setupPlaybackDevice()
{
    int err = 0;
    if((err = setup_device(&m_pdev, m_devname, SND_PCM_STREAM_PLAYBACK)) < 0) {
        fprintf (stderr, "Cannot setup playback device (%s)\n", snd_strerror (err));
    }
    return err;
}

int ALSADuplex::playback( char* buffer, int num_frames )
{
    if( len==0 ) {
        len = m_writer->fillBuffer( buffer, num_frames*BytesPerFrame );
        ptr = buffer;
        if ( len <= 0 ) m_done=true;
    }

    int nfw = snd_pcm_writei(m_pdev.handle, ptr, len > FramesPerPeriod*BytesPerFrame ? FramesPerPeriod : len/BytesPerFrame);
    // TODO handle write errors

    if( nfw > 0 ) {
        len -= nfw * BytesPerFrame;
        ptr += nfw * BytesPerFrame;
    }

    if( nfw < 0 ) {
        fprintf (stderr, "Playback error: (%s)\n", snd_strerror (nfw));
        m_abort = true; // Abort duplex loop.
    }

    return nfw;
}

int ALSADuplex::capture( char* buffer, int num_frames )
{
    int nfr = snd_pcm_readi (m_cdev.handle, buffer, num_frames > FramesPerPeriod ? FramesPerPeriod : num_frames);
    // TODO handle read errors

    if (nfr > 0) {
        m_reader->processBuffer( buffer, nfr*BytesPerFrame );
    }

    if( nfr < 0 && nfr!=-EAGAIN ) {
        fprintf (stderr, "Capture error: (%s)\n", snd_strerror (nfr));
        m_abort = true; // Abort duplex loop.
    }

    return nfr;
}

/* Seems we don't need poll and alsa-lib/latency.c optionally uses it
   https://github.com/alsa-project/alsa-lib/blob/master/test/latency.c
   It does two playback writes before capture read */
int ALSADuplex::run()
{
    const int PlaybackPrefill = 2;

    int err=0;

	err = setupCaptureDevice();
	if (err < 0) return err;

	err = setupPlaybackDevice();
	if (err < 0) return err;

	int num_frames = 2*FramesPerPeriod;
    char *cbuffer = static_cast<char*>( malloc(num_frames*BytesPerFrame) );
    char *pbuffer = static_cast<char*>( malloc(num_frames*BytesPerFrame) );

    int remaining=0;

    // Prefill the playback buffer
    for (int i=0; i<PlaybackPrefill; i++) {
        int nfw = playback( pbuffer, num_frames );
        if( nfw > 0 ) remaining += nfw;
    }

    // Main duplex loop
    while ( !m_done && !m_abort) {
        int nfr = capture( cbuffer, num_frames );
        if( nfr > 0 ) remaining -= nfr;

        int nfw = playback( pbuffer, num_frames );
        if( nfw > 0 ) remaining += nfw;
    }

    // Capture until we have as many frames as was played
    while( remaining > 0 && !m_abort)
    {
        int nfr = capture( cbuffer, remaining > num_frames ? num_frames : remaining );
        if( nfr > 0 ) remaining -= nfr;
    }

    free(cbuffer);
    free(pbuffer);

    snd_pcm_close(m_cdev.handle);
    snd_pcm_nonblock(m_pdev.handle,0);
    snd_pcm_drain(m_pdev.handle);
    snd_pcm_close(m_pdev.handle);

    return err;
}

