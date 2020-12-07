
#ifndef TFT_DATA_H_
#define TFT_DATA_H_

#if	defined (__AVR__)
	#include <avr/pgmspace.h>
#else
	#if !defined(PROGMEM)
		#define PROGMEM
	#endif
#endif

extern const uint8_t logo[239] PROGMEM;
extern const uint8_t pic[3844] PROGMEM;
extern const uint8_t flash[10784] PROGMEM;

#endif /* TFT_DATA_H_ */