/*
@file    EVE_target.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2020-10-29
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2020 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

4.0
- still 4.0 for EVE itself, switched to hardware-SPI on SAMC21
- minor maintenance
- added DMA to SAMC21 branch
- started testing things with a BT816
- added a block for the SAME51J18A
- started to add support for Imagecraft AVR
- moved all target specific lines from EVE_config.h to EVE_target.h
- cleaned up history
- added support for MSP432 - it compiles with Code Composer Studio but is for the most part untested...
- wrote a couple lines of explanation on how DMA is to be used
- replaced the dummy read of the SPI data register with a var for ATSAMC21 and ATSAME51 with "(void) REG_SERCOM0_SPI_DATA;"
- added support for RISC-V, more specifically the GD32VF103 that is on the Sipeed Longan Nano - not tested with a display yet but it looks very good with the Logic-Analyzer
- added support for STM32F407 by adding code supplied by User "mokka" on MikroController.net and modifying it by replacing the HAL functions with direct register accesses
- added comment lines to separate the various targets visually
- reworked ATSAMC21 support code to use defines for ports, pins and SERCOM, plus changed the "legacy register definitions" to more current ones
- changed ATSAME51 support code to the new "template" as well
- bugifx: STM32F407 support was neither working or compiling, also changed it to STM32F4 as it should support the whole family now
- bugifx: second attempt to fix STM32F4 support, thanks again to user "mokka" on MikroController.net
- combined ATSAMC21 and ATSAME51 targets into one block since these were using the same code anyways
- moved the very basic DELAY_MS() function for ATSAM to EVE_target.c and therefore removed the unneceesary inlining for this function
- expanded the STM32F4 section with lines for STM32L073, STM32F1, STM32F207 and STM32F3
- forgot to add the "#include <Arduino.h>" line I found to be necessary for ESP32/Arduino
- started to implement DMA support for STM32
- added a few more controllers as examples from the ATSAMC2x and ATSAMx5x family trees
- measured the delay for ATSAME51 again and changed EVE_DELAY_1MS to 20000 for use with 120MHz core-clock and activated cache

5.0
- replaced spi_transmit_async() with spi_transmit_burst()
- changed the DMA buffer from uin8_t to uint32_t
- added spi_transmit_32(uint32_t data) to help shorten EVE_commands.c a bit
- added spi_transmit_32() to all targets and changed the non-DMA version of spi_transmit_burst() to use spi_transmit_32()

*/

#ifndef EVE_TARGET_H_
#define EVE_TARGET_H_

/* While the following lines make things a lot easier like automatically compiling the code for the target you are compiling for, */
/* a few things are expected to be taken care of beforehand. */
/* - setting the Chip-Select and Power-Down pins to Output, Chip-Select = 1 and Power-Down = 0 */
/* - setting up the SPI which may or not include things like
       - setting the pins for the SPI to output or some alternate I/O function or mapping that functionality to that pin
	   - if that is an option with the controller your are using you probably should set the drive-strength for the SPI pins to high
	   - setting the SS pin on AVRs to output in case it is not used for Chip-Select or Power-Down
	   - setting SPI to mode 0
	   - setting SPI to 8 bit with MSB first
	   - setting SPI clock to no more than 11 MHz for the init - if the display-module works as high

  For the SPI transfers single 8-Bit transfers are used with busy-wait for completion.
  While this is okay for AVRs that run at 16MHz with the SPI at 8 MHz and therefore do one transfer in 16 clock-cycles,
  this is wasteful for any 32 bit controller even at higher SPI speeds.

  Check out the section for SAMC21E18A as it has code to transparently add DMA.

  If the define "EVE_DMA" is set the spi_transmit_async() is changed at compile time to write in a buffer instead directly to SPI.
  EVE_init() calls EVE_init_dma() which sets up the DMA channel and enables an IRQ for end of DMA.
  EVE_start_cmd_burst() resets the DMA buffer instead of transferring the first bytes by SPI.
  EVE_end_cmd_burst() just calls EVE_start_dma_transfer() which triggers the transfer of the SPI buffer by DMA.
  EVE_cmd_start() just instantly returns if there is an active DMA transfer.
  EVE_busy() does nothing but to report that EVE is busy if there is an active DMA transfer.
  At the end of the DMA transfer an IRQ is executed which clears the DMA active state, calls EVE_cs_clear() and EVE_cmd_start().

*/

