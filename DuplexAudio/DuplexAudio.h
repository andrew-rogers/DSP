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

#ifndef DUPLEX_AUDIO_H
#define DUPLEX_AUDIO_H

#include "portaudio.h"

typedef float sample_t;
#define PA_SAMPLE_TYPE  paFloat32


class DuplexAudio
{
    public:
        DuplexAudio();
        PaError initialise();
        PaError playRecordWait(sample_t* play_buf, sample_t* record_buf, int num_frames);
        float getSampleRate(){ return m_sampleRate; }
        unsigned int getNumOutputChannels(){ return m_nOutputChannels; }
        unsigned int getNumInputChannels(){ return m_nInputChannels; }

        friend int PortAudioCallback(
            const void *inputBuffer,
            void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* outTime,
			PaStreamCallbackFlags statusFlags,
            void *userData );
    private:
        unsigned long m_nFrames;
        unsigned long m_cFrame;
        unsigned int m_nInputChannels;
        unsigned int m_nOutputChannels;
        float m_sampleRate;
        sample_t* m_pRecordBuffer;
        sample_t* m_pPlaybackBuffer;
        int callback(
            const void *pRecordBuffer,
            void *pPlaybackBuffer,
            unsigned long nFramesPerBuffer,
            const PaStreamCallbackTimeInfo* p_timeInfo,
            PaStreamCallbackFlags statusFlags );
};

#endif // DUPLEX_AUDIO_H

