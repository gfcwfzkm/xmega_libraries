/*
 * uartSPI.c
 *
 * Created: 26.01.2021 11:34:11
 *  Author: gfcwfzkm
 */ 

#include "uartSPI.h"

void usartSPI_init(USARTSPI_Master_t *uspi_p, USART_t *uartPtr, int8_t _bscaleFactor, uint16_t bselValue)
{
	uspi_p->usart_p = uartPtr;
	uspi_p->usart_p->CTRLC = USART_CMODE_MSPI_gc;
	uspi_p->usart_p->CTRLA = 0;
	uspi_p->usart_p->STATUS = USART_TXCIF_bm;
	uspi_p->usart_p->CTRLB = USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm;
	uspi_p->usart_p->BAUDCTRLA = (uint8_t)bselValue;
	uspi_p->usart_p->BAUDCTRLB = ((_bscaleFactor & 0x0F) << USART_BSCALE_gp)|((bselValue & 0xF00) >> 8);
}

void usartSPI_port(USARTSPI_Master_t *uspi_p, PORT_t *port, uint8_t cs_pin, uint8_t activeLow)
{
	uspi_p->port = port;
	uspi_p->cs_pins = cs_pin;
	uspi_p->activelow = (activeLow ? 1 : 0);
}

void usartSPI_setBitOrder(USARTSPI_Master_t *uspi_p, uint8_t bitOrder)
{
	if (bitOrder)
	{
		uspi_p->usart_p->CTRLC |= (1<<2);
	}
	else
	{
		uspi_p->usart_p->CTRLC &=~ (1<<2);
	}
}

void usartSPI_setClockDivider(USARTSPI_Master_t *uspi_p, int8_t _bscaleFactor, uint16_t bselValue, uint8_t useDoubleSpeed)
{
	uspi_p->usart_p->CTRLB = USART_RXEN_bm | USART_TXEN_bm | (useDoubleSpeed ? USART_CLK2X_bm : 0);
	uspi_p->usart_p->BAUDCTRLA = (uint8_t)bselValue;
	uspi_p->usart_p->BAUDCTRLB = ((_bscaleFactor & 0x0F) << USART_BSCALE_gp)|((bselValue & 0xF00) >> 8);
}

uint8_t usartSPI_transfer(USARTSPI_Master_t *uspi_p, uint8_t _data)
{
	unsigned char rx_byte;
	
	while(!(uspi_p->usart_p->STATUS & USART_DREIF_bm));
	uspi_p->usart_p->DATA = _data;
	
	while(!(uspi_p->usart_p->STATUS & USART_RXCIF_bm));
	rx_byte = uspi_p->usart_p->DATA;
	
	return rx_byte;
}

uint8_t usartSPI_InterfacePrepare(void *intSPI)
{
	USARTSPI_Master_t *usartSPI = (USARTSPI_Master_t*)intSPI;
	
	// chip-select
	if (usartSPI->activelow)
	{
		usartSPI->port->OUTCLR = usartSPI->cs_pins;
	}
	else
	{
		usartSPI->port->OUTSET = usartSPI->cs_pins;
	}
	return 0;
}

uint8_t usartSPI_InterfaceSendBytes(void *intSPI, uint8_t addr,
									uint8_t *buf_ptr, uint16_t buf_len)
{
	USARTSPI_Master_t *usartSPI = (USARTSPI_Master_t*)intSPI;
	uint32_t byteCnt;
	
	if (addr != 0)	usartSPI_transfer(usartSPI, addr);
	
	for(byteCnt = 0; byteCnt < buf_len; byteCnt++)
	{
		usartSPI_transfer(usartSPI,buf_ptr[byteCnt]);
	}
	
	return 0;
}

uint8_t usartSPI_InterfaceTransceiveBytes(void *intSPI, uint8_t addr,
									uint8_t *buf_ptr, uint16_t buf_len)
{
	USARTSPI_Master_t *usartSPI = (USARTSPI_Master_t*)intSPI;
	uint32_t byteCnt;
	
	if (addr != 0)	usartSPI_transfer(usartSPI, addr);
	
	for(byteCnt = 0; byteCnt < buf_len; byteCnt++)
	{
		buf_ptr[byteCnt] = usartSPI_transfer(usartSPI,buf_ptr[byteCnt]);
	}
	
	return 0;
}

uint8_t usartSPI_InterfaceGetBytes(void *intSPI, uint8_t addr,
									uint8_t *buf_ptr, uint16_t buf_len)
{
	USARTSPI_Master_t *usartSPI = (USARTSPI_Master_t*)intSPI;
	uint32_t byteCnt;
	
	if (addr != 0)	usartSPI_transfer(usartSPI, addr);
	
	for(byteCnt = 0; byteCnt < buf_len; byteCnt++)
	{
		buf_ptr[byteCnt] = usartSPI_transfer(usartSPI,0);
	}
	
	return 0;
}

uint8_t usartSPI_InterfaceFinish(void *intSPI)
{
	USARTSPI_Master_t *usartSPI = (USARTSPI_Master_t*)intSPI;
	
	// chip-select
	if (usartSPI->activelow)
	{
		usartSPI->port->OUTSET = usartSPI->cs_pins;
	}
	else
	{
		usartSPI->port->OUTCLR = usartSPI->cs_pins;
	}
	
	return 0;
}