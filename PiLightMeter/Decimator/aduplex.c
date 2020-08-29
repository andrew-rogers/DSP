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

#include <stdio.h>
#include <poll.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE "hw:1"

struct device {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hw_params;
    struct pollfd fd;
};

int setup_device(struct device* dev, char* dev_name, snd_pcm_stream_t stream)
{
    int err=0;
    unsigned int rate = 96000;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    snd_pcm_uframes_t period = 512;  // Get poll event every 512 frames

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

int playback(snd_pcm_t *pcm_handle, char* buffer, int num_frames)
{
    static int len=0;
    static char* ptr=0;
    
    if( len==0 ) {
        len = fread(buffer, 1, num_frames*4, stdin);
        // TODO handle read errors
        ptr = buffer;
    }
    
    int nfw = snd_pcm_writei(pcm_handle, ptr, len > 2048 ? 512 : len/4);
    // TODO handle write errors
    
    if( nfw > 0 ) {
        len -= nfw * 4;
        ptr += nfw * 4;
    }
    
    return nfw;
}

int capture(snd_pcm_t *pcm_handle, char* buffer, int num_frames)
{
    int nfr = snd_pcm_readi (pcm_handle, buffer, num_frames > 512 ? 512 : num_frames);
    // TODO handle read errors

    if (nfr > 0) {
        fwrite(buffer, 1, nfr*4, stdout);
        // TODO handle write errors
    }

    return nfr;
}

int do_playback()
{
    int err=0;
	struct device dev;
	if((err = setup_device(&dev, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK)) < 0) {
	    fprintf (stderr, "Cannot setup playback device (%s)\n", snd_strerror (err));
	    return err;
	}

    int num_frames = 1024;
    char *buffer = malloc(num_frames*4); // Each frames has 4 bytes, 2 * 16bit

    int nfw = playback(dev.handle, buffer, num_frames);
    while (nfw>0) {
        poll(&dev.fd, 1, 1000);
        nfw = playback(dev.handle, buffer, num_frames);
    }

    snd_pcm_nonblock(dev.handle,0);
    snd_pcm_drain(dev.handle);
    snd_pcm_close(dev.handle);

    free(buffer);

    return err;
}

int do_capture(int N)
{
    int err=0;
    struct device dev;
    if((err = setup_device(&dev, PCM_DEVICE, SND_PCM_STREAM_CAPTURE)) < 0) {
	    fprintf (stderr, "Cannot setup capture device (%s)\n", snd_strerror (err));
	    return err;
	}

    int num_frames = 1024;
    char *buffer = malloc(num_frames*4); // Each frames has 4 bytes, 2 * 16bit

    int n = capture(dev.handle, buffer, num_frames);
    while( n < N ) {
        poll(&dev.fd, 1, 1000);
        int nfr = capture(dev.handle, buffer, num_frames);
        n = n + nfr*4;
    }

    free(buffer);

    snd_pcm_close (dev.handle);

    return err;
}

int do_both()
{
    int err=0;
    struct device cdev;
    if((err = setup_device(&cdev, PCM_DEVICE, SND_PCM_STREAM_CAPTURE)) < 0) {
	    fprintf (stderr, "Cannot setup capture device (%s)\n", snd_strerror (err));
	    return err;
	}

	struct device pdev;
	if((err = setup_device(&pdev, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK)) < 0) {
	    fprintf (stderr, "Cannot setup playback device (%s)\n", snd_strerror (err));
	    return err;
	}

	int num_frames = 1024;
    char *cbuffer = malloc(num_frames*4); // Each frames has 4 bytes, 2 * 16bit
    char *pbuffer = malloc(num_frames*4); // Each frames has 4 bytes, 2 * 16bit

    struct pollfd fds[2];
    fds[0] = cdev.fd;
    fds[1] = pdev.fd;

    int nfr_total = 0;
    int nfr = capture(cdev.handle, cbuffer, num_frames);
    if( nfr > 0 ) nfr_total = nfr;

    int nfw_total = 0;
    int nfw = playback(pdev.handle, pbuffer, num_frames);
    if( nfw > 0 ) nfw_total = nfw;

    int done=0;
    while ( !done ) {
        poll(fds, 2, 1000);

        nfr = 0;
        if(fds[0].revents) {
            nfr = capture(cdev.handle, cbuffer, num_frames);
            if( nfr > 0 ) nfr_total += nfr;
        }

        nfw = 0;
        if(fds[1].revents) {
            nfw = playback(pdev.handle, pbuffer, num_frames);
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
            nfr = capture(cdev.handle, cbuffer, remaining > num_frames ? num_frames : remaining);
            if( nfr > 0 ) remaining -= nfr;
        }
    }

    free(cbuffer);
    free(pbuffer);

    snd_pcm_close(cdev.handle);
    snd_pcm_nonblock(pdev.handle,0);
    snd_pcm_drain(pdev.handle);
    snd_pcm_close(pdev.handle);
}

int main (int argc, char *args[])
{
    int err=0;
    if (argc > 1) {
        if(strcmp(args[1],"both") == 0) {
            err = do_both();
        }
        else {
            int N=500000;
            sscanf(args[1], "%d", &N);
            err = do_capture(N);
        }
    }
    else {
        err = do_playback();
    }

    return err;
}
