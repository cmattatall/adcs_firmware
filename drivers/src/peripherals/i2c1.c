/**
 * @file i2c.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement an i2c driver using UCB1 for msp430f5529
 * @version 0.1
 * @date 2021-02-22
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <string.h>
#include <stdlib.h>

#include <msp430.h>

#include "i2c.h"

static volatile uint8_t  I2C1_i2c_txbuf[32];
static volatile uint8_t *I2C1_i2c_txbuf_ptr;
static volatile uint16_t I2C1_i2c_txcnt;

static volatile uint8_t  I2C1_i2c_rxbuf[32];
static volatile uint8_t *I2C1_i2c_rxbuf_inptr;
static volatile uint8_t *I2C1_i2c_rxbuf_outptr;

static void I2C1_PHY_init(void);


void I2C1_init(void)
{
    I2C1_PHY_init();

    UCB1CTL1 |= UCSWRST; /* unlock peripheral to modify config */

    /** @todo IMPLEMENT REGISTER LEVEL CONFIGURATION STUFF*/
#warning TODO: IMPLEMENT REGISTER LEVEL CONFIGURATION FOR I2C1

    UCB1CTL1 &= ~UCSWRST;
}

int I2C1_write_bytes(uint8_t dev_addr, uint8_t *bytes, uint16_t byte_count)
{
    int retval = 0;
    if (bytes != NULL)
    {
        /* First byte in transmit buffer is device address */
        I2C1_i2c_txbuf_ptr  = I2C1_i2c_txbuf;
        *I2C1_i2c_txbuf_ptr = dev_addr;
        I2C1_i2c_txbuf_ptr++;

        /* Rest of data comes next */
        unsigned int bcnt = sizeof(dev_addr) + byte_count;

        /* subtract 1 bc first byte is dev address */
        if (bcnt > sizeof(I2C1_i2c_txbuf))
        {
            retval = -1;
        }
        else
        {
            strncpy((char *)I2C1_i2c_txbuf_ptr, (char *)bytes, bcnt);

            I2C1_i2c_txcnt = bcnt;

            /** @todo IMPLEMENT THE STUFF THAT ACTUALLY TRANSMITS, CALLER DATA
             * IS JUST LOADED RIGHT NOW*/
#warning TODO: NOT FULLY IMPLEMENTED YET


            retval = bcnt;
        }
    }
    else
    {
        retval = 0;
    }
    return retval;
}


int I2C1_read_bytes(uint8_t *caller_buf, uint16_t caller_buflen)
{
    if (caller_buf == NULL)
    {
        return 0;
    }
    else
    {
        unsigned int bcnt = 0;
        while (I2C1_i2c_rxbuf_outptr != I2C1_i2c_rxbuf_inptr)
        {
            caller_buf[bcnt] = *I2C1_i2c_rxbuf_outptr;
            I2C1_i2c_rxbuf_outptr++;
            if (I2C1_i2c_rxbuf_outptr > I2C1_i2c_rxbuf + sizeof(I2C1_i2c_rxbuf))
            {
                I2C1_i2c_rxbuf_outptr = I2C1_i2c_rxbuf_outptr;
            }
            ++bcnt;

            /* Data overrun condition */
            if (bcnt > sizeof(I2C1_i2c_rxbuf))
            {
                bcnt = -1;
                break;
            }

            if (bcnt == caller_buflen)
            {
                break;
            }
        }
        return bcnt;
    }
}


static void I2C1_PHY_init(void)
{
    P4SEL |= BIT2; /* P4.2 == scl */
    P4SEL |= BIT1; /* P4.1 == sda */
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
        {
/** @todo IMPLEMENT THIS STUFF */
#warning RECEIVE ISR HANDLING NOT IMPLEMENTED FOR I2C1
        }
        break;   // Vector 10: RXIFG
        case 12: // Vector 12: TXIFG
        {
            if (I2C1_i2c_txcnt)
            {
                UCB1TXBUF = *I2C1_i2c_txbuf_ptr;
                I2C1_i2c_txbuf_ptr++; /* advance data ptr */
                I2C1_i2c_txcnt--;     // Decrement TX byte counter
            }
            else
            {
                UCB1CTL1 |= UCTXSTP; // I2C stop condition
                UCB1IFG &= ~UCTXIFG; // Clear USCI_B0 TX int flag
                __no_operation();    /* NOP to fix silicon errata with ISR */
            }
        }
        break;
        default:
            break;
    }
}
