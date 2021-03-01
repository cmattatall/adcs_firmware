//*****************************************************************************
//
// wdt.h - Driver for the WDT Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_WDT_H__
#define __MSP430WARE_WDT_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_WDT__

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
// The following are values that can be passed to the clockSelect parameter for
// functions: WDT_initWatchdogTimer(), and WDT_initIntervalTimer().
//
//*****************************************************************************
#define WDT_CLOCKSOURCE_SMCLK                                            (0x00)
#define WDT_CLOCKSOURCE_ACLK                                          (WDTSSEL)

//*****************************************************************************
//
// The following are values that can be passed to the clockDivider parameter
// for functions: WDT_initWatchdogTimer(), and WDT_initIntervalTimer().
//
//*****************************************************************************
#define WDT_CLOCKDIVIDER_32K                                             (0x00)
#define WDT_CLOCKDIVIDER_8192                                          (WDTIS0)
#define WDT_CLOCKDIVIDER_512                                           (WDTIS1)
#define WDT_CLOCKDIVIDER_64                                   (WDTIS0 | WDTIS1)

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Holds the Watchdog Timer.
//!
//! This function stops the watchdog timer from running, that way no interrupt
//! or PUC is asserted.
//!
//! \param baseAddress is the base address of the WDT module.
//!
//! \return None
//
//*****************************************************************************
extern void WDT_hold(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Starts the Watchdog Timer.
//!
//! This function starts the watchdog timer functionality to start counting
//! again.
//!
//! \param baseAddress is the base address of the WDT module.
//!
//! \return None
//
//*****************************************************************************
extern void WDT_start(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Resets the timer counter of the Watchdog Timer.
//!
//! This function resets the watchdog timer to 0x0000h.
//!
//! \param baseAddress is the base address of the WDT module.
//!
//! \return None
//
//*****************************************************************************
extern void WDT_resetTimer(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Sets the clock source for the Watchdog Timer in watchdog mode.
//!
//! This function sets the watchdog timer in watchdog mode, which will cause a
//! PUC when the timer overflows. When in the mode, a PUC can be avoided with a
//! call to WDT_resetTimer() before the timer runs out.
//!
//! \param baseAddress is the base address of the WDT module.
//! \param clockSelect is the clock source that the watchdog timer will use.
//!        Valid values are:
//!        - \b WDT_CLOCKSOURCE_SMCLK [Default]
//!        - \b WDT_CLOCKSOURCE_ACLK
//!        \n Modified bits are \b WDTSSEL of \b WDTCTL register.
//! \param clockDivider is the divider of the clock source, in turn setting the
//!        watchdog timer interval.
//!        Valid values are:
//!        - \b WDT_CLOCKDIVIDER_32K [Default]
//!        - \b WDT_CLOCKDIVIDER_8192
//!        - \b WDT_CLOCKDIVIDER_512
//!        - \b WDT_CLOCKDIVIDER_64
//!        \n Modified bits are \b WDTIS and \b WDTHOLD of \b WDTCTL register.
//!
//! \return None
//
//*****************************************************************************
extern void WDT_initWatchdogTimer(uint16_t baseAddress,
                                  uint8_t clockSelect,
                                  uint8_t clockDivider);

//*****************************************************************************
//
//! \brief Sets the clock source for the Watchdog Timer in timer interval mode.
//!
//! This function sets the watchdog timer as timer interval mode, which will
//! assert an interrupt without causing a PUC.
//!
//! \param baseAddress is the base address of the WDT module.
//! \param clockSelect is the clock source that the watchdog timer will use.
//!        Valid values are:
//!        - \b WDT_CLOCKSOURCE_SMCLK [Default]
//!        - \b WDT_CLOCKSOURCE_ACLK
//!        \n Modified bits are \b WDTSSEL of \b WDTCTL register.
//! \param clockDivider is the divider of the clock source, in turn setting the
//!        watchdog timer interval.
//!        Valid values are:
//!        - \b WDT_CLOCKDIVIDER_32K [Default]
//!        - \b WDT_CLOCKDIVIDER_8192
//!        - \b WDT_CLOCKDIVIDER_512
//!        - \b WDT_CLOCKDIVIDER_64
//!        \n Modified bits are \b WDTIS and \b WDTHOLD of \b WDTCTL register.
//!
//! \return None
//
//*****************************************************************************
extern void WDT_initIntervalTimer(uint16_t baseAddress,
                                  uint8_t clockSelect,
                                  uint8_t clockDivider);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_WDT_H__
