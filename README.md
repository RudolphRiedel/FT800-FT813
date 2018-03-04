# FT810-FT813
This is a code library for EVE/EVE2 graphics controller ICs from FTDI/Bridgetek:

http://www.ftdichip.com/EVE.htm

http://brtchip.com/eve/

http://brtchip.com/ft80x/

http://brtchip.com/ft81x/

It contains code for and has been used with various mikro-controllers and displays.

8-Bit AVR, specifically the 90CAN series

Arduino, Uno, mini-pro, ESP8266

Renesas F1L RH850

The TFTs I used so far:

VM800B35A http://www.ftdichip.com/Products/Modules/VM800B.html

VM800B50A

FT800CB-HY50B http://www.hotmcu.com/5-graphical-lcd-touchscreen-480x272-spi-ft800-p-124.html

FT810CB-HY50HD http://www.hotmcu.com/5-graphical-lcd-touchscreen-800x480-spi-ft810-p-286.html

FT811CB-HY50HD  http://www.hotmcu.com/5-graphical-lcd-capacitive-touch-screen-800x480-spi-ft811-p-301.html

RVT70UQFNWC0x https://riverdi.com/product/rvt70uqfnwc0x/

The examples in the "example_projects" drawer are for use with AtmelStudio7. For Arduino I am using the plugin from www.visualmicro.com .

The platform the code is compiled for is automatically detected thru compiler flags in FT8_config.h. This is the only files that should need editing to customize the library to your needs.
Note: there also used to be a file FT8_config.c but it is obsolete now.

Choose between FT80x/FT81x by enabling/disabling the FT8_81X_ENABLE define.

Select the TFT attached by enabling one of the pre-defined setups.

When compiling for AVR you need to provide the clock it is running at in order to make the delay() calls used to initialise the TFT work with the intended timing. Next you provide ports and pins for the TFTs chip-select and power-down pins.


Originally the project went public in the German mikrocontroller.net forum, the thread contains some insight: https://www.mikrocontroller.net/topic/395608

Feel free to add to the discussion with questions or remarks.
