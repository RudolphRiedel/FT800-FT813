/**
 * @file	Serialport.c
 * @author	Robert Steigemann
 * @date	15 January 2017
 * @brief	Serialport communication via RS232
 *
 * This library contains functions to communicate with the PC via RS232.
 *
 * @license creative commons license CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "Serialport.h"

static volatile uint8_t txd_buffer[64];								/**< the 64 Byte transmit buffer */
static volatile uint8_t txd_point_tail = 0;						    /**< pointer offset to the next byte to transmit in buffer */
static volatile uint8_t txd_point_head = 0;							/**< pointer offset to the next position to write in transmit buffer */

static volatile uint8_t rxd_buffer[256];							/**< the 256 Byte receive buffer */
static volatile uint8_t rxd_point_tail = 0;						    /**< pointer offset to the last read byte in buffer */
static volatile uint8_t rxd_point_head = 0;							/**< pointer offset to the next position to write in receive buffer */


// This function add an array to the transmit buffer and control the DRE interrupt.
void serialport_write(uint8_t *anArray, uint8_t arrayLength)
{	
	uint8_t counter;												// the count variable over the array length
	
	for (counter=0;counter<arrayLength;counter++)					// for each byte in the array
	{				
		SERIALPORT_UART.CTRLA &= 0b11111100;						// deactivate the DRE (data register empty) interrupt 			
		*(txd_buffer+txd_point_head) = *(anArray+counter);			// add a byte from the array to the transmit buffer		
		txd_point_head = 0b00111111 & (txd_point_head + 1);			// increment the last byte "pointer" and mask it for 64 to match the buffer size
		SERIALPORT_UART.CTRLA |= 0b00000001;						// activate the DRE interrupt 		
	}			
}


// This interrupt is executed, when the data register is empty. It put the next byte into the data register of the UART to transmit it.
// This interrupt is only enabled, if the transmit buffer hold at least one byte to transmit!
ISR(SERIALPORT_DRE_vect)
{		
	SERIALPORT_UART.DATA = txd_buffer[txd_point_tail];				// put the next byte into the data register	
	txd_point_tail = 0b00111111 & (txd_point_tail + 1);				// increment the next byte pointer and mask it for 64 to match the buffer size
	
	// if the last byte is transmitting	
	if (txd_point_tail==txd_point_head)
	{
		SERIALPORT_UART.CTRLA &= 0b11111100;						// deactivate the DRE interrupt 		
	}		
}


// This function checks how many space are in the transmit buffer.
uint8_t serialport_checkSpace(void)
{
	if ((SERIALPORT_UART.CTRLA & 0b00000011) == 0)					// there is no data to transmit, because the interrupt is off
	{
		return 64;													// so simply return 64 bytes are free
	} else if (txd_point_head == txd_point_tail)					// buffer is complete full!
	{
		return 0;
	} else															// calculate free buffer
	{
		return 64 - ((txd_point_head - txd_point_tail) & 0b00111111);			
	}
}


// This interrupt is executed, when a byte is received. It put it on the head of the receive buffer and increment the head pointer.
ISR(SERIALPORT_RXC_vect)
{	
	rxd_buffer[rxd_point_head] = SERIALPORT_UART.DATA;				// copy byte from data register to receive buffer			
	rxd_point_head++;												// increment the receive head pointer	
}



