# FT810-FT813, BT81x
This is a code library for EVE/EVE2/EVE3 graphics controller ICs from FTDI/Bridgetek:

http://www.ftdichip.com/EVE.htm

http://brtchip.com/eve/

http://brtchip.com/ft80x/

http://brtchip.com/ft81x/

https://brtchip.com/bt81x/

It contains code for and has been used with various mikro-controllers and displays.

I have used it so far with:

- 8-Bit AVR, specifically the 90CAN series
- Arduino, Uno, mini-pro, ESP8266
- Renesas F1L RH850
- Infineon Aurix TC222
- ATSAMC21E18A (DMA)
- ATSAME51J19A

I have reports of successfully using it with:

- ATSAMV70
- ATSAMD20
- ATSAME4
- STM32
- MSP430
- MSP432
- some PICs
- ESP32

The TFTs I have tested myself so far:

- VM800B35A http://www.ftdichip.com/Products/Modules/VM800B.html
- VM800B50A
- FT800CB-HY50B http://www.hotmcu.com/5-graphical-lcd-touchscreen-480x272-spi-ft800-p-124.html
- FT810CB-HY50HD http://www.hotmcu.com/5-graphical-lcd-touchscreen-800x480-spi-ft810-p-286.html
- FT811CB-HY50HD  http://www.hotmcu.com/5-graphical-lcd-capacitive-touch-screen-800x480-spi-ft811-p-301.html
- RVT70UQFNWC0x https://riverdi.com/product/rvt70uqfnwc0x/
- RVT50
- ADAM101-LCP-SWVGA-NEW from Glyn, 10.1" 1024x600 cap-touch
- EVE2-38A https://www.matrixorbital.com/eve2-38a
- EVE2-35G https://www.matrixorbital.com/eve2-35g
- EVE2-43G https://www.matrixorbital.com/eve2-43g
- EVE2-50G https://www.matrixorbital.com/eve2-50g
- EVE2-70G https://www.matrixorbital.com/eve2-70g
- NHD-3.5-320240FT-CSXV-CTP
- RVT43ULBNWC00 (RiTFT-43-CAP-UX) https://riverdi.com/product/ritft43capux/
- RVT50AQBNWC00 (RiTFT-50-CAP) https://riverdi.com/product/ritft50cap/
- EVE3-50G https://www.matrixorbital.com/eve3-50g
- PAF90B5WFNWC01 http://www.panadisplay.com/ftdi-intelligent-display/9-inch-lcd-with-touch-with-bt815-board.html
- EVE3-43G https://www.matrixorbital.com/eve3-43g
- EVE3-35G https://www.matrixorbital.com/eve3-35g
- CFAF240400C0-030SC https://www.crystalfontz.com/product/cfaf240400c0030sca11-240x400-eve-touchscreen-tft-ft813
- CFAF320240F-035T https://www.crystalfontz.com/product/cfaf320240f035ttsa11-320x240-eve-tft-lcd-display-kit
- CFAF480128A0-039TC
- CFAF800480E0-050SC https://www.crystalfontz.com/product/cfaf800480e1050sca11-800x480-eve-accelerated-tft

The examples in the "example_projects" drawer are for use with AtmelStudio7.
For Arduino I am using PlatformIO with Visual Studio Code.

The platform the code is compiled for is automatically detected thru compiler flags in EVE_target.h. This is the only file that should need editing to customize the library to your needs.

- Select the TFT attached by enabling one of the pre-defined setups in EVE_config.h.
- Provide the pins used for Chip-Select and Power-Down in EVE_target.h for the target configuration you are using

When compiling for AVR you need to provide the clock it is running at in order to make the _delay_ms() calls used to initialise the TFT work with the intended timing.
For other plattforms you need to provide a DELAY_MS(ms) function that works between 1ms and 56ms at least and is at least not performing these delays shorter than requested.
The DELAY_MS(ms) is only used during initialisation of the FT8xx/BT8xx
See EVE_target.h for examples.

In Addition you need to initialise the pins used for Chip-Select and PowerDown in your hardware correctly to output.
Plus setup the SPI accordingly, mode-0, 8-bit, MSB-first, not more than 11MHz for the init.

Originally the project went public in the German mikrocontroller.net forum, the thread contains some insight: https://www.mikrocontroller.net/topic/395608

Feel free to add to the discussion with questions or remarks.

A word of "warning", you have to take a little care yourself to for example not send more than 4kB at once to the command co-processor
or to not generate display lists that are longer than 8kB.
My library does not check and re-check the command-FIFO on every step.
This is optimised for speed so the training wheels are off.

Note, with so many options to choose from now, FT80x support will be removed at some point in the future.
