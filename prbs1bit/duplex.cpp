/*
    PRBS 1-bit DAC and ADC
    Copyright (C) 2022  Andrew Rogers

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

/* Support configurable sample rate conversion using command line options.
*/

#include "ALSADuplex.h"
#include "prbs1bit.h"

#include <stdio.h>
#include <unistd.h>

class BufferProducerStdin : public BufferProvider
{
public:
    BufferProducerStdin()
    {
        m_buf = 0;
        m_in = new uint8_t[1024];
        m_x1 = 0;
    }

    ~BufferProducerStdin()
    {
        if (m_buf) delete m_buf;
        delete[] m_in;
    }

    virtual Buffer* getBuffer()
    {
        if (m_buf==0) m_buf = new Buffer(16384);
        char *out = &(*m_buf)[0];
        int len = fread(m_in, 1, 1024, stdin);
        // TODO handle read errors
        
        const float scale=8.0/(15.0*256);
        for (size_t i=0; i<len; i++)
        {
            uint16_t x = m_in[i];
            for (uint8_t cnt=0; cnt<16; cnt++)
            {
                float y = (15-cnt)*m_x1 + cnt*x; // Linear interpolation
                out[i*16+cnt] = int8.sample(ds.sample(y*scale));
            }
            m_x1 = x;
        }

        m_buf->setSize(len*16);

        return m_buf;
    }

    virtual void release( Buffer* buf )
    {
    }

private:
    Int8 int8;
    DeltaSigma ds;
    Buffer* m_buf;
    uint8_t* m_in;
    uint16_t m_x1;
};

class BufferConsumerStdout : public BufferProvider
{
public:
    BufferConsumerStdout()
    {
        m_buf = new Buffer(16384);
        m_cnt = 0;
        m_sum = 0;
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
        for (size_t i=0; i<buf->getSize(); i++)
        {
            m_sum += dec8.sample((*buf)[i]);
            m_cnt++;
            if (m_cnt>=16)
            {
                if (m_sum<128) m_sum *= 2;
                else m_sum=255;
                fwrite(&m_sum, 1, 1, stdout);
                m_cnt=0;
                m_sum = 0;
            }
            // TODO handle write errors
        }
    }

private:
    Dec8 dec8;
    Buffer* m_buf;
    size_t m_cnt;
    uint8_t m_sum;
};

int test_decimator( uint16_t M )
{
    BufferConsumerStdout reader;

    while(1)
    {
        Buffer* cbuffer = reader.getBuffer();
        char* buffer = &(*cbuffer)[0];
        size_t nr=fread(buffer, 1, cbuffer->getCapacity(), stdin);
        if (nr > 0) {
            cbuffer->setSize(nr);
            reader.release(cbuffer);
        }
        else break;
    }
    return 0;
}

int test_interleaver( uint16_t M )
{
    BufferProducerStdin writer;

    Buffer* buffer = 0;
    int len;
    char* ptr;

    while(1)
    {
        if (buffer) writer.release(buffer);
        buffer = writer.getBuffer();
        len = buffer->getSize();
        ptr = &(*buffer)[0];
        fwrite( ptr, 1, len, stdout);
        if ( len <= 0 ) break;
    }
    return 0;
}

int simple96000()
{
    BufferProducerStdin writer;
    BufferConsumerStdout reader;
    char dev[]="hw:1";
    ALSADuplex duplex( dev, writer, reader );
    int err = duplex.run(10);
    return err;
}

int main(int argc, char *argv[])
{
    int opt;
    int bitrate = 12288000;
    int rate = 96000;
    char test = 'n';
    while ((opt = getopt(argc, argv, "r:t:")) != -1) {
        switch (opt) {
        case 'r':
            rate = atoi(optarg);
            break;
        case 't':
            test = optarg[0];
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-r rate] [-t <d|i>]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
       }
    }

    int rate_factor = bitrate/rate;
    double actual_rate = (double)bitrate/rate_factor;

    fprintf(stderr, "Actual rate=%f\n", actual_rate);

    if (test=='i') return test_interleaver( rate_factor );
    if (test=='d') return test_decimator( rate_factor );

    // For now, ignore options and just run simple 96000 until the bufferring for rate conversion is understood.
    return simple96000();
}

