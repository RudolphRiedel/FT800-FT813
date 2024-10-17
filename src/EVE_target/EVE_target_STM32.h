/*
@file    EVE_target_STM32.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2024-10-17
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2024 Rudolph Riedel

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
- extracted from EVE_target.h
- basic maintenance: checked for violations of white space and indent rules
- added STM32WB55xx
- reworked
- fix: switched EVE_cs_clear() and EVE_cs_set() from using LL to using HAL after making
  the very weird observation that CS was not rising high in between two consecutive
  host commands while sending three host commands was just fine - see issue #136

*/

#ifndef EVE_TARGET_STM32_H
#define EVE_TARGET_STM32_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

/* set with "build_flags" in platformio.ini or as defines in your build environment */
#if defined (STM32L0) \
    || defined (STM32F0) \
    || defined (STM32F1) \
    || defined (STM32F3) \
    || defined (STM32F4) \
    || defined (STM32G4) \
    || defined (STM32G0) \
    || defined (STM32WB55xx)

#if defined (STM32L0) /* set with "build_flags = -D STM32L0" in platformio.ini */
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_gpio.h"
#endif

#if defined (STM32F0) /* set with "build_flags = -D STM32F0" in platformio.ini */
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_gpio.h"
#endif

#if defined (STM32F1) /* set with "build_flags = -D STM32F1" in platformio.ini */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_gpio.h"
#endif

#if defined (STM32F3) /* set with "build_flags = -D STM32F3" in platformio.ini */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_spi.h"
#include "stm32f3xx_ll_gpio.h"
#endif

#if defined (STM32F4) /* set with "build_flags = -D STM32F4" in platformio.ini */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_gpio.h"
#endif

#if defined (STM32G4) /* set with "build_flags = -D STM32G4" in platformio.ini */
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_spi.h"
#include "stm32g4xx_ll_gpio.h"
#endif

#if defined (STM32G0) /* set with "build_flags = -D STM32G0" in platformio.ini */
#include "stm32g0xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_gpio.h"
#endif

#if defined (STM32WB55xx) /* set with "build_flags = -D STM32WB55xx" in platformio.ini */
#include "stm32wbxx.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_ll_spi.h"
#include "stm32wbxx_ll_gpio.h"
#endif

/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
    #define EVE_CS_PORT_NUM 1U
    #define EVE_CS GPIO_PIN_4
#endif

#if !defined (EVE_PD)
    #define EVE_PD_PORT_NUM 1U
    #define EVE_PD GPIO_PIN_3
#endif

#if !defined (EVE_SPI_NUM)
    #define EVE_SPI_NUM 1U
    #define EVE_SPI_PORT_NUM 1U
    #define EVE_SCK GPIO_PIN_5
    #define EVE_MOSI GPIO_PIN_7
    #define EVE_MISO GPIO_PIN_6
    #define EVE_SPI_PRESCALER SPI_BAUDRATEPRESCALER_8
#endif

#if !defined (EVE_SPI_GPIO_ALT_FUNCTION)
#error "EVE_SPI_GPIO_ALT_FUNCTION must be defined in order to configure the SPI GPIO pins (e.g. -DEVE_SPI_GPIO_ALT_FUNCTION=GPIO_AF5_SPI4)"
#endif

/* you may define these in your build-environment to use different settings */

#if EVE_CS_PORT_NUM == 1U
    #define EVE_CS_PORT GPIOA
#elif EVE_CS_PORT_NUM == 2U
    #define EVE_CS_PORT GPIOB
#elif EVE_CS_PORT_NUM == 3U
    #define EVE_CS_PORT GPIOC
#elif EVE_CS_PORT_NUM == 4U
    #define EVE_CS_PORT GPIOD
#elif EVE_CS_PORT_NUM == 5U
    #define EVE_CS_PORT GPIOE
#elif EVE_CS_PORT_NUM == 6U
    #define EVE_CS_PORT GPIOF
#elif EVE_CS_PORT_NUM == 7U
    #define EVE_CS_PORT GPIOG
#elif EVE_CS_PORT_NUM == 8U
    #define EVE_CS_PORT GPIOH
#elif EVE_CS_PORT_NUM == 9U
    #define EVE_CS_PORT GPIOI
#else
    #error "EVE_CS_PORT_NUM must be a numerical value in range of 1U to 9U for PORT A to PORT I"
#endif

#if EVE_PD_PORT_NUM == 1U
    #define EVE_PD_PORT GPIOA
#elif EVE_PD_PORT_NUM == 2U
    #define EVE_PD_PORT GPIOB
#elif EVE_PD_PORT_NUM == 3U
    #define EVE_PD_PORT GPIOC
#elif EVE_PD_PORT_NUM == 4U
    #define EVE_PD_PORT GPIOD
