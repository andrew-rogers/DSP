#include <stdio.h>
#include <stdint.h>


void makeLUT(uint8_t *lut);

int main(int argc, char* args[])
{
    int M=50000;
    if (argc > 1) sscanf(args[1], "%d", &M);


    uint32_t sum=0;
    uint8_t in;
    int nr=fread(&in, 1, 1, stdin);
    int cnt=0;
    while( nr > 0 ) {
        sum+=in;
        nr=fread(&in, 1, 1, stdin);
        cnt++;
        if( cnt >= M) {
            printf("%d\n", sum);
            cnt -= M;
            sum=0;
        }
    }
}

