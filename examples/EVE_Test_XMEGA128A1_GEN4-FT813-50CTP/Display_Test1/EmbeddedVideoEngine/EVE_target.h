/*
@file    EVE_target.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2022-04-23
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2022 Rudolph Riedel

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
- added a couple of measures to speed up things for Arduino-ESP32
- stretched out the different Arduino targets, more difficult to maintain but easier to read
- sped up ARDUINO_AVR_UNO a little by making spi_transmit() native and write only and by direct writes of EVE_CS
- reverted the chip-select optimisation for ARDUINO_AVR_UNO to avoid confusion, left in the code but commented-out
- sped up ESP8266 by using 32 bit transfers for spi_transmit_32()
- added DMA to ARDUINO_METRO_M4 target
- added a STM32 target: ARDUINO_NUCLEO_F446RE
- added DMA to ARDUINO_NUCLEO_F446RE target
- added DMA to Arduino-ESP32 target
- Bugfix: the generic Arduino target was missing EVE_cs_set() / EVE_cs_clear()
- added a native ESP32 target with DMA
- missing note: Robert S. added an AVR XMEGA target by pull-request on Github
- added an experimental ARDUINO_TEENSY41 target with DMA support - I do not have any Teensy to test this with
- added a target for the Raspberry Pi Pico - RP2040
- added a target for Arduino-BBC_MICROBIT_V2
- activated DMA for the Raspberry Pi Pico - RP2040
- added ARDUINO_TEENSY35 to the experimental ARDUINO_TEENSY41 target
- transferred the little experimental STM32 code I had over from my experimental branch
- added S32K144 support including DMA
- modified the Arduino targets to use C++ wrapper functions
- fixed a few CERT warnings
- added an Arduino XMC1100_XMC2GO target
- changed ATSAM defines so that they can be defined outside the module
- started to add a target for NXPs K32L2B3
- converted all TABs to SPACEs
- added a few lines for STM32H7
- made the pin defines for all targets that have one optional
- split the ATSAMC21 and ATSAMx51 targets into separate sections
- updated the explanation of how DMA works
- added a TMS320F28335 target
- added more defines for ATSAMC21 and ATSAMx51 - chip crises...

*/

