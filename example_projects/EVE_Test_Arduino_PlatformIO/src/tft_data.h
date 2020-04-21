
#ifndef TFT_DATA_H_
#define TFT_DATA_H_

#if	defined (__AVR__)
	#include <avr/pgmspace.h>
#else
	#define PROGMEM
#endif

extern const uint8_t logo[406] PROGMEM;
extern const uint8_t pic[3844] PROGMEM;

#endif /* TFT_DATA_H_ */