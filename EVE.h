/*
@file    EVE.h
@brief   Contains FT80x/FT81x/BT81x API definitions
@version 5.0
@date    2023-06-24
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

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
- started to add BT817 / BT818 defines
- cleanup: removed FT80x defines
- replaced BT81X_ENABLE with "EVE_GEN > 2"
- removed FT81X_ENABLE as FT81x already is the lowest supported chip revision now
- added more BT817 / BT818 defines
- removed undocumented registers and commands
- merged FT80x and FT81x definitions as FT81x is baseline now
- removed the history from before 4.0
- fixed typo: REG_AH_CYCLE_MAX -> REG_AH_HCYCLE_MAX
- re-arranged the host commands, removed EVE_CLKINT for BT817/BT818,
    removed FT80x EVE_CLK36M and EVE_CLK48M
- added EVE_OPT_OVERLAY
- removed the 4.0 history
- fixed some MISRA-C issues
- removed macro BEGIN(prim) - use (DL_BEGIN | EVE_BITMAPS) for example
- removed macro END() - use define DL_END
- removed macro RESTORE_CONTEXT() - use define DL_RESTORE_CONTEXT
- removed macro RETURN() - use define DL_RETURN
- removed macro SAVE_CONTEXT() - use define DL_SAVE_CONTEXT
- basic maintenance: checked for violations of white space and indent rules
- more linter fixes
- changed EVE_COMPRESSED_RGBA_ASTC_nxn_KHR to EVE_ASTC_nXn to fix linter
    warnings and used the opportunity to make these shorter
- added DL_COLOR_A as alternative to the COLOR_A macro
- added defines for all DL_ display list commands
- cleaned up the macros
- fix: changed DL_CLEAR_RGB to DL_CLEAR_COLOR_RGB
    as this is what the programming guide uses
- fix: renamed EVE_ROM_FONT_ADDR to EVE_ROM_FONTROOT
- added #ifdef __cplusplus / extern "C" to allow
    adding EVE_ functions to C++ code
- fix: typo REG_COPRO_PATCH_DTR -> REG_COPRO_PATCH_PTR

*/

#ifndef EVE_H
#define EVE_H

#include "EVE_target.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "EVE_config.h"
#include "EVE_commands.h"

/* Memory */
#define EVE_RAM_G         0x00000000UL
#define EVE_ROM_CHIPID    0x000C0000UL
#define EVE_ROM_FONT      0x001E0000UL
#define EVE_ROM_FONTROOT  0x002FFFFCUL
#define EVE_RAM_DL        0x00300000UL
#define EVE_RAM_REG       0x00302000UL
#define EVE_RAM_CMD       0x00308000UL

/* Memory buffer sizes */
#define EVE_RAM_G_SIZE   (1024U*1024UL)
#define EVE_CMDFIFO_SIZE (4U*1024UL)
#define EVE_RAM_DL_SIZE  (8U*1024UL)

/* diplay list list commands, most need OR's arguments */
#define DL_DISPLAY       0x00000000UL
#define DL_BITMAP_SOURCE 0x01000000UL
#define DL_CLEAR_COLOR_RGB 0x02000000UL
#define DL_TAG           0x03000000UL
#define DL_COLOR_RGB     0x04000000UL
#define DL_BITMAP_HANDLE 0x05000000UL
#define DL_CELL          0x06000000UL
#define DL_BITMAP_LAYOUT 0x07000000UL
#define DL_BITMAP_SIZE   0x08000000UL
#define DL_ALPHA_FUNC    0x09000000UL
#define DL_STENCIL_FUNC  0x0A000000UL
#define DL_BLEND_FUNC    0x0B000000UL
#define DL_STENCIL_OP    0x0C000000UL
#define DL_POINT_SIZE    0x0D000000UL
#define DL_LINE_WIDTH    0x0E000000UL
#define DL_CLEAR_COLOR_A 0x0F000000UL
#define DL_COLOR_A       0x10000000UL
#define DL_CLEAR_STENCIL 0x11000000UL
#define DL_CLEAR_TAG     0x12000000UL
#define DL_STENCIL_MASK  0x13000000UL
#define DL_TAG_MASK      0x14000000UL
#define DL_BITMAP_TRANSFORM_A 0x15000000UL
#define DL_BITMAP_TRANSFORM_B 0x16000000UL
#define DL_BITMAP_TRANSFORM_C 0x17000000UL
#define DL_BITMAP_TRANSFORM_D 0x18000000UL
#define DL_BITMAP_TRANSFORM_E 0x19000000UL
#define DL_BITMAP_TRANSFORM_F 0x1A000000UL
#define DL_SCISSOR_XY    0x1B000000UL
#define DL_SCISSOR_SIZE  0x1C000000UL
#define DL_CALL          0x1D000000UL
#define DL_JUMP          0x1E000000UL
#define DL_BEGIN         0x1F000000UL
#define DL_COLOR_MASK    0x20000000UL
#define DL_END           0x21000000UL
#define DL_SAVE_CONTEXT  0x22000000UL
#define DL_RESTORE_CONTEXT 0x23000000UL
#define DL_RETURN        0x24000000UL
#define DL_MACRO         0x25000000UL
#define DL_CLEAR         0x26000000UL
#define DL_VERTEX_FORMAT 0x27000000UL
#define DL_BITMAP_LAYOUT_H 0x28000000UL
#define DL_BITMAP_SIZE_H 0x29000000UL
#define DL_PALETTE_SOURCE 0x2A000000UL
#define DL_VERTEX_TRANSLATE_X 0x2B000000UL
#define DL_VERTEX_TRANSLATE_Y 0x2C000000UL
#define DL_NOP            0x2D000000UL

