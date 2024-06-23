/*
@file    EVE_HelloWorld.cpp
@brief   Main file for PlatformIO/Arduino EVE HelloWorld
@version 1.2
@date    2024-06-23
@author  Rudolph Riedel
*/

#include <Arduino.h>
#include <SPI.h>
#include "EVE.h"

void setup()
{
    pinMode(EVE_CS, OUTPUT);
    digitalWrite(EVE_CS, HIGH);
    pinMode(EVE_PDN, OUTPUT);
    digitalWrite(EVE_PDN, LOW);

#if defined (ESP32)
    #if defined (EVE_USE_ESP_IDF)
        /* not using the Arduino SPI class in order to use DMA */
        EVE_init_spi();
    #else
        /* using the Arduino SPI class to be compatible with other devices */
        SPI.begin(EVE_SCK, EVE_MISO, EVE_MOSI);
    #endif
#elif defined (ARDUINO_NUCLEO_F446RE) || defined (WIZIOPICO) || defined (PICOPI)
    /* not using the Arduino SPI class in order to use DMA */
    EVE_init_spi();
#else
    SPI.begin(); /* sets up the SPI to run in Mode 0 and 1 MHz */
    /* switch to 8MHz, note, init must be done with <11MHz */
    SPI.beginTransaction(SPISettings(8UL * 1000000UL, MSBFIRST, SPI_MODE0));
#endif

    if(E_OK == EVE_init()) /* make sure the init finished correctly */
    {
        EVE_cmd_dl(CMD_DLSTART); /* instruct the co-processor to start a new display list */
        EVE_cmd_dl(DL_CLEAR_COLOR_RGB | 0xffffff); /* set the default clear color to white */
        EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG); /* clear the screen - this and the previous prevent artifacts between lists, attributes are the color, stencil and tag buffers */
        EVE_color_rgb(0x000000); /* set the color to black */
        EVE_cmd_text(EVE_HSIZE/2, EVE_VSIZE/2, 30, EVE_OPT_CENTER, "Hello, World!");
        EVE_cmd_dl(DL_DISPLAY); /* mark the end of the display-list */
        EVE_cmd_dl(CMD_SWAP); /* make this list active */
//        EVE_execute_cmd(); /* wait for EVE to be no longer busy */
    }
}

void loop()
{
}
