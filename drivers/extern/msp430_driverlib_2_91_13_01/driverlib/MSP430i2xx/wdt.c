//*****************************************************************************
//
// wdt.c - Driver for the wdt Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup wdt_api wdt
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_WDT__
#include "wdt.h"

#include <assert.h>

void WDT_hold (uint16_t baseAddress)
{
    // Set Hold bit
    uint8_t newWDTStatus =
        ((HWREG16(baseAddress + OFS_WDTCTL) & 0x00FF) | WDTHOLD);

    HWREG16(baseAddress + OFS_WDTCTL) = WDTPW + newWDTStatus;
}

void WDT_start (uint16_t baseAddress)
{
    // Reset Hold bit
    uint8_t newWDTStatus =
        ((HWREG16(baseAddress + OFS_WDTCTL) & 0x00FF) & ~(WDTHOLD));

    HWREG16(baseAddress + OFS_WDTCTL) = WDTPW + newWDTStatus;
}

void WDT_resetTimer (uint16_t baseAddress)
{
    // Set Counter Clear bit
    uint8_t newWDTStatus =
        ((HWREG16(baseAddress + OFS_WDTCTL) & 0x00FF) | WDTCNTCL);

    HWREG16(baseAddress + OFS_WDTCTL) = WDTPW + newWDTStatus;
}

void WDT_initWatchdogTimer (uint16_t baseAddress,
    uint8_t clockSelect,
    uint8_t clockDivider)
{
    HWREG16(baseAddress + OFS_WDTCTL) =
        WDTPW + WDTCNTCL + WDTHOLD + clockSelect + clockDivider;
}

void WDT_initIntervalTimer (uint16_t baseAddress,
    uint8_t clockSelect,
    uint8_t clockDivider)
{
    HWREG16(baseAddress + OFS_WDTCTL) =
        WDTPW + WDTCNTCL + WDTHOLD + WDTTMSEL + clockSelect + clockDivider;
}


#endif
//*****************************************************************************
//
//! Close the doxygen group for wdt_api
//! @}
//
//*****************************************************************************
