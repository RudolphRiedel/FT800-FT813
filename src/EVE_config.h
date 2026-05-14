/*
@file    EVE_config.h
@brief   configuration information for some TFTs
@version 5.0
@date    2026-04-15
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2026 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- re-added the EVE_CSPREAD parameters to all profiles
- added a profile for the Gameduino3 shield
- cleanup: removed FT80x profiles
- replaced FT81x_enable and BT81x_enable with "EVE_GEN n"
- added a couple of test profiles for BT817
- moved the profile for the RiTFT50 over to the one for the RiTFT70
- added a profile for the VM816C50A-D from Bridgetek
- added a profile for PH800480T024-IFC03 and PH800480T013-IFC05 from PowerTip
- added profiles for EVE4_40G and EVE4_101G from Matrix Orbital
- added profiles for the EVE4 series modules from Riverdi
- tuned the profile for the RVT101H a little
- sorted the display-defines by chip
- added a profile for the EVE4x-70G IPS from Matrix Orbital
- added a profile for RiTFT35 from Riverdi
- changed the clock-polarity for EVE_EVE4_70G
- added a profile for CFAF800480Ex-050SC-A2 modules from Crystalfontz
- removed the define for the selected configuration to allow setting the define in the build-environment instead
- added an error message if no valid define was setup and therefore no set of parameters is configured
- converted all TABs to SPACEs
- removed EVE_TOUCH_RZTHRESH as it only applies to resistive touch screens and as EVE_init() still writes it if the
define exists - it can be configured thru project options
- added EVE_Display_Parameters_t to be used with an additional init function, still not sure how to procede exactly
- split the settings for EVE_RiTFT70 and EVE_RiTFT50 after a report for the EVE_RiTFT70 not working properly and
confirmation that the provided alternative parameters do work, the EVE_RiTFT50 however are confirmed to be working with
the IOT5
- commented out EVE_Display_Parameters_t for now
- removed the 4.0 history
- added EVE_TYPE_BAR, EVE_HSIZE_VISIBLE and EVE_VSIZE_VISIBLE defines to the bar-type display configurations
- changed the timings for EVE3x-39A and EVE3x-39G to what Matrix Orbital is using in their library
- changed the timings for EVE2_38A and EVE2_38G to what Matrix Orbital is using in their library
- added profiles for EVE_EVE3x_38A and EVE_EVE3x_38G
- fixed some MISRA-C issues
- basic maintenance: checked for violations of white space and indent rules
- added an EVE_CFAF800480E1_050SC_A2 profile without EVE_HAS_GT911 as there is a newer version of this
  model around that mimics a FT5316 while still using a GT911.
- added a profile for the CFAF1024600B0-070SC-A1 from Crystalfontz
- added a profile for the Gameduino GD3X 7"
- slightly adjusted the Sunflower config and removed the "untested" tag
- added profiles for new displays from Panasys
- switched from using CMD_PCLKFREQ to writing to REG_PCLK_FREQ directly
- added define EVE_SET_REG_PCLK_2X to set REG_PCLK_2X to 1 when necessary
- split the EVE_NHD_43_800480 in a separate config to add a new optional parameter: EVE_BACKLIGHT_FREQ
- added a configuration for Crystalfonts CFA800480E3-050Sx
- added a configuration for Crystalfonts CFA240400E1-030Tx
- added a configuration for Crystalfonts CFA240320Ex-024Sx
- added EVE_BACKLIGHT_FREQ to all Riverdi modules with a value of 4kHz as recommended by Riverdi
- EVE_PCLK_FREQ was incorrectly using "UL"
- added EVE_BACKLIGHT_FREQ to all Newhaven modules with a value of 800Hz
- added EVE_SOFT_RESET to EVE_GD3X profile
- added EVE_CUSTOM_MODULE_H to allow loading a custom configuration from an extra header file
- changed the backlight frequency for Riverdi FT81x / BT815 / BT816 modules to 250Hz as at least the RVT5UQBNWC01
 is populated with a CAT4238 and it's PWM range is 100Hz to 2kHz
- added PS817-070WS-C-IPS and RVT121H

*/

#ifndef EVE_CONFIG_H
#define EVE_CONFIG_H

/* define one of these in your build-environment to select the settings for the TFT attached */
#if 0

/* BT817 / BT818 */
#define EVE_RVT35H
#define EVE_RVT43H
#define EVE_RVT50H
#define EVE_RVT70H
#define EVE_RVT101H
#define EVE_RVT121H
#define EVE_EVE4_40G
#define EVE_EVE4_70G
#define EVE_EVE4_101G
#define EVE_CFAF800480Ex_050SC_A2
#define EVE_CFAF800480E1_050SC_A2
#define EVE_CFAF1024600B0_070SC_A1
#define EVE_CFA800480E3_050SX
#define EVE_PS817_043WQ_C_IPS
#define EVE_PS817_070WS_C_IPS


