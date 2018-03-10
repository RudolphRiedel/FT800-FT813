/*
@file    main.c
@brief   main file for FT8xx_Test project, version for ARDUINO
@version 1.1
@date    2018-03-10
@author  Rudolph Riedel

@section History

1.0
- initial release

1.1
- updated to the same functionality as the AVR example, compiles but is not tested

*/


#include <SPI.h>
#include "tft.h"
#include "FT8_config.h"


void setup()
{
	digitalWrite(FT8_CS, HIGH);
	pinMode(FT8_CS, OUTPUT);
	digitalWrite(FT8_PDN, HIGH);
	pinMode(FT8_PDN, OUTPUT);
	
	pinMode(16, OUTPUT);
	
	SPI.begin(); /* sets up the SPI to run in Mode 0 and 1 MHz */
//	SPI.setClockDivider(SPI_CLOCK_DIV2);
	
	TFT_init();
}


void loop()
{
	uint32_t current_millis;
	static uint32_t previous_millis = 0;

	static	uint8_t toggle = 0;
	uint32_t micros_start, micros_end;

	current_millis = millis();
	
	if((current_millis - previous_millis) > 9) /* execute the code only every 10 milli-seconds */
	{
		previous_millis = current_millis;
		
		micros_start = micros();
		TFT_loop(); /* on the first call touch-events are handled, on the second call the display is refreshed */
		micros_end = micros();
		
		micros_end = (micros_start - micros_end); /* calculate the micro-seconds passed during the call to TFT_loop */
		
		if(toggle == 0)
		{
			num_profile_a = (uint16_t) micros_end; /* put the calculated time difference into a var to be displayed by TFT_loop() */
			toggle = 1;
		}
		else
		{
			num_profile_b = (uint16_t) micros_end;
			toggle = 0;
		}		
	}
}
