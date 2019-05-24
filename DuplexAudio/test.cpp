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



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "DuplexAudio.h"

#include <iostream>

using namespace std;

void testTone(sample_t* buffer, unsigned long num_samples, float freq, float sample_rate);
void duplicateChannels(sample_t* src, sample_t* dst, unsigned long num_frames, unsigned int num_channels);
void dumpRecording(sample_t* samples, unsigned long num_frames, unsigned int num_channels, float sample_rate);

int main(int argc, char *argv[])
{
    DuplexAudio da;

    float sample_rate = da.getSampleRate();
    unsigned int num_channels = da.getNumOutputChannels();
    unsigned int num_rec_channels = da.getNumInputChannels();

    float duration=1.0;
    unsigned long num_frames=sample_rate*duration;

    // Create the test signal vector
    sample_t* test_vec = (sample_t*)malloc(num_frames*sizeof(sample_t));
	if (!test_vec) {
		cerr << "ERROR: Could not allocate test vector." << endl;
		exit(1);
	}
    testTone(test_vec, num_frames, 1000.0, sample_rate); 

	// Create multiple playback channel signals from test vector
    unsigned long num_bytes = num_frames * num_channels * sizeof(sample_t);
    sample_t* playback_buf = (sample_t *) malloc( num_bytes );
    if( playback_buf == NULL )
    {
        cerr << "ERROR: Could not allocate playback buffer." << endl;
        exit(1);
    }
	duplicateChannels(test_vec, playback_buf, num_frames, num_channels);
    free( test_vec );

    // Create the zero-initialised recording buffer
    num_bytes = num_frames * num_rec_channels * sizeof(sample_t);
    sample_t* record_buf = (SAMPLE *) calloc( 1, num_bytes );
    if( record_buf == NULL )
    {
        cerr << "ERROR: Could not allocate recording buffer." << endl;
        exit(1);
    }

    da.playRecordWait(playback_buf, record_buf, num_frames);

    dumpRecording(record_buf, num_frames, num_rec_channels, sample_rate);

	free(playback_buf);
    free(record_buf);


	return 0;
}

void testTone(sample_t* buffer, unsigned long num_samples, float freq, float sample_rate)
{
    const float PI=3.141592653589793;
    for( unsigned long n=0; n<num_samples; n++ )
    {
        buffer[n]=sin(2.0*PI*n*freq/sample_rate);
    }
}

void duplicateChannels(sample_t* src, sample_t* dst, unsigned long num_frames, unsigned int num_channels)
{
    for( unsigned int ch = 0; ch < num_channels; ch++ ) {
		for( unsigned long n = 0; n < num_frames; n++ ) {
			dst[num_channels*n+ch] = src[n];
		}
	}
}

/* The code below is based on TestTone in the MATAA project at https://github.com/mbrennwa/mataa 
 * Copyright (C) 2006, 2007, 2008 Matthias S. Brennwald.
 */

void dumpRecording(sample_t* samples, unsigned long num_frames, unsigned int num_channels, float sample_rate)
{
    // print header:
    printf("%% Number of frames = %d\n", num_frames);
    printf("%% Number of sound input channels = %d\n", num_channels);
    printf("%% Sampling rate = %f Hz\n", sample_rate);
	printf("%%\n");
	printf("%% Recorded data:\n"),
    printf("%% time (s)\t");
    for( unsigned long iChannel=0; iChannel < num_channels;)
    {
        iChannel++;
        printf("channel-%d ",iChannel);
    }
    printf("\n");

	// print recorded data:
    float frameTime;
    for( unsigned long iFrame=0; iFrame<num_frames; iFrame++ )
    {
	frameTime = (float)iFrame / sample_rate;
        printf("%E",frameTime); // print frame sampling time
        for( unsigned long iChannel = 0; iChannel < num_channels; iChannel++ )
        {
            printf("\t");
            printf("%E",samples[iFrame*num_channels+iChannel]);
        }
        printf("\n");
    }
}