/* BT815 / BT816 */
#define EVE_VM816C50AD
#define EVE_PAF90
#define EVE_RiTFT35
#define EVE_RiTFT43
#define EVE_RiTFT50
#define EVE_RiTFT70
#define EVE_GD3X
#define EVE_EVE3_29
#define EVE_EVE3_35
#define EVE_EVE3_35G
#define EVE_EVE3_43
#define EVE_EVE3_43G
#define EVE_EVE3_50
#define EVE_EVE3_50G
#define EVE_EVE3_70
#define EVE_EVE3_70G
#define EVE_EVE3x_38
#define EVE_EVE3x_38G
#define EVE_EVE3x_39
#define EVE_EVE3x_39G
#define EVE_PS816_043WQ_R_IPS
#define EVE_PS815_043W_C_IPS
#define EVE_PS815_050W_C_IPS
#define EVE_PS815_070W_C_IPS
#define EVE_PS815_090W_C_IPS

/* FT812 / F813 */
#define EVE_ME812A
#define EVE_ME813A
#define EVE_ET07
#define EVE_RVT50
#define EVE_RVT70
#define EVE_EVE2_29
#define EVE_EVE2_35
#define EVE_EVE2_35G
#define EVE_EVE2_38
#define EVE_EVE2_38G
#define EVE_EVE2_43
#define EVE_EVE2_43G
#define EVE_EVE2_50
#define EVE_EVE2_50G
#define EVE_EVE2_70
#define EVE_EVE2_70G
#define EVE_NHD_35
#define EVE_NHD_43
#define EVE_NHD_43_800480
#define EVE_NHD_50
#define EVE_NHD_70
#define EVE_ADAM101
#define EVE_CFAF240400C1_030SC
#define EVE_CFAF320240F_035T
#define EVE_CFAF480128A0_039TC
#define EVE_CFAF800480E0_050SC
#define EVE_GEN4_FT81X_43
#define EVE_GEN4_FT812_50
#define EVE_GEN4_FT812_70
#define EVE_GEN4_FT813_50
#define EVE_GEN4_FT813_70
#define EVE_SUNFLOWER
#define EVE_PH800480

/* F810 / F811 */
#define EVE_VM810C
#define EVE_FT810CB_HY50HD
#define EVE_FT811CB_HY50HD
#define EVE_CFA240400E1_030TX
#define EVE_CFA240320EX_024SX
#define EVE_GAMEDUINO3

#endif

/* display timing parameters below */

/* use this to add a header file with your custom module configuration */
/* -DEVE_CUSTOM_MODULE_H='"..\mycfg.h"' */
#if defined (EVE_CUSTOM_MODULE_H)
#include EVE_CUSTOM_MODULE_H
#endif

/* ########## 320 x 240 ########## */

/* EVE2-35G 320x240 3.5" Matrix Orbital, capacitive touch, FT813 */
/* EVE2-35A 320x240 3.5" Matrix Orbital, resistive, or non-touch, FT812 */
#if defined (EVE_EVE2_35) || defined (EVE_EVE2_35G)
#define Resolution_320x240

#define EVE_PCLK ((uint32_t) 9UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#endif

#if defined (EVE_EVE2_35G)
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* EVE3-35G 320x240 3.5" Matrix Orbital, capacitive-touch, BT815 */
/* EVE3-35A 320x240 3.5" Matrix Orbital, resistive, or non-touch, BT816 */
#if defined (EVE_EVE3_35) || defined (EVE_EVE3_35G)
#define Resolution_320x240

#define EVE_PCLK ((uint32_t) 11UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

#if defined (EVE_EVE3_35G)
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* Crystalfonts CFAF320240F-035T 320x240 3.5" , FT810 resistive touch */
#if defined (EVE_CFAF320240F_035T)
#define EVE_HSIZE ((uint32_t) 320UL)
#define EVE_VSIZE ((uint32_t) 240UL)

#define EVE_VSYNC0 ((uint32_t) 1UL)
#define EVE_VSYNC1 ((uint32_t) 4UL)
#define EVE_VOFFSET ((uint32_t) 4UL)
#define EVE_VCYCLE ((uint32_t) 245UL)
#define EVE_HSYNC0 ((uint32_t) 10UL)
#define EVE_HSYNC1 ((uint32_t) 20UL)
#define EVE_HOFFSET ((uint32_t) 40UL)
#define EVE_HCYCLE ((uint32_t) 510UL)
#define EVE_PCLK ((uint32_t) 8UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 2UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#endif

/* Sunflower Arduino Shield, 320x240 3.5" from Cowfish, FT813 */
// https://github.com/Cowfish-Studios/Cowfish_Sunflower_Shield_PCB
/* note: CS is on pin D6 and PD is on pin D5 */
#if defined (EVE_SUNFLOWER)
#define Resolution_320x240

#define EVE_PCLK ((uint32_t) 9UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 2UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#endif

/* NHD-3.5-320240FT-CxXx-xxx 320x240 3.5" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_35)
#define Resolution_320x240

#define EVE_PCLK ((uint32_t) 9UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 2UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 800UL) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* 320x240 3.5" Riverdi, various options, BT815/BT816 */
#if defined (EVE_RiTFT35)
#define EVE_HSIZE ((uint32_t) 320UL)
#define EVE_VSIZE ((uint32_t) 240UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 2UL)
#define EVE_VOFFSET ((uint32_t) 13UL)
#define EVE_VCYCLE ((uint32_t) 263UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 10UL)
#define EVE_HOFFSET ((uint32_t) 70UL)
#define EVE_HCYCLE ((uint32_t) 408UL)
#define EVE_PCLK ((uint32_t) 11UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 2UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 250UL)
#endif
#endif

