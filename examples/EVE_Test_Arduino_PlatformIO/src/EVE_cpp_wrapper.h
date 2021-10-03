/*
@file    EVE_cpp_wrapper.h
@brief   wrapper functions to make C++ class methods callable from C functions
@version 5.0
@date    2021-09-18
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2021 Rudolph Riedel

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


*/

#ifndef EVE_CPP_WRAPPER_H
#define EVE_CPP_WRAPPER_H

#if defined (ARDUINO)

#ifdef __cplusplus
extern "C" {
#endif

#if defined (ESP8266)

    void wrapper_spi_transmit(uint8_t data);
    void wrapper_spi_transmit_32(uint32_t data);
    uint8_t wrapper_spi_receive(uint8_t data);

#else

    void wrapper_spi_transmit(uint8_t data);
    uint8_t wrapper_spi_receive(uint8_t data);

#endif

#ifdef __cplusplus
}
#endif

#endif /* Arduino */

#endif /* EVE_CPP_WRAPPER_H */
