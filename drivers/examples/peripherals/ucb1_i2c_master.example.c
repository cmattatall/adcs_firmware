/**
 * @file ucb1_i2c.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module demonstrating transmission of bytes to
 * device address 0x0F using I2C as a master device from UCB1
 * @version 0.1
 * @date 2021-03-19
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */

#include <msp430.h>

static unsigned char *PTxData; // Pointer to TX data
static unsigned char  TXByteCtr;

const unsigned char TxData[] = // Table of data to transmit
    {0x11, 0x22, 0x33, 0x44, 0x55};

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    /* Give control of pins to UCB1 peripheral */
    P4SEL |= BIT2;
    P4SEL |= BIT1;

    UCB1CTL1 |= UCSWRST;                   // Enable SW reset
    UCB1CTL0  = UCMST + UCMODE_3 + UCSYNC; // I2C Master, synchronous mode
    UCB1CTL1  = UCSSEL_2 + UCSWRST;        // Use SMCLK, keep SW reset
    UCB1BR0   = 12;                        // fSCL = SMCLK/12 = ~100kHz
    UCB1BR1   = 0;
    UCB1I2CSA = 0x48;     // Slave Address is 048h
    UCB1CTL1 &= ~UCSWRST; // Clear SW reset, resume operation

    UCB1IE |= UCTXIE; // Enable TX interrupt


    while (1)
    {
        while (1)
        {
            int i;
            for (i = 0; i < 10; ++i)
                ; // Delay required between bus transactions


            PTxData = (unsigned char *)TxData; // TX array start address
                                               // Place breakpoint here to see
                                               // each transmit operation.
            TXByteCtr = sizeof TxData;         // Load TX byte counter

            UCB1CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition

            __bis_SR_register(LPM0_bits + GIE); // Enter LPM0, enable interrupts
            __no_operation();                   // Remain in LPM0 until all data
                                                // is TX'd
            while (UCB1CTL1 & UCTXSTP)
                ; // Ensure stop condition got sent
        }
    }
}

__interrupt_vec(USCI_B1_VECTOR) void USCI_I2C_ISR(void)
{
    switch (__even_in_range(UCB1IV, 12))
    {
        case 0:
            break; // Vector  0: No interrupts
        case 2:
            break; // Vector  2: ALIFG
        case 4:
            break; // Vector  4: NACKIFG
        case 6:
            break; // Vector  6: STTIFG
        case 8:
            break; // Vector  8: STPIFG
        case 10:
            break;         // Vector 10: RXIFG
        case 12:           // Vector 12: TXIFG
            if (TXByteCtr) // Check TX byte counter
            {
                UCB1TXBUF = 0x0C; //*PTxData++;               // Load TX buffer
                TXByteCtr--;      // Decrement TX byte counter
            }
            else
            {
                UCB1CTL1 |= UCTXSTP; // I2C stop condition
                UCB1IFG &= ~UCTXIFG; // Clear USCI_B0 TX int flag
                __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
            }
        default:
            break;
    }
}