/* untested */
/* RVT35HHBxxxxx 320x240 3.5" Riverdi, various options, BT817 */
#if defined (EVE_RVT35H)
#define EVE_HSIZE ((uint32_t) 320UL)
#define EVE_VSIZE ((uint32_t) 240UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 4UL)
#define EVE_VOFFSET ((uint32_t) 12UL)
#define EVE_VCYCLE ((uint32_t) 260UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 4UL)
#define EVE_HOFFSET ((uint32_t) 43UL)
#define EVE_HCYCLE ((uint32_t) 371UL)
#define EVE_PCLK ((uint32_t) 12UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 4000UL) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* ########## 480 x 272 ########## */

/* EVE2-43A 480x272 4.3" Matrix Orbital, resistive or no touch, FT812 */
/* EVE2-43G 480x272 4.3" Matrix Orbital, capacitive touch, FT813 */
#if defined (EVE_EVE2_43) || defined (EVE_EVE2_43G)
#define Resolution_480x272

#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#endif

#if defined (EVE_EVE2_43G)
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* EVE3-43A 480x272 4.3" Matrix Orbital, resistive, or non-touch, BT816 */
/* EVE3-43G 480x272 4.3" Matrix Orbital, capacitive-touch, BT815 */
#if defined (EVE_EVE3_43) || defined (EVE_EVE3_43G)
#define Resolution_480x272

#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

#if defined (EVE_EVE3_43G)
#define EVE_HAS_GT911
#endif

/* 480x272 4.3" Riverdi, various options, BT815/BT816 */
#if defined (EVE_RiTFT43)
#define Resolution_480x272

#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 250UL)
#endif
#endif

/* untested */
/* RVT43HLBxxxxx 480x272 4.3" Riverdi, various options, BT817 */
#if defined (EVE_RVT43H)
#define EVE_HSIZE ((uint32_t) 480UL)
#define EVE_VSIZE ((uint32_t) 272UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 4UL)
#define EVE_VOFFSET ((uint32_t) 12UL)
#define EVE_VCYCLE ((uint32_t) 292UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 4UL)
#define EVE_HOFFSET ((uint32_t) 43UL)
#define EVE_HCYCLE ((uint32_t) 531UL)
#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 4000UL) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* PS817-043WQ-C-IPS 480x272 4.3" Panasys, BT817 */
/* PS816-043WQ-R-IPS 480x272 4.3" Panasys, resistive touch, BT816 */
#if defined (EVE_PS817_043WQ_C_IPS) || defined (EVE_PS816_043WQ_R_IPS)
#define EVE_HSIZE ((uint32_t) 480UL)
#define EVE_VSIZE ((uint32_t) 272UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 4UL)
#define EVE_VOFFSET ((uint32_t) 12UL)
#define EVE_VCYCLE ((uint32_t) 292UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 4UL)
#define EVE_HOFFSET ((uint32_t) 43UL)
#define EVE_HCYCLE ((uint32_t) 531UL)
#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#endif

#if defined (EVE_PS817_043WQ_C_IPS)
#define EVE_GEN 4
#endif

#if defined (EVE_PS816_043WQ_R_IPS)
#define EVE_GEN 3
#endif

/* untested */
/* NHD-4.3-480272FT-CxXx-xxx 480x272 4.3" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_43)
#define Resolution_480x272

#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 800UL) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* 4D-Systems GEN4-FT81x-43xx 480x272 4.3",resistive or capacitive, FT812 / FT813 */
#if defined (EVE_GEN4_FT81X_43)
#define Resolution_480x272

#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* Gameduino 3, 480x272 4.3",resistive touch, FT810 */
#if defined (EVE_GAMEDUINO3)
#define Resolution_480x272

#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 3UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#endif

/* ########## 800 x 480 ########## */

/* untested */
/* NHD-4.3-800480FT-CSXP-CTP 800x480 4.3" Newhaven, capacitive touch, FT813 */
#if defined (EVE_NHD_43_800480)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 800UL) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* FTDI/BRT EVE2 modules VM810C50A-D, ME812A-WH50R and ME813A-WH50C, 800x480 5.0" */
/* 4D-Systems GEN4 FT812/FT813 5.0/7.0 */
#if defined (EVE_VM810C) \
    || defined (EVE_ME812A) \
    || defined (EVE_ME813A) \
    || defined (EVE_GEN4_FT812_50) \
    || defined (EVE_GEN4_FT813_50) \
    || defined (EVE_GEN4_FT812_70) \
    || defined (EVE_GEN4_FT813_70)