#define DL_VERTEX2F       0x40000000UL
#define DL_VERTEX2II      0x80000000UL

#define CLR_COL       0x4U
#define CLR_STN       0x2U
#define CLR_TAG       0x1U

/* Host commands */
#define EVE_ACTIVE       0x00U /* place FT8xx in active state */
#define EVE_STANDBY      0x41U /* place FT8xx in Standby (clk running) */
#define EVE_SLEEP        0x42U /* place FT8xx in Sleep (clk off) */
#define EVE_CLKEXT       0x44U /* select external clock source */
#if EVE_GEN < 4
#define EVE_CLKINT       0x48U /* select internal clock source, not a valid option for BT817 / BT818 */
#endif
#define EVE_PWRDOWN      0x50U /* place FT8xx in Power Down (core off) */
#define EVE_CLKSEL       0x61U /* configure system clock */
#define EVE_RST_PULSE    0x68U /* reset core - all registers default and processors reset */
#define EVE_CORERST      0x68U /* reset core - all registers default and processors reset */
#define EVE_PINDRIVE     0x70U /* setup drive strength for various pins */
#define EVE_PIN_PD_STATE 0x71U /* setup how pins behave during power down */

/* Graphic command defines */
#define EVE_NEVER      0UL
#define EVE_LESS       1UL
#define EVE_LEQUAL     2UL
#define EVE_GREATER    3UL
#define EVE_GEQUAL     4UL
#define EVE_EQUAL      5UL
#define EVE_NOTEQUAL   6UL
#define EVE_ALWAYS     7UL

/* Bitmap formats */
#define EVE_ARGB1555   0UL
#define EVE_L1         1UL
#define EVE_L4         2UL
#define EVE_L8         3UL
#define EVE_RGB332     4UL
#define EVE_ARGB2      5UL
#define EVE_ARGB4      6UL
#define EVE_RGB565     7UL
#define EVE_PALETTED   8UL
#define EVE_TEXT8X8    9UL
#define EVE_TEXTVGA    10UL
#define EVE_BARGRAPH   11UL

/* Bitmap filter types */
#define EVE_NEAREST    0UL
#define EVE_BILINEAR   1UL

/* Bitmap wrap types */
#define EVE_BORDER     0UL
#define EVE_REPEAT     1UL

/* Stencil defines */
#define EVE_KEEP       1UL
#define EVE_REPLACE    2UL
#define EVE_INCR       3UL
#define EVE_DECR       4UL
#define EVE_INVERT     5UL

/* Graphics display list swap defines */
#define EVE_DLSWAP_DONE   0UL
#define EVE_DLSWAP_LINE   1UL
#define EVE_DLSWAP_FRAME  2UL

/* Interrupt bits */
#define EVE_INT_SWAP          0x01
#define EVE_INT_TOUCH         0x02
#define EVE_INT_TAG           0x04
#define EVE_INT_SOUND         0x08
#define EVE_INT_PLAYBACK      0x10
#define EVE_INT_CMDEMPTY      0x20
#define EVE_INT_CMDFLAG       0x40
#define EVE_INT_CONVCOMPLETE  0x80

/* Touch mode */
#define EVE_TMODE_OFF        0U
#define EVE_TMODE_ONESHOT    1U
#define EVE_TMODE_FRAME      2U
#define EVE_TMODE_CONTINUOUS 3U

/* Alpha blending */
#define EVE_ZERO                 0UL
#define EVE_ONE                  1UL
#define EVE_SRC_ALPHA            2UL
#define EVE_DST_ALPHA            3UL
#define EVE_ONE_MINUS_SRC_ALPHA  4UL
#define EVE_ONE_MINUS_DST_ALPHA  5UL

/* Graphics primitives */
#define EVE_BITMAPS              1UL
#define EVE_POINTS               2UL
#define EVE_LINES                3UL
#define EVE_LINE_STRIP           4UL
#define EVE_EDGE_STRIP_R         5UL
#define EVE_EDGE_STRIP_L         6UL
#define EVE_EDGE_STRIP_A         7UL
#define EVE_EDGE_STRIP_B         8UL
#define EVE_RECTS                9UL
#define EVE_INT_G8               18UL
#define EVE_INT_L8C              12UL
#define EVE_INT_VGA              13UL
#define EVE_PALETTED565          14UL
#define EVE_PALETTED4444         15UL
#define EVE_PALETTED8            16UL
#define EVE_L2                   17UL

/* Widget command options */
#define EVE_OPT_MONO             1U
#define EVE_OPT_NODL             2U
#define EVE_OPT_FLAT             256U
#define EVE_OPT_CENTERX          512U
#define EVE_OPT_CENTERY          1024U
#define EVE_OPT_CENTER           (EVE_OPT_CENTERX | EVE_OPT_CENTERY)
#define EVE_OPT_NOBACK           4096U
#define EVE_OPT_NOTICKS          8192U
#define EVE_OPT_NOHM             16384U
#define EVE_OPT_NOPOINTER        16384U
#define EVE_OPT_NOSECS           32768U
#define EVE_OPT_NOHANDS          49152U
#define EVE_OPT_RIGHTX           2048U
#define EVE_OPT_SIGNED           256U

