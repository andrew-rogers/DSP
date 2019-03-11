#include "arm_math.h"

#include <iostream>

using namespace std;

#ifdef   __cplusplus
extern "C"
{
#endif

// This would go in arm_math.h if/when part of CMSIS
void arm_rfft_inplace_q15(
  const arm_rfft_instance_q15 * S,
  q15_t * pSrc);

void arm_split_rfft_q15(
    q15_t * pSrc,
    uint32_t fftLen,
    q15_t * pATable,
    q15_t * pBTable,
    q15_t * pDst,
    uint32_t modifier);

void arm_split_rifft_q15(
    q15_t * pSrc,
    uint32_t fftLen,
    q15_t * pATable,
    q15_t * pBTable,
    q15_t * pDst,
    uint32_t modifier);

void arm_split_rfft_inplace_q15(
    q15_t * pSrc,
    uint32_t fftLen,
    q15_t * pATable,
    q15_t * pBTable,
    uint32_t modifier);

void arm_split_rifft_inplace_q15(
    q15_t * pSrc,
    uint32_t fftLen,
    q15_t * pATable,
    q15_t * pBTable,
    uint32_t modifier);

#ifdef   __cplusplus
}
#endif

void split_test();
void inplace_test(int inv);
bool compare(q15_t* a, q15_t* b, int N);

bool check(const arm_rfft_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pGot);

int main(int argc, char *args[])
{
    int N=256;
    q15_t src[N];
    q15_t dst[N*2];
    arm_rfft_instance_q15 rfft;

    for( int i=0; i<N; i++) src[i]=0;

    src[0]=0x4000;
    src[4]=0x2000;

    arm_status status=arm_rfft_init_q15( &rfft, N, 0 ,1);
    arm_rfft_q15( &rfft, src, dst);

    for(int i=0; i<N; i++) cout << dst[2*i] << " " << dst[2*i+1] << "; ";
    cout << endl << endl;

    arm_rfft_init_q15( &rfft, N, 1 ,1);
    arm_rfft_q15( &rfft, dst, src);
    for(int i=0; i<N/2; i++) cout << src[2*i] << " " << src[2*i+1] << " ";
    cout << endl << endl;

    split_test();
    inplace_test(0);
    inplace_test(1);

    return 0;
}

void split_test()
{
    int N=32;
    q15_t in[N];
    q15_t vec1[N];
    q15_t vec2[N*2];
    arm_rfft_instance_q15 rfft;

    // Initialise the input
    for( int i=0; i<N; i++){
        in[i]=100*i;
    }
    in[0]=0x4000;
    in[4]=0x2000;
    in[5]=0x2025;

    // Do the CMSIS RFFT split
    arm_rfft_init_q15( &rfft, N, 0 ,1);
    uint32_t L2 = rfft.fftLenReal >> 1;
    arm_split_rfft_q15(in, L2, rfft.pTwiddleAReal,
                            rfft.pTwiddleBReal, vec2, rfft.twidCoefRModifier);
    vec2[1]=vec2[N];

    // Copy input
    for( int i=0; i<N; i++){
        vec1[i] = in[i];
    }

    // Do the new in-place RFFT spit
    arm_split_rfft_inplace_q15(vec1, L2, rfft.pTwiddleAReal,
                            rfft.pTwiddleBReal, rfft.twidCoefRModifier);

    cout << endl << "RFFT split test:" << endl;
    compare(vec2, vec1, N);



    // Do the CMSIS inverse RFFT split
    vec2[1]=0;
    arm_rfft_init_q15( &rfft, N, 1 ,1);
    arm_split_rifft_q15(vec2, L2, rfft.pTwiddleAReal,
                            rfft.pTwiddleBReal, vec1, rfft.twidCoefRModifier);

    // Do the new in-place inverse RFFT split
    vec2[1]=vec2[N];
    arm_split_rifft_inplace_q15(vec2, L2, rfft.pTwiddleAReal,
                            rfft.pTwiddleBReal, rfft.twidCoefRModifier);

    cout << endl << "Inverse RFFT split test:" << endl;
    compare(vec1,vec2,N);
}

void inplace_test(int inv)
{
    int N=32;
    int Nsrc=N;
    int Ndst=N*2;

    if(inv){
        Nsrc=N*2;
        Ndst=N;
    }

    q15_t src[Nsrc];
    arm_rfft_instance_q15 rfft;

    for( int i=0; i<Nsrc; i++){
        src[i]=100*i;
    }

    src[0]=0x4000;
    src[4]=0x2000;
    src[5]=0x2025;

    if(inv){
        src[1]=0;
        src[N]=0;
        src[N+1]=0;
        cout << endl << "In-place inverse RFFT test:" << endl;
    }
    else
    {
        cout << endl << "In-place RFFT test:" << endl;
    }

    // Copy the src as CMSIS RFFT overwrites src!
    q15_t src1[Nsrc];
    for( int i=0; i<Nsrc; i++){
        src1[i]=src[i];
    }

    // Do the CMSIS RFFT
    arm_status status=arm_rfft_init_q15( &rfft, N, inv ,1);
    q15_t dst[Ndst]; // CMSIS RFFT also computes conjugates, requires twice the RAM.
    arm_rfft_q15( &rfft, src, dst); // Modifies src and dst

    // Do the new in-place RFFT
    arm_rfft_inplace_q15( &rfft, src1);

    // Compare CMSIS RFFT with new in-place RFFT

    compare(dst,src1,N);
}

bool compare(q15_t* a, q15_t* b, int N)
{
    bool pass(true);
    for( int i=0; i<N; i++ )
    {
        char err(' ');
        if(a[i] != b[i]){
            err='*';
            pass=false;
        }
        cout << err << " " << i << "\t" << a[i] << "\t" << b[i] << endl;
    }
    return pass;
}