#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* PH800480T024-IFC03 800x480 7.0" FT813 from PowerTip */
/* PH800480T013-IFC05 800x480 7.0" FT813 from PowerTip */
#if defined (EVE_PH800480)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* NHD-7.0-800480FT-CxXx-xxx 800x480 7.0" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_70)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 800UL) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* Matrix Orbital EVE2 modules EVE2-50A, EVE2-70A : 800x480 5.0" and 7.0" resistive, or no touch, FT812 */
#if defined (EVE_EVE2_50) \
    || defined (EVE_EVE2_70)

#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#endif

/* Matrix Orbital EVE2 modules EVE2-50G, EVE2-70G : 800x480 5.0" and 7.0" capacitive touch, FT813 */
/* Crystalfonts CFAF800480E0-050SC 800x480 5.0" , FT813 capacitive touch */
#if defined (EVE_EVE2_50G) \
    || defined (EVE_EVE2_70G) \
    || defined (EVE_CFAF800480E0_050SC)

#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* Crystalfonts CFAF800480E1-050SC-A2 800x480 5.0" , BT817 capacitive touch */
/* Crystalfonts CFAF800480E2-050SC-A2 800x480 5.0" , BT817 capacitive touch with overhanging glass bezel */
/* this should also work with CFAF800480E2-050SN-A2 (no touch) and CFAF800480E2-050SR-A2 (resistive touch) */
#if defined (EVE_CFAF800480Ex_050SC_A2)
#define Resolution_800x480

#define EVE_PCLK_FREQ ((uint32_t) 0x0451UL) /* value to be put into REG_PCLK_FREQ -> 30MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* Crystalfonts CFAF800480E1-050SC-A2 800x480 5.0" , BT817 capacitive touch */
/* note: there is a new variant of the CFAF800480E1-050SC-A2 which mimics a FT5316 as touch controller */
/* this should also work with CFAF800480E2-050SN-A2 (no touch) and CFAF800480E2-050SR-A2 (resistive touch) */
#if defined (EVE_CFAF800480E1_050SC_A2)
#define Resolution_800x480

#define EVE_PCLK_FREQ ((uint32_t) 0x0451UL) /* value to be put into REG_PCLK_FREQ -> 30MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
/* Crystalfonts CFA800480E3-050Sx Family, 800x480, 5.0", IPS, sunlight readable, BT817 */
/* CFA800480E3-050SN - no touch */
/* CFA800480E3-050SR - resistive touch */
/* CFA800480E3-050SC - capacitive touch */
/* CFA800480E3-050SW - capactive with wide glass bezel */
#if defined (EVE_CFA800480E3_050SX)
#define Resolution_800x480

#define EVE_PCLK_FREQ ((uint32_t) 0x0451UL) /* value to be put into REG_PCLK_FREQ -> 30MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
/* Matrix Orbital EVE3 modules EVE3-50A, EVE3-70A : 800x480 5.0" and 7.0" resistive, or no touch, BT816 */
/* PAF90B5WFNWC01 800x480 9.0" Panasys, BT815 */
#if defined (EVE_EVE3_50) \
    || defined (EVE_EVE3_70) \
    || defined (EVE_PAF90)

#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* Matrix Orbital EVE3 modules EVE3-50G, EVE3-70G : 800x480 5.0" and 7.0" capacitive touch, BT815 */
#if defined (EVE_EVE3_50G) \
    || defined (EVE_EVE3_70G)

#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_HAS_GT911
#endif

/* untested */
/* Bridgtek 800x480 5.0" BT816 */
#if defined (EVE_VM816C50AD)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* RVT50xQFxxxxx 800x480 5.0" Riverdi, various options, FT812/FT813 */
/* RVT70xQFxxxxx 800x480 7.0" Riverdi, various options, FT812/FT813, tested with RVT70UQFNWC0x */
#if defined (EVE_RVT70) \
    || defined (EVE_RVT50)

#define EVE_HSIZE ((uint32_t) 800UL) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 480UL) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 ((uint32_t) 0UL)    /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 10UL)   /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 23UL)  /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 525UL)  /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 0UL)    /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 10UL)   /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 46UL)  /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 1056UL) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK ((uint32_t) 2UL)      /* 60MHz / REG_PCLK = PCLK frequency 30 MHz */
#define EVE_PCLKPOL ((uint32_t) 1UL)   /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE ((uint32_t) 0UL)   /* Defines the arrangement of the RGB pins of the FT800 */
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 250UL)
#endif
#endif

/* untested but confirmed to be working */
/* RVT50xQBxxxxx 800x480 5.0" Riverdi, various options, BT815/BT816 */
/* not working properly? try the EVE_RiTFT70 profile */
#if defined (EVE_RiTFT50)
#define EVE_HSIZE ((uint32_t) 800UL)
#define EVE_VSIZE ((uint32_t) 480UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 23UL)
#define EVE_VCYCLE ((uint32_t) 525UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 10UL)
#define EVE_HOFFSET ((uint32_t) 46UL)
#define EVE_HCYCLE ((uint32_t) 1056UL)
#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 250UL)
#endif
#endif

/* untested but confirmed to be working */
/* RVT70xQBxxxxx 800x480 7.0" Riverdi, various options, BT815/BT816 */
/* not working properly? try the EVE_RiTFT50 profile */
#if defined (EVE_RiTFT70)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 250UL)
#endif
#endif