// This function initializes the USART to 8N1 with given Baudrate and enables the RXC interrupt.
uint8_t serialport_init(uint32_t baudrate)
{		
	uint8_t  clk2_bm;		// double speed bit mask, set to USART_CLK2X_bm for double speed
	int8_t   bscale;		// -7 to 7
	uint32_t bsel;			// 1 to 4095
	uint32_t temp;
		
	if ((F_CPU / baudrate) <= 32 )	// Baudrate is high compared to CPU Frequency, so use double speed
	{
		clk2_bm = USART_CLK2X_bm;
		
		// check the negative bscale values -7 to -1
		for (bscale=7; bscale > 0; bscale--)	// 7 to 1
		{
			temp = baudrate<<3;
			bsel = ((F_CPU<<bscale)+(temp>>1))/temp - (1<<bscale);
			if (bsel <= 4095)
			{
				bscale *= -1;
				break;
			}
		}
		
		if (bsel > 4095)	// no match so far
		{
			// check the positive bscale values 0 to 7
			for (bscale=0; bscale <= 7; bscale++)	// 0 to 7
			{
				temp = baudrate<<(3+bscale);
				bsel = (F_CPU+(temp>>1))/temp - 1;
				if (bsel <= 4095)
				{
					break;
				}
			}
		}
	}
	else							// use normal speed with better sampling
	{
		clk2_bm = 0;
		
		// check the negative bscale values -7 to -1
		for (bscale=7; bscale > 0; bscale--)	// 7 to 1
		{
			temp = baudrate<<4;
			bsel = ((F_CPU<<bscale)+(temp>>1))/temp - (1<<bscale);
			if (bsel <= 4095)
			{
				bscale *= -1;
				break;
			}
		}
		
		if (bsel > 4095)	// no match so far
		{
			// check the positive bscale values 0 to 7
			for (bscale=0; bscale <= 7; bscale++)	// 0 to 7
			{
				temp = baudrate<<(4+bscale);
				bsel = (F_CPU+(temp>>1))/temp - 1;
				if (bsel <= 4095)
				{
					break;
				}
			}
		}
	}
	
	if ((bsel > 0) && (bsel <= 4095))	// match found
	{
		SERIALPORT_UART.BAUDCTRLB = (((uint8_t)bscale<<4) | (uint8_t)(bsel/256));	// map values to baudrate registers
		SERIALPORT_UART.BAUDCTRLA = (uint8_t)(bsel % 256);								
		SERIALPORT_UART.CTRLA = USART_RXCINTLVL_LO_gc;							// set interrupt level for receive interrupt to high 
		SERIALPORT_UART.CTRLC = USART_CHSIZE_8BIT_gc;							// use 8 Bit Modus	
		SERIALPORT_UART.CTRLB = USART_TXEN_bm | USART_RXEN_bm | clk2_bm;		// enable receiver and transmitter and double speed if needed		
		// set pin directions
        SERIALPORT_TXD_TO_OUTPUT;
        SERIALPORT_RXD_TO_INPUT;
        return 0;		
	}
	else		// selected baudrate is impossible
	{
		return 1;
	}
}


// This function gives you all received data to the next CR (carriage return) in string format.
// To get the string, it is necessary that your array is big enough to hold all data to the CR.
// If is not (maxLength reached, before CR is) this function return 0 and data is lost.
uint8_t serialport_getLastString(uint8_t *anArray, uint8_t maxLength)
{	
	uint8_t counter;												// the count variable over the array length
	uint8_t cr_is_there = 0;										// becomes true if we found a CR
	
	// run through all filled buffer bytes and search for a CR
	for (counter=rxd_point_tail;counter!=rxd_point_head;counter++)
	{						
		if (rxd_buffer[counter]==13) {								// 13 == CR (carriage return)
			cr_is_there = 1;										// CR found!
			break;													// don't search for a second one
		}		
	}
	
	// if we have a CR
	if (cr_is_there)
	{		
		counter = 0;												// clear counter		
				
		while (rxd_buffer[rxd_point_tail]!=13)						// until we reach the CR
		{			
			*(anArray+counter) = rxd_buffer[rxd_point_tail];		// copy data to array			
			rxd_point_tail++;										// increment tail
			counter++;												// and the counter
			
			// If the array is full and CR is not reached!
			if ((counter == (maxLength-1)) && (rxd_buffer[rxd_point_tail]!=13))
			{
				*(anArray+counter) = 0;								// add the '\0' to the end of string
				rxd_point_tail++;									// also need to increment the tail				
				return 0;											// return 0 (change this if you want)
			}									
		}
				
		*(anArray+counter) = 0;										// add the '\0' to the end of string	
		rxd_point_tail++;											// need to increment the tail ones more
		
		return counter+1;											// return the received characters + '\0'
				
	}
	else															// we haven't found a CR
	{																
		return 0;													// nothing received to return		
	}	
}

// printf for uart
void serialport_printf(const char* __fmt, ...)
{
	char aString[64];
	va_list argumentlist;
	va_start(argumentlist, __fmt);
	vsprintf(aString, __fmt, argumentlist);
	va_end(argumentlist);
	while(strlen(aString)>serialport_checkSpace());
	serialport_write((uint8_t *)aString, strlen(aString));
}

// Simple print a string
void serialport_print(char* aString)
{
	while(strlen(aString)>serialport_checkSpace());
	serialport_write((uint8_t *)aString, strlen(aString));
}