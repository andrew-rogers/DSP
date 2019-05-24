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

#include <stdio.h>

DuplexAudio::DuplexAudio()
{
    initialise();
}

/* The code below is based on TestTone in the MATAA project at https://github.com/mbrennwa/mataa 
 * Copyright (C) 2006, 2007, 2008 Matthias S. Brennwald.
 */

PaError DuplexAudio::initialise()
{
    // initialize PortAudio:
    PaError err = Pa_Initialize();
    if( err != paNoError ) return err;
	
	// get audio devices info:
	int inputDevice = Pa_GetDefaultInputDevice();
	if (inputDevice < 0)
	{
        printf( "ERROR: Pa_GetDefaultInputDevice returned %i\n", inputDevice );
        err = inputDevice;
        return err;
    }
	const   PaDeviceInfo *inputInfo;
	inputInfo = Pa_GetDeviceInfo( inputDevice );
	
	int outputDevice = Pa_GetDefaultOutputDevice();
	if (outputDevice < 0)
	{
        printf( "ERROR: Pa_GetDefaultOutputDevice returned %i\n", outputDevice );
        err = outputDevice;
        return err;;
    }
	const   PaDeviceInfo *outputInfo;
	outputInfo = Pa_GetDeviceInfo( outputDevice );

	// Prepare data:
    data.processedFrames = 0;
    data.numInputDeviceChannels = inputInfo->maxInputChannels;
    if( data.numInputDeviceChannels > 2) data.numInputDeviceChannels=2;
    data.numOutputDeviceChannels = outputInfo->maxOutputChannels;
	data.samplingRate = 44100.0;

    return err;
}

PaError DuplexAudio::playRecordWait(sample_t* play_buf, sample_t* record_buf, int num_frames)
{
    data.outputSamples=play_buf;
    data.inputSamples=record_buf;
    data.numFrames=num_frames;
    PaStream		*stream;
    // Record and play audio data:	
    PaError err = Pa_OpenDefaultStream( &stream,
                                data.numInputDeviceChannels,          // number of input channels
                                data.numOutputDeviceChannels,         // number of input channels
								PA_SAMPLE_TYPE,					// sample type
								data.samplingRate,				// sampling rate
                                256,							// frames per buffer (use something in the 128-1024 range, or use paFramesPerBufferUnspecified to let portaudio decide)
								RecordAndPlayCallback,			// the callback function
                                this );						// pointer to the audio data
	if( err != paNoError ) 
	{
		printf( "ERROR: Pa_OpenDefaultStream returned %i\n", err );
        return err;
	}
									
    err = Pa_StartStream( stream );
	if( err != paNoError ) 
	{
		printf( "ERROR: Pa_StartStream returned %i\n", err );
        return err;
	}

    while( Pa_IsStreamActive( stream ) )
    {
        Pa_Sleep(1); // sleep while audio I/O
    }
    err = Pa_CloseStream( stream );
	if( err != paNoError ) 
	{
		printf( "ERROR: Pa_CloseStream returned %i\n", err );
        return err;
	}

    Pa_Terminate();

    return err;

}

int RecordAndPlayCallback(
                            const void *inputBuffer,
                            void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* outTime,
							PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    unsigned long iF,iFmax,remainingFrames,iC;
    DuplexAudio* da=(DuplexAudio*)userData;
    paTestData* data;
    int finished;
    
/* Cast data passed through stream to our structure. */
    data = &(da->data);
    
/* Handle sound output buffer */
    SAMPLE *out = (SAMPLE*)outputBuffer;
    //(void) outTime; /* Prevent unused variable warnings. */
    //(void) inputBuffer;
    remainingFrames = data->numFrames - data->processedFrames;
    if (remainingFrames > framesPerBuffer)
    {
        iFmax=framesPerBuffer;
        finished=0;
    }
    else
    { /* last buffer... */
        iFmax=remainingFrames;
        finished=1;
    }
		
    for( iF=0; iF<iFmax; iF++ )
    {
        for( iC=0; iC < data->numOutputDeviceChannels; iC++ ) {
			*out++ = data->outputSamples[(iF+data->processedFrames)*data->numOutputDeviceChannels+iC];
		}
    }

/* Handle sound input buffer */
    SAMPLE *in = (SAMPLE*)inputBuffer;
    for( iF=0; iF<iFmax; iF++ )
    {
        for( iC=0; iC<data->numInputDeviceChannels; iC++ ) data->inputSamples[(iF+data->processedFrames)*data->numInputDeviceChannels+iC]=*in++;
    }
    
/* Prepare for next callback-cycle: */    
    data->processedFrames += iFmax;

return finished;
}