/* untested */
/* PS815-043W-C-IPS 800x480 4.3" Panasys, BT815 */
/* PS815-050W-C-IPS 800x480 5.0" Panasys, BT815 */
/* PS815-070W-C-IPS 800x480 7.0" Panasys, BT815 */
/* PS815-090W-C-IPS 800x480 9.0" Panasys, BT815 */
#if defined (EVE_PS815_043W_C_IPS) || defined (EVE_PS815_050W_C_IPS) || defined (EVE_PS815_070W_C_IPS) || defined (EVE_PS815_090W_C_IPS)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* Gameduino GD3X 7" shield with BT816 */
/* note: CS is on pin D8 and there is no PD pin */
#if defined (EVE_GD3X)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 3UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 3
#define EVE_SOFT_RESET
#endif

/* untested */
/* RVT50HQBxxxxx 800x480 5.0" Riverdi, various options, BT817 */
#if defined (EVE_RVT50H)
#define EVE_HSIZE ((uint32_t) 800UL)
#define EVE_VSIZE ((uint32_t) 480UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 4UL)
#define EVE_VOFFSET ((uint32_t) 8UL)
#define EVE_VCYCLE ((uint32_t) 496UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 4UL)
#define EVE_HOFFSET ((uint32_t) 8UL)
#define EVE_HCYCLE ((uint32_t) 816UL)
#define EVE_PCLK ((uint32_t) 3UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 4000UL) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* NHD-5.0-800480FT-CxXx-xxx 800x480 5.0" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_50)
#define Resolution_800x480

#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 800UL) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* FT810CB-HY50HD: FT810 800x480 5.0" HAOYU */
/* FT811CB-HY50HD: FT811 800x480 5.0" HAOYU */
#if defined (EVE_FT810CB_HY50HD) || defined (EVE_FT811CB_HY50HD)
#define EVE_HSIZE ((uint32_t) 800UL)
#define EVE_VSIZE ((uint32_t) 480UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 2UL)
#define EVE_VOFFSET ((uint32_t) 13UL)
#define EVE_VCYCLE ((uint32_t) 525UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 20UL)
#define EVE_HOFFSET ((uint32_t) 64UL)
#define EVE_HCYCLE ((uint32_t) 952UL)
#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* G-ET0700G0DM6 800x480 7.0" Glyn */
#if defined (EVE_ET07)
#define EVE_HSIZE ((uint32_t) 800UL)
#define EVE_VSIZE ((uint32_t) 480UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 2UL)
#define EVE_VOFFSET ((uint32_t) 35UL)
#define EVE_VCYCLE ((uint32_t) 525UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 128UL)
#define EVE_HOFFSET ((uint32_t) 203UL)
#define EVE_HCYCLE ((uint32_t) 1056UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#endif

/* ########## 1024 x 600 ########## */

/* ADAM101-LCP-SWVGA-NEW 1024x600 10.1" Glyn, capacitive, FT813 */
#if defined (EVE_ADAM101)
#define EVE_HSIZE ((uint32_t) 1024UL)
#define EVE_VSIZE ((uint32_t) 600UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 1UL)
#define EVE_VOFFSET ((uint32_t) 1UL)
#define EVE_VCYCLE ((uint32_t) 720UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 1UL)
#define EVE_HOFFSET ((uint32_t) 1UL)
#define EVE_HCYCLE ((uint32_t) 1100UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* tested with RVT70HSBNWC00-B */
/* RVT70HSBxxxxx 1024x600 7.0" Riverdi, various options, BT817 */
#if defined (EVE_RVT70H)
#define EVE_HSIZE ((uint32_t) 1024UL)
#define EVE_VSIZE ((uint32_t) 600UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 23UL)
#define EVE_VCYCLE ((uint32_t) 635UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 70UL)
#define EVE_HOFFSET ((uint32_t) 160UL)
#define EVE_HCYCLE ((uint32_t) 1344UL)
#define EVE_PCLK_FREQ ((uint32_t) 0x0D12UL) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 4000UL) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* PS817-070WS-C-IPS 1024x600 7.0" Panasys, BT817 */
#if defined (EVE_PS817_070WS_C_IPS)
#define EVE_HSIZE ((uint32_t) 1024UL)
#define EVE_VSIZE ((uint32_t) 600UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 23UL)
#define EVE_VCYCLE ((uint32_t) 635UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 70UL)
#define EVE_HOFFSET ((uint32_t) 160UL)
#define EVE_HCYCLE ((uint32_t) 1344UL)
#define EVE_PCLK_FREQ ((uint32_t) 0x0D12UL) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 250UL) /* backlight driver unknown -> set to default with the option to override */
#endif
#endif

/* untested but confirmed to be working */
/* EVE4x-70G IPS 1024x600 7" Matrix Orbital, capacitive touch, BT817 */
#if defined (EVE_EVE4_70G)
#define EVE_HSIZE ((uint32_t) 1024UL)
#define EVE_VSIZE ((uint32_t) 600UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 23UL)
#define EVE_VCYCLE ((uint32_t) 632UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 70UL)
#define EVE_HOFFSET ((uint32_t) 160UL)
#define EVE_HCYCLE ((uint32_t) 1344UL)
#define EVE_PCLK_FREQ ((uint32_t) 0x0D12UL) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911
#endif