#pragma once

#if !defined (ARDUINO)

	#if defined (__IMAGECRAFT__)
		#if defined (_AVR)
			#include <iccioavr.h>

			static inline void DELAY_MS(uint16_t val)
			{
				uint16_t counter;

				while(val > 0)
				{
					for(counter=0; counter < 2000;counter++) // maybe ~1ms at 16MHz clock
					{
						__asm__ volatile ("nop");
					}
					val--;
				}
			}

			#define EVE_CS_PORT	PORTB
			#define EVE_CS 		(1<<PB5)
			#define EVE_PDN_PORT	PORTB
			#define EVE_PDN		(1<<PB4)

			static inline void EVE_pdn_set(void)
			{
				EVE_PDN_PORT &= ~EVE_PDN;	/* Power-Down low */
			}

			static inline void EVE_pdn_clear(void)
			{
				EVE_PDN_PORT |= EVE_PDN;	/* Power-Down high */
			}

			static inline void EVE_cs_set(void)
			{
				EVE_CS_PORT &= ~EVE_CS;	/* cs low */
			}

			static inline void EVE_cs_clear(void)
			{
				EVE_CS_PORT |= EVE_CS;	/* cs high */
			}

			static inline void spi_transmit(uint8_t data)
			{
				SPDR = data; /* start transmission */
				while(!(SPSR & (1<<SPIF))); /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
			}

			static inline void spi_transmit_32(uint32_t data)
			{
				spi_transmit((uint8_t)(data));
				spi_transmit((uint8_t)(data >> 8));
				spi_transmit((uint8_t)(data >> 16));
				spi_transmit((uint8_t)(data >> 24));
			}

			/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
			static inline void spi_transmit_burst(uint32_t data)
			{
				spi_transmit_32(data);
			}

			static inline uint8_t spi_receive(uint8_t data)
			{
				SPDR = data; /* start transmission */
				while(!(SPSR & (1<<SPIF))); /* wait for transmission to complete - 1us @ 8MHz SPI-CLock */
				return SPDR;
			}

			static inline uint8_t fetch_flash_byte(const uint8_t *data)
			{
				return *data;
			}


		#endif
	#endif

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

	#if defined (__GNUC__)

		#if	defined (__AVR__)

			#include <avr/io.h>
			#include <avr/pgmspace.h>
			#define F_CPU 16000000UL
			#include <util/delay.h>

			#define DELAY_MS(ms) _delay_ms(ms)

			#define EVE_CS_PORT	PORTB
			#define EVE_CS 		(1<<PB5)
			#define EVE_PDN_PORT	PORTB
			#define EVE_PDN		(1<<PB4)

			static inline void EVE_pdn_set(void)
			{
				EVE_PDN_PORT &= ~EVE_PDN;	/* Power-Down low */
			}

			static inline void EVE_pdn_clear(void)
			{
				EVE_PDN_PORT |= EVE_PDN;	/* Power-Down high */
			}

			static inline void EVE_cs_set(void)
			{
				EVE_CS_PORT &= ~EVE_CS;	/* cs low */
			}

			static inline void EVE_cs_clear(void)
			{
				EVE_CS_PORT |= EVE_CS;	/* cs high */
			}

			static inline void spi_transmit(uint8_t data)
			{
#if 1
				SPDR = data; /* start transmission */
				while(!(SPSR & (1<<SPIF))); /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
#endif

#if 0
				uint8_t spiIndex  = 0x80;
				uint8_t k;

				for(k = 0; k <8; k++) // Output each bit of spiOutByte
				{
					if(data & spiIndex) // Output MOSI Bit
					{
						PORTC |= (1<<PORTC1);
					}
					else
					{
						PORTC &= ~(1<<PORTC1);
					}

					PORTA |= (1<<PORTA1); // toggle SCK
					PORTA &= ~(1<<PORTA1);

					spiIndex >>= 1;
				}
#endif
			}

			static inline void spi_transmit_32(uint32_t data)
			{
				spi_transmit((uint8_t)(data));
				spi_transmit((uint8_t)(data >> 8));
				spi_transmit((uint8_t)(data >> 16));
				spi_transmit((uint8_t)(data >> 24));
			}

			/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
			static inline void spi_transmit_burst(uint32_t data)
			{
				spi_transmit_32(data);
			}

			static inline uint8_t spi_receive(uint8_t data)
			{
#if 1
				SPDR = data; /* start transmission */
				while(!(SPSR & (1<<SPIF))); /* wait for transmission to complete - 1us @ 8MHz SPI-CLock */
				return SPDR;
#endif

#if 0
				uint8_t spiIndex  = 0x80;
				uint8_t spiInByte = 0;
				uint8_t k;

				for(k = 0; k <8; k++) // Output each bit of spiOutByte
				{
					if(data & spiIndex) // Output MOSI Bit
					{
						PORTC |= (1<<PORTC1);
					}
					else
					{
						PORTC &= ~(1<<PORTC1);
					}

					PORTA |= (1<<PORTA1); // toggle SCK
					PORTA &= ~(1<<PORTA1);

					if(PINC & (1<<PORTC0))
					{
						spiInByte |= spiIndex;
					}

					spiIndex >>= 1;
				}
				return spiInByte;
#endif

			}

			static inline uint8_t fetch_flash_byte(const uint8_t *data)
			{
				#if defined (__AVR_HAVE_ELPM__)	/* we have an AVR with more than 64kB FLASH memory */
					return(pgm_read_byte_far(data));
				#else
					return(pgm_read_byte_near(data));
				#endif
			}

		#endif /* AVR */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

		#if defined (__v851__)

			#include <stdint.h>
			#include "rh850_regs.h"
			#include "os.h"

			#define DELAY_MS(ms)	OS_Wait(ms * 1000)

			static inline void EVE_pdn_set(void)
			{
				P0 &= ~(1u<<6);
			}

			static inline void EVE_pdn_clear(void)
			{
				P0 |= (1u<<6);
			}

			static inline void EVE_cs_set(void)
			{
				P8 &= ~(1u<<2); /* manually set chip-select to low */
			}

			static inline void EVE_cs_clear(void)
			{
				P8 |= (1u<<2);  /* manually set chip-select to high */
			}

			static inline void spi_transmit(uint8_t data)
			{
				CSIH0CTL0 = 0xC1; /* CSIH2PWR = 1;  CSIH2TXE=1; CSIH2RXE = 0; direct access mode  */
				CSIH0TX0H = data;	/* start transmission */
				while(CSIH0STR0 & 0x00080);	/* wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
			}

			static inline void spi_transmit_32(uint32_t data)
			{
				spi_transmit((uint8_t)(data));
				spi_transmit((uint8_t)(data >> 8));
				spi_transmit((uint8_t)(data >> 16));
				spi_transmit((uint8_t)(data >> 24));
			}

			/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
			static inline void spi_transmit_burst(uint32_t data)
			{
				spi_transmit_32(data);
			}

			static inline uint8_t spi_receive(uint8_t data)
			{
				CSIH0CTL0 = 0xE1; /* CSIH2PWR = 1;  CSIH2TXE=1; CSIH2RXE = 1; direct access mode  */
				CSIH0TX0H = data;	/* start transmission */
				while(CSIH0STR0 & 0x00080);	/* wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
				return (uint8_t) CSIH0RX0H;
			}

			static inline uint8_t fetch_flash_byte(const uint8_t *data)
			{
				return *data;
			}

		#endif /* RH850 */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

		#if defined (__TRICORE__)

			#include "types.h"
			#include "os.h"
			#include "dio.h"
			#include "spi.h"

			#define DELAY_MS(ms) OS_Wait(ms * 1000)

			static inline void EVE_pdn_set(void)
			{
				HW_DIO_SetSync(IO_DIO_DIGOUT_PD_TFT, 0);/* Power-Down low */
			}

			static inline void EVE_pdn_clear(void)
			{
				HW_DIO_SetSync(IO_DIO_DIGOUT_PD_TFT, 1);/* Power-Down high */
			}

			static inline void EVE_cs_set(void)
			{
				HW_DIO_SetSync(IO_DIO_DIGOUT_CS_TFT, 0); /* manually set chip-select to low */
			}

			static inline void EVE_cs_clear(void)
			{
				HW_DIO_SetSync(IO_DIO_DIGOUT_CS_TFT, 1);  /* manually set chip-select to high */
			}

			static inline void spi_transmit(uint8_t data)
			{
				SPI_ReceiveByte(data);
			}

			static inline void spi_transmit_32(uint32_t data)
			{
				spi_transmit((uint8_t)(data));
				spi_transmit((uint8_t)(data >> 8));
				spi_transmit((uint8_t)(data >> 16));
				spi_transmit((uint8_t)(data >> 24));
			}

			/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
			static inline void spi_transmit_burst(uint32_t data)
			{
				spi_transmit_32(data);
			}

			static inline uint8_t spi_receive(uint8_t data)
			{
				return SPI_ReceiveByte(data);
			}

			static inline uint8_t fetch_flash_byte(const uint8_t *data)
			{
				return *data;
			}

		#endif /* __TRICORE__ */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

		#if defined (__SAMC21E18A__) || (__SAMC21J18A__) || (__SAME51J19A__) || (__SAMD51P20A__) || (__SAMD51J19A__) || (__SAMD51G18A__)
		/* note: target as set by AtmelStudio, valid  are all from the same family, ATSAMC2x and ATSAMx5x use the same SERCOM units */

		#include "sam.h"

		#if defined (__SAMC21E18A__) || (__SAMC21J18A__)
		#define EVE_CS_PORT 0
		#define EVE_CS PORT_PA05
		#define EVE_PDN_PORT 0
		#define EVE_PDN PORT_PA03
		#define EVE_SPI SERCOM0
		#define EVE_SPI_DMA_TRIGGER SERCOM0_DMAC_ID_TX
		#define EVE_DMA_CHANNEL 0
		#define EVE_DMA
		#define EVE_DELAY_1MS 8000	/* ~1ms at 48MHz Core-Clock */
		#endif

		#if defined (__SAME51J19A__) || (__SAMD51P20A__) || (__SAMD51J19A__) || (__SAMD51G18A__)
		#define EVE_CS_PORT 1
		#define EVE_CS PORT_PB01
		#define EVE_PDN_PORT 1
		#define EVE_PDN PORT_PB31
		#define EVE_SPI SERCOM5
		#define EVE_SPI_DMA_TRIGGER SERCOM5_DMAC_ID_TX
		#define EVE_DMA_CHANNEL 0
		#define EVE_DMA
		#define EVE_DELAY_1MS 20000	/* ~1ms at 120MHz Core-Clock and activated cache, according to my Logic-Analyzer */
		#endif


		#if defined (EVE_DMA)
			extern uint32_t EVE_dma_buffer[1025];
			extern volatile uint16_t EVE_dma_buffer_index;
			extern volatile uint8_t EVE_dma_busy;

			void EVE_init_dma(void);
			void EVE_start_dma_transfer(void);
		#endif

		void DELAY_MS(uint16_t val);


		static inline void EVE_pdn_set(void)
		{
			PORT->Group[EVE_PDN_PORT].OUTCLR.reg = EVE_PDN;
		}

		static inline void EVE_pdn_clear(void)
		{
			PORT->Group[EVE_PDN_PORT].OUTSET.reg = EVE_PDN;
		}

		static inline void EVE_cs_set(void)
		{
			PORT->Group[EVE_CS_PORT].OUTCLR.reg = EVE_CS;
		}

		static inline void EVE_cs_clear(void)
		{
			PORT->Group[EVE_CS_PORT].OUTSET.reg = EVE_CS;
		}

		static inline void spi_transmit(uint8_t data)
		{
			EVE_SPI->SPI.DATA.reg = data;
			while((EVE_SPI->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) == 0);
			(void) EVE_SPI->SPI.DATA.reg; /* dummy read-access to clear SERCOM_SPI_INTFLAG_RXC */
		}

		static inline void spi_transmit_32(uint32_t data)
		{
			spi_transmit((uint8_t)(data));
			spi_transmit((uint8_t)(data >> 8));
			spi_transmit((uint8_t)(data >> 16));
			spi_transmit((uint8_t)(data >> 24));
		}

		/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
		static inline void spi_transmit_burst(uint32_t data)
		{
			#if defined (EVE_DMA)
				EVE_dma_buffer[EVE_dma_buffer_index++] = data;
			#else
				spi_transmit_32(data);
			#endif
		}

		static inline uint8_t spi_receive(uint8_t data)
		{
			EVE_SPI->SPI.DATA.reg = data;
			while((EVE_SPI->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) == 0);
			return EVE_SPI->SPI.DATA.reg;
		}

		static inline uint8_t fetch_flash_byte(const uint8_t *data)
		{
			return *data;
		}

		#endif /* SAMC2x / SAMx5x */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

		#if defined (__riscv)

//		#warning Compiling for GD32VF103CBT6

		#include "gd32vf103.h"

		static inline void DELAY_MS(uint16_t val)
		{
			uint16_t counter;

			while(val > 0)
			{
				for(counter=0; counter < 18000;counter++) /* ~1ms at 108MHz Core-Clock, according to my Logic-Analyzer */
				{
					__asm__ volatile ("nop");
				}
				val--;
			}
		}

		static inline void EVE_pdn_set(void)
		{
			gpio_bit_reset(GPIOB,GPIO_PIN_1);
		}

		static inline void EVE_pdn_clear(void)
		{
			gpio_bit_set(GPIOB,GPIO_PIN_1);
		}

		static inline void EVE_cs_set(void)
		{
			gpio_bit_reset(GPIOB,GPIO_PIN_0);
		}

		static inline void EVE_cs_clear(void)
		{
			gpio_bit_set(GPIOB,GPIO_PIN_0);
		}

		static inline void spi_transmit(uint8_t data)
		{
				SPI_DATA(SPI0) = (uint32_t) data;
				while(SPI_STAT(SPI0) & SPI_STAT_TRANS);
		}

		static inline void spi_transmit_32(uint32_t data)
		{
			spi_transmit((uint8_t)(data));
			spi_transmit((uint8_t)(data >> 8));
			spi_transmit((uint8_t)(data >> 16));
			spi_transmit((uint8_t)(data >> 24));
		}

		/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
		static inline void spi_transmit_burst(uint32_t data)
		{
			spi_transmit_32(data);
		}

		static inline uint8_t spi_receive(uint8_t data)
		{
				SPI_DATA(SPI0) = (uint32_t) data;
				while(SPI_STAT(SPI0) & SPI_STAT_TRANS);
				return (uint8_t) SPI_DATA(SPI0);
		}

		static inline uint8_t fetch_flash_byte(const uint8_t *data)
		{
			return *data;
		}

		#endif /* __riscv */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

		#if defined (STM32L073xx) || (STM32F1) || (STM32F207xx) || (STM32F3) || (STM32F4)

		#if defined (STM32L073xx) /* set by PlatformIO board definition file nucleo_l073z.json */
		#include "stm32l0xx.h"
		#include "stm32l0xx_hal.h"
		#include "stm32l0xx_ll_spi.h"
		#endif

		#if defined (STM32F1) /* set by PlatformIO board definition file genericSTM32F103C8.json */
		#include "stm32f1xx.h"
		#include "stm32f1xx_hal.h"
		#include "stm32f1xx_ll_spi.h"
		#endif

		#if defined (STM32F207xx) /* set by PlatformIO board definition file nucleo_f207zg.json */
		#include "stm32f2xx.h"
		#include "stm32f2xx_hal.h"
		#include "stm32f2xx_ll_spi.h"
		#endif

		#if defined (STM32F3) /* set by PlatformIO board definition file genericSTM32F303CB.json */
		#include "stm32f3xx.h"
		#include "stm32f3xx_hal.h"
		#include "stm32f3xx_ll_spi.h"
		#endif

		#if defined (STM32F4) /* set by PlatformIO board definition file genericSTM32F407VET6.json */
		#include "stm32f4xx.h"
		#include "stm32f4xx_hal.h"
		#include "stm32f4xx_ll_spi.h"
		#endif


		#define EVE_CS_PORT GPIOD
		#define EVE_CS GPIO_PIN_12
		#define EVE_PDN_PORT GPIOD
		#define EVE_PDN GPIO_PIN_13
		#define EVE_SPI SPI1
		#define EVE_DMA_INSTANCE DMA2
		#define EVE_DMA_CHANNEL 3
		#define EVE_DMA_STREAM 3
//		#define EVE_DMA		/* do not active, it is not working yet */

		#if defined (EVE_DMA)
			extern uint32_t EVE_dma_buffer[1025];
			extern volatile uint16_t EVE_dma_buffer_index;
			extern volatile uint8_t EVE_dma_busy;

			void EVE_init_dma(void);
			void EVE_start_dma_transfer(void);
		#endif

		#define DELAY_MS(ms) HAL_Delay(ms)

		static inline void EVE_pdn_clear(void)
		{
			HAL_GPIO_WritePin(EVE_PDN_PORT, EVE_PDN, GPIO_PIN_SET);
		}

		static inline void EVE_pdn_set(void)
		{
			HAL_GPIO_WritePin(EVE_PDN_PORT, EVE_PDN, GPIO_PIN_RESET);
		}

		static inline void EVE_cs_clear(void)
		{
			HAL_GPIO_WritePin(EVE_CS_PORT, EVE_CS, GPIO_PIN_SET);
		}

		static inline void EVE_cs_set(void)
		{
			HAL_GPIO_WritePin(EVE_CS_PORT, EVE_CS, GPIO_PIN_RESET);
		}

		static inline void spi_transmit(uint8_t data)
		{
			LL_SPI_TransmitData8(EVE_SPI, data);
			while(!LL_SPI_IsActiveFlag_TXE(EVE_SPI));
			while(!LL_SPI_IsActiveFlag_RXNE(EVE_SPI));
			LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXNE */
		}

		static inline void spi_transmit_32(uint32_t data)
		{
			spi_transmit((uint8_t)(data));
			spi_transmit((uint8_t)(data >> 8));
			spi_transmit((uint8_t)(data >> 16));
			spi_transmit((uint8_t)(data >> 24));
		}

		/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
		static inline void spi_transmit_burst(uint32_t data)
		{
			#if defined (EVE_DMA)
				EVE_dma_buffer[EVE_dma_buffer_index++] = data;
			#else
				spi_transmit_32(data);
			#endif
		}

		static inline uint8_t spi_receive(uint8_t data)
		{
			LL_SPI_TransmitData8(EVE_SPI, data);
			while(!LL_SPI_IsActiveFlag_TXE(EVE_SPI));
			while(!LL_SPI_IsActiveFlag_RXNE(EVE_SPI));
			return LL_SPI_ReceiveData8(EVE_SPI);
		}

		static inline uint8_t fetch_flash_byte(const uint8_t *data)
		{
			return *data;
		}

		#endif  /* STM32 */

	#endif /* __GNUC__ */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #if defined (__TI_ARM__)

        #if defined (__MSP432P401R__)

        #include <ti/devices/msp432p4xx/inc/msp.h>
        #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
        #include <stdint.h>

        #define RIVERDI_PORT GPIO_PORT_P1
        #define RIVERDI_SIMO BIT6   // P1.6
        #define RIVERDI_SOMI BIT7   // P1.7
        #define RIVERDI_CLK BIT5    // P1.5
        #define EVE_CS_PORT         GPIO_PORT_P5
        #define EVE_CS              GPIO_PIN0           //P5.0
        #define EVE_PDN_PORT        GPIO_PORT_P5
        #define EVE_PDN             GPIO_PIN1           //P5.1

        void EVE_SPI_Init(void);

        static inline void DELAY_MS(uint16_t val)
        {
            uint16_t counter;

            while(val > 0)
            {
                for(counter=0; counter < 8000;counter++) // ~1ms at 48MHz Core-Clock
                {
                    __nop();
                }
                val--;
            }
        }

        static inline void EVE_pdn_set(void)
        {
//            GPIO_setOutputLowOnPin(EVE_PDN_PORT,EVE_PDN);   /* Power-Down low */
            P5OUT &= ~EVE_PDN;   /* Power-Down low */
        }

        static inline void EVE_pdn_clear(void)
        {
//            GPIO_setOutputHighOnPin(EVE_PDN_PORT,EVE_PDN);   /* Power-Down high */
            P5OUT |= EVE_PDN;    /* Power-Down high */
        }

        static inline void EVE_cs_set(void)
        {
//            GPIO_setOutputLowOnPin(EVE_CS_PORT,EVE_CS);   /* CS low */
            P5OUT &= ~EVE_CS;   /* CS low */
        }

        static inline void EVE_cs_clear(void)
        {
//            GPIO_setOutputHighOnPin(EVE_CS_PORT,EVE_CS);    /* CS high */
            P5OUT |= EVE_CS;    /* CS high */
        }

        static inline void spi_transmit(uint8_t data)
        {
//            SPI_transmitData(EUSCI_B0_BASE, data);
//            while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT)));

            UCB0TXBUF_SPI = data;
            while(!(UCB0IFG_SPI & UCTXIFG)); /* wait for transmission to complete */
        }

		static inline void spi_transmit_32(uint32_t data)
		{
			spi_transmit((uint8_t)(data));
			spi_transmit((uint8_t)(data >> 8));
			spi_transmit((uint8_t)(data >> 16));
			spi_transmit((uint8_t)(data >> 24));
		}

		/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
		static inline void spi_transmit_burst(uint32_t data)
		{
			#if defined (EVE_DMA)
				EVE_dma_buffer[EVE_dma_buffer_index++] = data;
			#else
				spi_transmit_32(data);
			#endif
		}

        static inline uint8_t spi_receive(uint8_t data)
        {
//            SPI_transmitData(EUSCI_B0_BASE, data);
//            while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
//            return EUSCI_B_CMSIS(EUSCI_B0_BASE)->RXBUF;

            UCB0TXBUF_SPI = data;
            while(!(UCB0IFG_SPI & UCTXIFG)); /* wait for transmission to complete */
            return UCB0RXBUF_SPI;
         }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif /* __MSP432P401R__ */

    #endif /* __TI_ARM */
#endif

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

#if defined (ARDUINO)

	#include <Arduino.h>
	#include <stdio.h>
	#include <SPI.h>

	#if defined (ESP32)
		#define EVE_CS 		13
		#define EVE_PDN		12
		#define EVE_SCK		18
		#define EVE_MISO	19
		#define EVE_MOSI	23

		#define EVE_DMA

		#if defined (EVE_DMA)
			extern uint32_t EVE_dma_buffer[1025];
			extern volatile uint16_t EVE_dma_buffer_index;
			extern volatile uint8_t EVE_dma_busy;
		#endif
	#else
		#define EVE_CS 		9
		#define EVE_PDN		8
	#endif

	#define DELAY_MS(ms) delay(ms)

	#if defined (ESP8266)

	#endif

	#if	defined (__AVR__)
		#include <avr/pgmspace.h>
	#endif


	static inline void EVE_pdn_set(void)
	{
		digitalWrite(EVE_PDN, LOW);	/* Power-Down low */
	}

	static inline void EVE_pdn_clear(void)
	{
		digitalWrite(EVE_PDN, HIGH);	/* Power-Down high */
	}

	static inline void EVE_cs_set(void)
	{
		SPI.setDataMode(SPI_MODE0);
		digitalWrite(EVE_CS, LOW);
	}

	static inline void EVE_cs_clear(void)
	{
		digitalWrite(EVE_CS, HIGH);
	}

	#if defined (ESP8266) || (ESP32)
		static inline void spi_transmit(uint8_t data)
		{
			SPI.write(data);
		}
	#else
		static inline void spi_transmit(uint8_t data)
		{
			SPI.transfer(data);
		}
	#endif

	static inline void spi_transmit_32(uint32_t data)
	{
	#if defined (ESP32)
#if 0
		spi_transmit((uint8_t)(data));
		spi_transmit((uint8_t)(data >> 8));
		spi_transmit((uint8_t)(data >> 16));
		spi_transmit((uint8_t)(data >> 24));
#endif

#if 0
		uint8_t buffer[4];
		buffer[0] = (uint8_t)(data);
		buffer[1] = (uint8_t)(data >> 8);
		buffer[2] = (uint8_t)(data >> 16);
		buffer[3] = (uint8_t)(data >> 24);
		SPI.writeBytes(buffer, 4);
//		SPI.transfer(buffer, 4);
#endif

#if 1
		uint32_t swap;
		swap = ((uint8_t)(data >> 24)) + ((data >> 8) & 0xff00) + ((data << 8) &0xff0000) + ((data << 24) & 0xff000000); /* we need a different byte-order */
		SPI.write32(swap);
#endif

	#else
		spi_transmit((uint8_t)(data));
		spi_transmit((uint8_t)(data >> 8));
		spi_transmit((uint8_t)(data >> 16));
		spi_transmit((uint8_t)(data >> 24));
	#endif
	}

#if defined (EVE_DMA)
/*
note: this is not using DMA!
as a quick and easy optimisation this makes use of the existing DMA support to
write a display list in a buffer and send this buffer out as one big chunk of data
this is what the Arduino implementation for ESP32 directly supports
work in progress...
*/
	static inline void EVE_init_dma(void)
	{
	}

	static inline void EVE_start_dma_transfer(void)
	{
		uint8_t buffer[3];
		buffer[0] = (uint8_t)(EVE_dma_buffer[0] >> 8) ;
		buffer[1] = (uint8_t)(EVE_dma_buffer[0] >> 16);
		buffer[2] = (uint8_t)(EVE_dma_buffer[0] >> 24);

		EVE_cs_set();
		SPI.writeBytes(buffer, 3);
		SPI.writeBytes((uint8_t *) &EVE_dma_buffer[1], (EVE_dma_buffer_index-1)*4);
		EVE_cs_clear();
	}
#endif

	/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
	static inline void spi_transmit_burst(uint32_t data)
	{
		#if defined (EVE_DMA)
			EVE_dma_buffer[EVE_dma_buffer_index++] = data;
		#else
			spi_transmit_32(data);
		#endif
	}

	static inline uint8_t spi_receive(uint8_t data)
	{
		return SPI.transfer(data);
	}

	static inline uint8_t fetch_flash_byte(const uint8_t *data)
	{
		#if	defined (__AVR__)
			#if defined(RAMPZ)
				return(pgm_read_byte_far(data));
			#else
				return(pgm_read_byte_near(data));
			#endif
		#else /* this may fail on your Arduino system that is not AVR and that I am not aware of */
			return *data;
		#endif
	}

#endif /* Arduino */


#endif /* EVE_TARGET_H_ */
