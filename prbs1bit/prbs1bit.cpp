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

#include <stdlib.h>

namespace
{
    uint8_t bitcnt(uint8_t value)
    {
        /* Count number of bits that are set in value */
        int8_t cnt=0;
        if(value&1) cnt++;
        if(value&2) cnt++;
        if(value&4) cnt++;
        if(value&8) cnt++;
        if(value&16) cnt++;
        if(value&32) cnt++;
        if(value&64) cnt++;
        if(value&128) cnt++;
        return cnt;
    }
}

Dec8::Dec8()
{

    // Populate the LUT
    for( int i = 0; i<255; i++ ) {
        m_lut[i]=bitcnt(i);
    }
    m_lut[255]=(uint8_t)8;
}

Int8::Int8()
{
    m_rand_index = 0;
    m_rand280 = 0;

    // Populate the random number table
    for( int i = 0; i<65536; i++ ) {
        m_rand_tab[i] = rand()%256;
    }

    /* Tables of length 280 are used to store combinations with an integer
       number of repeats.

    | num bits set in byte | num combinations | 280 / num combinations |
    |          0,8         |        1         |           280          |
    |          1,7         |        8         |           35           |
    |          2,6         |        28        |           10           |
    |          3,5         |        56        |           5            |
    |          4           |        70        |           4            |
    */

    uint16_t repeat_tab[9]={280,35,10,5,4,5,10,35,280};
    uint8_t comb_cnt[9]={0,0,0,0,0,0,0,0,0};
    for( int value = 0; value<256; value++ ) {
        int8_t cnt=bitcnt( value );

        /* Get number of repeats and populate bit sequence table */
        uint16_t nr = repeat_tab[cnt];
        uint16_t step = 280/nr;
        uint8_t offset = comb_cnt[cnt];
        comb_cnt[cnt] = comb_cnt[cnt] + 1;
        for( int j = 0; j<nr; j++) m_bs_tab[cnt][j*step+offset] = value;
    }
}