#elif EVE_PD_PORT_NUM == 5U
    #define EVE_PD_PORT GPIOE
#elif EVE_PD_PORT_NUM == 6U
    #define EVE_PD_PORT GPIOF
#elif EVE_PD_PORT_NUM == 7U
    #define EVE_PD_PORT GPIOG
#elif EVE_PD_PORT_NUM == 8U
    #define EVE_PD_PORT GPIOH
#elif EVE_PD_PORT_NUM == 9U
    #define EVE_PD_PORT GPIOI
#else
    #error "EVE_PD_PORT_NUM must be a numerical value in range of 1U to 9U for PORT A to PORT I"
#endif

#if EVE_SPI_PORT_NUM == 1U
    #define EVE_SPI_PORT GPIOA
#elif EVE_SPI_PORT_NUM == 2U
    #define EVE_SPI_PORT GPIOB
#elif EVE_SPI_PORT_NUM == 3U
    #define EVE_SPI_PORT GPIOC
#elif EVE_SPI_PORT_NUM == 4U
    #define EVE_SPI_PORT GPIOD
#elif EVE_SPI_PORT_NUM == 5U
    #define EVE_SPI_PORT GPIOE
#elif EVE_SPI_PORT_NUM == 6U
    #define EVE_SPI_PORT GPIOF
#elif EVE_SPI_PORT_NUM == 7U
    #define EVE_SPI_PORT GPIOG
#elif EVE_SPI_PORT_NUM == 8U
    #define EVE_SPI_PORT GPIOH
#elif EVE_SPI_PORT_NUM == 9U
    #define EVE_SPI_PORT GPIOI
#else
    #error "EVE_SPI_PORT_NUM must be a numerical value in range of 1U to 9U for PORT A to PORT I"
#endif

#if EVE_SPI_NUM == 1U
    #define EVE_SPI SPI1
#elif EVE_SPI_NUM == 2U
    #define EVE_SPI SPI2
#elif EVE_SPI_NUM == 3U
    #define EVE_SPI SPI3
#elif EVE_SPI_NUM == 4U
    #define EVE_SPI SPI4
#elif EVE_SPI_NUM == 5U
    #define EVE_SPI SPI5
#elif EVE_SPI_NUM == 6U
    #define EVE_SPI SPI6
#else
    #error "EVE_SPI_NUM must be a numerical value in range of 1U to 5U for SPI1 to SPI6"
#endif

void EVE_init_spi(void);
extern SPI_HandleTypeDef eve_spi_handle;

/* tested with: STM32F407 */
#if defined (EVE_DMA) /* to be defined in the build-environment */

#if !defined (EVE_DMA_UNIT_NUM)
    #define EVE_DMA_UNIT_NUM 2U
    #define EVE_DMA_STREAM_NUM 0U
    #define EVE_DMA_CHANNEL_NUM 4U
#endif

#if EVE_DMA_UNIT_NUM == 1U
    #if EVE_DMA_STREAM_NUM == 0U
        #define EVE_DMA_INSTANCE DMA1_Stream0
        #define EVE_DMA_IRQ DMA1_Stream0_IRQn
    #elif EVE_DMA_STREAM_NUM == 1U
        #define EVE_DMA_INSTANCE DMA1_Stream1
        #define EVE_DMA_IRQ DMA1_Stream1_IRQn
    #elif EVE_DMA_STREAM_NUM == 2U
        #define EVE_DMA_INSTANCE DMA1_Stream2
        #define EVE_DMA_IRQ DMA1_Stream2_IRQn
    #elif EVE_DMA_STREAM_NUM == 3U
        #define EVE_DMA_INSTANCE DMA1_Stream3
        #define EVE_DMA_IRQ DMA1_Stream3_IRQn
    #elif EVE_DMA_STREAM_NUM == 4U
        #define EVE_DMA_INSTANCE DMA1_Stream4
        #define EVE_DMA_IRQ DMA1_Stream4_IRQn
    #elif EVE_DMA_STREAM_NUM == 5U
        #define EVE_DMA_INSTANCE DMA1_Stream5
        #define EVE_DMA_IRQ DMA1_Stream5_IRQn
    #elif EVE_DMA_STREAM_NUM == 6U
        #define EVE_DMA_INSTANCE DMA1_Stream6
        #define EVE_DMA_IRQ DMA1_Stream6_IRQn
    #elif EVE_DMA_STREAM_NUM == 7U
        #define EVE_DMA_INSTANCE DMA1_Stream7
        #define EVE_DMA_IRQ DMA1_Stream7_IRQn
    #endif
