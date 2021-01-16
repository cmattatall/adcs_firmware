/**
 * @file interrupts.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to populate default interrupts
 * @version 0.1
 * @date 2020-12-28
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include <msp430.h>

#include "attributes.h"

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

/* clang-format off */

/* Okay since TI is hot fucking garbage, we aren't allowed to provide 
 * weak implementations of interrupts. Because of that, if we don't catch
 * a cpu interrupt (as opposed to a device interrupt), then if we haven't 
 * implemented an IRQ for it, aparently TI just wants the processor to 
 * branch to whatever garbage address is currently sitting in the memory
 * map of the vector table....
 * 
 * Great design TI...
 * 
 * And whats even worse, this isn't even freaking conformant to the GNU_C
 * standard, BUT THE COMPILER USES __GNU_C__ TO IDENTIFY ITSELFFF
 * 
 * FFFFFSSSSSS
 * 
 * 
 * What is SUPPOSED to happen, in any sane compiler, is that interrupts can
 * be made weak as a default implementation.
 * 
 * EG:
 * 
 *      .PUB WEAK BUSFAULT_IRQ:
 *          BX BUSFAULT_IRQ
 * 
 * That way, user can override the default implementation and faults don't
 * cause the entire system to break...
 * 
 * Next time, PLEASE PLEASE PLEASE PLEASE use an arm cortex M/R or safety 
 * rated processor with good software support. Not this vendor locked in 
 * garbage from 1978
 * 
 * - Carl
 *
 */

#if 0


__attribute__((used, interrupt(RTC_VECTOR))) void RTC_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}



__attribute__((used, interrupt(PORT2_VECTOR))) void PORT2_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER2_A1_VECTOR))) void TIMER2_A1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER2_A0_VECTOR))) void TIMER2_A0_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(USCI_B1_VECTOR))) void USCI_B1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(USCI_A1_VECTOR))) void USCI_A1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(PORT1_VECTOR))) void PORT1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER1_A1_VECTOR))) void TIMER1_A1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER1_A0_VECTOR))) void TIMER1_A0_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(DMA_VECTOR))) void DMA_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(USB_UBM_VECTOR))) void USB_UBM_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER0_A1_VECTOR))) void TIMER0_A1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER0_A0_VECTOR))) void TIMER0_A0_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(ADC12_VECTOR))) void ADC12_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(USCI_B0_VECTOR))) void USCI_B0_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(USCI_A0_VECTOR))) void USCI_A0_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(WDT_VECTOR))) void WDT_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER0_B1_VECTOR))) void TIMER0_B1_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(TIMER0_B0_VECTOR))) void TIMER0_B0_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(COMP_B_VECTOR))) void COMP_B_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}





__attribute__((used, interrupt(UNMI_VECTOR))) void UNMI_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}


__attribute__((used, interrupt(SYSNMI_VECTOR))) void SYSNMI_IRQ(void)
{
    while(1)
    {
        /* do nothing */
    }
}

#endif