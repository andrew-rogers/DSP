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
    BufferProducerStdin( int M=16 )
    {
        m_M = M;
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
        if (m_buf==0) m_buf = new Buffer(m_M*1024);
        char *out = &(*m_buf)[0];
        int len = fread(m_in, 1, 1024, stdin);
        // TODO handle read errors
        
        const float scale=8.0/((m_M-1)*256);
        for (size_t i=0; i<len; i++)
        {
            uint32_t x = m_in[i];
            for (uint8_t cnt=0; cnt<m_M; cnt++)
            {
                float y = (m_M-1-cnt)*m_x1 + cnt*x; // Linear interpolation
                out[i*m_M+cnt] = int8.sample(ds.sample(y*scale));
            }
            m_x1 = x;
        }

        m_buf->setSize(len*m_M);

        return m_buf;
    }

    virtual void release( Buffer* buf )
    {
    }

private:
    int m_M;
    Int8 int8;
    DeltaSigma ds;
    Buffer* m_buf;
    uint8_t* m_in;
    uint32_t m_x1;
};

class BufferConsumerStdout : public BufferProvider
{
public:
    BufferConsumerStdout( int M=16 )
    {
        m_M = M;
        m_buf = new Buffer(m_M*1024);
        m_cnt = 0;
        m_sum = 0;
        m_scale = 32.0/M;
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
            if (m_cnt>=m_M)
            {
                m_sum = m_sum * m_scale;
                uint8_t y = (uint8_t)m_sum;
                if (m_sum>255) y=255;
                fwrite(&y, 1, 1, stdout);
                m_cnt=0;
                m_sum = 0;
            }
            // TODO handle write errors
        }
    }

private:
    int m_M;
    Dec8 dec8;
    Buffer* m_buf;
    size_t m_cnt;
    uint32_t m_sum;
    float m_scale;
};

int test_decimator( uint16_t M )
{
    BufferConsumerStdout reader( M );

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
    BufferProducerStdin writer( M );

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

int duplex_run( int M )
{
    BufferProducerStdin writer( M );
    BufferConsumerStdout reader( M );
    char dev[]="hw:1";
    ALSADuplex duplex( dev, writer, reader );
    int err = duplex.run(10);
    return err;
}

int main(int argc, char *argv[])
{
    int opt;
    int byterate = 12288000/8;
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

    int rate_factor = byterate/rate;
    double actual_rate = (double)byterate/rate_factor;

    fprintf(stderr, "Actual rate=%f\n", actual_rate);

    if (test=='i') return test_interleaver( rate_factor );
    if (test=='d') return test_decimator( rate_factor );

    return duplex_run( rate_factor );
}