#ifndef EVE_TARGET_H
#define EVE_TARGET_H

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
  EVE_busy() does nothing but to report that EVE is busy if there is an active DMA transfer.
  At the end of the DMA transfer an IRQ is executed which clears the DMA active state and calls EVE_cs_clear() by which the
  command buffer is executed by the command co-processor.

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
                    for(counter=0; counter < 2000;counter++) /* maybe ~1ms at 16MHz clock */
                    {
                        __asm__ volatile ("nop");
                    }
                    val--;
                }
            }

            #if !defined (EVE_CS)
                #define EVE_CS_PORT PORTB
                #define EVE_CS      (1<<PB5)
                #define EVE_PDN_PORT    PORTB
                #define EVE_PDN     (1<<PB4)
            #endif

            static inline void EVE_pdn_set(void)
            {
                EVE_PDN_PORT &= ~EVE_PDN;   /* Power-Down low */
            }

            static inline void EVE_pdn_clear(void)
            {
                EVE_PDN_PORT |= EVE_PDN;    /* Power-Down high */
            }

            static inline void EVE_cs_set(void)
            {
                EVE_CS_PORT &= ~EVE_CS; /* cs low */
            }

            static inline void EVE_cs_clear(void)
            {
                EVE_CS_PORT |= EVE_CS;  /* cs high */
            }

            static inline void spi_transmit(uint8_t data)
            {
                SPDR = data; /* start transmission */
                while(!(SPSR & (1<<SPIF))) {}; /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
            }

            static inline void spi_transmit_32(uint32_t data)
            {
                spi_transmit((uint8_t)(data & 0x000000ff));
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
                while(!(SPSR & (1<<SPIF))) {}; /* wait for transmission to complete - 1us @ 8MHz SPI-CLock */
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

        #if defined (__AVR_XMEGA__)

        #include <avr/io.h>
        #include <avr/pgmspace.h>
        #ifndef F_CPU
            #define F_CPU 32000000UL
        #endif      
        #include <util/delay.h>

        #define DELAY_MS(ms) _delay_ms(ms)

        #if !defined (EVE_CS)
            #define EVE_CS_PORT     PORTC
            #define EVE_CS          PIN4_bm
            #define EVE_PDN_PORT    PORTC
            #define EVE_PDN_PIN     PIN1_bm
            #define EVE_SPI         SPIC    /* the SPI port to use */
        #endif

        static inline void EVE_pdn_set(void)
        {
            EVE_PDN_PORT.OUTCLR = EVE_PDN_PIN;  /* Power-Down low */    
        }

        static inline void EVE_pdn_clear(void)
        {
            EVE_PDN_PORT.OUTSET = EVE_PDN_PIN;  /* Power-Down high */
        }

        static inline void EVE_cs_set(void)
        {
            EVE_CS_PORT.OUTCLR = EVE_CS;    /* cs low */
        }

        static inline void EVE_cs_clear(void)
        {
            EVE_CS_PORT.OUTSET= EVE_CS;     /* cs high */
        }

        static inline void spi_transmit(uint8_t data)
        {
            EVE_SPI.DATA = data;
            while(!(EVE_SPI.STATUS & 0x80)) {}; /* wait for transmit complete */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            EVE_SPI.DATA = data;
            while(!(EVE_SPI.STATUS & 0x80)) {}; /* wait for transmit complete */
            return EVE_SPI.DATA;
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            #if defined (__AVR_HAVE_ELPM__) /* we have an AVR with more than 64kB FLASH memory */
            return(pgm_read_byte_far(data));
            #else
            return(pgm_read_byte_near(data));
            #endif
        }

        #endif /* XMEGA */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (__AVR__) && ! defined (__AVR_XMEGA__)

        #include <avr/io.h>
        #include <avr/pgmspace.h>
        #define F_CPU 16000000UL
        #include <util/delay.h>

        #define DELAY_MS(ms) _delay_ms(ms)

        #if !defined (EVE_CS)
            #define EVE_CS_PORT PORTB
            #define EVE_CS      (1<<PB5)
            #define EVE_PDN_PORT    PORTB
            #define EVE_PDN     (1<<PB4)
        #endif

        static inline void EVE_pdn_set(void)
        {
            EVE_PDN_PORT &= ~EVE_PDN;   /* Power-Down low */
        }

        static inline void EVE_pdn_clear(void)
        {
            EVE_PDN_PORT |= EVE_PDN;    /* Power-Down high */
        }

        static inline void EVE_cs_set(void)
        {
            EVE_CS_PORT &= ~EVE_CS; /* cs low */
        }

        static inline void EVE_cs_clear(void)
        {
            EVE_CS_PORT |= EVE_CS;  /* cs high */
        }

        static inline void spi_transmit(uint8_t data)
        {
#if 1
            SPDR = data; /* start transmission */
            while(!(SPSR & (1<<SPIF))) {}; /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
#else
            /* software-spi example */
            uint8_t spiIndex  = 0x80;
            uint8_t k;

            for(k = 0; k <8; k++) /* Output each bit of spiOutByte */
            {
                if(data & spiIndex) /* Output MOSI Bit */
                {
                    PORTC |= (1<<PORTC1);
                }
                else
                {
                    PORTC &= ~(1<<PORTC1);
                }

                PORTA |= (1<<PORTA1); /* toggle SCK */
                PORTA &= ~(1<<PORTA1);

                spiIndex >>= 1;
            }
#endif
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            while(!(SPSR & (1<<SPIF))) {}; /* wait for transmission to complete - 1us @ 8MHz SPI-CLock */
            return SPDR;
#else
            uint8_t spiIndex  = 0x80;
            uint8_t spiInByte = 0;
            uint8_t k;

            for(k = 0; k <8; k++) /* Output each bit of spiOutByte */
            {
                if(data & spiIndex) /* Output MOSI Bit */
                {
                    PORTC |= (1<<PORTC1);
                }
                else
                {
                    PORTC &= ~(1<<PORTC1);
                }

                PORTA |= (1<<PORTA1); /* toggle SCK */
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
            #if defined (__AVR_HAVE_ELPM__) /* we have an AVR with more than 64kB FLASH memory */
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

        #define DELAY_MS(ms)    OS_Wait(ms * 1000)

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
            CSIH0TX0H = data;   /* start transmission */
            while(CSIH0STR0 & 0x00080) {};  /* wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            CSIH0TX0H = data;   /* start transmission */
            while(CSIH0STR0 & 0x00080) {};  /* wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
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
            spi_transmit((uint8_t)(data & 0x000000ff));
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

         #if defined (__SAMC21E18A__) || (__SAMC21J18A__) || (__SAMC21J17A__) || (__SAMC21J16A__)
        /* note: target as set by AtmelStudio, valid  are all from the same family */

        #include "sam.h"

        #if !defined (EVE_CS)
            #define EVE_CS_PORT 0
            #define EVE_CS PORT_PA05
            #define EVE_PDN_PORT 0
            #define EVE_PDN PORT_PA03
            #define EVE_SPI SERCOM0
            #define EVE_SPI_DMA_TRIGGER SERCOM0_DMAC_ID_TX
            #define EVE_DMA_CHANNEL 0
//          #define EVE_DMA
        #endif

        #define EVE_DELAY_1MS 8000  /* ~1ms at 48MHz Core-Clock */

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
            while((EVE_SPI->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) == 0) {};
            (void) EVE_SPI->SPI.DATA.reg; /* dummy read-access to clear SERCOM_SPI_INTFLAG_RXC */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            while((EVE_SPI->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) == 0) {};
            return EVE_SPI->SPI.DATA.reg;
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif /* SAMC2x */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (__SAME51J19A__) || (__SAME51J18A__) || (__SAMD51P20A__) || (__SAMD51J19A__) || (__SAMD51G18A__)
        /* note: target as set by AtmelStudio, valid  are all from the same family */

        #include "sam.h"

        #if !defined (EVE_CS)
            #define EVE_CS_PORT 0
            #define EVE_CS PORT_PA00
            #define EVE_PDN_PORT 0
            #define EVE_PDN PORT_PA01
            #define EVE_SPI SERCOM5
            #define EVE_SPI_DMA_TRIGGER SERCOM5_DMAC_ID_TX
            #define EVE_DMA_CHANNEL 0
//          #define EVE_DMA
        #endif

        #define EVE_DELAY_1MS 20000 /* ~1ms at 120MHz Core-Clock and activated cache, according to my Logic-Analyzer */

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
            while((EVE_SPI->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) == 0) {};
            (void) EVE_SPI->SPI.DATA.reg; /* dummy read-access to clear SERCOM_SPI_INTFLAG_RXC */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            while((EVE_SPI->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC) == 0) {};
            return EVE_SPI->SPI.DATA.reg;
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif /* SAMx5x */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (__riscv)

//      #warning Compiling for GD32VF103CBT6

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
                while(SPI_STAT(SPI0) & SPI_STAT_TRANS) {};
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
                while(SPI_STAT(SPI0) & SPI_STAT_TRANS) {};
                return (uint8_t) SPI_DATA(SPI0);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif /* __riscv */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        /* set with "build_flags" in platformio.ini or as defines in your build environment */
        #if defined (STM32L0) || (STM32F0) || (STM32F1) || (STM32F3) || (STM32F4) || (STM32G4) || (STM32H7)

        #if defined (STM32L0) /* set with "build_flags = -D STM32L0" in platformio.ini */
        #include "stm32l0xx.h"
        #include "stm32l0xx_hal.h"
        #include "stm32l0xx_ll_spi.h"
        #endif

        #if defined (STM32F0) /* set with "build_flags = -D STM32F0" in platformio.ini */
        #include "stm32f0xx.h"
        #include "stm32f0xx_hal.h"
        #include "stm32f0xx_ll_spi.h"
        #endif

        #if defined (STM32F1) /* set with "build_flags = -D STM32F1" in platformio.ini */
        #include "stm32f1xx.h"
        #include "stm32f1xx_hal.h"
        #include "stm32f1xx_ll_spi.h"
        #endif

        #if defined (STM32F3) /* set with "build_flags = -D STM32F3" in platformio.ini */
        #include "stm32f3xx.h"
        #include "stm32f3xx_hal.h"
        #include "stm32f3xx_ll_spi.h"
        #endif

        #if defined (STM32F4) /* set with "build_flags = -D STM32F4" in platformio.ini */
        #include "stm32f4xx.h"
        #include "stm32f4xx_hal.h"
        #include "stm32f4xx_ll_spi.h"
        #endif

        #if defined (STM32G4) /* set with "build_flags = -D STM32G4" in platformio.ini */
        #include "stm32g4xx.h"
        #include "stm32g4xx_hal.h"
        #include "stm32g4xx_ll_spi.h"
        #endif

        #if defined (STM32H7) /* set with "build_flags = -D STM32H7" in platformio.ini */
        #include "stm32h7xx.h"
        #include "stm32h7xx_hal.h"
        #include "stm32h7xx_ll_spi.h"
        #endif

        #if !defined (EVE_CS)
            #define EVE_CS_PORT GPIOD
            #define EVE_CS GPIO_PIN_12
            #define EVE_PDN_PORT GPIOD
            #define EVE_PDN GPIO_PIN_13
            #define EVE_SPI SPI1
            #define EVE_DMA_INSTANCE DMA2
            #define EVE_DMA_CHANNEL 3
            #define EVE_DMA_STREAM 3
    //      #define EVE_DMA     /* do not activate, it is not working yet */
        #endif

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

    #if defined (STM32H7)
        static inline void spi_transmit(uint8_t data)
        {
            LL_SPI_TransmitData8(EVE_SPI, data);
            while(!LL_SPI_IsActiveFlag_TXP(EVE_SPI)) {};
            while(!LL_SPI_IsActiveFlag_RXWNE(EVE_SPI)) {};
            LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXWNE */
        }
    #else
        static inline void spi_transmit(uint8_t data)
        {
            LL_SPI_TransmitData8(EVE_SPI, data);
            while(!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {};
            while(!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {};
            LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXNE */
        }
    #endif

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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

    #if defined (STM32H7)
        static inline uint8_t spi_receive(uint8_t data)
        {
            LL_SPI_TransmitData8(EVE_SPI, data);
            while(!LL_SPI_IsActiveFlag_TXP(EVE_SPI)) {};
            while(!LL_SPI_IsActiveFlag_RXWNE(EVE_SPI)) {};
            return LL_SPI_ReceiveData8(EVE_SPI);
        }
    #else
        static inline uint8_t spi_receive(uint8_t data)
        {
            LL_SPI_TransmitData8(EVE_SPI, data);
            while(!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {};
            while(!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {};
            return LL_SPI_ReceiveData8(EVE_SPI);
        }
    #endif

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif  /* STM32 */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (ESP_PLATFORM)

        #include "driver/spi_master.h"
        #include "driver/gpio.h"
        #include "freertos/task.h"

        #if !defined (EVE_CS)
            #define EVE_CS      GPIO_NUM_13
            #define EVE_PDN     GPIO_NUM_12
            #define EVE_SCK     GPIO_NUM_18
            #define EVE_MISO    GPIO_NUM_19
            #define EVE_MOSI    GPIO_NUM_23
        #endif

        extern spi_device_handle_t EVE_spi_device;
        extern spi_device_handle_t EVE_spi_device_simple;

        #define EVE_DMA

        void DELAY_MS(uint16_t ms);

        void EVE_init_spi(void);

        static inline void EVE_cs_set(void)
        {
            spi_device_acquire_bus(EVE_spi_device_simple, portMAX_DELAY);
            gpio_set_level(EVE_CS, 0);
        }

        static inline void EVE_cs_clear(void)
        {
            gpio_set_level(EVE_CS, 1);
            spi_device_release_bus(EVE_spi_device_simple);
        }

        static inline void EVE_pdn_set(void)
        {
            gpio_set_level(EVE_PDN, 0);
        }

        static inline void EVE_pdn_clear(void)
        {
            gpio_set_level(EVE_PDN, 1);
        }

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void spi_transmit(uint8_t data)
        {
            spi_transaction_t trans = {0};
            trans.length = 8;
            trans.rxlength = 0;
            trans.flags = SPI_TRANS_USE_TXDATA;
            trans.tx_data[0] = data;
            spi_device_polling_transmit(EVE_spi_device_simple, &trans);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transaction_t trans = {0};
            trans.length = 32;
            trans.rxlength = 0;
            trans.flags = 0;
            trans.tx_buffer = &data;
            spi_device_polling_transmit(EVE_spi_device_simple, &trans);
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
            spi_transaction_t trans = {0};
            trans.length = 8;
            trans.rxlength = 8;
            trans.flags = (SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA);
            trans.tx_data[0] = data;
            spi_device_polling_transmit(EVE_spi_device_simple, &trans);

            return trans.rx_data[0];
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

    #endif /* ESP_PLATFORM */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (RP2040)
        /* note: set in platformio.ini by "build_flags = -D RP2040" */

        #include "pico/stdlib.h"
        #include "hardware/spi.h"

        #if !defined (EVE_CS)
            #define EVE_CS      5
            #define EVE_PDN     6
            #define EVE_SCK     2
            #define EVE_MOSI    3
            #define EVE_MISO    4
            #define EVE_SPI spi0
            #define EVE_DMA
        #endif

        #define DELAY_MS(ms) sleep_ms(ms)

        void EVE_init_spi(void);

        static inline void EVE_cs_set(void)
        {
            gpio_put(EVE_CS, 0);
        }

        static inline void EVE_cs_clear(void)
        {
            gpio_put(EVE_CS, 1);
        }

        static inline void EVE_pdn_set(void)
        {
            gpio_put(EVE_PDN, 0);
        }

        static inline void EVE_pdn_clear(void)
        {
            gpio_put(EVE_PDN, 1);
        }

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void spi_transmit(uint8_t data)
        {
            spi_write_blocking(EVE_SPI, &data, 1);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_write_blocking(EVE_SPI, (uint8_t *) &data, 4);
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
            uint8_t result;

            spi_write_read_blocking(EVE_SPI, &data, &result, 1);
            return result;
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

    #endif /* RP2040 */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (CPU_S32K148) || (CPU_S32K144HFT0VLLT)

        #if defined (CPU_S32K144HFT0VLLT)
        #include "S32K144.h"
        #endif

        #if defined (CPU_S32K148)
        #include "S32K148.h"
        #endif

        #include <stdint.h>

        #if !defined (EVE_CS)
            #define EVE_CS 5
            #define EVE_CS_GPIO PTB

            #define EVE_PDN 14
            #define EVE_PDN_GPIO PTD

            /* LPSPI0 on J2 header: PTB2 = SCK, PTB3 = MISO, PTB4 = MOSI */
            #define EVE_SPI LPSPI0
            #define EVE_SPI_INDEX PCC_LPSPI0_INDEX

            //#define EVE_DELAY_1MS 15000   /* maybe ~1ms at 112MHz Core-Clock */
            #define EVE_DELAY_1MS 5300  /* maybe ~1ms at 48MHz Core-Clock */

            #define EVE_DMA
        #endif

        void DELAY_MS(uint16_t val);
        void EVE_init_spi(void);

        static inline void EVE_cs_set(void)
        {
            EVE_CS_GPIO->PCOR = (1 << EVE_CS); /* set CS low */
        }

        static inline void EVE_cs_clear(void)
        {
            EVE_CS_GPIO->PSOR = (1 << EVE_CS); /* set CS high */
        }

        static inline void EVE_pdn_set(void)
        {
            EVE_PDN_GPIO->PCOR = (1 << EVE_PDN); /* set PDN low */
        }

        static inline void EVE_pdn_clear(void)
        {
            EVE_PDN_GPIO->PSOR = (1 << EVE_PDN); /* set PDN high */
        }

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            #define EVE_DMA_CHANNEL 0
            #define EVE_DMAMUX_CHCFG_SOURCE 15 /* this needs to be the EDMA_REQ_LPSPIx_TX */
            #define EVE_DMA_IRQ  DMA0_IRQn
            #define EVE_DMA_IRQHandler DMA0_IRQHandler

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void spi_transmit(uint8_t data)
        {
            EVE_SPI->SR |= LPSPI_SR_RDF_MASK; /* clear Receive Data Flag */
            EVE_SPI->TDR = data; /* transmit data */
            while((EVE_SPI->SR & LPSPI_SR_RDF_MASK) == 0);
            (void) EVE_SPI->RDR; /* dummy read-access to clear Receive Data Flag */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            EVE_SPI->SR |= LPSPI_SR_RDF_MASK; /* clear Receive Data Flag */
            EVE_SPI->TDR = data; /* transmit data */
            while((EVE_SPI->SR & LPSPI_SR_RDF_MASK) == 0);
            return EVE_SPI->RDR;
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif /* S32K14x */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
        #if defined (CPU_K32L2B31VLH0A)

        #include <stdint.h>
        #include "K32L2B31A.h"
        #include "fsl_gpio.h"
        #include "fsl_spi.h"

        #if !defined (EVE_CS)
            #define EVE_CS 4
            #define EVE_CS_GPIO GPIOD
            #define EVE_PDN 2
            #define EVE_PDN_GPIO GPIOD
            #define EVE_SPI SPI1
            #define EVE_DELAY_1MS 8000  /* ~1ms at 48MHz Core-Clock */
        #endif

        static inline void DELAY_MS(uint16_t val)
        {
            uint16_t counter;

            while(val > 0)
            {
                for(counter=0; counter < EVE_DELAY_1MS;counter++)
                {
                    __asm__ volatile ("nop");
                }
                val--;
            }
        }

        //void DELAY_MS(uint16_t val);
        //void EVE_init_spi(void);
        //#define EVE_DMA

        static inline void EVE_cs_set(void)
        {
            GPIO_PortClear(EVE_CS_GPIO, 1u << EVE_CS); /* set CS low */
        }

        static inline void EVE_cs_clear(void)
        {
            GPIO_PortSet(EVE_CS_GPIO, 1u << EVE_CS); /* set CS high */
        }

        static inline void EVE_pdn_set(void)
        {
            GPIO_PortClear(EVE_PDN_GPIO, 1u << EVE_PDN); /* set PDN low */
        }

        static inline void EVE_pdn_clear(void)
        {
            GPIO_PortSet(EVE_PDN_GPIO, 1u << EVE_PDN); /* set PDN high */
        }

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            #define EVE_DMA_CHANNEL 0
            #define EVE_DMAMUX_CHCFG_SOURCE 15 /* this needs to be the EDMA_REQ_LPSPIx_TX */
            #define EVE_DMA_IRQ  DMA0_IRQn
            #define EVE_DMA_IRQHandler DMA0_IRQHandler

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void spi_transmit(uint8_t data)
        {
#if 1
            while((EVE_SPI->S & SPI_S_SPTEF_MASK) == 0) {}
            EVE_SPI->DL = data;
            while((EVE_SPI->S & SPI_S_SPTEF_MASK) == 0) {}
            while((EVE_SPI->S & SPI_S_SPRF_MASK) == 0) {}
            (void) EVE_SPI->DL;
#else
            while((SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag) == 0) {}
            SPI_WriteData(EVE_SPI, data);
            while((SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag) == 0) {}
            while((SPI_GetStatusFlags(EVE_SPI) & kSPI_RxBufferFullFlag) == 0U)  {}
            (void) SPI_ReadData(EVE_SPI);
#endif
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
#if 0
            while((EVE_SPI->S & SPI_S_SPTEF_MASK) == 0) {}
            EVE_SPI->DL = data;
            while((EVE_SPI->S & SPI_S_SPTEF_MASK) == 0) {}
            while((EVE_SPI->S & SPI_S_SPRF_MASK) == 0) {}
            return EVE_SPI->DL;
#else
            while((SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag) == 0) {}
            SPI_WriteData(EVE_SPI, data);
            while((SPI_GetStatusFlags(EVE_SPI) & kSPI_TxBufferEmptyFlag) == 0) {}
            while((SPI_GetStatusFlags(EVE_SPI) & kSPI_RxBufferFullFlag) == 0U)  {}
            return SPI_ReadData(EVE_SPI);
#endif
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

        #endif /* K32L2B3 */


    #endif /* __GNUC__ */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #if defined (__TI_ARM__)

        #if defined (__MSP432P401R__)

        #include <ti/devices/msp432p4xx/inc/msp.h>
        #include <ti/devices/msp432p4xx/driverlib/driverlib.h>
        #include <stdint.h>

        #if !defined (EVE_CS)
            #define RIVERDI_PORT GPIO_PORT_P1
            #define RIVERDI_SIMO BIT6   /* P1.6 */
            #define RIVERDI_SOMI BIT7   /* P1.7 */
            #define RIVERDI_CLK BIT5    /* P1.5 */
            #define EVE_CS_PORT         GPIO_PORT_P5
            #define EVE_CS              GPIO_PIN0     /* P5.0 */
            #define EVE_PDN_PORT        GPIO_PORT_P5
            #define EVE_PDN             GPIO_PIN1     /* P5.1 */
        #endif

        void EVE_SPI_Init(void);

        static inline void DELAY_MS(uint16_t val)
        {
            uint16_t counter;

            while(val > 0)
            {
                for(counter=0; counter < 8000;counter++) /* ~1ms at 48MHz Core-Clock */
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
            while(!(UCB0IFG_SPI & UCTXIFG)) {}; /* wait for transmission to complete */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            while(!(UCB0IFG_SPI & UCTXIFG)) {}; /* wait for transmission to complete */
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

/* this is for TIs C2000 compiled with their ti-cgt-c2000 compiler which does not define this many symbols */
#if defined (__TMS320C28XX__)

    /* the designated target actually is a TMS320F28335 */
    /* credit for this goes to David Sakal-Sega */
	/* note: the SPI unit of the TMS320F28335 does not support DMA, using one of the UARTs in SPI mode would allow DMA */

    #include <stdint.h>
    #include <DSP2833x_Device.h>

    typedef uint_least8_t uint8_t;  /* this architecture does not actually know what a byte is, uint_least8_t is 16 bits wide */

    /* 150MHz -> 6.67ns per cycle, 5 cycles for the loop itself and 8 NOPs -> 1ms / (6.67ns * 13) = 11532 */
    #define EVE_DELAY_1MS 12000

    static inline void DELAY_MS(uint16_t val)
    {
        uint16_t counter;

        while(val > 0)
        {
            for(counter=0; counter < EVE_DELAY_1MS;counter++)
            {
                asm(" RPT #7 || NOP");
            }
            val--;
        }
    }

    static inline void EVE_pdn_set(void)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO14 = 1; /* Power-Down low */
    }

    static inline void EVE_pdn_clear(void)
    {
        GpioDataRegs.GPASET.bit.GPIO14 = 1; /* Power-Down high */
    }

    static inline void EVE_cs_set(void)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO19 = 1; /* CS low */
    }

    static inline void EVE_cs_clear(void)
    {
        asm(" RPT #60 || NOP"); /* wait 60 cycles to make sure CS is not going high too early */ 
        GpioDataRegs.GPASET.bit.GPIO19 = 1; /* CS high */
    }

    static inline void spi_transmit(uint8_t data)
    {
        SpiaRegs.SPITXBUF = (data & 0xFF) << 8; /* start transfer, looks odd with data = uint8_t but uint8_t actually is 16 bits wide on this controller */
        while(SpiaRegs.SPISTS.bit.INT_FLAG == 0); /* wait for transmission to complete */
        (void) SpiaRegs.SPIRXBUF; /* dummy read to clear the flags */
    }

    static inline void spi_transmit_32(uint32_t data)
    {
        spi_transmit((uint8_t)(data & 0x000000ff));
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
        SpiaRegs.SPITXBUF = (data & 0xFF) << 8; /* start transfer */
        while(SpiaRegs.SPISTS.bit.INT_FLAG == 0); /* wait for transmission to complete */
        return (SpiaRegs.SPIRXBUF & 0x00FF); /* data is right justified in SPIRXBUF */
    }

    static inline uint8_t fetch_flash_byte(const uint8_t *data)
    {
        return *data;
    }

#endif


/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

#if defined (ARDUINO)

    #include <Arduino.h>
    #include <stdio.h>
    #include "EVE_cpp_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

    #if defined (__AVR__)
//  #if defined (ARDUINO_AVR_UNO)
        #include <avr/pgmspace.h>

        #if !defined (EVE_CS)
            #define EVE_CS      10
            #define EVE_PDN     8
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
//          wrapper_spi_transmit(data);
            SPDR = data;
            asm volatile("nop");
            while (!(SPSR & (1<<SPIF)));
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            #if defined(RAMPZ)
                return(pgm_read_byte_far(data));
            #else
                return(pgm_read_byte_near(data));
            #endif
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #elif defined (ARDUINO_METRO_M4)

        #if !defined (EVE_CS)
            #define EVE_CS      10
            #define EVE_PDN     8
        #endif

        #define EVE_DMA

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
            wrapper_spi_transmit(data);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #elif defined (ARDUINO_NUCLEO_F446RE)
        #include "stm32f4xx_hal.h"
        #include "stm32f4xx_ll_spi.h"

        #if !defined (EVE_CS)
            #define EVE_CS      10
            #define EVE_PDN     8
            #define EVE_SPI SPI1
        #endif

        void EVE_init_spi(void);

        #define EVE_DMA

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
//          SPI.transfer(data);
            LL_SPI_TransmitData8(EVE_SPI, data);
            while(!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {};
            while(!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {};
            LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXNE */
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
//          return SPI.transfer(data);
            LL_SPI_TransmitData8(EVE_SPI, data);
            while(!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {};
            while(!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {};
            return LL_SPI_ReceiveData8(EVE_SPI);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #elif defined (ESP8266)

        #if !defined (EVE_CS)
            #define EVE_CS      D2  // D2 on D1 mini
            #define EVE_PDN     D1  // D1 on D1 mini
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
            wrapper_spi_transmit(data);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            wrapper_spi_transmit_32(__builtin_bswap32(data));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #elif defined (ESP32)
    /* note: this is using the ESP-IDF driver as the Arduino class and driver does not allow DMA for SPI */
        #include "driver/spi_master.h"

        #if !defined (EVE_CS)
            #define EVE_CS      13
            #define EVE_PDN     12
            #define EVE_SCK     18
            #define EVE_MISO    19
            #define EVE_MOSI    23
        #endif

        #define EVE_DMA

        void EVE_init_spi(void);

        extern spi_device_handle_t EVE_spi_device;
        extern spi_device_handle_t EVE_spi_device_simple;

        #if defined (EVE_DMA)
            extern uint32_t EVE_dma_buffer[1025];
            extern volatile uint16_t EVE_dma_buffer_index;
            extern volatile uint8_t EVE_dma_busy;

            void EVE_init_dma(void);
            void EVE_start_dma_transfer(void);
        #endif

        static inline void EVE_cs_set(void)
        {
            spi_device_acquire_bus(EVE_spi_device_simple, portMAX_DELAY);
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
            spi_device_release_bus(EVE_spi_device_simple);
        }

        static inline void spi_transmit(uint8_t data)
        {
            spi_transaction_t trans = {0};
            trans.length = 8;
            trans.rxlength = 0;
            trans.flags = SPI_TRANS_USE_TXDATA;
            trans.tx_data[0] = data;
            spi_device_polling_transmit(EVE_spi_device_simple, &trans);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transaction_t trans = {0};
            trans.length = 32;
            trans.rxlength = 0;
            trans.flags = 0;
            trans.tx_buffer = &data;
            spi_device_polling_transmit(EVE_spi_device_simple, &trans);
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
            spi_transaction_t trans = {0};
            trans.length = 8;
            trans.rxlength = 8;
            trans.flags = (SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA);
            trans.tx_data[0] = data;
            spi_device_polling_transmit(EVE_spi_device_simple, &trans);

            return trans.rx_data[0];
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #elif defined (ARDUINO_TEENSY41) || (ARDUINO_TEENSY35) /* note: this is mostly untested */

        #if !defined (EVE_CS)
            #define EVE_CS      10
            #define EVE_PDN     9
        #endif

        #define EVE_DMA

        #if defined (EVE_DMA)
        extern uint32_t EVE_dma_buffer[1025];
        extern volatile uint16_t EVE_dma_buffer_index;
        extern volatile uint8_t EVE_dma_busy;

        void EVE_init_dma(void);
        void EVE_start_dma_transfer(void);
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
            wrapper_spi_transmit(data);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #elif defined (ARDUINO_BBC_MICROBIT_V2) /* note: this is mostly untested */

        #if !defined (EVE_CS)
            #define EVE_CS      12
            #define EVE_PDN     9
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
            wrapper_spi_transmit(data);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #elif defined (XMC1100_XMC2GO)

        #if !defined (EVE_CS)
            #define EVE_CS      3
            #define EVE_PDN     4
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
            wrapper_spi_transmit(data);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
    #else       /* generic functions for other Arduino architectures */

        #if !defined (EVE_CS)
            #define EVE_CS      10
            #define EVE_PDN     8
        #endif

        static inline void EVE_cs_set(void)
        {
            digitalWrite(EVE_CS, LOW); /* make EVE listen */
        }

        static inline void EVE_cs_clear(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
        }

        static inline void spi_transmit(uint8_t data)
        {
            wrapper_spi_transmit(data);
        }

        static inline void spi_transmit_32(uint32_t data)
        {
            spi_transmit((uint8_t)(data & 0x000000ff));
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
            return wrapper_spi_receive(data);
        }

        static inline uint8_t fetch_flash_byte(const uint8_t *data)
        {
            return *data;
        }
    #endif


    /* functions that should be common across Arduino architectures */

    #define DELAY_MS(ms) delay(ms)

    static inline void EVE_pdn_set(void)
    {
        digitalWrite(EVE_PDN, LOW); /* go into power-down */
    }

    static inline void EVE_pdn_clear(void)
    {
        digitalWrite(EVE_PDN, HIGH); /* power up */
    }

#ifdef __cplusplus
}
#endif

#endif /* Arduino */

#endif /* EVE_TARGET_H_ */