#elif EVE_DMA_UNIT_NUM == 2U
    #if EVE_DMA_STREAM_NUM == 0U
        #define EVE_DMA_INSTANCE DMA2_Stream0
        #define EVE_DMA_IRQ DMA2_Stream0_IRQn
    #elif EVE_DMA_STREAM_NUM == 1U
        #define EVE_DMA_INSTANCE DMA2_Stream1
        #define EVE_DMA_IRQ DMA2_Stream1_IRQn
    #elif EVE_DMA_STREAM_NUM == 2U
        #define EVE_DMA_INSTANCE DMA2_Stream2
        #define EVE_DMA_IRQ DMA2_Stream2_IRQn
    #elif EVE_DMA_STREAM_NUM == 3U
        #define EVE_DMA_INSTANCE DMA2_Stream3
        #define EVE_DMA_IRQ DMA2_Stream3_IRQn
    #elif EVE_DMA_STREAM_NUM == 4U
        #define EVE_DMA_INSTANCE DMA2_Stream4
        #define EVE_DMA_IRQ DMA2_Stream4_IRQn
    #elif EVE_DMA_STREAM_NUM == 5U
        #define EVE_DMA_INSTANCE DMA2_Stream5
        #define EVE_DMA_IRQ DMA2_Stream5_IRQn
    #elif EVE_DMA_STREAM_NUM == 6U
        #define EVE_DMA_INSTANCE DMA2_Stream6
        #define EVE_DMA_IRQ DMA2_Stream6_IRQn
    #elif EVE_DMA_STREAM_NUM == 7U
        #define EVE_DMA_INSTANCE DMA2_Stream7
        #define EVE_DMA_IRQ DMA2_Stream7_IRQn
    #endif
#endif

#if EVE_DMA_CHANNEL_NUM == 0U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_0
#elif EVE_DMA_CHANNEL_NUM == 1U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_1
#elif EVE_DMA_CHANNEL_NUM == 2U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_2
#elif EVE_DMA_CHANNEL_NUM == 3U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_3
#elif EVE_DMA_CHANNEL_NUM == 4U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_4
#elif EVE_DMA_CHANNEL_NUM == 5U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_5
#elif EVE_DMA_CHANNEL_NUM == 6U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_6
#elif EVE_DMA_CHANNEL_NUM == 7U
    #define EVE_DMA_CHANNEL DMA_CHANNEL_7
#endif

extern volatile uint32_t EVE_dma_buffer[1025U];
extern volatile uint16_t EVE_dma_buffer_index;
extern volatile uint8_t EVE_dma_busy;

void EVE_init_dma(void);
void EVE_start_dma_transfer(void);

#endif /* EVE_DMA */

#define DELAY_MS(ms) HAL_Delay(ms)

static inline void EVE_cs_clear(void)
{
    while (LL_SPI_IsActiveFlag_BSY(EVE_SPI)) {}
//    LL_GPIO_SetOutputPin(EVE_CS_PORT, EVE_CS);
    HAL_GPIO_WritePin(EVE_CS_PORT, EVE_CS, GPIO_PIN_SET);
}

static inline void EVE_pdn_clear(void)
{
    HAL_GPIO_WritePin(EVE_PD_PORT, EVE_PD, GPIO_PIN_SET);
}

static inline void EVE_pdn_set(void)
{
    HAL_GPIO_WritePin(EVE_PD_PORT, EVE_PD, GPIO_PIN_RESET);
}

static inline void EVE_cs_set(void)
{
//    LL_GPIO_ResetOutputPin(EVE_CS_PORT, EVE_CS);
    HAL_GPIO_WritePin(EVE_CS_PORT, EVE_CS, GPIO_PIN_RESET);
}

static inline void spi_transmit(uint8_t data)
{
    if (LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) /* if the previous transmit left data in the RX buffer, we need to clear it */
    { /* this is significantly faster than to wait after each transfer for RX to finish */
        (void) EVE_SPI->DR; /* dummy read to clear SPI_SR_RXNE */
    }
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {}
}

static inline void spi_transmit_32(uint32_t data)
{
    spi_transmit((uint8_t)(data & 0x000000ff));
    spi_transmit((uint8_t)(data >> 8));
    spi_transmit((uint8_t)(data >> 16));
    spi_transmit((uint8_t)(data >> 24));
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
    if (LL_SPI_IsActiveFlag_BSY(EVE_SPI)) /* set when switching from write-only to read-write -> need to flush the RX buffer */
    {
        while (LL_SPI_IsActiveFlag_BSY(EVE_SPI)) {}
        (void) EVE_SPI->DR; /* dummy read to clear SPI_SR_RXNE */
    }

    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {}
    return (LL_SPI_ReceiveData8(EVE_SPI));
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif  /* STM32 */

#endif /* __GNUC__ */
#endif /* !Arduino */
#endif /* EVE_TARGET_STM32_H */
