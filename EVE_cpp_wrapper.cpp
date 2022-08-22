/*
@file    EVE_cpp_wrapper.cpp
@brief   wrapper functions to make C++ class methods callable from C functions
@version 5.0
@date    2022-08-22
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
- Bugfix: broke ESP8266 when I implemented this by using "__builtin_bswap32" a second time

*/

#if defined (ARDUINO)

#include <Arduino.h>
//#include <stdio.h>
#include <SPI.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined (ESP8266)

    void wrapper_spi_transmit(uint8_t data)
    {
        SPI.write(data);
    }

    void wrapper_spi_transmit_32(uint32_t data)
    {
        SPI.write32(data);
    }

    uint8_t wrapper_spi_receive(uint8_t data)
    {
        return SPI.transfer(data);
    }

#else

    void wrapper_spi_transmit(uint8_t data)
    {
        SPI.transfer(data);
    }

    uint8_t wrapper_spi_receive(uint8_t data)
    {
        return SPI.transfer(data);
    }

#endif

#ifdef __cplusplus
}
#endif

#endif /* Arduino */
