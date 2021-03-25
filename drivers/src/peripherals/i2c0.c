/**
 * @file i2c0.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to implement an i2c driver using UCB0 for msp430f5529
 * @brief
 * @version 0.1
 * @date 2021-03-25
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */

#if !defined(TARGET_MCU)
#error DRIVER COMPILATION SHOULD ONLY OCCUR ON CROSSCOMPILED TARGETS
#endif /* !defined(TARGET_MCU) */

#include <string.h>

#include <msp430.h>

#include "i2c.h"

static volatile uint8_t  I2C0_i2c_txbuf[32];
static volatile uint8_t *I2C0_i2c_txbuf_ptr;

static volatile uint8_t  I2C0_i2c_rxbuf[32];
static volatile uint8_t *I2C0_i2c_rxbuf_inptr;
static volatile uint8_t *I2C0_i2c_rxbuf_outptr;


static void I2C0_PHY_init(void);


void I2C0_init(void)
{
    I2C0_PHY_init();

    UCB0CTL1 |= UCSWRST; /* unlock peripheral to modify config */

    /** @todo IMPLEMENT REGISTER LEVEL CONFIGURATION STUFF*/
#warning TODO: IMPLEMENT REGISTER LEVEL CONFIGURATION FOR I2C0

    UCB0CTL1 &= ~UCSWRST;
}


void I2C0_write_bytes(uint8_t dev_addr, uint8_t *bytes, uint16_t byte_count)
{
    if (bytes != NULL)
    {
        /* First byte in transmit buffer is device address */
        I2C0_i2c_txbuf_ptr  = I2C0_i2c_txbuf;
        *I2C0_i2c_txbuf_ptr = dev_addr;
        I2C0_i2c_txbuf++;

        /* Rest of data comes next */
        uint16_t bcnt = byte_count;
        if (bcnt > sizeof(I2C0_i2c_txbuf))
        {
            bcnt = I2C0_i2c_txbuf;
        }
        strncpy((char *)I2C0_i2c_txbuf_ptr, bytes, bcnt);

        /** @todo IMPLEMENT THE STUFF THAT ACTUALLY TRANSMITS, CALLER DATA IS
         * JUST LOADED RIGHT NOW*/
#warning TODO: NOT FULLY IMPLEMENTED YET
    }
}


int I2C0_read_bytes(uint8_t *caller_buf, uint16_t caller_buflen)
{
    if (caller_buf == NULL)
    {
        return 0;
    }
    else
    {
        int bcnt = 0;
        while (I2C0_i2c_rxbuf_outptr != I2C0_i2c_rxbuf_inptr)
        {
            caller_buf[bcnt] = *I2C0_i2c_rxbuf_outptr;
            I2C0_i2c_rxbuf_outptr++;
            if (I2C0_i2c_rxbuf_outptr > I2C0_i2c_rxbuf + sizeof(I2C0_i2c_rxbuf))
            {
                I2C0_i2c_rxbuf_outptr = I2C0_i2c_rxbuf_outptr;
            }
            ++bcnt;

            /* Data overrun condition */
            if (bcnt > sizeof(I2C0_i2c_rxbuf))
            {
                bcnt = -1;
                break;
            }
        }
        return bcnt;
    }
}


static void I2C0_PHY_init(void)
{
    P3SEL |= BIT0; /* p3.0 == SDA */
    P3SEL |= BIT1; /* p3.1 == SCL */
}


__interrupt_vec(USCI_B0_VECTOR) void USCI_I2C_ISR(void)
{
    switch (__even_in_range(UCB0IV, 12))
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
#warning RECEIVE ISR HANDLING NOT IMPLEMENTED FOR I2C0
        }
        break;   // Vector 10: RXIFG
        case 12: // Vector 12: TXIFG
        {
            if (i2c_data_cnt)
            {
                UCB0TXBUF = *i2c_data_ptr;
                i2c_data_ptr++; /* advance data ptr */
                i2c_data_cnt--; // Decrement TX byte counter
            }
            else
            {
                UCB0CTL1 |= UCTXSTP; // I2C stop condition
                UCB0IFG &= ~UCTXIFG; // Clear USCI_B0 TX int flag
                __no_operation();    /* NOP to fix silicon errata with ISR */
            }
        }
        break;
        default:
            break;
    }
}