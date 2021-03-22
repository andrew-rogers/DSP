#include "prbs1bit.h"

#include <stdio.h>
#include <math.h>

int main( int argc, char *args[] )
{
    uint32_t fs = 1536000;
    double f = 1000;
    int N=10*fs;
    DeltaSigma delta_sigma;
    if (argc > 1) {
        double duration;
        sscanf(args[1], "%lf", &duration);
        N = duration * fs;
    }
    if (argc > 2) {
        sscanf(args[2], "%lf", &f);
    }

    for( int n=0; n<N; n++ ) {
        double theta = 2 * M_PI * f * n / fs;
        int8_t val = delta_sigma.sample(4 * sin(theta) + 4);
        fwrite(&val, 1, 1, stdout);
    }
    return 0;
}