#define EVE_OPT_MEDIAFIFO        16U
#define EVE_OPT_FULLSCREEN       8U
#define EVE_OPT_NOTEAR           4U
#define EVE_OPT_SOUND            32U

/* ADC */
#define EVE_ADC_DIFFERENTIAL     1UL
#define EVE_ADC_SINGLE_ENDED     0UL

/* Fonts */
#define EVE_NUMCHAR_PERFONT     (128UL)  /* number of font characters per bitmap handle */
#define EVE_FONT_TABLE_SIZE     (148UL)  /* size of the font table - utilized for loopup by the graphics engine */
#define EVE_FONT_TABLE_POINTER  (0xFFFFCUL) /* pointer to the inbuilt font tables starting from bitmap handle 16 */

/* Audio sample type defines */
#define EVE_LINEAR_SAMPLES       0UL /* 8bit signed samples */
#define EVE_ULAW_SAMPLES         1UL /* 8bit ulaw samples */
#define EVE_ADPCM_SAMPLES        2UL /* 4bit ima adpcm samples */

/* Synthesized sound */
#define EVE_SILENCE      0x00U
#define EVE_SQUAREWAVE   0x01U
#define EVE_SINEWAVE     0x02U
#define EVE_SAWTOOTH     0x03U
#define EVE_TRIANGLE     0x04U
#define EVE_BEEPING      0x05U
#define EVE_ALARM        0x06U
#define EVE_WARBLE       0x07U
#define EVE_CAROUSEL     0x08U
#define EVE_PIPS(n)      (0x0FU + (n))
#define EVE_HARP         0x40U
#define EVE_XYLOPHONE    0x41U
#define EVE_TUBA         0x42U
#define EVE_GLOCKENSPIEL 0x43U
#define EVE_ORGAN        0x44U
#define EVE_TRUMPET      0x45U
#define EVE_PIANO        0x46U
#define EVE_CHIMES       0x47U
#define EVE_MUSICBOX     0x48U
#define EVE_BELL         0x49U
#define EVE_CLICK        0x50U
#define EVE_SWITCH       0x51U
#define EVE_COWBELL      0x52U
#define EVE_NOTCH        0x53U
#define EVE_HIHAT        0x54U
#define EVE_KICKDRUM     0x55U
#define EVE_POP          0x56U
#define EVE_CLACK        0x57U
#define EVE_CHACK        0x58U
#define EVE_MUTE         0x60U
#define EVE_UNMUTE       0x61U

/* Synthesized sound frequencies, midi note */
#define EVE_MIDI_A0   21U
#define EVE_MIDI_A_0  22U
#define EVE_MIDI_B0   23U
#define EVE_MIDI_C1   24U
#define EVE_MIDI_C_1  25U
#define EVE_MIDI_D1   26U
#define EVE_MIDI_D_1  27U
#define EVE_MIDI_E1   28U
#define EVE_MIDI_F1   29U
#define EVE_MIDI_F_1  30U
#define EVE_MIDI_G1   31U
#define EVE_MIDI_G_1  32U
#define EVE_MIDI_A1   33U
#define EVE_MIDI_A_1  34U
#define EVE_MIDI_B1   35U
#define EVE_MIDI_C2   36U
#define EVE_MIDI_C_2  37U
#define EVE_MIDI_D2   38U
#define EVE_MIDI_D_2  39U
#define EVE_MIDI_E2   40U
#define EVE_MIDI_F2   41U
#define EVE_MIDI_F_2  42U
#define EVE_MIDI_G2   43U
#define EVE_MIDI_G_2  44U
#define EVE_MIDI_A2   45U
#define EVE_MIDI_A_2  46U
#define EVE_MIDI_B2   47U
#define EVE_MIDI_C3   48U
#define EVE_MIDI_C_3  49U
#define EVE_MIDI_D3   50U
#define EVE_MIDI_D_3  51U
#define EVE_MIDI_E3   52U
#define EVE_MIDI_F3   53U
#define EVE_MIDI_F_3  54U
#define EVE_MIDI_G3   55U
#define EVE_MIDI_G_3  56U
#define EVE_MIDI_A3   57U
#define EVE_MIDI_A_3  58U
#define EVE_MIDI_B3   59U
#define EVE_MIDI_C4   60U
#define EVE_MIDI_C_4  61U
#define EVE_MIDI_D4   62U
#define EVE_MIDI_D_4  63U
#define EVE_MIDI_E4   64U
#define EVE_MIDI_F4   65U
#define EVE_MIDI_F_4  66U
#define EVE_MIDI_G4   67U
#define EVE_MIDI_G_4  68U
#define EVE_MIDI_A4   69U
#define EVE_MIDI_A_4  70U
#define EVE_MIDI_B4   71U
#define EVE_MIDI_C5   72U
#define EVE_MIDI_C_5  73U
#define EVE_MIDI_D5   74U
#define EVE_MIDI_D_5  75U
#define EVE_MIDI_E5   76U
#define EVE_MIDI_F5   77U
#define EVE_MIDI_F_5  78U
#define EVE_MIDI_G5   79U
#define EVE_MIDI_G_5  80U
#define EVE_MIDI_A5   81U
#define EVE_MIDI_A_5  82U
#define EVE_MIDI_B5   83U
#define EVE_MIDI_C6   84U
#define EVE_MIDI_C_6  85U
#define EVE_MIDI_D6   86U
#define EVE_MIDI_D_6  87U
#define EVE_MIDI_E6   88U
#define EVE_MIDI_F6   89U
#define EVE_MIDI_F_6  90U
#define EVE_MIDI_G6   91U
#define EVE_MIDI_G_6  92U
#define EVE_MIDI_A6   93U
#define EVE_MIDI_A_6  94U
#define EVE_MIDI_B6   95U
#define EVE_MIDI_C7   96U
#define EVE_MIDI_C_7  97U
#define EVE_MIDI_D7   98U
#define EVE_MIDI_D_7  99U
#define EVE_MIDI_E7   100U
#define EVE_MIDI_F7   101U
#define EVE_MIDI_F_7  102U
#define EVE_MIDI_G7   103U
#define EVE_MIDI_G_7  104U
#define EVE_MIDI_A7   105U
#define EVE_MIDI_A_7  106U
#define EVE_MIDI_B7   107U
#define EVE_MIDI_C8   108U

