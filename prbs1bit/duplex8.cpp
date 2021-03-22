/*
    PRBS 1-bit DAC and ADC
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

/* Input values between 0 and 8 inclusive are read from stdin and converted to
   a bitstream by the 8x interpolator. The bitstream is then filtered by the
   electronics circuit connected to the I2S port. This filtered reference
   signal is combined with the signal to be sampled an read back from the I2S
   interface as in input bitstream. This is supplied to the 8x decimator to
   produce values between 0 and 8 inclusive.

        +--------------+      +-----------------+      +-----------+
    --->|   8x PRBS    |--bs->| I2S electronics |--bs->|     8x    |--->
        | Interpolator |      |   ALSA duplex   |      | Decimator |
        +--------------+      +-----------------+      +-----------+

   bs = bitstream
   
   The electronics will vary depending on application. A lowpass filter filters
   the I2S DOUT signal for DAC applications. A highpass filter is used when the
   PRBS interpolator is used as a dithering signal for ADC applications. The
   PRBS interpolator can be used for both ADC dithering and DAC simultaneously
   if both lowpass and highpass filters are used.
*/

#include "ALSADuplex.h"
#include "prbs1bit.h"

#include <stdio.h>

class PCMWriterStdin : public PCMWriter
{
public:
    int fillBuffer( char *buffer, int num_bytes )
    {
        int len = fread(buffer, 1, num_bytes, stdin);
        // TODO handle read errors

        for( int i=0; i < len; i++ ) buffer[i] = int8.sample( buffer[i] );

        return len;
    }
private:
    Int8 int8;
};

class PCMReaderStdout : public PCMReader
{
public:
    void processBuffer( char *buffer, int num_bytes )
    {
        for( int i=0; i < num_bytes; i++ ) buffer[i] = dec8.sample( buffer[i] );
        fwrite(buffer, 1, num_bytes, stdout);
        // TODO handle write errors
    }
private:
    Dec8 dec8;
};

int main (int argc, char *args[])
{
    PCMWriterStdin writer;
    PCMReaderStdout reader;
    char dev[]="hw:1";
    ALSADuplex duplex( dev, writer, reader );
    int err = duplex.run();
    return err;
}

