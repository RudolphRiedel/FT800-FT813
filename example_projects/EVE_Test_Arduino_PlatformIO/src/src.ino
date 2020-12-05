/*
@file    src.ino
@brief   Main file for Arduino EVE test-code
@version 2.2
@date    2020-12-05
@author  Rudolph Riedel
*/

#include <Arduino.h>

#include "EVE_commands.h"
#include "tft.h"

void setup()
{
	digitalWrite(EVE_CS, HIGH);
	pinMode(EVE_CS, OUTPUT);
	digitalWrite(EVE_PDN, LOW);
	pinMode(EVE_PDN, OUTPUT);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

#if defined (ESP32)
	SPI.begin(EVE_SCK, EVE_MISO, EVE_MOSI); /* we need to setup the pins as they are used for our board, defines are in EVE_target.h */
#else
	SPI.begin(); /* sets up the SPI to run in Mode 0 and 1 MHz */
#endif

	SPI.setClockDivider(SPI_CLOCK_DIV2); /* speed up SPI to 8MHz*/

	TFT_init();
}

void loop()
{
	uint32_t current_millis;
	static uint32_t previous_millis = 0;
	static uint8_t display_delay = 0;
	static uint16_t led = 0;
	static uint8_t led_state = 0;

	uint32_t micros_start, micros_end;

	current_millis = millis();

	if((current_millis - previous_millis) > 4) /* execute the code every 5 milli-seconds */
	{
		previous_millis = current_millis;

		led++;
		if(led > 69)
		{
			led = 0;
			if(led_state)
			{
				led_state = 0;
				digitalWrite(LED_BUILTIN, LOW);
			}
			else
			{
				led_state = 42;
				digitalWrite(LED_BUILTIN, HIGH);
			}
		}

		micros_start = micros();
		TFT_touch();
		micros_end = micros();
		num_profile_b = (micros_end - micros_start); /* calculate the micro-seconds passed during the call to TFT_touch */

		display_delay++;
		if(display_delay > 3) /* refresh the display every 20ms */
		{
			display_delay = 0;
			micros_start = micros();
			TFT_display();
			micros_end = micros();
			num_profile_a = (micros_end - micros_start); /* calculate the micro-seconds passed during the call to TFT_display */
		}
	}
}
