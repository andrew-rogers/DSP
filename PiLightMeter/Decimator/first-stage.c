#include <stdio.h>
#include <stdint.h>


void makeLUT(uint8_t *lut);

int main(int argc, char* args[])
{
    uint8_t lut[256];
    makeLUT(lut);
    uint8_t in;
    int nr=fread(&in, 1, 1, stdin);
    while( nr > 0 ) {
        fwrite(&lut[in], 1, 1, stdout);
        nr=fread(&in, 1, 1, stdin);
    }
}

void makeLUT(uint8_t *lut)
{
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
        lut[i]=cnt;
    }
    lut[255]=(uint8_t)8;
}
