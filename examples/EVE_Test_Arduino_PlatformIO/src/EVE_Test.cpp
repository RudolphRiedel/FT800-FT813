/*
@file    EVE_Test.cpp
@brief   Main file for PlatformIO/Arduino EVE test-code
@version 2.12
@date    2022-12-10
@author  Rudolph Riedel
*/

#include <Arduino.h>
#include <SPI.h>
#include "EVE.h"
#include "tft.h"

void setup()
{
    pinMode(EVE_CS, OUTPUT);
    digitalWrite(EVE_CS, HIGH);
    pinMode(EVE_PDN, OUTPUT);
    digitalWrite(EVE_PDN, LOW);

/* we are not using the Arduino SPI class since we want DMA */
#if defined(ESP32) || (ARDUINO_NUCLEO_F446RE) || (WIZIOPICO) || (PICOPI)
    EVE_init_spi();
#else
    SPI.begin(); /* sets up the SPI to run in Mode 0 and 1 MHz */
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#endif

    TFT_init();
    // SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

#if defined(ESP8266)
    SPI.setFrequency(16000000);
#endif
}

void loop()
{
    uint32_t current_millis;
    static uint32_t previous_millis = 0;
    static uint8_t display_delay = 0;
    uint32_t micros_start, micros_end;
    current_millis = millis();

    if ((current_millis - previous_millis) > 4) /* execute the code every 5 milli-seconds */
    {
        previous_millis = current_millis;

        micros_start = micros();
        TFT_touch();
        micros_end = micros();
        num_profile_b = (micros_end - micros_start); /* calculate the micro-seconds passed during the call to TFT_touch */

        display_delay++;
        if (display_delay > 3) /* refresh the display every 20ms */
        {
            display_delay = 0;
            micros_start = micros();
            TFT_display();
            micros_end = micros();
            num_profile_a = (micros_end - micros_start); /* calculate the micro-seconds passed during the call to TFT_display */
        }
    }
}
