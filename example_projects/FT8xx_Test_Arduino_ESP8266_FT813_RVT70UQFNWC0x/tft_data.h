

#ifndef TFT_DATA_H_
#define TFT_DATA_H_

#if	defined (__AVR__)
	#include <avr/pgmspace.h>
#else
	#define PROGMEM
#endif

extern const uint16_t logo_size;
extern const uint8_t logo[406] PROGMEM;

extern const uint16_t pngpic_size;
extern const uint8_t pngpic[3867] PROGMEM;

#endif /* TFT_DATA_H_ */