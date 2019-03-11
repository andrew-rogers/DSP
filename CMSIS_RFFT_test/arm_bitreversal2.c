
/*
 * Copyright (C) 2010-2017 ARM Limited or its affiliates. All rights reserved.
 * Copyright (C) 2018 Andrew Rogers.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/* Translated from the CMSIS DSP Library arm_bitreversal2.S assembler language file */

#include "arm_math.h"

/*
* @brief  In-place bit reversal function.
* @param[in, out] *pSrc        points to the in-place buffer of unknown 32-bit data type.
* @param[in]      bitRevLen    bit reversal table length
* @param[in]      *pBitRevTab  points to bit reversal table.
* @return none.
*/
void arm_bitreversal_16(uint16_t * pSrc, const uint16_t bitRevLen, const uint16_t * pBitRevTable)
{

/* Assembler with c-like annotations
arm_bitreversal_32 PROC
	ADDS     r3,r1,#1
	PUSH     {r4-r6}
	ADDS     r1,r2,#0
	LSRS     r3,r3,#1

;r3=r1+1 // r3=bitRevLen+1
;r1=r2   // r1=pBitRevTable
;r3=r3/2 // r3 is used as a loop counter below

; Loop: swap complex numbers according to bit reversing table
arm_bitreversal_32_0 LABEL
	LDRH     r2,[r1,#2]
	LDRH     r6,[r1,#0]
	ADD      r2,r0,r2
	ADD      r6,r0,r6

;r2=r1[1]
;r6=r1[0]
;r2=r0+r2
;r6=r0+r6
; r2 and r6 become the addresses of the complex numbers to be swapped.

	LDR      r5,[r2,#0]
	LDR      r4,[r6,#0]
	STR      r5,[r6,#0]
	STR      r4,[r2,#0]

;r5=r2[0]
;r4=r6[0]
;r6[0]=r5
;r2[0]=r4
; Swaps the real part

	LDR      r5,[r2,#4]
	LDR      r4,[r6,#4]
	STR      r5,[r6,#4]
	STR      r4,[r2,#4]

;r5=r2[1]
;r4=r6[1]
;r6[1]=r5
;r2[1]=r4
; Swaps the imaginary part

	ADDS     r1,r1,#4
	SUBS     r3,r3,#1

;r1=r1+4 // Increment address for bit reverse table, increment is two 16-bit entries.
;r3=r3-1 // Decrement the loop conter

	BNE      arm_bitreversal_32_0
	POP      {r4-r6}
	BX       lr
	ENDP
*/

    //printf("Len %04x\n", bitRevLen);
    //for( int i=0; i<256; i++) printf("%04x %04x\n",i,pBitRevTable[i]);
    
    uint16_t temp;

    for(int i=0; i<bitRevLen; i+=2)
    {
        uint16_t index0=pBitRevTable[i]/4;
        uint16_t index1=pBitRevTable[i+1]/4;
        
        /* Swap the real part */
        temp=pSrc[index0];
        pSrc[index0]=pSrc[index1];
        pSrc[index1]=temp;
        
        /* Swap the imaginary part */
        temp=pSrc[index0+1];
        pSrc[index0+1]=pSrc[index1+1];
        pSrc[index1+1]=temp;
    }
}
