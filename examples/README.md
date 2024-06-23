# Examples for https://github.com/RudolphRiedel/FT800-FT813


## Examples using PlatformIO

PlatformIO ( https://platformio.org/ ) is a multi-plattform build system for 1000+ boards and dev-kits supporting 20+ frameworks.
PlatformIO is available as an extension for VSCode ( https://code.visualstudio.com/ ).

The configuration of the PlatformIO examples is done thru the file "platformio.ini".
All examples use these four files and these are identical for all examples:
tft.h - makes TFT_init(), TFT_touch(), TFT_display(), num_profile_a and num_profile_b from tft.c available
tft_data.h - makes the arrays logo[], pic[] and flash[] from tft_data.c available
tft.c
TFT_init() - this initializes the EVE chip, writes calibration values, uploads images and builds a display-list fragment
TFT_touch() - this polls the EVE chip for touch events and processes them, meant to be called every 5ms
TFT_display() - this builds the display list and either sends it to the EVE chip directly or triggers DMA transfer, meant to be called every 20ms

tft_data.c
logo[206] - my "logo", displayed in the top right, 56x56 pixel, ARGB1555 format and zlib compressed
pic[3391] - a test-image, 100x100 pixel, .jpg format
flash[7765] - a zlib compressed flash-container to be used with BT81x modules, it contains a glyph-reduced UTF-8 font

The library code itself is downloaded from my Github repository by PlatformIO.

## EVE_Test_Arduino_PlatformIO

This is an example for Arduino and it builds for a number of different boards:

|Environment|Status|
|---|---|
|uno|SUCCESS|
|avr_pro|SUCCESS|
|nano328|SUCCESS|
|mega2560|SUCCESS|
|adafruit_metro_m4|SUCCESS|
|samd21_m0-mini|SUCCESS|
|samd21_seeed_xiao|SUCCESS|
|ESP32|SUCCESS|
|ESP32-S2|SUCCESS|
|ESP32-S3|SUCCESS|
|ESP32-C3|SUCCESS|
|ESP8266_d1_mini|SUCCESS|
|nucleo_f446re|SUCCESS|
|teensy40|SUCCESS|
|teensy41|SUCCESS|
|adafruit_feather_nrf52840|SUCCESS|
|bbcmicrobit_v2|SUCCESS|
|teensy35|SUCCESS|
|xmc1100_xmc2go|SUCCESS|
|xmc4700|SUCCESS|
|pico|SUCCESS|
|dueUSB|SUCCESS|
|portenta_h7_m4|SUCCESS|
|portenta_h7_m7|SUCCESS|
|uno_r4_minima|SUCCESS|
|uno_r4_wifi|SUCCESS|

These all share a single "EVE_Test.cpp" which contains the setup() and loop() functions.

## EVE_Test_ESP32_PlatformIO

This has a "main.c" for ESP32 ESP-IDF.

## EVE_Test_SAMC21_DMA_EVE3-50G_Dev_PlatformIO

This has a "main.c" for an ATSAMC21 controller from Microchip.
This is Bare-Metal with ARM CMSIS for SysTick.
I am using a fork ( https://github.com/glingy/platform-atmelsam/tree/feat-cmsis-build-2 ) of platform-atmelsam to which I added my board files since the main repository "only" supports Arduino and Zephyr and requests to add CMSIS support are mostly ignored, therefore it does not allow bare-metal programming.

## EVE_Test_SAME51_DMA_EVE3-50G_Dev_PlatformIO

This has a "main.c" for an ATSAME51 controller from Microchip.
This is Bare-Metal with ARM CMSIS for SysTick.
I am using a fork ( https://github.com/glingy/platform-atmelsam/tree/feat-cmsis-build-2 ) of platform-atmelsam to which I added my board files since the main repository "only" supports Arduino and Zephyr and requests to add CMSIS support are mostly ignored, therefore it does not allow bare-metal programming.

## EVE_Test_STM32_RiTFT50_PlatformIO

This is a non-functional example.
It builds for several STM32 across almost all families:

|Environment|Status|
|---|---|
|STM32L073|SUCCESS|
|STM32F030|SUCCESS|
|STM32F103|SUCCESS|
|STM32F303|SUCCESS|
|STM32F446|SUCCESS|
|STM32G474|SUCCESS|
|STM32G431|SUCCESS|
|nucleo_f439zi|SUCCESS|
|nucleo_h743zi|SUCCESS|

These all share a single "main.c" with the functions SysTick_Handler() and main().
It builds the complete EVE code including hardware specific SPI functions due to use
of the STM32 HAL library in EVE_target.h.
But it does not initialize any of the controllers, not the clock, not the pins, not the SPI.


## Examples using Microchip Studio

Microchip Studio, formerly known as Atmel Studio, is an IDE based on Visual Studio.
The board specific settings are set as global defines in the project properties:
Toolchain/Compiler/Symbols/Defined Symbols
These examples have a copy of the library in a folder "EmbeddedVideoEngine".

## EVE_Test_SAMC21_EVE2-50G

This is a Bare-Metal with ARM CMSIS example for an ATSAMC21 controller from Microchip.

## EVE_Test_SAME51_EVE3-43G

This is a Bare-Metal with ARM CMSIS example for an ATSAME51 controller from Microchip.

## EVE_Test_XMEGA128A1_GEN4-FT813-50CTP

This is a Bare-Metal with AVR libc example for an ATxmega128A1 controller from Microchip.

