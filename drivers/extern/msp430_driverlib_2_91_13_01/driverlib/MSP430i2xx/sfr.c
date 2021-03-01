//*****************************************************************************
//
// sfr.c - Driver for the sfr Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup sfr_api sfr
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SFR__
#include "sfr.h"

#include <assert.h>

void SFR_enableInterrupt (uint8_t interruptMask)
{
    HWREG8(SFR_BASE + OFS_SFRIE1_L) |= interruptMask;
}

void SFR_disableInterrupt (uint8_t interruptMask)
{
    HWREG8(SFR_BASE + OFS_SFRIE1_L) &= ~(interruptMask);
}

uint8_t SFR_getInterruptStatus (uint8_t interruptFlagMask)
{
    return ( HWREG8(SFR_BASE + OFS_SFRIFG1_L) & interruptFlagMask );
}

void SFR_clearInterrupt (uint8_t interruptFlagMask)
{
    HWREG8(SFR_BASE + OFS_SFRIFG1_L) &= ~(interruptFlagMask);
}


#endif
//*****************************************************************************
//
//! Close the doxygen group for sfr_api
//! @}
//
//*****************************************************************************
