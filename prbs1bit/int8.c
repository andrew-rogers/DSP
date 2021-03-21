/*
    8x interpolation for 1bit DAC
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

/* Takes values 0 to 8 inclusive from stdin and encodes then into a bitstream.
   When the bitstream is lowpass filtered, this could be considered as a
   9-level quantiser. Levels 1 to 7 can have different possible bitstreams so
   these are randomly chosen. */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* Tables are used for fast processing. */
uint8_t g_rand_tab[65536];
uint8_t g_bs_tab[9][280];

void populateRandTable();
void populateBSTable();

int main(int argc, char* args[])
{
    populateRandTable();
    populateBSTable();
    uint8_t in;
    int nr=fread(&in, 1, 1, stdin);
    uint16_t rand_index = 0;
    uint16_t rand280 = 0;
    while( nr > 0 ) {
        if(in>8) in=8;
        fwrite(&g_bs_tab[in][rand280], 1, 1, stdout);
        nr=fread(&in, 1, 1, stdin);
        rand280 = (rand280 + g_rand_tab[rand_index++]) % 280;
    }
}

void populateRandTable()
{
    for( int i = 0; i<65536; i++ ) {
        g_rand_tab[i] = rand()%256;
    }
}

void populateBSTable()
{
    /* Tables of length 280 are used to store combinations with an integer
       number of repeats

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
    
        /* Count bits set in value */
        int8_t cnt=0;
        if(value&1) cnt++;
        if(value&2) cnt++;
        if(value&4) cnt++;
        if(value&8) cnt++;
        if(value&16) cnt++;
        if(value&32) cnt++;
        if(value&64) cnt++;
        if(value&128) cnt++;
        
        /* Get number of repeats and populate table */
        uint16_t nr = repeat_tab[cnt];
        uint16_t step = 280/nr;
        uint8_t offset = comb_cnt[cnt];
        comb_cnt[cnt] = comb_cnt[cnt] + 1;
        for( int j = 0; j<nr; j++) g_bs_tab[cnt][j*step+offset] = value;
    }
}