/* GPIO bits */
#define EVE_GPIO0  0U
#define EVE_GPIO1  1U /* default gpio pin for audio shutdown, 1 - enable, 0 - disable */
#define EVE_GPIO7  7U /* default gpio pin for display enable, 1 - enable, 0 - disable */

/* Display rotation */
#define EVE_DISPLAY_0   0U /* 0 degrees rotation */
#define EVE_DISPLAY_180 1U /* 180 degrees rotation */

/* Commands */
#define CMD_APPEND       0xFFFFFF1EUL
#define CMD_BGCOLOR      0xFFFFFF09UL
#define CMD_BUTTON       0xFFFFFF0DUL
#define CMD_CALIBRATE    0xFFFFFF15UL
#define CMD_CLOCK        0xFFFFFF14UL
#define CMD_COLDSTART    0xFFFFFF32UL
#define CMD_DIAL         0xFFFFFF2DUL
#define CMD_DLSTART      0xFFFFFF00UL
#define CMD_FGCOLOR      0xFFFFFF0AUL
#define CMD_GAUGE        0xFFFFFF13UL
#define CMD_GETMATRIX    0xFFFFFF33UL
#define CMD_GETPROPS     0xFFFFFF25UL
#define CMD_GETPTR       0xFFFFFF23UL
#define CMD_GRADCOLOR    0xFFFFFF34UL
#define CMD_GRADIENT     0xFFFFFF0BUL
#define CMD_INFLATE      0xFFFFFF22UL
#define CMD_INTERRUPT    0xFFFFFF02UL
#define CMD_KEYS         0xFFFFFF0EUL
#define CMD_LOADIDENTITY 0xFFFFFF26UL
#define CMD_LOADIMAGE    0xFFFFFF24UL
#define CMD_LOGO         0xFFFFFF31UL
#define CMD_MEDIAFIFO    0xFFFFFF39UL
#define CMD_MEMCPY       0xFFFFFF1DUL
#define CMD_MEMCRC       0xFFFFFF18UL
#define CMD_MEMSET       0xFFFFFF1BUL
#define CMD_MEMWRITE     0xFFFFFF1AUL
#define CMD_MEMZERO      0xFFFFFF1CUL
#define CMD_NUMBER       0xFFFFFF2EUL
#define CMD_PLAYVIDEO    0xFFFFFF3AUL
#define CMD_PROGRESS     0xFFFFFF0FUL
#define CMD_REGREAD      0xFFFFFF19UL
#define CMD_ROMFONT      0xFFFFFF3FUL
#define CMD_ROTATE       0xFFFFFF29UL
#define CMD_SCALE        0xFFFFFF28UL
#define CMD_SCREENSAVER  0xFFFFFF2FUL
#define CMD_SCROLLBAR    0xFFFFFF11UL
#define CMD_SETBASE      0xFFFFFF38UL
#define CMD_SETBITMAP    0xFFFFFF43UL
#define CMD_SETFONT      0xFFFFFF2BUL
#define CMD_SETFONT2     0xFFFFFF3BUL
#define CMD_SETMATRIX    0xFFFFFF2AUL
#define CMD_SETROTATE    0xFFFFFF36UL
#define CMD_SETSCRATCH   0xFFFFFF3CUL
#define CMD_SKETCH       0xFFFFFF30UL
#define CMD_SLIDER       0xFFFFFF10UL
#define CMD_SNAPSHOT     0xFFFFFF1FUL
#define CMD_SNAPSHOT2    0xFFFFFF37UL
#define CMD_SPINNER      0xFFFFFF16UL
#define CMD_STOP         0xFFFFFF17UL
#define CMD_SWAP         0xFFFFFF01UL
#define CMD_TEXT         0xFFFFFF0CUL
#define CMD_TOGGLE       0xFFFFFF12UL
#define CMD_TRACK        0xFFFFFF2CUL
#define CMD_TRANSLATE    0xFFFFFF27UL
#define CMD_VIDEOFRAME   0xFFFFFF41UL
#define CMD_VIDEOSTART   0xFFFFFF40UL

