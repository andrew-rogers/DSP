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

#include "DuplexAudio.h"

#include <iostream>

using namespace std;

int PortAudioCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* outTime,
	PaStreamCallbackFlags statusFlags,
    void *userData )
{
    DuplexAudio* da=(DuplexAudio*)userData;
    return da->callback(inputBuffer, outputBuffer, framesPerBuffer, outTime, statusFlags);
}

DuplexAudio::DuplexAudio()
{
    initialise();
}

PaError DuplexAudio::initialise()
{
    m_sampleRate = 44100.0;

    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if( err != paNoError ) return err;

	// Get input device info
	int inputDevice = Pa_GetDefaultInputDevice();
	if( inputDevice < 0 )
	{
        cerr << "ERROR: Pa_GetDefaultInputDevice() returned " << inputDevice << "." << endl;
        err = inputDevice;
        return err;
    }
	const PaDeviceInfo* inputInfo( Pa_GetDeviceInfo( inputDevice ) );
    m_nInputChannels = inputInfo->maxInputChannels;
    if( m_nInputChannels > 2) m_nInputChannels=2; // Just record two channels

    // Get output device info
	int outputDevice = Pa_GetDefaultOutputDevice();
	if (outputDevice < 0)
	{
        cerr << "ERROR: Pa_GetDefaultOutputDevice() returned " << outputDevice << "." << endl;
        err = outputDevice;
        return err;
    }
	const PaDeviceInfo* outputInfo( Pa_GetDeviceInfo( outputDevice ) );
    m_nOutputChannels = outputInfo->maxOutputChannels;
    if( m_nOutputChannels > 2) m_nOutputChannels=2; // Just playback two channels

    return err;
}

PaError DuplexAudio::playRecordWait(sample_t* play_buf, sample_t* record_buf, int num_frames)
{
    // Setup pointers and counters
    m_pPlaybackBuffer=play_buf;
    m_pRecordBuffer=record_buf;
    m_nFrames=num_frames;
    m_cFrame = 0;

    // Open the stream
    PaStream* p_stream;
    PaError err = Pa_OpenDefaultStream( &p_stream,
        m_nInputChannels,
        m_nOutputChannels,
		PA_SAMPLE_TYPE,
		m_sampleRate,
        256, // frames per buffer
		PortAudioCallback,
        this );
	if( err != paNoError )
	{
		cerr << "ERROR: Pa_OpenDefaultStream() returned " << err << "." << endl;
        return err;
	}

    // Start streaming
    err = Pa_StartStream( p_stream );
	if( err != paNoError )
	{
		cerr << "ERROR: Pa_StartStream() returned " << err << "." <<  endl;
        return err;
	}

    // Wait for audio to finish
    while( Pa_IsStreamActive( p_stream ) )
    {
        Pa_Sleep(1);
    }

    // Close the stream
    err = Pa_CloseStream( p_stream );
	if( err != paNoError )
	{
		cerr << "ERROR: Pa_CloseStream() returned " << err << "." <<  endl;
        return err;
	}

    Pa_Terminate();

    return err;
}

int DuplexAudio::callback(
    const void *pRecordBuffer,
    void *pPlaybackBuffer,
    unsigned long nFramesPerBuffer,
    const PaStreamCallbackTimeInfo* p_timeInfo,
    PaStreamCallbackFlags statusFlags )
{
    // Calculate how many frames to copy
    int finished = 0;
    unsigned long nFrames = nFramesPerBuffer;
    unsigned long remainingFrames = m_nFrames - m_cFrame;
    if( remainingFrames <= nFramesPerBuffer )
    {
        nFrames=remainingFrames;
        finished=1;
    }

    // Copy playback buffer
    sample_t *p_dst = (sample_t*)pPlaybackBuffer;
    for( unsigned long cFrame=0; cFrame<nFrames; cFrame++ )
    {
        for( unsigned long ch=0; ch < m_nOutputChannels; ch++ )
        {
			*p_dst++ = m_pPlaybackBuffer[(m_cFrame+cFrame)*m_nOutputChannels+ch];
		}
    }

    // Copy record buffer
    sample_t *p_src = (sample_t*)pRecordBuffer;
    for( unsigned long cFrame=0; cFrame<nFrames; cFrame++ )
    {
        for( unsigned long ch=0; ch<m_nInputChannels; ch++ )
        {
            m_pRecordBuffer[(m_cFrame+cFrame)*m_nInputChannels+ch] = *p_src++;
        }
    }

    // Update the frame counter
    m_cFrame += nFrames;

    return finished;
}

