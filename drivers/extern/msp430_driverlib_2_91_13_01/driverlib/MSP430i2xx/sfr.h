//*****************************************************************************
//
// sfr.h - Driver for the SFR Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_SFR_H__
#define __MSP430WARE_SFR_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SFR__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The following are values that can be passed to the interruptMask parameter
// for functions: SFR_enableInterrupt(), and SFR_disableInterrupt(); the
// interruptFlagMask parameter for functions: SFR_getInterruptStatus(), and
// SFR_clearInterrupt() as well as returned by the SFR_getInterruptStatus()
// function.
//
//*****************************************************************************
#define SFR_NMI_PIN_INTERRUPT                                             NMIIE
#define SFR_OSCILLATOR_FAULT_INTERRUPT                                     OFIE
#define SFR_WATCHDOG_INTERRUPT                                            WDTIE
#define SFR_FLASH_ACCESS_VIOLATION_INTERRUPT                             ACCVIE
#define SFR_EXTERNAL_RESET_INTERRUPT                                     RSTIFG
#define SFR_BROWN_OUT_RESET_INTERRUPT                                    BORIFG

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Enables selected SFR interrupt sources.
//!
//! This function enables the selected SFR interrupt sources. Only the sources
//! that are enabled can be reflected to the processor interrupt; disabled
//! sources have no effect on the processor. Does not clear interrupt flags.
//!
//! \param interruptMask is the bit mask of interrupts that will be enabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b SFR_NMI_PIN_INTERRUPT - NMI pin interrupt, if NMI function is
//!           chosen
//!        - \b SFR_OSCILLATOR_FAULT_INTERRUPT - Oscillator fault interrupt
//!        - \b SFR_WATCHDOG_INTERRUPT - Watchdog interrupt
//!        - \b SFR_FLASH_ACCESS_VIOLATION_INTERRUPT - Flash access violation
//!           interrupt
//!
//! \return None
//
//*****************************************************************************
extern void SFR_enableInterrupt(uint8_t interruptMask);

//*****************************************************************************
//
//! \brief Disables selected SFR interrupt sources.
//!
//! This function disables the selected SFR interrupt sources. Only the sources
//! that are enabled can be reflected to the processor interrupt; disabled
//! sources have no effect on the processor.
//!
//! \param interruptMask is the bit mask of interrupts that will be disabled.
//!        Mask value is the logical OR of any of the following:
//!        - \b SFR_NMI_PIN_INTERRUPT - NMI pin interrupt, if NMI function is
//!           chosen
//!        - \b SFR_OSCILLATOR_FAULT_INTERRUPT - Oscillator fault interrupt
//!        - \b SFR_WATCHDOG_INTERRUPT - Watchdog interrupt
//!        - \b SFR_FLASH_ACCESS_VIOLATION_INTERRUPT - Flash access violation
//!           interrupt
//!
//! \return None
//
//*****************************************************************************
extern void SFR_disableInterrupt(uint8_t interruptMask);

//*****************************************************************************
//
//! \brief Returns the status of the selected SFR interrupt flags.
//!
//! This function returns the status of the selected SFR interrupt flags in a
//! bit mask format matching that passed into the interruptFlagMask parameter.
//!
//! \param interruptFlagMask is the bit mask of interrupt flags that the status
//!        of should be returned.
//!        Mask value is the logical OR of any of the following:
//!        - \b SFR_NMI_PIN_INTERRUPT - NMI pin interrupt, if NMI function is
//!           chosen
//!        - \b SFR_OSCILLATOR_FAULT_INTERRUPT - Oscillator fault interrupt
//!        - \b SFR_WATCHDOG_INTERRUPT - Watchdog interrupt
//!        - \b SFR_EXTERNAL_RESET_INTERRUPT - External reset interrupt
//!        - \b SFR_BROWN_OUT_RESET_INTERRUPT - Brown out reset interrupt
//!
//! \return A bit mask of the status of the selected interrupt flags.
//!         - \b SFR_NMI_PIN_INTERRUPT NMI pin interrupt, if NMI function is
//!         chosen
//!         - \b SFR_OSCILLATOR_FAULT_INTERRUPT Oscillator fault interrupt
//!         - \b SFR_WATCHDOG_INTERRUPT Watchdog interrupt
//!         - \b SFR_EXTERNAL_RESET_INTERRUPT External reset interrupt
//!         - \b SFR_BROWN_OUT_RESET_INTERRUPT Brown out reset interrupt
//!         \n indicating the status of the masked interrupts
//
//*****************************************************************************
extern uint8_t SFR_getInterruptStatus(uint8_t interruptFlagMask);

//*****************************************************************************
//
//! \brief Clears the selected SFR interrupt flags.
//!
//! This function clears the status of the selected SFR interrupt flags.
//!
//! \param interruptFlagMask is the bit mask of interrupt flags that will be
//!        cleared.
//!        Mask value is the logical OR of any of the following:
//!        - \b SFR_NMI_PIN_INTERRUPT - NMI pin interrupt, if NMI function is
//!           chosen
//!        - \b SFR_OSCILLATOR_FAULT_INTERRUPT - Oscillator fault interrupt
//!        - \b SFR_WATCHDOG_INTERRUPT - Watchdog interrupt
//!        - \b SFR_EXTERNAL_RESET_INTERRUPT - External reset interrupt
//!        - \b SFR_BROWN_OUT_RESET_INTERRUPT - Brown out reset interrupt
//!
//! \return None
//
//*****************************************************************************
extern void SFR_clearInterrupt(uint8_t interruptFlagMask);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_SFR_H__