/* CFAF1024600B0-070SC-A1 1024x600 7.0" Crystalfontz, BT817 */
#if defined (EVE_CFAF1024600B0_070SC_A1)
#define EVE_HSIZE   ((uint32_t) 1024UL)
#define EVE_VSIZE   ((uint32_t) 600UL)

#define EVE_VSYNC0  ((uint32_t) 1UL)
#define EVE_VSYNC1  ((uint32_t) 2UL)
#define EVE_VOFFSET ((uint32_t) 25UL)
#define EVE_VCYCLE  ((uint32_t) 626UL)
#define EVE_HSYNC0  ((uint32_t) 16UL)
#define EVE_HSYNC1  ((uint32_t) 17UL)
#define EVE_HOFFSET ((uint32_t) 177UL)
#define EVE_HCYCLE  ((uint32_t) 1344UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 3UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_PCLK_FREQ ((uint32_t) 0x0D12UL) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#endif

/* ########## 1280 x 800 ########## */

/* untested */
/* EVE4-101G 1280x800 10.1" Matrix Orbital, capacitive touch, BT817 */
#if defined (EVE_EVE4_101G)
#define EVE_HSIZE ((uint32_t) 1280UL)         /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 800UL)          /* Tvd Number of visible lines (in lines) - display height */
#define EVE_VSYNC0 ((uint32_t) 11UL)          /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 12UL)          /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 22UL)         /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 823UL)         /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 78UL)          /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 80UL)          /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 158UL)        /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 1440UL)        /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK_FREQ ((uint32_t) 0x08C1UL) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL ((uint32_t) 0UL)          /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE ((uint32_t) 0UL)          /* Defines the arrangement of the RGB pins */
#define EVE_CSPREAD ((uint32_t) 0UL) /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911
#endif

/* tested with RVT101HVBNWC00-B */
/* RVT101HVBxxxxx 1280x800 10.1" Riverdi, various options, BT817 */
/* RVT121HVBxxxxx 1280x800 12.1" Riverdi, various options, BT817 */
#if defined (EVE_RVT101H) \
    || defined (EVE_RVT121H)

#define EVE_HSIZE ((uint32_t) 1280UL)
#define EVE_VSIZE ((uint32_t) 800UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 23UL)
#define EVE_VCYCLE ((uint32_t) 838UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 20UL)
#define EVE_HOFFSET ((uint32_t) 88UL)
#define EVE_HCYCLE ((uint32_t) 1440UL)
#define EVE_PCLK_FREQ ((uint32_t) 0x08C1UL) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ ((uint32_t) 4000UL) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
#if defined (EVE_WXGA_TEST1)
#define EVE_HSIZE ((uint32_t) 1280UL)        /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 800UL)         /* Tvd Number of visible lines (in lines) - display height */
#define EVE_VSYNC0 ((uint32_t) 0UL)          /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 15UL)         /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 38UL)        /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 838UL)        /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 0UL)          /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 72UL)         /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 160UL)       /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 1440UL)       /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK_FREQ ((uint32_t) 0x08C1UL) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL ((uint32_t) 0UL)         /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE ((uint32_t) 0UL)         /* Defines the arrangement of the RGB pins */
#define EVE_CSPREAD ((uint32_t) 0UL)         /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
#if defined (EVE_WXGA_TEST2)
#define EVE_HSIZE ((uint32_t) 1280UL)        /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 800UL)         /* Tvd Number of visible lines (in lines) - display height */
#define EVE_VSYNC0 ((uint32_t) 0UL)          /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 15UL)         /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 38UL)        /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 838UL)        /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 0UL)          /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 72UL)         /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 160UL)       /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 1440UL)       /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK_FREQ ((uint32_t) 0x08C1UL) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL ((uint32_t) 1UL)         /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE ((uint32_t) 0UL)         /* Defines the arrangement of the RGB pins */
#define EVE_CSPREAD ((uint32_t) 0UL)         /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* ########## non-standard ########## */

/* untested */
/* Crystalfonts CFA240320Ex-024Sx 240x320 2.4" , FT811 */
/* CFA240320E0-024SN - no touch */
/* CFA240320E0-024SC - capacitve touch */
#if defined (EVE_CFA240320EX_024SX)
#define EVE_HSIZE ((uint32_t) 240UL)
#define EVE_VSIZE ((uint32_t) 320UL)

#define EVE_VSYNC0 ((uint32_t) 8UL)
#define EVE_VSYNC1 ((uint32_t) 12UL)
#define EVE_VOFFSET ((uint32_t) 16UL)
#define EVE_VCYCLE ((uint32_t) 337UL)
#define EVE_HSYNC0 ((uint32_t) 38UL)
#define EVE_HSYNC1 ((uint32_t) 48UL)
#define EVE_HOFFSET ((uint32_t) 68UL)
#define EVE_HCYCLE ((uint32_t) 458UL)
#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 3UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#endif

