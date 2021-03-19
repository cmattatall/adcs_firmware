/**
 * @file ucb1_i2c.example.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module demonstrating transmission of bytes to
 * device address 0x48 using I2C as a master device from UCB1
 * @version 0.1
 * @date 2021-03-19
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 *
 * PINOUT:
 *              3.3V
 *                |
 *             10Kohm
 *                |
 * P4.1 (SDA) ----+---------- SDA OF SLAVE DEVICE
 *
 *              3.3V
 *                |
 *             10Kohm
 *                |
 * P4.2 (SCL)-----+---------  SCL OF SLAVE DEVICE
 */

#include <msp430.h>
#include <stdint.h>

#define UCMODE_I2C (UCMODE_3)
#define SLAVE_DEVICE_ADDR (0x48u);

static unsigned char *PTxData; // Pointer to TX data
static unsigned char  TXByteCtr;

// data to transmit (values that are easy to see on scope)
const unsigned char TxData[] = {0x11, 0x22, 0x33, 0x44, 0x55};

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    /* Give control of pins to UCB1 peripheral */
    P4SEL |= BIT2;
    P4SEL |= BIT1;

    UCB1CTL1 |= UCSWRST;                     // Enable SW reset
    UCB1CTL0  = UCMST + UCMODE_I2C + UCSYNC; // I2C Master, synchronous mode
    UCB1CTL1  = UCSSEL_2 + UCSWRST;          // Use SMCLK, keep SW reset
    UCB1BR0   = 12;                          // fSCL = SMCLK/12 = ~100kHz
    UCB1BR1   = 0;
    UCB1I2CSA = SLAVE_DEVICE_ADDR; // Slave Address is 048h
    UCB1CTL1 &= ~UCSWRST;          // Clear SW reset, resume operation

    UCB1IE |= UCTXIE; // Enable TX interrupt

    __bis_SR_register(GIE); // Enter LPM0, enable interrupts
    __no_operation();       /* more fixes for silicon errata, see user guide */
    while (1)
    {
        while (1)
        {
            volatile int i;
            for (i = 0; i < 100; ++i)
                ; // Delay required between bus transactions

            // TX array start address
            // Place breakpoint here to see
            // each transmit operation.
            PTxData = TxData;

            TXByteCtr = sizeof(TxData); // Load TX byte counter

            // I2C TX, start condition
            UCB1CTL1 |= (UCTR | UCTXSTT);

            while (UCB1CTL1 & UCTXSTP)
                ; // Ensure stop condition got sent to slave
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
