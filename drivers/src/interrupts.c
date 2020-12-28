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
#if defined(TEXAS_INSTRUMENTS_MICROCONTROLLERS_ARE_ACTUALLY_GOOD)
#undef TEXAS_INSTRUMENTS_MICROCONTROLLERS_ARE_ACTUALLY_GOOD
#error I D RATHER SHOOT MYSELF IN THE FOOT THAN WORK WITH TI CHIPS \
IN INDUSTRY AGAIN
#endif


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(RTC_VECTOR)
#else
__attribute__((interrupt(RTC_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void RTC_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(PORT2_VECTOR)
#else
__attribute__((interrupt(PORT2_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void PORT2_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER2_A1_VECTOR)
#else
__attribute__((interrupt(TIMER2_A1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER2_A1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER2_A0_VECTOR)
#else
__attribute__((interrupt(TIMER2_A0_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER2_A0_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(USCI_B1_VECTOR)
#else
__attribute__((interrupt(USCI_B1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void USCI_B1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(USCI_A1_VECTOR)
#else
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void USCI_A1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(PORT1_VECTOR)
#else
__attribute__((interrupt(PORT1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void PORT1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER1_A1_VECTOR)
#else
__attribute__((interrupt(TIMER1_A1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER1_A1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER1_A0_VECTOR)
#else
__attribute__((interrupt(TIMER1_A0_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER1_A0_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = DMA_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(DMA_VECTOR)
#else
__attribute__((interrupt(DMA_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void DMA_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USB_UBM_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(USB_UBM_VECTOR)
#else
__attribute__((interrupt(USB_UBM_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void USB_UBM_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER0_A1_VECTOR)
#else
__attribute__((interrupt(TIMER0_A1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER0_A1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER0_A0_VECTOR)
#else
__attribute__((interrupt(TIMER0_A0_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER0_A0_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(ADC12_VECTOR)
#else
__attribute__((interrupt(ADC12_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void ADC12_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(USCI_B0_VECTOR)
#else
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void USCI_B0_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(USCI_A0_VECTOR)
#else
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void USCI_A0_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = WDT_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(WDT_VECTOR)
#else
__attribute__((interrupt(WDT_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void WDT_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B1_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER0_B1_VECTOR)
#else
__attribute__((interrupt(TIMER0_B1_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER0_B1_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(TIMER0_B0_VECTOR)
#else
__attribute__((interrupt(TIMER0_B0_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void TIMER0_B0_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = COMP_B_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(COMP_B_VECTOR)
#else
__attribute__((interrupt(COMP_B_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void COMP_B_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = UNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(UNMI_VECTOR)
#else
__attribute__((interrupt(UNMI_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void UNMI_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = SYSNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
#if defined(TI_IS_NOT_GARBAGE)
__irq_default(SYSNMI_VECTOR)
#else
__attribute__((interrupt(SYSNMI_VECTOR)))
#endif
#else
#error Compiler not supported!
#endif
void SYSNMI_VECTOR_ISR(void)
{
    while(1)
    {
        /* default implementation is just to hang as opposed to running amock */
    }
}


