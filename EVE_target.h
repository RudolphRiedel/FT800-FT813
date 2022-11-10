/*
@file    EVE_target.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2022-11-10
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
- added a GD32C103 target - not 100% working, yet
- added a RP2040 Arduino target using wizio-pico
- modified the WIZIOPICO target for Arduino RP2040 to also work with ArduinoCore-mbed
- removed the 4.0 history
- fixed the GD32C103 target, as a first step it works without DMA now
- added DMA support for the GD32C103 target
- moved targets to extra header files: ATSAMC21, ICCAVR, V851, XMEGA, AVR, Tricore, ATSAMx5x
- moved targets to extra header files: GD32VF103, STM32, ESP32, RP2040, S32K14x, K32L2B31, GD32C103

*/

#ifndef EVE_TARGET_H
#define EVE_TARGET_H

/*
While the following lines make things a lot easier like automatically compiling the code for the target you are compiling for,
a few things are expected to be taken care of beforehand.
- setting the Chip-Select and Power-Down pins to Output, Chip-Select = 1 and Power-Down = 0
- setting up the SPI which may or not include things like
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

#include "EVE_target/EVE_target_ICCAVR.h"

#endif
#endif

#if defined (__GNUC__)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__AVR_XMEGA__)

#include "EVE_target/EVE_EVE_target_XMEGA.h"

#endif /* XMEGA */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__AVR__) && ! defined (__AVR_XMEGA__)

#include "EVE_target/EVE_EVE_target_AVR.h"

#endif /* AVR */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__v851__)

#include "EVE_target_V851.h"

#endif /* V851 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__TRICORE__)

#include "EVE_target_Tricore.h"

#endif /* __TRICORE__ */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__SAMC21E18A__) || (__SAMC21J18A__) || (__SAMC21J17A__)
/* note: target as set by AtmelStudio, valid  are all from the same family */

#include "EVE_target/EVE_target_ATSAMC21.h"

#endif /* SAMC2x */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__SAME51J19A__) || (__SAME51J18A__) || (__SAMD51P20A__) || (__SAMD51J19A__) || (__SAMD51G18A__)
/* note: target as set by AtmelStudio, valid  are all from the same family */

#include "EVE_target/EVE_target_ATSAMx5x.h"

#endif /* SAMx5x */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (__riscv)

#include "EVE_target/EVE_target_GD32VF103.h"

#endif /* __riscv */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (STM32L0) || (STM32F0) || (STM32F1) || (STM32F3) || (STM32F4) || (STM32G4) || (STM32H7)
/* set with "build_flags" in platformio.ini or as defines in your build environment */

#include "EVE_target/EVE_target_STM32.h"

#endif  /* STM32 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (ESP_PLATFORM)

#include "EVE_target/EVE_target_ESP32.h"

#endif /* ESP_PLATFORM */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (RP2040)
/* note: set in platformio.ini by "build_flags = -D RP2040" */

#include "EVE_target/EVE_target_RP2040.h"

#endif /* RP2040 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (CPU_S32K148) || (CPU_S32K144HFT0VLLT)

#include "EVE_target/EVE_target_S32K14x.h"

#endif /* S32K14x */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (CPU_K32L2B31VLH0A)

#include "EVE_target/EVE_target_K32L2B31.h"

#endif /* K32L2B3 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (GD32C103)
/* note: set in platformio.ini by "build_flags = -D GD32C103" */

#include "EVE_target/EVE_target_GD32C103.h"

#endif /* GD32C103 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* __GNUC__ */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined (ARDUINO)

    #include <stdint.h>

#if defined (PICOPI)
    #include <stdbool.h> /* only included to fix a bug in Common.h from https://github.com/arduino/ArduinoCore-API */
#endif

    #include <Arduino.h>
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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

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

        #elif defined (WIZIOPICO) || (PICOPI)
        /* note: set in platformio.ini by "build_flags = -D WIZIOPICO" */

        #include "hardware/pio.h"
        #include "hardware/spi.h"

        #if !defined (EVE_CS)
            #define EVE_CS      5
            #define EVE_PDN     6
            #define EVE_SCK     2
            #define EVE_MOSI    3
            #define EVE_MISO    4
            #define EVE_SPI spi0
        #endif

        void EVE_init_spi(void);

        static inline void EVE_cs_set(void)
        {
            gpio_put(EVE_CS, 0);
        }

        static inline void EVE_cs_clear(void)
        {
            gpio_put(EVE_CS, 1);
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

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/
        #else   /* generic functions for other Arduino architectures */

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
