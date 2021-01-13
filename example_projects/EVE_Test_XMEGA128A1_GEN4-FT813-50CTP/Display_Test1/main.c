/*
 * Display_Test1.c
 *
 * Created: 12.01.2021 13:53:55
 * Author : steigemann
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <avr/pgmspace.h>

#include "Clockconfig.h"
#include "Serialport.h"
#include "tft.h"

#define VERSION "0.01"


// just my example:
// Use connector J4 for Display on A1 Xplained
// Pin - Function - Port - 4D Display
//  1  -  INT     - PC0  - 3
//  2  -  PDN     - PC1  - 2
//  3  -  (RXD0)  - PC2
//  4  -  (TXD0)  - PC3
//  5  -  /CS     - PC4	 - 4
//  6  -  MOSI    - PC5	 - 6
//  7  -  MISO    - PC6	 - 7
//  8  -  SCK     - PC7	 - 8
//  9  -  GND			 - 1, 5, 9, 11, 13, 17
// 10  -  3V3			 - 


/* a few things are expected to be taken care of beforehand. */
/* - setting the Chip-Select and Power-Down pins to Output, Chip-Select = 1 and Power-Down = 0 */
/* - setting up the SPI which may or not include things like
       - setting the pins for the SPI to output or some alternate I/O function or mapping that functionality to that pin
	   - if that is an option with the controller your are using you probably should set the drive-strength for the SPI pins to high
	   - setting the SS pin on AVRs to output in case it is not used for Chip-Select or Power-Down
	   - setting SPI to mode 0
	   - setting SPI to 8 bit with MSB first
	   - setting SPI clock to no more than 11 MHz for the init - if the display-module works as high
	   */



void eve_spi_init(void)
{
	PORTC.DIR = 0b10111010;	// set pin directions
	PORTC.OUTCLR = PIN1_bm;	// pdn auf 0
	PORTC.OUTSET= PIN4_bm;     // cs auf 1
	
	_delay_ms(50);
	
	SPIC.CTRL = (SPI_ENABLE_bm | SPI_MASTER_bm);    // SPI master, MSB first, mode 0, prescaler = 4 -> 8 MHz
	SPIC.INTCTRL = 0;							// SPI interrupts are disabled	
}



int main(void)
{
	uint8_t display_delay = 0;
	
	
    clk_init();
    serialport_init(115200);
	eve_spi_init();
	
	PMIC_CTRL |= PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
	sei();
	
	serialport_print("\rDisplay_Test1 V: ");
	serialport_print(VERSION);
	serialport_print("\r");
	
	
	TFT_init();
	
	// speedup SPI to 16 MHz
	SPIC.CTRL |= SPI_CLK2X_bm;    // double speed mode -> (32 MHz / 4) x2 = 16 MHz
	
    while (1) 
    {
		_delay_ms(5);
		
		TFT_touch();
		
		display_delay++;
		if(display_delay > 3)
		{
			display_delay = 0;

						
			TFT_display();
			serialport_print(".");
			
			num_profile_b--;
			
			
		}
		num_profile_a++;
		
    }
}