/* Registers */
#define REG_ANA_COMP         0x00302184UL /* only listed in datasheet */
#define REG_BIST_EN          0x00302174UL /* only listed in datasheet */
#define REG_CLOCK            0x00302008UL
#define REG_CMDB_SPACE       0x00302574UL
#define REG_CMDB_WRITE       0x00302578UL
#define REG_CMD_DL           0x00302100UL
#define REG_CMD_READ         0x003020f8UL
#define REG_CMD_WRITE        0x003020fcUL
#define REG_CPURESET         0x00302020UL
#define REG_CSPREAD          0x00302068UL
#define REG_CTOUCH_EXTENDED  0x00302108UL
#define REG_CTOUCH_TOUCH0_XY 0x00302124UL /* only listed in datasheet */
#define REG_CTOUCH_TOUCH4_X  0x0030216cUL
#define REG_CTOUCH_TOUCH4_Y  0x00302120UL
#define REG_CTOUCH_TOUCH1_XY 0x0030211cUL
#define REG_CTOUCH_TOUCH2_XY 0x0030218cUL
#define REG_CTOUCH_TOUCH3_XY 0x00302190UL
#define REG_TOUCH_CONFIG     0x00302168UL
#define REG_DATESTAMP        0x00302564UL /* only listed in datasheet */
#define REG_DITHER           0x00302060UL
#define REG_DLSWAP           0x00302054UL
#define REG_FRAMES           0x00302004UL
#define REG_FREQUENCY        0x0030200cUL
#define REG_GPIO             0x00302094UL
#define REG_GPIOX            0x0030209cUL
#define REG_GPIOX_DIR        0x00302098UL
#define REG_GPIO_DIR         0x00302090UL
#define REG_HCYCLE           0x0030202cUL
#define REG_HOFFSET          0x00302030UL
#define REG_HSIZE            0x00302034UL
#define REG_HSYNC0           0x00302038UL
#define REG_HSYNC1           0x0030203cUL
#define REG_ID               0x00302000UL
#define REG_INT_EN           0x003020acUL
#define REG_INT_FLAGS        0x003020a8UL
#define REG_INT_MASK         0x003020b0UL
#define REG_MACRO_0          0x003020d8UL
#define REG_MACRO_1          0x003020dcUL
#define REG_MEDIAFIFO_READ   0x00309014UL /* only listed in programmers guide */
#define REG_MEDIAFIFO_WRITE  0x00309018UL /* only listed in programmers guide */
#define REG_OUTBITS          0x0030205cUL
#define REG_PCLK             0x00302070UL
#define REG_PCLK_POL         0x0030206cUL
#define REG_PLAY             0x0030208cUL
#define REG_PLAYBACK_FORMAT  0x003020c4UL
#define REG_PLAYBACK_FREQ    0x003020c0UL
#define REG_PLAYBACK_LENGTH  0x003020b8UL
#define REG_PLAYBACK_LOOP    0x003020c8UL
#define REG_PLAYBACK_PLAY    0x003020ccUL
#define REG_PLAYBACK_READPTR 0x003020bcUL
#define REG_PLAYBACK_START   0x003020b4UL
#define REG_PWM_DUTY         0x003020d4UL
#define REG_PWM_HZ           0x003020d0UL
#define REG_RENDERMODE       0x00302010UL /* only listed in datasheet */
#define REG_ROTATE           0x00302058UL
#define REG_SNAPFORMAT       0x0030201cUL /* only listed in datasheet */
#define REG_SNAPSHOT         0x00302018UL /* only listed in datasheet */
#define REG_SNAPY            0x00302014UL /* only listed in datasheet */
#define REG_SOUND            0x00302088UL
#define REG_SPI_WIDTH        0x00302188UL /* listed with false offset in programmers guide V1.1 */
#define REG_SWIZZLE          0x00302064UL
#define REG_TAG              0x0030207cUL
#define REG_TAG_X            0x00302074UL
#define REG_TAG_Y            0x00302078UL
#define REG_TAP_CRC          0x00302024UL /* only listed in datasheet */
#define REG_TAP_MASK         0x00302028UL /* only listed in datasheet */
#define REG_TOUCH_ADC_MODE   0x00302108UL
#define REG_TOUCH_CHARGE     0x0030210cUL
#define REG_TOUCH_DIRECT_XY  0x0030218cUL
#define REG_TOUCH_DIRECT_Z1Z2 0x00302190UL
#define REG_TOUCH_MODE       0x00302104UL
#define REG_TOUCH_OVERSAMPLE 0x00302114UL
#define REG_TOUCH_RAW_XY     0x0030211cUL
#define REG_TOUCH_RZ         0x00302120UL
#define REG_TOUCH_RZTHRESH   0x00302118UL
#define REG_TOUCH_SCREEN_XY  0x00302124UL
#define REG_TOUCH_SETTLE     0x00302110UL
#define REG_TOUCH_TAG        0x0030212cUL
#define REG_TOUCH_TAG1       0x00302134UL /* only listed in datasheet */
#define REG_TOUCH_TAG1_XY    0x00302130UL /* only listed in datasheet */
#define REG_TOUCH_TAG2       0x0030213cUL /* only listed in datasheet */
#define REG_TOUCH_TAG2_XY    0x00302138UL /* only listed in datasheet */
#define REG_TOUCH_TAG3       0x00302144UL /* only listed in datasheet */
#define REG_TOUCH_TAG3_XY    0x00302140UL /* only listed in datasheet */
#define REG_TOUCH_TAG4       0x0030214cUL /* only listed in datasheet */
#define REG_TOUCH_TAG4_XY    0x00302148UL /* only listed in datasheet */
#define REG_TOUCH_TAG_XY     0x00302128UL
#define REG_TOUCH_TRANSFORM_A 0x00302150UL
#define REG_TOUCH_TRANSFORM_B 0x00302154UL
#define REG_TOUCH_TRANSFORM_C 0x00302158UL
#define REG_TOUCH_TRANSFORM_D 0x0030215cUL
#define REG_TOUCH_TRANSFORM_E 0x00302160UL
#define REG_TOUCH_TRANSFORM_F 0x00302164UL
#define REG_TRACKER          0x00309000UL /* only listed in programmers guide */
#define REG_TRACKER_1        0x00309004UL /* only listed in programmers guide */
#define REG_TRACKER_2        0x00309008UL /* only listed in programmers guide */
#define REG_TRACKER_3        0x0030900cUL /* only listed in programmers guide */
#define REG_TRACKER_4        0x00309010UL /* only listed in programmers guide */
#define REG_TRIM             0x00302180UL
#define REG_VCYCLE           0x00302040UL
#define REG_VOFFSET          0x00302044UL
#define REG_VOL_PB           0x00302080UL
#define REG_VOL_SOUND        0x00302084UL
#define REG_VSIZE            0x00302048UL
#define REG_VSYNC0           0x0030204cUL
#define REG_VSYNC1           0x00302050UL


