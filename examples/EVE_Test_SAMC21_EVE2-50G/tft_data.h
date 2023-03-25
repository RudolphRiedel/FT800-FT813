
#ifndef TFT_DATA_H
#define TFT_DATA_H

#if	defined (__AVR__)
    #include <avr/pgmspace.h>
#else
    #if !defined(PROGMEM)
        #define PROGMEM
    #endif
#endif

extern const uint8_t logo[206] PROGMEM;
extern const uint8_t pic[3391] PROGMEM;
extern const uint8_t flash[7765] PROGMEM;

#endif /* TFT_DATA_H */