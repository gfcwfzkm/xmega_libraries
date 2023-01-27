/*
 * uartSPI.h
 *
 * Created: 26.01.2021 11:34:04
 *  Author: gfcwfzkm
 */ 


#ifndef UARTSPI_H_
#define UARTSPI_H_

#include <avr/io.h>

enum USPI_bitOrder {
	USPI_MSB_FIRST = 0,
	USPI_LSB_FIRST = 1
};

typedef struct {
	USART_t *usart_p;
	PORT_t *port;
	uint8_t cs_pins;
	uint8_t activelow:1;
}USARTSPI_Master_t;


void usartSPI_init(USARTSPI_Master_t *uspi_p, USART_t *uartPtr, int8_t _bscaleFactor, uint16_t bselValue);

void usartSPI_port(USARTSPI_Master_t *uspi_p, PORT_t *port, uint8_t cs_pin, uint8_t activeLow);

void usartSPI_setBitOrder(USARTSPI_Master_t *uspi_p, enum USPI_bitOrder bitOrder);

void usartSPI_setClockDivider(USARTSPI_Master_t *uspi_p, int8_t _bscaleFactor, uint16_t bselValue, uint8_t useDoubleSpeed);

uint8_t usartSPI_transfer(USARTSPI_Master_t *uspi_p, uint8_t _data);

uint8_t usartSPI_InterfacePrepare(void *intUSPI);

uint8_t usartSPI_InterfaceSendBytes(void *intUSPI,
								uint8_t addr,
								uint8_t *buf_ptr,
								uint16_t buf_len);

uint8_t usartSPI_InterfaceTransceiveBytes(void *intUSPI,
								uint8_t addr,
								uint8_t *buf_ptr,
								uint16_t buf_len);

uint8_t usartSPI_InterfaceGetBytes(void *intUSPI,
								uint8_t addr,
								uint8_t *buf_ptr,
								uint16_t buf_len);

uint8_t usartSPI_InterfaceFinish(void *intUSPI);

#endif /* UARTSPI_H_ */