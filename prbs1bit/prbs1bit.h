/*
    PRBS 1-bit ADC and DAC
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

#ifndef PRBS1BIT_H
#define PRBS1BIT_H

#include <stdint.h>

class Dec8
{
public:
    Dec8();
    inline uint8_t sample(uint8_t x){ return m_lut[x]; }
private:
    uint8_t m_lut[256];
};

/* Takes values 0 to 8 inclusive from stdin and encodes then into a bitstream.
   When the bitstream is lowpass filtered, this could be considered as a
   9-level quantiser. Levels 1 to 7 can have different possible bitstreams so
   these are randomly chosen. */
class Int8
{
public:
    Int8();
    inline uint8_t sample(uint8_t in)
    {
        if(in>8) in=8;
        uint8_t out = m_bs_tab[in][m_rand280];
        m_rand280 = (m_rand280 + m_rand_tab[m_rand_index++]) % 280;
        return out;
    }
private:
    /* Tables are used for fast processing. */
    uint8_t m_rand_tab[65536];
    uint8_t m_bs_tab[9][280];
    
    uint16_t m_rand_index;
    uint16_t m_rand280;
};

#endif // PRBS1BIT_H