/* Macros for static display list generation */
#define ALPHA_FUNC(func,ref) ((DL_ALPHA_FUNC) | (((func) & 7UL) << 8U) | ((ref) & 0xFFUL))
#define BITMAP_HANDLE(handle) ((DL_BITMAP_HANDLE) | ((handle) & 0x1FUL))
#define BITMAP_LAYOUT(format,linestride,height) ((DL_BITMAP_LAYOUT) | (((format) & 0x1FUL) << 19U) | (((linestride) & 0x3FFUL) << 9U) | ((height) & 0x1FFUL))
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((DL_BITMAP_SIZE) | (((filter) & 1UL) << 20U) | (((wrapx) & 1UL) << 19U) | (((wrapy) & 1UL) << 18U) | (((width) & 0x1FFUL) << 9U) | ((height) & 0x1FFUL))

/* beware, this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits */
#define BITMAP_LAYOUT_H(linestride,height) ((DL_BITMAP_LAYOUT_H) | (((((linestride) & 0xC00U) >> 10U)&3UL) << 2U) | ((((height) & 0x600U) >> 9U) & 3UL))

/* beware, this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits */
#define BITMAP_SIZE_H(width,height) ((DL_BITMAP_SIZE_H) | (((((width) & 0x600U) >> 9U) & 3UL) << 2U) | ((((height) & 0x600U) >> 9U) & 3UL))

#define BITMAP_SOURCE(addr) ((DL_BITMAP_SOURCE) | ((addr) & 0x3FFFFFUL))

#if EVE_GEN < 3 /* only define these for FT81x */
#define BITMAP_TRANSFORM_A(a) ((DL_BITMAP_TRANSFORM_A) | ((a) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_B(b) ((DL_BITMAP_TRANSFORM_B) | ((b) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_D(d) ((DL_BITMAP_TRANSFORM_D) | ((d) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_E(e) ((DL_BITMAP_TRANSFORM_E) | ((e) & 0x1FFFFUL))
#endif

#define BITMAP_TRANSFORM_C(c) ((DL_BITMAP_TRANSFORM_C) | ((c) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_F(f) ((DL_BITMAP_TRANSFORM_F) | ((f) & 0x1FFFFUL))

