#include "prbs1bit.h"
#include "../CIC/cic.h"

#include <stdio.h>
#include <stdint.h>

int main(int argc, char* args[])
{
    // Rate 16 for 96kHz input sample rate
    // TODO allow rate to be specified on command line.
    // TODO support signed sample values and add DC offset so that zero is centred in DAC range.
    const size_t rate = 16;
    const size_t block_size = 128;

    // Allocate buffers
    uint8_t in[ block_size ];
    int32_t cic_out[ block_size * rate ];
    int8_t out[ block_size * rate ];

    CICInt3<uint8_t> cic(rate);
    DeltaSigma delta_sigma;

    float cic_max = 255.0*cic.gain();
    float gain = 8.0/cic_max;

    int nr=fread( &in, 1, block_size, stdin) ;
    while( nr > 0 ) {
        cic( in, cic_out, nr );
        for( size_t i=0; i<(nr * rate); i++) out[i] = delta_sigma.sample(cic_out[i]*gain);
        fwrite( &out, 1, nr * rate, stdout );
        nr=fread( &in, 1, block_size, stdin );
    }
}

