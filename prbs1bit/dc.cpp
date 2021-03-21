#include "prbs1bit.h"

#include <stdio.h>
#include <math.h>

int main( int argc, char *args[] )
{
    uint32_t fs = 768000;
    int N=10*fs;
    double dc=0.5;
    DeltaSigma delta_sigma;
    if (argc > 1) {
        double duration;
        sscanf(args[1], "%lf", &duration);
        N = duration * fs;
    }
    if (argc > 2) {
        sscanf(args[2], "%lf", &dc);
    }
    for( int n=0; n<N; n++ ) {
        int8_t val = delta_sigma.sample( dc * 8.0 );
        fwrite(&val, 1, 1, stdout);
    }
    return 0;
}

