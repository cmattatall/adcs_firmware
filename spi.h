/*
 * spi.h
 *
 *  Created on: Nov. 4, 2020
 *      Author: carl
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#define SPI_APPLICATION_BUFFER_SIZE 500
#define SPI_DELIM_CHAR '\0'
#define SPI_SIGNAL_SET 1
#define SPI_SIGNAL_CLR 0

void SPI0_init(volatile int **receive_signal_watcher, volatile int **transmit_signal_watcher);
int SPI0_receive_payload(uint8_t *userbuf, uint16_t len);
unsigned int SPI0_transmit_IT(uint8_t* bytes, uint16_t len);


#endif /* SPI_H_ */
