//*****************************************************************************
//
// pmm.c - Driver for the pmm Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup pmm_api pmm
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_PMM__
#include "pmm.h"

#include <assert.h>

void PMM_setupVoltageMonitor(uint8_t voltageMonitorLevel) {
    uint8_t currentStatus = VMONCTL;
    currentStatus &= ~(0x07);
    currentStatus |= voltageMonitorLevel;
    VMONCTL = currentStatus;
}

void PMM_calibrateReference(void) {
    REFCAL0 = HWREG8(TLV_START + TLV_CAL_REFCAL0);
    REFCAL1 = HWREG8(TLV_START + TLV_CAL_REFCAL1);
}

void PMM_setRegulatorStatus(uint8_t status) {
    uint8_t currentStatus = LPM45CTL;
    currentStatus &= ~(PMMREGOFF);
    currentStatus |= status;
    LPM45CTL = currentStatus;
}

void PMM_unlockIOConfiguration(void) {
    LPM45CTL &= ~(LOCKLPM45);
}

void PMM_enableInterrupt(uint8_t mask) {
    VMONCTL |= mask;
}

void PMM_disableInterrupt(uint8_t mask) {
    VMONCTL &= ~(mask);
}

uint8_t PMM_getInterruptStatus(uint8_t mask) {
    uint8_t result = 0x00;

    if((mask & PMM_VMON_INTERRUPT) && (VMONCTL & VMONIFG)) {
        result |= PMM_VMON_INTERRUPT;
    }

    if(mask & PMM_LPM45_INTERRUPT) {
        result |= (LPM45CTL & LPM45IFG);
    }

    return result;
}

void PMM_clearInterrupt(uint8_t mask) {
    LPM45CTL &= ~(mask);
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for pmm_api
//! @}
//
//*****************************************************************************
