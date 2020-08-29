/*
    PiLightMeter - ALSA Playback
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
#include <alsa/asoundlib.h>

#define PCM_DEVICE "plughw:1"

int setup(snd_pcm_t *pcm_handle, snd_pcm_hw_params_t *hw_params)
{
    int err=0;
    unsigned int rate = 96000;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    if ((err = snd_pcm_hw_params_any (pcm_handle, hw_params)) < 0) {
        fprintf (stderr, "Canot set parameters (%s)\n", snd_strerror (err));
        return (err);
    }

    if ((err = snd_pcm_hw_params_set_access (pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "Cannot set interleaved (%s)\n", snd_strerror (err));
        return (err);
    }

    if ((err = snd_pcm_hw_params_set_rate_near (pcm_handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "Cannot set rate (%s)\n", snd_strerror (err));
        return (err);
    }

    if ((err = snd_pcm_hw_params_set_format (pcm_handle, hw_params, format)) < 0) {
        fprintf (stderr, "Cannot set format (%s)\n", snd_strerror (err));
        return (err);
    }

    if ((err = snd_pcm_hw_params_set_channels (pcm_handle, hw_params, 2)) < 0) {
        fprintf (stderr, "Cannot set stereo (%s)\n", snd_strerror (err));
        return (err);
    }

    if ((err = snd_pcm_hw_params (pcm_handle, hw_params)) < 0) {
        fprintf (stderr, "Cannot apply parameters (%s)\n", snd_strerror (err));
        return (err);
    }

    if ((err = snd_pcm_prepare (pcm_handle)) < 0) {
        fprintf (stderr, "Cannot prepare  (%s)\n", snd_strerror (err));
        return (err);
    }

    return err;
}

int main (int argc, char *args[])
{

    int err=0;
    snd_pcm_t *pcm_handle;
    if ((err = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf (stderr, "Cannot open device %s (%s)\n", PCM_DEVICE, snd_strerror (err));
        return (err);
    }

    // Allocate and initialise parameters.
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    if( (err=setup(pcm_handle, hw_params)) !=0 ) return(err);

    int num_frames = 1024;
    char *buffer = malloc(num_frames*4); // Each frames has 4 bytes, 2 * 16bit

    int nr=1;
    while (nr>0) {
        nr = fread(buffer, 1, num_frames*4, stdin);
        // TODO handle read errors

        err = snd_pcm_writei(pcm_handle, buffer, num_frames);
        // TODO handle write errors
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);

    free(buffer);

    return 0;
}
