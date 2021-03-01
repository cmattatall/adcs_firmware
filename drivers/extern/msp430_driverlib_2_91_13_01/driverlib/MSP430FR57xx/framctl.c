//*****************************************************************************
//
// framctl.c - Driver for the framctl Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup framctl_api framctl
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_FRAM_FR5XX__
#include "framctl.h"

#include <assert.h>

void FRAMCtl_write8(uint8_t *dataPtr,
    uint8_t *framPtr,
    uint16_t numberOfBytes
    )
{
    while (numberOfBytes > 0)
    {
        //Write to Fram
        *framPtr++ = *dataPtr++;
        numberOfBytes--;
    }
}

void FRAMCtl_write16(uint16_t *dataPtr,uint16_t *framPtr,
		uint16_t numberOfWords)
{
    while (numberOfWords > 0)
    {
        //Write to Fram
        *framPtr++ = *dataPtr++;
        numberOfWords--;
    }

}

void FRAMCtl_write32(uint32_t *dataPtr,uint32_t *framPtr,
		uint16_t count)
{
    while (count > 0)
    {
        //Write to Fram
        *framPtr++ = *dataPtr++;
        count--;
    }
}

void FRAMCtl_memoryFill32 (uint32_t value,
    uint32_t *framPtr,
    uint16_t count
    )
{
    while (count> 0)
    {
        //Write to Fram
        *framPtr++ = value;
        count--;
    }
}

void FRAMCtl_enableInterrupt (uint8_t interruptMask)
{

	uint16_t waitSelection;

	waitSelection=(HWREG16(FRAM_BASE + OFS_FRCTL0) & 0x00FF);
	// Clear lock in FRAM control registers
	HWREG16(FRAM_BASE + OFS_FRCTL0) = FWPW + waitSelection;

	// Enable user selected interrupt sources
    HWREG16(FRAM_BASE + OFS_GCCTL0) |= interruptMask;

}

uint8_t FRAMCtl_getInterruptStatus(uint16_t interruptFlagMask)
{
	return ( HWREG16(FRAM_BASE + OFS_GCCTL1) & interruptFlagMask );
}

void FRAMCtl_disableInterrupt(uint16_t interruptMask)
{
	uint16_t waitSelection;

	waitSelection=(HWREG16(FRAM_BASE + OFS_FRCTL0) & 0x00FF);
	//Clear lock in FRAM control registers
	HWREG16(FRAM_BASE + OFS_FRCTL0) = FWPW + waitSelection;

    HWREG16(FRAM_BASE + OFS_GCCTL0) &= ~(interruptMask);
}

void FRAMCtl_configureWaitStateControl(uint8_t manualWaitState,
		uint8_t accessTime,
		uint8_t prechargeTime )
{
	// Clear lock in FRAM control registers
	HWREG16(FRAM_BASE + OFS_FRCTL0) = FWPW;

	HWREG8(FRAM_BASE + OFS_FRCTL0_L) |= ( accessTime | manualWaitState | prechargeTime);
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for framctl_api
//! @}
//
//*****************************************************************************
