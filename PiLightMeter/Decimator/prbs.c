#include <stdio.h>
#include <stdint.h>

int main(int argc, char* args[])
{
    // dither=[0 0 1 1 0 1 0 0 0 1 1 0 1 1 1 1 1 1 0 0 1 0 0 0]
    //   in hex    3  |    4  |   6   |   f   |   c   |   8
    uint8_t seq[]={0x34, 0x6f, 0xc8};
    int seq_len=sizeof(seq);
    
    int N=500000;
    if (argc > 1) sscanf(args[1], "%d", &N);
    
    for( int n=0; n<N; n++ ) {
        fwrite(&seq[n%seq_len], 1, 1, stdout);
    }
}