#define BLEND_FUNC(src,dst) ((DL_BLEND_FUNC) | (((src) & 7UL) << 3U) | ((dst) & 7UL))
#define CALL(dest) ((DL_CALL) | ((dest) & 0xFFFFUL))
#define CELL(cell) ((DL_CELL) | ((cell) & 0x7FUL))
#define CLEAR(c,s,t) ((DL_CLEAR) | (((c) & 1UL) << 2U) | (((s) & 1UL) << 1U) | ((t) & 1UL))
#define CLEAR_COLOR_A(alpha) ((DL_CLEAR_COLOR_A) | ((alpha) & 0xFFUL))
#define CLEAR_COLOR_RGB(red,green,blue) ((DL_CLEAR_COLOR_RGB) | (((red) & 0xFFUL) << 16U) | (((green) & 0xFFUL) << 8U) | ((blue) & 0xFFUL))
#define CLEAR_STENCIL(s) ((DL_CLEAR_STENCIL) | ((s) & 0xFFUL))
#define CLEAR_TAG(s) ((DL_CLEAR_TAG) | ((s) & 0xFFUL))
#define COLOR_A(alpha) ((DL_COLOR_A) | ((alpha) & 0xFFUL))
#define COLOR_MASK(r,g,b,a) ((DL_COLOR_MASK) | (((r) & 1UL) << 3U) | (((g) & 1UL) << 2U) | (((b) & 1UL) << 1U) | ((a) & 1UL))
#define COLOR_RGB(red,green,blue) ((DL_COLOR_RGB) | (((red) & 0xFFUL) << 16U) | (((green) & 0xFFUL) << 8U) | ((blue) & 0xFFUL))
#define JUMP(dest) ((DL_JUMP) | ((dest) & 0xFFFFUL))
#define LINE_WIDTH(width) ((DL_LINE_WIDTH) | (((uint32_t) (width)) & 0xFFFUL))
#define MACRO(m) ((DL_MACRO) | ((m) & 1UL))
#define PALETTE_SOURCE(addr) ((DL_PALETTE_SOURCE) | ((addr) & 0x3FFFFF3UL))
#define POINT_SIZE(size) ((DL_POINT_SIZE) | ((size) & 0x1FFFUL))
#define SCISSOR_SIZE(width,height) ((DL_SCISSOR_SIZE) | (((width) & 0xFFFUL) << 12U) | ((height) & 0xFFFUL))
#define SCISSOR_XY(x,y) ((DL_SCISSOR_XY) | (((x) & 0x7FFUL) << 11U) | ((y) & 0x7FFUL))
#define STENCIL_FUNC(func,ref,mask) ((DL_STENCIL_FUNC) | (((func) & 7UL) << 16U) | (((ref) & 0xFFUL) << 8U)|((mask) & 0xFFUL))
#define STENCIL_MASK(mask) ((DL_STENCIL_MASK) | ((mask) & 0xFFUL))
#define STENCIL_OP(sfail,spass) ((DL_STENCIL_OP) | (((sfail) & 7UL) << 3U) | ((spass) & 7UL))
#define TAG(s) ((DL_TAG) | ((s) & 0xFFUL))
#define TAG_MASK(mask) ((DL_TAG_MASK) | ((mask) & 1UL))
#define VERTEX2F(x,y) ((DL_VERTEX2F) | ((((uint32_t) (x)) & 0x7FFFUL) << 15U) | (((uint32_t) (y)) & 0x7FFFUL))
#define VERTEX2II(x,y,handle,cell) ((DL_VERTEX2II) | (((x) & 0x1FFUL) << 21U) | (((y) & 0x1FFUL) << 12U) | (((handle) & 0x1FUL) << 7U) | ((cell) & 0x7FUL))
#define VERTEX_FORMAT(frac) ((DL_VERTEX_FORMAT) | ((frac) & 7UL))
#define VERTEX_TRANSLATE_X(x) ((DL_VERTEX_TRANSLATE_X) | ((x) & 0x1FFFFUL))
#define VERTEX_TRANSLATE_Y(y) ((DL_VERTEX_TRANSLATE_Y) | ((y) & 0x1FFFFUL))

/* #define BEGIN(prim) ((DL_BEGIN) | ((prim) & 15UL)) */ /* use define DL_BEGIN */
/* #define DISPLAY() ((DL_DISPLAY)) */ /* use define DL_DISPLAY */
/* #define END() ((DL_END)) */ /* use define DL_END */
/* #define RESTORE_CONTEXT() ((DL_RESTORE_CONTEXT)) */ /* use define DL_RESTORE_CONTEXT */
/* #define RETURN() ((DL_RETURN)) */ /* use define DL_RETURN */
/* #define SAVE_CONTEXT() ((DL_SAVE_CONTEXT)) */ /* use define DL_SAVE_CONTEXT */
/* #define NOP() ((DL_NOP)) */

/* ########## EVE Generation 3: BT815 / BT816 definitions ########## */

#if EVE_GEN > 2

#define EVE_GLFORMAT 31UL /* used with BITMAP_LAYOUT to indicate bitmap-format is specified by BITMAP_EXT_FORMAT */

#define DL_BITMAP_EXT_FORMAT 0x2E000000UL /* requires OR'd arguments */
#define DL_BITMAP_SWIZZLE    0x2F000000UL
/* #define DL_INT_FRR           0x30000000UL */ /* ESE displays "Internal: flash read result" - undocumented display list command */

/* Extended Bitmap formats */
#define EVE_ASTC_4X4   37808UL
#define EVE_ASTC_5X4   37809UL
#define EVE_ASTC_5X5   37810UL
#define EVE_ASTC_6X5   37811UL
#define EVE_ASTC_6X6   37812UL
#define EVE_ASTC_8X5   37813UL
#define EVE_ASTC_8X6   37814UL
#define EVE_ASTC_8X8   37815UL
#define EVE_ASTC_10X5  37816UL
#define EVE_ASTC_10X6  37817UL
#define EVE_ASTC_10X8  37818UL
#define EVE_ASTC_10X10 37819UL
#define EVE_ASTC_12X10 37820UL
#define EVE_ASTC_12X12 37821UL

#define EVE_RAM_ERR_REPORT      0x309800UL /* max 128 bytes null terminated string */
#define EVE_RAM_FLASH           0x800000UL
#define EVE_RAM_FLASH_POSTBLOB  0x801000UL

#define EVE_OPT_FLASH  64U
#define EVE_OPT_OVERLAY 128U
#define EVE_OPT_FORMAT 4096U
#define EVE_OPT_FILL   8192U

