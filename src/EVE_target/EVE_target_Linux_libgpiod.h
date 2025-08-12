/*
@file    EVE_target_Linux_libgpiod.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2025-08-12
@author  Vladimir Shupilov

@section LICENSE

MIT License

Copyright (c) 2016-2025 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0


*/

#ifndef EVE_TARGET_LINUX_LIBGPIOD_H
#define EVE_TARGET_LINUX_LIBGPIOD_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

#if defined (LINUX_LIBGPIOD)

#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS 18
#endif

#if !defined (EVE_PDN)
#define EVE_PDN 24
#endif

#if !defined (EVE_SCK)
#define EVE_SCK 21
#endif

#if !defined (EVE_MOSI)
#define EVE_MOSI 20
#endif

#if !defined (EVE_MISO)
#define EVE_MISO 19
#endif

#if !defined (EVE_GPIO_CHIP)
#define EVE_GPIO_CHIP "gpiochip0"
#endif

#if !defined (EVE_SPI)
#define EVE_SPI "/dev/spidev1.0"
#endif

#if !defined (EVE_SPI_SPEED)
#define EVE_SPI_SPEED 30000000
#endif

#if !defined (EVE_SPI_BITS_PER_WORD)
#define EVE_SPI_BITS_PER_WORD 8
#endif

#if !defined (EVE_SPI_MODE)
#define EVE_SPI_MODE SPI_MODE_0
#endif
/* you may define these in your build-environment to use different settings */

// #define EVE_DMA /* to be defined in the build-environment */

#define DELAY_MS(ms) usleep(ms*1000)

typedef int spi_handle_t;
extern struct gpiod_line *pd_n_line;
extern struct gpiod_line *cs_line;
extern spi_handle_t spi_fd;

typedef enum {
    EVE_INIT_SPI_OK                     = 0,
    EVE_INIT_SPI_OPEN_ERR               = -1,
    EVE_INIT_SPI_SET_MODE_ERR           = -2,
    EVE_INIT_SPI_SET_BITS_PER_WORD_ERR  = -3,
    EVE_INIT_SPI_SET_MAX_SPEED_ERR      = -4,
} eve_init_spi_ret_code_t;

typedef enum {
    EVE_INIT_GPIO_OK                     = 0,
    EVE_INIT_GPIO_CHIP_ERR               = -1,
    EVE_INIT_GPIO_PDN_GET_LINE_ERR       = -2,
    EVE_INIT_GPIO_CS_GET_LINE_ERR        = -3,
    EVE_INIT_GPIO_PDN_REQUEST_OUTPUT_ERR = -4,
    EVE_INIT_GPIO_CS_REQUEST_OUTPUT_ERR  = -5,
} eve_init_gpio_ret_code_t;

eve_init_spi_ret_code_t EVE_init_spi(void);
void EVE_deinit_spi(void);
eve_init_gpio_ret_code_t EVE_init_gpio(void);
void EVE_deinit_gpio(void);

static inline void EVE_cs_set(void)
{
    gpiod_line_set_value(cs_line, 0);
}

static inline void EVE_cs_clear(void)
{
    gpiod_line_set_value(cs_line, 1);
}

static inline void EVE_pdn_set(void)
{
    // pdn_set 0 in ALL other targets
    gpiod_line_set_value(pd_n_line, 0);
}

static inline void EVE_pdn_clear(void)
{
    // pdn_clear 1 in ALL other targets
    gpiod_line_set_value(pd_n_line, 1);
}

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

static inline uint8_t _spi_transmit(uint32_t data, uint8_t len)
{
    uint8_t result = 0;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)&data,
        .rx_buf = (unsigned long)&result,
        .len = len,
        .delay_usecs = 0,
        .speed_hz = EVE_SPI_SPEED,
        .bits_per_word = EVE_SPI_BITS_PER_WORD,
    };
    int ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 0) {
        printf("SPI fd:%d transfer ERROR: [%d] %s\n", spi_fd, ret, strerror(errno));
    }
    return result;
}

static inline void spi_transmit(uint8_t data)
{
    _spi_transmit(data, 1);
}

static inline void spi_transmit_32(uint32_t data)
{
    _spi_transmit(data, 4);
}

/* spi_transmit_burst() is only used for cmd-FIFO commands */
/* so it *always* has to transfer 4 bytes */
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
    return _spi_transmit(data, 1);
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* LINUX_LIBGPIOD */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_LINUX_LIBGPIOD_H */
