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

#include "prbs1bit.h"

Dec8::Dec8()
{

    // Populate the LUT
    for( int i = 0; i<255; i++ ) {
        int8_t cnt=0;
        if(i&1) cnt++;
        if(i&2) cnt++;
        if(i&4) cnt++;
        if(i&8) cnt++;
        if(i&16) cnt++;
        if(i&32) cnt++;
        if(i&64) cnt++;
        if(i&128) cnt++;
        m_lut[i]=cnt;
    }
    m_lut[255]=(uint8_t)8;
}

