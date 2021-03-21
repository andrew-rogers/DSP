#include "prbs1bit.h"

#include <stdio.h>
#include <stdint.h>

int main(int argc, char* args[])
{
    Dec8 dec;
    uint8_t in;
    int nr=fread(&in, 1, 1, stdin);
    while( nr > 0 ) {
        uint8_t out=dec.sample(in);
        fwrite(&out, 1, 1, stdout);
        nr=fread(&in, 1, 1, stdin);
    }
}

