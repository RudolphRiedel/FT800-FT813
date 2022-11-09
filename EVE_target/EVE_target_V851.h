/*
@file    EVE_target_V851.h
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


#ifndef EVE_TARGET_V851_H
#define EVE_TARGET_V851_H

#pragma once

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (__v851__)

#include <stdint.h>
#include "rh850_regs.h"
#include "os.h"

#define DELAY_MS(ms)    OS_Wait(ms * 1000U)

static inline void EVE_pdn_set(void)
{
    P0 &= ~(1U<<6U);
}

static inline void EVE_pdn_clear(void)
{
    P0 |= (1U<<6U);
}

static inline void EVE_cs_set(void)
{
    P8 &= ~(1U<<2U); /* manually set chip-select to low */
}

static inline void EVE_cs_clear(void)
{
    P8 |= (1U<<2U);  /* manually set chip-select to high */
}

static inline void spi_transmit(uint8_t data)
{
    CSIH0CTL0 = 0xC1U; /* CSIH2PWR = 1;  CSIH2TXE=1; CSIH2RXE = 0; direct access mode  */
    CSIH0TX0H = data;   /* start transmission */
    while(CSIH0STR0 & 0x00080U) {}  /* wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
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
    CSIH0CTL0 = 0xE1U; /* CSIH2PWR = 1;  CSIH2TXE=1; CSIH2RXE = 1; direct access mode  */
    CSIH0TX0H = data;   /* start transmission */
    while(CSIH0STR0 & 0x00080U) {}  /* wait for transmission to complete - 800ns @ 10MHz SPI-Clock */
    return (uint8_t) CSIH0RX0H;
}

static inline uint8_t fetch_flash_byte(const uint8_t *data)
{
    return *data;
}

#endif /* V851 */

#endif /* __GNUC__ */
#endif /* !Arduino */

#endif /* EVE_TARGET_V851_H */
