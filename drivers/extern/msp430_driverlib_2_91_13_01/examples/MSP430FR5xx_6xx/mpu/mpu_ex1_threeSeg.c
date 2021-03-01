/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR59x Demo - MPU Write protection violation - Interrupt notification
//
//  Description: The MPU segment boundaries are defined by:
//  Border 1 = 0x6000 [MPUSEGB1 = 0x0600]
//  Border 2 = 0x8000 [MPUSEGB2 = 0x0800]
//  Segment 1 = 0x4400 - 0x5FFF
//  Segment 2 = 0x6000 - 0x7FFF
//  Segment 3 = 0x8000 - 0x13FFF
//  Segment 2 is write protected. Any write to an address in the segment 2 range
//  causes a PUC. The LED toggles after accessing SYS NMI ISR.
//
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//
//           MSP430FR5969
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |           P1.0|-->LED
//
//******************************************************************************

#include "driverlib.h"

uint8_t SYSNMIflag = 0;
uint16_t *ptr = 0;
uint16_t Data =0;

void main(void)
{
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);

    // Configure P1.0 for LED
    GPIO_setAsOutputPin(
            GPIO_PORT_P1,
            GPIO_PIN0);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    // Configure MPU
    MPU_initThreeSegmentsParam param = {0};
    param.seg1boundary = 0x0600;
    param.seg2boundary = 0x0800;
    param.seg1accmask = MPU_READ|MPU_WRITE|MPU_EXEC;
    param.seg2accmask = MPU_READ|MPU_EXEC;
    param.seg3accmask = MPU_READ|MPU_WRITE|MPU_EXEC;
    MPU_initThreeSegments(MPU_BASE, &param);
  
    MPU_disablePUCOnViolation(MPU_BASE,MPU_SECOND_SEG);

    MPU_enableNMIevent(MPU_BASE);
    MPU_start(MPU_BASE);

    Data = 0x88;

    // Cause an MPU violation by writing to segment 2+
    ptr = (uint16_t  *)0x6002;
    *ptr = Data;

    __delay_cycles(100);

    while(SYSNMIflag)
    {
      GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
        );
      __delay_cycles(100000);
    }

    // No violation - trap here
    while(1);
}

// Sys NMI vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SYSNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(SYSNMI_VECTOR)))
#endif
void SYSNMI_ISR(void)
{
    switch(__even_in_range(SYSSNIV,0x18))
    {
        case 0x00: break;
        case 0x02: break;
        case 0x04: break;
        case 0x06: break;
        case 0x08: break;
        case 0x0A: break;
        case 0x0C: break;                       //MPUSEG1IFG
        case 0x0E:                              //MPUSEG2IFG
            // Clear violation interrupt flag
            SYSNMIflag = 1;                      // Set flag
            break;
        case 0x10: break;                       // MPUSEG3IFG
        case 0x12: break;
        case 0x14: break;
        case 0x16: break;
        case 0x18: break;
        default: break;
    }
}