/* untested */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
/* EVE2-29A 320x102 2.9" 1U Matrix Orbital, non-touch, FT812 */
#if defined (EVE_EVE2_29)
#define EVE_HSIZE ((uint32_t) 320UL)
#define EVE_VSIZE ((uint32_t) 102UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 2UL)
#define EVE_VOFFSET ((uint32_t) 156UL)
#define EVE_VCYCLE ((uint32_t) 262UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 10UL)
#define EVE_HOFFSET ((uint32_t) 70UL)
#define EVE_HCYCLE ((uint32_t) 408UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 9UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE ((uint32_t) 320UL)
#define EVE_VSIZE_VISIBLE ((uint32_t) 102UL)
#endif

/* untested */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
/* EVE3-29A 320x102 2.9" 1U Matrix Orbital, non-touch, BT816 */
#if defined (EVE_EVE3_29)
#define EVE_HSIZE ((uint32_t) 320UL)
#define EVE_VSIZE ((uint32_t) 102UL)

#define EVE_VSYNC0 ((uint32_t) 0UL)
#define EVE_VSYNC1 ((uint32_t) 2UL)
#define EVE_VOFFSET ((uint32_t) 156UL)
#define EVE_VCYCLE ((uint32_t) 262UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 10UL)
#define EVE_HOFFSET ((uint32_t) 70UL)
#define EVE_HCYCLE ((uint32_t) 408UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 11UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 3
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE ((uint32_t) 320UL)
#define EVE_VSIZE_VISIBLE ((uint32_t) 102UL)
#endif

/* Crystalfonts CFAF240400C1-030SC 240x400 3.0" , FT811 capacitive touch */
#if defined (EVE_CFAF240400C1_030SC)
#define EVE_HSIZE ((uint32_t) 240UL)
#define EVE_VSIZE ((uint32_t) 400UL)

#define EVE_VSYNC0 ((uint32_t) 4UL)
#define EVE_VSYNC1 ((uint32_t) 6UL)
#define EVE_VOFFSET ((uint32_t) 8UL)
#define EVE_VCYCLE ((uint32_t) 409UL)
#define EVE_HSYNC0 ((uint32_t) 10UL)
#define EVE_HSYNC1 ((uint32_t) 20UL)
#define EVE_HOFFSET ((uint32_t) 40UL)
#define EVE_HCYCLE ((uint32_t) 489UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 2UL)
#define EVE_PCLK ((uint32_t) 5UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#endif

/* untested */
/* Crystalfonts CFA240400E1-030Tx 240x400 3.0" , FT811 */
/* CFA240400E1-030TN - no touch */
/* CFA240400E1-030TC - capacitve touch */
#if defined (EVE_CFA240400E1_030TX)
#define EVE_HSIZE ((uint32_t) 240UL)
#define EVE_VSIZE ((uint32_t) 400UL)

#define EVE_VSYNC0 ((uint32_t) 41UL)
#define EVE_VSYNC1 ((uint32_t) 43UL)
#define EVE_VOFFSET ((uint32_t) 45UL)
#define EVE_VCYCLE ((uint32_t) 444UL)
#define EVE_HSYNC0 ((uint32_t) 2UL)
#define EVE_HSYNC1 ((uint32_t) 4UL)
#define EVE_HOFFSET ((uint32_t) 8UL)
#define EVE_HCYCLE ((uint32_t) 370UL)
#define EVE_PCLKPOL ((uint32_t) 0UL)
#define EVE_SWIZZLE ((uint32_t) 2UL)
#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#endif

