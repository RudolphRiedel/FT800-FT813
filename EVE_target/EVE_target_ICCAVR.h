/*
@file    EVE_target_ICCAVR.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2022-11-09
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
- extracted from EVE_target.h

*/

#ifndef EVE_TARGET_ICCAVR_H
#define EVE_TARGET_ICCAVR_H

#pragma once

#if !defined (ARDUINO)
#if defined (__IMAGECRAFT__)
#if defined (_AVR)

#include <iccioavr.h>

static inline void DELAY_MS(uint16_t val)
{
    uint16_t counter;

    while(val > 0U)
    {
        for(counter=0; counter < 2000U;counter++) // maybe ~1ms at 16MHz clock
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
    while(!(SPSR & (1<<SPIF))) {} /* wait for transmission to complete - 1us @ 8MHz SPI-Clock */
}

static inline void spi_transmit_32(uint32_t data)
{
    spi_transmit((uint8_t)(data & 0x000000ffUL));
    spi_transmit((uint8_t)(data >> 8U));
    spi_transmit((uint8_t)(data >> 16U));
    spi_transmit((uint8_t)(data >> 24U));
}

/* spi_transmit_burst() is only used for cmd-FIFO commands so it *always* has to transfer 4 bytes */
static inline void spi_transmit_burst(uint32_t data)
{
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
    SPDR = data; /* start transmission */
    while(!(SPSR & (1<<SPIF))) {} /* wait for transmission to complete - 1us @ 8MHz SPI-CLock */
    return SPDR;
}

static inline uint8_t fetch_flash_byte(const uint8_t *data)
{
    return *data;
}

#endif /* _AVR */
#endif /* __IMAGECRAFT__ */
#endif /* !Arduino */
#endif /* EVE_TARGET_ICCAVR_H */
