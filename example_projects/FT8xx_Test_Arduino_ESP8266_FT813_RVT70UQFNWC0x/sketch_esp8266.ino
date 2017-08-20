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
	delay(4); /* TFT_loop() must not be called to often as the FT8xx do not like more than about 60 screens per second */
	digitalWrite(16, HIGH);
	TFT_loop();  /* at 1 MHz SPI this returns in <1ms/~12ms which makes a total refresh of 4+1+4+12 = 21ms or about 48 frames per second */
	digitalWrite(16, LOW);
}