/* Commands for BT815 / BT816 */
#define CMD_BITMAP_TRANSFORM 0xFFFFFF21UL
#define CMD_SYNC             0xFFFFFF42UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_SYNC) */
#define CMD_FLASHERASE       0xFFFFFF44UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHERASE) */
#define CMD_FLASHWRITE       0xFFFFFF45UL
#define CMD_FLASHREAD        0xFFFFFF46UL
#define CMD_FLASHUPDATE      0xFFFFFF47UL
#define CMD_FLASHDETACH      0xFFFFFF48UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHDETACH) */
#define CMD_FLASHATTACH      0xFFFFFF49UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHATTACH) */
#define CMD_FLASHFAST        0xFFFFFF4AUL
#define CMD_FLASHSPIDESEL    0xFFFFFF4BUL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHSPIDESEL) */
#define CMD_FLASHSPITX       0xFFFFFF4CUL
#define CMD_FLASHSPIRX       0xFFFFFF4DUL
#define CMD_FLASHSOURCE      0xFFFFFF4EUL
#define CMD_CLEARCACHE       0xFFFFFF4FUL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_CLEARCACHE) */
#define CMD_INFLATE2         0xFFFFFF50UL
#define CMD_ROTATEAROUND     0xFFFFFF51UL
#define CMD_RESETFONTS       0xFFFFFF52UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_RESETFONTS) */
#define CMD_ANIMSTART        0xFFFFFF53UL
#define CMD_ANIMSTOP         0xFFFFFF54UL
#define CMD_ANIMXY           0xFFFFFF55UL
#define CMD_ANIMDRAW         0xFFFFFF56UL
#define CMD_GRADIENTA        0xFFFFFF57UL
#define CMD_FILLWIDTH        0xFFFFFF58UL
#define CMD_APPENDF          0xFFFFFF59UL
#define CMD_ANIMFRAME        0xFFFFFF5AUL
#define CMD_VIDEOSTARTF      0xFFFFFF5FUL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_VIDEOSTARTF) */

/* Registers for BT815 / BT816 */
#define REG_ADAPTIVE_FRAMERATE 0x0030257cUL
#define REG_PLAYBACK_PAUSE     0x003025ecUL
#define REG_FLASH_STATUS       0x003025f0UL
#define REG_FLASH_SIZE         0x00309024UL
#define REG_PLAY_CONTROL       0x0030914eUL
#define REG_COPRO_PATCH_PTR    0x00309162UL

/* Macros for BT815 / BT816 */
#define BITMAP_EXT_FORMAT(format) ((DL_BITMAP_EXT_FORMAT) | ((format) & 0xFFFFUL))
#define BITMAP_SWIZZLE(r,g,b,a) ((DL_BITMAP_SWIZZLE) | (((r) & 7UL) << 9U) | (((g) & 7UL) << 6U) | (((b) & 7UL) << 3U) | ((a) & 7UL))

#define BITMAP_TRANSFORM_A_EXT(p,v) ((DL_BITMAP_TRANSFORM_A) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_B_EXT(p,v) ((DL_BITMAP_TRANSFORM_B) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_D_EXT(p,v) ((DL_BITMAP_TRANSFORM_D) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
#define BITMAP_TRANSFORM_E_EXT(p,v) ((DL_BITMAP_TRANSFORM_E) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))

#define BITMAP_TRANSFORM_A(a) BITMAP_TRANSFORM_A_EXT(0UL,(a))
#define BITMAP_TRANSFORM_B(b) BITMAP_TRANSFORM_B_EXT(0UL,(b))
#define BITMAP_TRANSFORM_D(d) BITMAP_TRANSFORM_D_EXT(0UL,(d))
#define BITMAP_TRANSFORM_E(e) BITMAP_TRANSFORM_E_EXT(0UL,(e))

#endif  /* EVE_GEN > 2 */

/* ########## EVE Generation 4: BT817 / BT818 definitions ########## */

#if EVE_GEN > 3

/* Commands for BT817 / BT818 */
#define CMD_ANIMFRAMERAM   0xFFFFFF6DUL
#define CMD_ANIMSTARTRAM   0xFFFFFF6EUL
#define CMD_APILEVEL       0xFFFFFF63UL
#define CMD_CALIBRATESUB   0xFFFFFF60UL
#define CMD_CALLLIST       0xFFFFFF67UL
#define CMD_ENDLIST        0xFFFFFF69UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_ENDLIST) */
#define CMD_FLASHPROGRAM   0xFFFFFF70UL
#define CMD_FONTCACHE      0xFFFFFF6BUL
#define CMD_FONTCACHEQUERY 0xFFFFFF6CUL
#define CMD_GETIMAGE       0xFFFFFF64UL
#define CMD_HSF            0xFFFFFF62UL
#define CMD_LINETIME       0xFFFFFF5EUL
#define CMD_NEWLIST        0xFFFFFF68UL
#define CMD_PCLKFREQ       0xFFFFFF6AUL
#define CMD_RETURN         0xFFFFFF66UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_RETURN) */
#define CMD_RUNANIM        0xFFFFFF6FUL
#define CMD_TESTCARD       0xFFFFFF61UL /* does not need a dedicated function, just use EVE_cmd_dl(CMD_TESTCARD) */
#define CMD_WAIT           0xFFFFFF65UL

/* Registers for BT817 / BT818 */
#define REG_UNDERRUN      0x0030260cUL
#define REG_AH_HCYCLE_MAX 0x00302610UL
#define REG_PCLK_FREQ     0x00302614UL
#define REG_PCLK_2X       0x00302618UL
#define REG_ANIM_ACTIVE   0x0030902CUL

#endif /*  EVE_GEN > 3 */

#ifdef __cplusplus
}
#endif

#endif /* EVE_H */
