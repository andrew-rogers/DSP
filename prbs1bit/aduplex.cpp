/*
    PiLightMeter - ALSA Duplex using non-blocking and poll
    Copyright (C) 2020,2022  Andrew Rogers

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

class BufferProducerStdin : public BufferProvider
{
public:
    BufferProducerStdin()
    {
        m_buf = 0;
    }

    ~BufferProducerStdin()
    {
        if (m_buf) delete m_buf;
    }

    virtual Buffer* getBuffer()
    {
        if (m_buf==0) m_buf = new Buffer(16384);
        int len = fread(&(*m_buf)[0], 1, 16384, stdin);
        // TODO handle read errors

        m_buf->setSize(len);

        return m_buf;
    }

    virtual void release( Buffer* buf )
    {
    }

private:
    Buffer* m_buf;
};

class BufferConsumerStdout : public BufferProvider
{
public:
    BufferConsumerStdout()
    {
        m_buf = new Buffer(16384);
    }

    ~BufferConsumerStdout()
    {
        if (m_buf) delete m_buf;
    }

    virtual Buffer* getBuffer()
    {
        return m_buf;
    }

    virtual void release( Buffer* buf )
    {
        fwrite(&(*buf)[0], 1, buf->getSize(), stdout);
        // TODO handle write errors
    }

private:
    Buffer* m_buf;
};

int main (int argc, char *args[])
{
    BufferProducerStdin writer;
    BufferConsumerStdout reader;
    char dev[]="hw:1";
    ALSADuplex duplex( dev, writer, reader );
    int err = duplex.run(10);
    return err;
}

