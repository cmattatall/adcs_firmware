//*****************************************************************************
//
// cs.c - Driver for the cs Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup cs_api cs
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_CS__
#include "cs.h"

#include <assert.h>

//*****************************************************************************
//
// DCO Frequency used for clock signal frequency calculations
//
//*****************************************************************************
#define CS_DCO_FREQ                                                    16384000

void CS_setupDCO(uint8_t mode) {
    CSCTL0 &= ~(DCOR | DCOBYP);
    CSCTL0 |= mode;

    // Set DCO calibration using TLV
    if(mode == CS_INTERNAL_RESISTOR) {
        CSIRFCAL = HWREG8(TLV_START + TLV_CAL_CSIRFCAL);
        CSIRTCAL = HWREG8(TLV_START + TLV_CAL_CSIRTCAL);
    } else if(mode == CS_EXTERNAL_RESISTOR) {
        CSERFCAL = HWREG8(TLV_START + TLV_CAL_CSERFCAL);
        CSERTCAL = HWREG8(TLV_START + TLV_CAL_CSERTCAL);
    }
}

void CS_initClockSignal(uint8_t clockSource, uint8_t clockSourceDivider) {

    uint8_t clearBits = DIVM_7;

    if(clockSource == CS_SMCLK) {
        clockSourceDivider = clockSourceDivider << 4;
        clearBits = clearBits << 4;
    }

    CSCTL1 = CSCTL1 & ~(clearBits) | clockSourceDivider;
}

uint32_t CS_getACLK(void) {
    return 32000;
}

uint32_t CS_getSMCLK(void) {

    uint8_t divider = (CSCTL1 & 0x70) >> 4;
    return CS_DCO_FREQ >> divider;
}

uint32_t CS_getMCLK(void) {

    uint8_t divider = (CSCTL1 & 0x07);
    return CS_DCO_FREQ >> divider;
}
uint8_t CS_getFaultFlagStatus(uint8_t mask) {
    return (CSCTL0 & mask);
}


#endif
//*****************************************************************************
//
//! Close the doxygen group for cs_api
//! @}
//
//*****************************************************************************
