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
//  MSP430FR57xx PMM Test
// This example shows transitions between a user Reset via the reset button, a
// software triggered BOR, a previous LPMx.5 state indicator flag, and entrance 
// into LPM4.5. Reading the reset interrupt flag from PMMIFG register is equivalent
// to reading from reset vector register.
//
// Upon device power on you will see the following sequence of LED's
// X = off
// O = on
//
// _7 = On indicates BOR
// _6 = On indicates Reset (from user button)
// _5 = on indicates device was previously in LPMx.5 state before the reset
// _4 = On indicates device is NOT sleeping.  Off means device is in LPM4.5
//*******************************************************************************
//
// 	1) Power on device:
// 		X7 X6 X5 O4  			LED4 is on, device is running and was not in LPM4.5 mode previously.
//
// 	2) Press User Reset button:
//
// 		X7 O6 X5 O4				Device is running and has reset due to RST pin
// 		O7 X6 X5 O4				Device is running and has reset due to a software BOR
// 		X7 X6 X5 X4				Device is now in LPM4.5
//
//  3) Press User Reset button:
//  	X7 X6 O5 O4				Device is running and was previously in LPM4.5
//
//	4) Press User Reset button:
//		Device will restart the sequence beginning at step 2).
//
//
//********************************************************************************
#include "driverlib.h"

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);
    //Unlock the GPIO pins.
      /*
       * Base Address of PMM,
       * By default, the pins are unlocked unless waking
       * up from an LPMx.5 state in which case all GPIO
       * are previously locked.
       *
       */
    PMM_unlockLPM5();
    //Set P3.4, 3.5, 3.6, 3.7 as output pins.
    /*

     * Select Port 3
     * Set Pin 4, 5, 6, 7 as output
     */
  	GPIO_setAsOutputPin(
  			GPIO_PORT_P3,
  			GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7
  	);
    //Set P3.5, 3.6, 3.7 Low.
    /*

     * Select Port 3
     * Set Pin 5, 6, 7 as Low
     */
  	GPIO_setOutputLowOnPin(
  			GPIO_PORT_P3,
  			GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7
  	);
    //Set P3.4 as High.
    /*

     * Select Port 3
     * Set Pin 4 as High
     */
  	GPIO_setOutputHighOnPin(
  			GPIO_PORT_P3,
  			GPIO_PIN4
  	);

	  //Get Interrupt Status from the PMMIFG register.
	    /* Base Address of PMM,
	     * mask:
	     * 	PMM_BOR_INTERRUPT
	     * 	PMM_RST_INTERRUPT,
	     * 	PMM_PMMPORIFG,
	     * 	PMM_SVSLIFG,
	     * 	PMM_SVSHIFG
	     * 	PMM_LPM5_INTERRUPT,
	     * return STATUS_SUCCESS (0x01) or STATUS_FAIL (0x00)
	     */
    if (PMM_getInterruptStatus(PMM_LPM5_INTERRUPT))	// Was this device in LPMx.5 mode before the reset was triggered?
	  {
        //Clear Interrupt Flag from the PMMIFG register.
        /* Base Address of Comparator D,
         * mask:
         * 	PMM_BOR_INTERRUPT
         * 	PMM_RST_INTERRUPT,
         * 	PMM_PMMPORIFG,
         * 	PMM_SVSLIFG,
         * 	PMM_SVSHIFG
         * 	PMM_LPM5_INTERRUPT,
         * 	PMM_ALL
         */
        PMM_clearInterrupt(PMM_LPM5_INTERRUPT);			// Clear the LPMx.5 flag
      	GPIO_setOutputHighOnPin(
      			GPIO_PORT_P3,
      			GPIO_PIN5
      	);
    }

    if (PMM_getInterruptStatus(PMM_RST_INTERRUPT))	// Was this reset triggered by the Reset flag?
	  {
		    PMM_clearInterrupt(PMM_RST_INTERRUPT);			// Clear reset flag
	  	  GPIO_setOutputHighOnPin(
          GPIO_PORT_P3,
	  			GPIO_PIN6
          );
        __delay_cycles(1000000); 													// to include a visual delay.

        //Trigger a software Brown Out Reset (BOR)
        /*
         * Base Address of PMM,
         * Forces the devices to perform a BOR.
         */
        PMM_trigBOR();								// Software trigger a BOR.
    }

	  if (PMM_getInterruptStatus(PMM_BOR_INTERRUPT))	// Was this reset triggered by the BOR flag?
	  {
		    PMM_clearInterrupt(PMM_BOR_INTERRUPT);			// Clear BOR flag
	  	  GPIO_setOutputHighOnPin(
	  			GPIO_PORT_P3,
	  			GPIO_PIN7
          );

        __delay_cycles(1000000); 													// to include a visual delay.
      	GPIO_setOutputLowOnPin(
      			GPIO_PORT_P3,
      			GPIO_PIN4 + GPIO_PIN7
      	);

        //Disable SVSH
        /*
         * Base Address of PMM,
         * High-side SVS (SVSH) is disabled in LPM4.5. SVSH is
         * always enabled in active mode and LPM0/1/2/3/4 and LPM3.5.
         */
        PMM_disableSVSH();
        //Disable SVSL
        /*
         * Base Address of PMM,
         * Low-side SVS (SVSL) is disabled in low power modes.
         * SVSL is always enabled in active mode and LPM0.
         */
        PMM_disableSVSL();
        //Disable Regulator
        /*
         * Base Address of PMM,
         * Regulator is turned off when going to LPM3/4.
         * System enters LPM3.5 or LPM4.5, respectively.
         */
        PMM_turnOffRegulator();
        __bis_SR_register(LPM4_bits); // Enter LPM4.5, This automatically locks
        							  //(if not locked already) all GPIO pins.
        							  //  and will set the LPM5 flag and set the LOCKLPM5 bit
        							  //  in the PM5CTL0 register upon wake up.
    }

    while (1)
    {
    	__no_operation();             // Don't sleep
    }
}
