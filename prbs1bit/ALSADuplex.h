/*
    ALSA Duplex Class
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

#ifndef ALSA_DUPLEX_H
#define ALSA_DUPLEX_H

#include <alsa/asoundlib.h>

class PCMWriter
{
public:
    virtual int fillBuffer( char *buffer, int num_bytes ) = 0;
};

class PCMReader
{
public:
    virtual void processBuffer( char *buffer, int num_bytes ) = 0;
};

class ALSADuplex
{
public:
    ALSADuplex( PCMWriter& writer, PCMReader& reader )
    {
        m_reader = &reader;
        m_writer = &writer;
        len=0;
        ptr=0;
    } 
    void setPHandle( snd_pcm_t* pcm_handle ){ m_phandle = pcm_handle; }
    void setCHandle( snd_pcm_t* pcm_handle ){ m_chandle = pcm_handle; }
    int playback( char* buffer, int num_frames );
    int capture( char* buffer, int num_frames );
private:
    snd_pcm_t* m_phandle;
    snd_pcm_t* m_chandle;
    PCMWriter* m_writer;
    PCMReader* m_reader;
    int len;
    char* ptr;
};

#endif // ALSA_DUPLEX_H

