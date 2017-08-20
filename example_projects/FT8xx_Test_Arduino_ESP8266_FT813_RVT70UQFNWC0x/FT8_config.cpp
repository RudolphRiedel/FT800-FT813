/*
@file    FT8_config.c
@brief   Contains hardware abstraction functions for using the FT8xx
@version 3.0
@date    2017-04-02
@author  Rudolph Riedel

This file needs to be renamed to FT8_config.cpp for use with Arduino. 

@section History

2.2
- extracted from FT8_commands.c
- added FT8_pdn_set() and FT8_pdn_clear()
- replaced spi_flash_write() with fetch_flash_byte()

2.3
- added set of functions for Arduino

2.4
- switched from custom AVR8 to standard __AVR__ symbol to automatically generate plattform specific code

2.5
- added support for RH850
- switched to standard-C compliant comment-style

3.0
- renamed from FT800_config.c to FT8_config.c
- changed FT_ prefixes to FT8_
- changed ft800_ prefixes to FT8_

*/

#include "FT8_config.h"

#ifndef ARDUINO

	#if defined (__GNUC__)
	
		#if defined (__AVR__)

			void FT8_cs_set(void)
			{
				FT8_CS_PORT &= ~FT8_CS;	/* cs low */
			}

			void FT8_cs_clear(void)
			{
				FT8_CS_PORT |= FT8_CS;	/* cs high */
			}

			void FT8_pdn_set(void)
			{
				FT8_PDN_PORT &= ~FT8_PDN;	/* Power-Down low */
			}

			void FT8_pdn_clear(void)
			{
				FT8_PDN_PORT |= FT8_PDN;	/* Power-Down high */
			}

			void spi_transmit(uint8_t data)
			{
				SPDR = data; /* Start transmission */
				while(!(SPSR & (1<<SPIF))); /* Wait for transmission to complete - 1�s @ 8MHz SPI-Clock */
			}

			uint8_t spi_receive(uint8_t data)
			{
				SPDR = data; /* Start transmission */
				while(!(SPSR & (1<<SPIF))); /* Wait for transmission to complete - 1�s @ 8MHz SPI-CLock */
				return SPDR;
			}

			uint8_t fetch_flash_byte(const uint8_t *data)
			{
				return(pgm_read_byte_far(data));
			}
		
		#endif /* AVR */

		#if defined (__v851__)

			void FT8_pdn_set(void)
			{
				P0 &= ~(1u<<6);
			}

			void FT8_pdn_clear(void)
			{
				P0 |= (1u<<6);
			}

			void FT8_cs_set(void)
			{
				P8 &= ~(1u<<2); /* manually set chip-select to low */
			}

			void FT8_cs_clear(void)
			{
				P8 |= (1u<<2);  /* manually set chip-select to high */
			}

			void spi_transmit(uint8_t data)
			{
				CSIH0CTL0 = 0xC1; /* CSIH2PWR = 1;  CSIH2TXE=1; CSIH2RXE = 0; direct access mode  */
				CSIH0TX0H = data;	/* Start transmission */
				while(CSIH0STR0 & 0x00080);	/* Wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
			}

			uint8_t spi_receive(uint8_t data)
			{
				CSIH0CTL0 = 0xE1; /* CSIH2PWR = 1;  CSIH2TXE=1; CSIH2RXE = 1; direct access mode  */
				CSIH0TX0H = data;	/* Start transmission */
				while(CSIH0STR0 & 0x00080);	/* Wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
				return (uint8_t) CSIH0RX0H;
			}

			uint8_t fetch_flash_byte(const uint8_t *data)
			{
				return *data;
			}

		#endif /* RH850 */

	#endif /* GCC */
#endif /* !Arduino */


#ifdef ARDUINO

void FT8_cs_set(void)
{
	SPI.setDataMode(SPI_MODE0);
	digitalWrite(FT8_CS, LOW);
}


void FT8_cs_clear(void)
{
	digitalWrite(FT8_CS, HIGH);
}

void FT8_pdn_set(void)
{
	digitalWrite(FT8_PDN, LOW);	/* Power-Down low */
}


void FT8_pdn_clear(void)
{
	digitalWrite(FT8_PDN, HIGH);	/* Power-Down high */
}

void spi_transmit(uint8_t data)
{
	SPI.write(data);
}


uint8_t spi_receive(uint8_t data)
{
	return SPI.transfer(data);
}

uint8_t fetch_flash_byte(const uint8_t *data)
{
	return(pgm_read_byte_near(data));
}

#endif