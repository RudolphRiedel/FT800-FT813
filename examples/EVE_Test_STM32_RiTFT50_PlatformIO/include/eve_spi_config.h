/*
@file    eve_spi_config.h
@brief   example for user changeable target specific macros
@version 1.0
@date    2026-01-18
@author  Rudolph Riedel

Add this in STM32Cube IDE to Properties / C/C++ Build / Settings / Tool Settings / MCU/MPU GCC Compiler / Preprocessor / Define Symbols (-D)
EVE_SPI_CONFIG_H="eve_spi_config.h"

Check the Include Paths, the project I am working with defines ../Core/Inc for example.
Place this file in the include path.

Now for example EVE_target_STM32.h will include this file to use your settings.


@section LICENSE

MIT License

Copyright (c) 2026 Rudolph Riedel

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


*/

#ifndef EVE_SPI_CONFIG_GUARD
#define EVE_SPI_CONFIG_GUARD

#if defined (STM32F4) /* this needs to be set with either -DSTM32F4 or in the project settings */

#define EVE_SPI_NUM 3U
#define EVE_SPI_PORT_NUM 3U
#define EVE_SPI_GPIO_ALT_FUNCTION GPIO_AF6_SPI3
#define EVE_SCK GPIO_PIN_10
#define EVE_MOSI GPIO_PIN_12
#define EVE_MISO GPIO_PIN_11
#define EVE_SPI_PRESCALER SPI_BAUDRATEPRESCALER_4
#define EVE_CS_PORT_NUM 4U
#define EVE_CS GPIO_PIN_0
#define EVE_PD_PORT_NUM 4U
#define EVE_PD GPIO_PIN_2
#define EVE_DMA
#define EVE_DMA_UNIT_NUM 1U
#define EVE_DMA_CHANNEL_NUM 0U
#define EVE_DMA_STREAM_NUM 5U

#endif

#endif