/* EVE2-38G 480x116 3.8" 1U Matrix Orbital, capacitive touch, FT813 */
/* EVE2-38A 480x116 3.8" 1U Matrix Orbital, resistive touch, FT812 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE2_38) || defined (EVE_EVE2_38G)
#define EVE_HSIZE ((uint32_t) 480UL)
#define EVE_VSIZE ((uint32_t) 272UL)

#define EVE_VSYNC0 ((uint32_t) 152UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 12UL)
#define EVE_VCYCLE ((uint32_t) 292UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 41UL)
#define EVE_HOFFSET ((uint32_t) 43UL)
#define EVE_HCYCLE ((uint32_t) 524UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 6UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 2
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE ((uint32_t) 480UL)
#define EVE_VSIZE_VISIBLE ((uint32_t) 116UL)
#endif

#if defined (EVE_EVE2_38G)
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* EVE3-38G 480x116 3.8" 1U Matrix Orbital, capacitive touch, BT815 */
/* EVE3x-38A 480x116 3.8" 1U Matrix Orbital, resistive touch, BT816 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE3x_38) || defined (EVE_EVE3x_38G)
#define EVE_HSIZE ((uint32_t) 480UL)
#define EVE_VSIZE ((uint32_t) 272UL)

#define EVE_VSYNC0 ((uint32_t) 152UL)
#define EVE_VSYNC1 ((uint32_t) 10UL)
#define EVE_VOFFSET ((uint32_t) 12UL)
#define EVE_VCYCLE ((uint32_t) 292UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 41UL)
#define EVE_HOFFSET ((uint32_t) 43UL)
#define EVE_HCYCLE ((uint32_t) 524UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_GEN 3
#define EVE_HSIZE_VISIBLE ((uint32_t) 480UL)
#define EVE_VSIZE_VISIBLE ((uint32_t) (116UL))
#endif

#if defined (EVE_EVE3x_38G)
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* EVE3x-39G 480x128 3.9" 1U Matrix Orbital, capacitive touch, BT815 */
/* EVE3x-39A 480x128 3.9" 1U Matrix Orbital, resistive touch, BT816 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE3x_39) || defined (EVE_EVE3x_39G)
#define EVE_HSIZE ((uint32_t) 480UL)
#define EVE_VSIZE ((uint32_t) 272UL)

#define EVE_VSYNC0 ((uint32_t) 7UL)
#define EVE_VSYNC1 ((uint32_t) 8UL)
#define EVE_VOFFSET ((uint32_t) 12UL)
#define EVE_VCYCLE ((uint32_t) 288UL)
#define EVE_HSYNC0 ((uint32_t) 0UL)
#define EVE_HSYNC1 ((uint32_t) 44UL)
#define EVE_HOFFSET ((uint32_t) 16UL)
#define EVE_HCYCLE ((uint32_t) 524UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_CSPREAD ((uint32_t) 1UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE ((uint32_t) 480UL)
#define EVE_VSIZE_VISIBLE ((uint32_t) 128UL)
#endif

#if defined (EVE_EVE3x_39G)
#define EVE_HAS_GT911
#endif

/* Crystalfonts CFAF480128A0-039TC 480x128 3.9" , FT811 capacitive touch */
#if defined (EVE_CFAF480128A0_039TC)
#define EVE_HSIZE ((uint32_t) 480UL)
#define EVE_VSIZE ((uint32_t) 128UL)

#define EVE_VSYNC0 ((uint32_t) 4UL)
#define EVE_VSYNC1 ((uint32_t) 5UL)
#define EVE_VOFFSET ((uint32_t) 8UL)
#define EVE_VCYCLE ((uint32_t) 137UL)
#define EVE_HSYNC0 ((uint32_t) 24UL)
#define EVE_HSYNC1 ((uint32_t) 35UL)
#define EVE_HOFFSET ((uint32_t) 41UL)
#define EVE_HCYCLE ((uint32_t) 1042UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_PCLK ((uint32_t) 7UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_GEN 2
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE ((uint32_t) 480UL)
#define EVE_VSIZE_VISIBLE ((uint32_t) 128UL)
#endif

/* untested */
/* note: timing parameters from Matrix Orbital, does not use the second pll, 58,64 FPS */
/* EVE4-40G 720x720 4.0" Matrix Orbital, capacitive touch, BT817 */
#if defined (EVE_EVE4_40G)
#define EVE_HSIZE ((uint32_t) 720UL)
#define EVE_VSIZE ((uint32_t) 720UL)

#define EVE_VSYNC0 ((uint32_t) 16UL)
#define EVE_VSYNC1 ((uint32_t) 18UL)
#define EVE_VOFFSET ((uint32_t) 35UL)
#define EVE_VCYCLE ((uint32_t) 756UL)
#define EVE_HSYNC0 ((uint32_t) 46UL)
#define EVE_HSYNC1 ((uint32_t) 48UL)
#define EVE_HOFFSET ((uint32_t) 91UL)
#define EVE_HCYCLE ((uint32_t) 812UL)
#define EVE_PCLK ((uint32_t) 2UL)
#define EVE_PCLKPOL ((uint32_t) 1UL)
#define EVE_SWIZZLE ((uint32_t) 0UL)
#define EVE_CSPREAD ((uint32_t) 0UL)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911
#endif

/* ########## Common Timings ########## */

#if defined (Resolution_320x240)
#define EVE_HSIZE ((uint32_t) 320UL) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 240UL) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 ((uint32_t) 0UL)   /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 2UL)   /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 18UL) /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 262UL) /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 0UL)   /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 10UL)  /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 70UL) /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 408UL) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#endif

#if defined (Resolution_480x272)
#define EVE_HSIZE ((uint32_t) 480UL) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 272UL) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 ((uint32_t) 0UL)   /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 10UL)  /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 12UL) /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 292UL) /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 0UL)   /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 41UL)  /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 43UL) /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 548UL) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#endif

#if defined (Resolution_800x480)
#define EVE_HSIZE ((uint32_t) 800UL) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE ((uint32_t) 480UL) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 ((uint32_t) 0UL)   /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 ((uint32_t) 3UL)   /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET ((uint32_t) 32UL) /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE ((uint32_t) 525UL) /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 ((uint32_t) 0UL)   /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 ((uint32_t) 48UL)  /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET ((uint32_t) 88UL) /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE ((uint32_t) 928UL) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#endif

#if !defined (EVE_HSIZE)
#error "Please add a define for the desired display to your build-environment, e.g. -DEVE_EVE3_50G"
#endif

#endif /* EVE_CONFIG_H */
