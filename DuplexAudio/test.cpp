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

/* The code below is based on TestTone in the MATAA project at https://github.com/mbrennwa/mataa 
 * Copyright (C) 2006, 2007, 2008 Matthias S. Brennwald.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "portaudio.h"
#include "DuplexAudio.h"

#define PI		(3.141592653589793)


typedef paTestData *paTestDataPtr;



/*******************************************************************/
int main(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    PaStream		*stream;
    PaError			err;
    DuplexAudio da;
    paTestData&		data=da.data;
    unsigned long	iFrame,iChannel;
    unsigned long	numBytes;
    SAMPLE			*testSignal;
	unsigned int	numChanTestSignal = 0;    // number of channels in the test signal generated or read from disk. This should be less or equal than the number of output channels supported by the sound output device

    da.initialise();
	
	unsigned long numFrames=0;
	
    
    printf("%% No input file given! Using default signal instead: 1 kHz sine, 1 sec duration\n");
	numChanTestSignal = 1;
    numFrames=data.samplingRate; // determines signal duration
    float f=1000.0; // frequency of test tone 
    testSignal = (SAMPLE*)malloc(numChanTestSignal*numFrames*sizeof(SAMPLE));
	if (!testSignal) {
		printf("ERROR: could not allocate memory for test data\n");
		goto error;
	}
	if (data.samplingRate <= 2*f) {
		printf("%% *** Warning: Nyquist frequency too low because sampling rate is too low!\n");
	}
    for (iFrame=0; iFrame<numFrames; iFrame++) testSignal[iFrame]=sin((float)iFrame/data.samplingRate*f*2.0*PI);
    
	
	// allocate memory for output data:
    numBytes = numFrames * data.numOutputDeviceChannels * sizeof(sample_t);
    sample_t* outputSamples= (sample_t *) malloc( numBytes );
    if( outputSamples == NULL )
    {
        printf("ERROR: could not allocate output frames buffer.");
        exit(1);
    }
    
	// move the testSignal to outputSamples:
	unsigned int dummy;
	for ( dummy = 0; dummy < data.numOutputDeviceChannels; dummy++ ) {
		if (dummy < numChanTestSignal) {
			iChannel = dummy;
		}
		else {
			iChannel = numChanTestSignal-1;
		}
		for( iFrame = 0; iFrame < numFrames; iFrame++ ) {
			outputSamples[data.numOutputDeviceChannels*iFrame+dummy] = testSignal[numChanTestSignal*iFrame+iChannel];
		}
	}
	
    free(testSignal);

    sample_t* inputSamples= (SAMPLE *) malloc( numBytes );
    if( inputSamples == NULL )
    {
        printf("Could not allocate output frames buffer");
        exit(1);
    }

    for( iFrame=0; iFrame<numFrames; iFrame++ ) // initialize input frames:
    {
        for( iChannel=0; iChannel < data.numInputDeviceChannels; iChannel++ ) {
			inputSamples[iFrame*data.numInputDeviceChannels+iChannel] = 0;
		}
    }
    
    err=da.playRecordWait(outputSamples, inputSamples, numFrames);
	


	// print header:
    printf("%% Number of frames = %d\n", numFrames);
    printf("%% Number of sound output channels = %d\n", data.numOutputDeviceChannels);
    printf("%% Number of sound input channels = %d\n", data.numInputDeviceChannels);
    printf("%% Sampling rate = %f Hz\n", data.samplingRate);
	printf("%%\n");
	printf("%% Recorded data:\n"),
    printf("%% time (s)\t");
    for( iChannel=0; iChannel < data.numInputDeviceChannels;)
    {
        iChannel++;
        printf("channel-%d ",iChannel);
    }
    printf("\n");
    
	// print recorded data:
    float frameTime;
    for( iFrame=0; iFrame<data.numFrames; iFrame++ )
    {
	frameTime = (float)iFrame / data.samplingRate;
        printf("%E",frameTime); // print frame sampling time
        for( iChannel = 0; iChannel < data.numInputDeviceChannels; iChannel++ )
        {
            printf("\t");
            printf("%E",inputSamples[iFrame*data.numInputDeviceChannels+iChannel]);
        }
        printf("\n");
    }
		
	// clean up:
    free(inputSamples);
    free(outputSamples);
					
	// exit:
    return 0;
	
pa_error:
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	fprintf( stderr, "Last error from host API: %s\n", Pa_GetLastHostErrorInfo()->errorText );
	goto error;

error:
    Pa_Terminate();
	return -1;
	
}
