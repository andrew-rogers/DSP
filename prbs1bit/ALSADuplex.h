/*
    ALSA Duplex Class
    Copyright (C) 2021,2022  Andrew Rogers

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

#ifndef ALSA_DUPLEX_H
#define ALSA_DUPLEX_H

#include <alsa/asoundlib.h>

class Buffer
{
public:
    Buffer( size_t capacity )
    {
        m_capacity = capacity;
        m_size = 0;
        m_buffer = new char[m_capacity];
        m_alloc = m_buffer;
    }

    Buffer( char* buffer, size_t capacity )
    {
        m_capacity = capacity;
        m_size = 0;
        m_buffer = buffer;
        m_alloc = 0;
    }

    ~Buffer()
    {
        if (m_alloc) delete[] m_alloc;
    }

    char& operator[]( size_t index)
    {
        return m_buffer[index];
    }

    size_t getCapacity()
    {
        return m_capacity;
    }

    void setSize( size_t size)
    {
        m_size = size;
    }

    size_t getSize()
    {
        return m_size;
    }

private:
    size_t m_capacity;
    size_t m_size;
    char *m_buffer;
    char *m_alloc;
};

class BufferProvider
{
public:
    virtual Buffer* getBuffer() = 0;
    virtual void release( Buffer* buf ) = 0;
};

struct device {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hw_params;
    struct pollfd fd;
};

class ALSADuplex
{
public:
    ALSADuplex( char* dev, BufferProvider& writer, BufferProvider& reader );
    ~ALSADuplex();
    int run(int playback_prefill);
private:
    int setupCaptureDevice();
    int setupPlaybackDevice();
    int playback();
    int capture( int num_frames );
    char* m_devname;
    device m_pdev;
    device m_cdev;
    BufferProvider* m_writer;
    BufferProvider* m_reader;
    bool m_done;   // Set when playback has completed.
    bool m_abort;  // Set when buffer underruns / overruns.
    int len;
    char* ptr;
    size_t m_frames_per_block;
    Buffer* m_cbuffer;
    Buffer* m_pbuffer;
};

#endif // ALSA_DUPLEX_H

