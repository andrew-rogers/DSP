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

#include "ALSADuplex.h"

#include <stdio.h>

class PCMWriterStdin : public PCMWriter
{
public:
    int fillBuffer( char *buffer, int num_bytes )
    {
        int len = fread(buffer, 1, num_bytes, stdin);
        // TODO handle read errors

        return len;
    }
};

class PCMReaderStdout : public PCMReader
{
public:
    void processBuffer( char *buffer, int num_bytes )
    {
        fwrite(buffer, 1, num_bytes, stdout);
        // TODO handle write errors
    }
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

