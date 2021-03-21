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

char g_devname1[] = "hw:1";
#define PCM_DEVICE g_devname1

/* TODO: Fix 8-bytes frames. Seems that some buffers aren't processed currently
   when 8-byte frames is used on Raspberry Pi. */
#define BYTES_PER_FRAME 4 // Each frames has 4 bytes, 2 * 16-bit
#define FRAMES_PER_PERIOD 1024 // Get poll event every 1024 frames
#define SAMPLE_RATE 192000 // Each frame 32 bits, 192000*32 = 6,144,000 bps

namespace
{
    int setup_device(struct device* dev, char* dev_name, snd_pcm_stream_t stream)
    {
        int err=0;
        unsigned int rate = SAMPLE_RATE;

        snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
        if (BYTES_PER_FRAME == 8) {
            snd_pcm_format_t format = SND_PCM_FORMAT_S32_LE;
        }
        snd_pcm_uframes_t period = FRAMES_PER_PERIOD;

        if ((err = snd_pcm_open(&(dev->handle), dev_name, stream, SND_PCM_NONBLOCK)) < 0) {
            fprintf (stderr, "Cannot open device %s (%s)\n", dev_name, snd_strerror (err));
            return (err);
        }

        // Allocate and initialise parameters.
        snd_pcm_hw_params_alloca(&(dev->hw_params));

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

        if((err = snd_pcm_hw_params_set_period_size(dev->handle, dev->hw_params, period, 0)) < 0) {
            fprintf (stderr, "Cannot set period size (%s)\n", snd_strerror (err));
            return (err);
        }

	    if ((err = snd_pcm_prepare (dev->handle)) < 0) {
            fprintf (stderr, "Cannot prepare  (%s)\n", snd_strerror (err));
            return (err);
        }

        if((err = snd_pcm_poll_descriptors(dev->handle, &(dev->fd), 1)) < 0)
	    {
		    fprintf (stderr, "Cannot get descriptor (%s)\n", snd_strerror (err));
            return (err);
	    }

	    return err;
    }
}

int ALSADuplex::setupCaptureDevice()
{
    int err = 0;
    if((err = setup_device(m_cdev, PCM_DEVICE, SND_PCM_STREAM_CAPTURE)) < 0) {
	    fprintf (stderr, "Cannot setup capture device (%s)\n", snd_strerror (err));
	}
	return err;
}

int ALSADuplex::setupPlaybackDevice()
{
    int err = 0;
    if((err = setup_device(m_pdev, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK)) < 0) {
        fprintf (stderr, "Cannot setup playback device (%s)\n", snd_strerror (err));
    }
    return err;
}

int ALSADuplex::playback( char* buffer, int num_frames )
{
    if( len==0 ) {
        len = m_writer->fillBuffer( buffer, num_frames*BYTES_PER_FRAME );
        ptr = buffer;
    }

    int nfw = snd_pcm_writei(m_pdev->handle, ptr, len > FRAMES_PER_PERIOD*BYTES_PER_FRAME ? FRAMES_PER_PERIOD : len/BYTES_PER_FRAME);
    // TODO handle write errors
    
    if( nfw > 0 ) {
        len -= nfw * BYTES_PER_FRAME;
        ptr += nfw * BYTES_PER_FRAME;
    }

    return nfw;
}

int ALSADuplex::capture( char* buffer, int num_frames )
{
    int nfr = snd_pcm_readi (m_cdev->handle, buffer, num_frames > FRAMES_PER_PERIOD ? FRAMES_PER_PERIOD : num_frames);
    // TODO handle read errors

    if (nfr > 0) {
        m_reader->processBuffer( buffer, nfr*BYTES_PER_FRAME );
    }

    return nfr;
}
