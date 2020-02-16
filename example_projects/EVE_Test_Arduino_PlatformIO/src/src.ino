/*
@file    src.ino
@brief   Main file for Arduino EVE test-code
@version 2.0
@date    2020-02-16
@author  Rudolph Riedel
*/

#include <Arduino.h>

#include "EVE_commands.h"
#include "tft.h"


void setup()
{
	digitalWrite(EVE_CS, HIGH);
	pinMode(EVE_CS, OUTPUT);
	digitalWrite(EVE_PDN, HIGH);
	pinMode(EVE_PDN, OUTPUT);
	
	SPI.begin(); /* sets up the SPI to run in Mode 0 and 1 MHz */
	SPI.setClockDivider(SPI_CLOCK_DIV2); /* speed up SPI */
	
	TFT_init();
}


void loop()
{
	uint32_t current_millis;
	static uint32_t previous_millis = 0;
	static uint8_t display_delay = 0;

	uint32_t micros_start, micros_end;

	current_millis = millis();
	
	if((current_millis - previous_millis) > 4) /* execute the code only every 5 milli-seconds */
	{
		previous_millis = current_millis;
		
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