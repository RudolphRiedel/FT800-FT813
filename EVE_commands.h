/*
@file    EVE_commands.h
@brief   contains FT8xx / BT8xx function prototypes
@version 5.0
@date    2020-09-05
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2020 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

2.2
- changes to this header

2.3

- changed type of result-value for EVE_init from void to uint8_t
- added prototype for EVE_cmd_romfont()
- added millis option to EVE_cmd_clock()
- switched to standard-C compliant comment-style

2.4
- added prototype for EVE_memWrite_flash_buffer()

3.0
- renamed from FT800_commands.h to EVE_commands.h
- changed FT_ prefixes to EVE_
- changed ft800_ prefixes to EVE_
- removed test-function EVE_cmd_loadimage_mf()

3.1
- added prototypes for EVE_cmd_setfont() and EVE_cmd_setfont2()

3.2
- removed several prototypes for commands that do not need a function of their own:
	EVE_cmd_stop(), EVE_cmd_loadidentity(), EVE_cmd_setmatrix(), EVE_cmd_screensaver(),
	EVE_cmd_logo(), EVE_cmd_coldstart()
	These all do not have any arguments and can be used with EVE_cmd_dl(), for example:
	EVE_cmd_dl(CMD_SCREENSAVER);
	EVE_cmd_dl(CMD_SETMATRIX);
- added prototype for EVE_cmd_snapshot2()
- added prototype for EVE_cmd_setscratch()

3.3
- added prototypes for EVE_cmd_memcrc(), EVE_cmd_getptr(), EVE_cmd_regread() and EVE_cmd_getprops()

3.4
- added protoypes for EVE_start_cmd_burst() and EVE_end_cmd_burst()

3.5
- added prototype for EVE_cmd_start()

3.6
- added prototype for EVE_report_cmdoffset()
- removed exporting var cmdOffset

3.7
- sorted functions
- changed #ifdef to #if defined for consistency

4.0
- changed FT8_ prefixes to EVE_
- added EVE_cmd_flashsource()
- added prototype for EVE_init_flash() - not functional yet
- added protoypes for EVE_cmd_flashwrite(), EVE_cmd_flashread(), EVE_cmd_flashupdate(), EVE_cmd_flashfast(), EVE_cmd_flashspitx() and EVE_cmd_flashspirx()
- added prototypes for EVE_cmd_inflate2(), EVE_cmd_rotatearound(), EVE_cmd_animstart(), EVE_cmd_animstop(), EVE_cmd_animxy(),
	EVE_cmd_animdraw(), EVE_cmd_animframe(), EVE_cmd_gradienta(), EVE_cmd_fillwidth() and EVE_cmd_appendf()
- added a paramter to EVE_get_touch_tag() to allow multi-touch
- expanded EVE_cmdWrite() from command only to command+parameter
- changed the prototype for EVE_cmd_getptr(), it returns the memory-address directly now
- changed the prototype for EVE_cmd_memcrc(), it returns the crc32 directly now
- changed the prototype for EVE_cmd_regread(), it returns the 32 bit value directly now
- changed cmd_getprops() and cmd_getmatrix(), these return structures now
- added EVE_cmd_text_var() after struggeling with varargs, this function adds a single paramter for string conversion if EVE_OPT_FORMAT is given
- changed EVE_cmd_text_var() to a varargs function with the number of arguments as additional argument
- added EVE_cmd_button_var() and EVE_cmd_toggle_var() functions
- added prototype for EVE_calibrate_manual()
- added prototypes EVE_cmd_flasherase(), EVE_cmd_flashattach(), EVE_cmd_flashdetach() and EVE_cmd_flashspidesel()
- added an include for "EVE.h" in order to reduce the necessary includes in the main project file, only including "EVE_commands.h" is fine now
- changed EVE_cmd_getprops() again, inspired by BRTs AN_025, changed the name to EVE_LIB_GetProps() and got rid of the returning data-structure
- replaced EVE_cmd_getmatrix() with an earlier implementation again, looks like it is supposed to write, not read
- added function EVE_color_rgb()
- marked EVE_get_touch_tag() as deprecated
- changed the "len" parameter for loadimage, inflate, inflate2 and EVE_memWrite_flash_buffer() to uint32_t

5.0
- replaced BT81X_ENABLE with "EVE_GEN > 2"
- removed FT81X_ENABLE as FT81x already is the lowest supported chip revision now
- removed the formerly as deprected marked EVE_get_touch_tag()
- changed EVE_color_rgb() to use a 32 bit value like the rest of the color commands
- removed the meta-commands EVE_cmd_point(), EVE_cmd_line() and EVE_cmd_rect()
- removed obsolete functions EVE_get_cmdoffset(void) and EVE_report_cmdoffset(void) - cmdoffset is gone
- renamed EVE_LIB_GetProps() back to EVE_cmd_getprops() since it does not do anything special to justify a special name
- added prototype for helper function EVE_memWrite_sram_buffer()
- added prototypes for EVE_cmd_bitmap_transform() and EVE_cmd_bitmap_transform_burst()
- added prototype for EVE_cmd_playvideo()
- added prototypes for EVE_cmd_setfont_burst() and EVE_cmd_setfont2_burst()
- added prototype for EVE_cmd_videoframe()
- restructured: functions are sorted by chip-generation and within their group in alphabetical order
- reimplementedEVE_cmd_getmatrix() again, it needs to read values, not write them
- removed everything BT817 / BT818 related for an earlier release


*/

#pragma once

#ifndef EVE_COMMANDS_H_
#define EVE_COMMANDS_H_

#include "EVE.h"

/*----------------------------------------------------------------------------------------------------------------------------*/
/*---- helper functions ------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------*/

void EVE_cmdWrite(uint8_t command, uint8_t parameter);

uint8_t EVE_memRead8(uint32_t ftAddress);
uint16_t EVE_memRead16(uint32_t ftAddress);
uint32_t EVE_memRead32(uint32_t ftAddress);
void EVE_memWrite8(uint32_t ftAddress, uint8_t ftData8);
void EVE_memWrite16(uint32_t ftAddress, uint16_t ftData16);
void EVE_memWrite32(uint32_t ftAddress, uint32_t ftData32);
void EVE_memWrite_flash_buffer(uint32_t ftAddress, const uint8_t *data, uint32_t len);
void EVE_memWrite_sram_buffer(uint32_t ftAddress, const uint8_t *data, uint32_t len);
uint8_t EVE_busy(void);
void EVE_cmd_start(void);
void EVE_cmd_execute(void);


/*----------------------------------------------------------------------------------------------------------------------------*/
/*---- commands and functions that to be used outside of display-lists -------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------*/


/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3



#endif /* EVE_GEN > 3 */


/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_clearcache(void);
void EVE_cmd_flashattach(void);
void EVE_cmd_flashdetach(void);
void EVE_cmd_flasherase(void);
uint32_t EVE_cmd_flashfast(void);
void EVE_cmd_flashspidesel(void);
void EVE_cmd_flashread(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashsource(uint32_t ptr);
void EVE_cmd_flashspirx(uint32_t dest, uint32_t num);
void EVE_cmd_flashspitx(uint32_t num, const uint8_t *data);
void EVE_cmd_flashupdate(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashwrite(uint32_t ptr, uint32_t num, const uint8_t *data);
void EVE_cmd_inflate2(uint32_t ptr, uint32_t options, const uint8_t *data, uint32_t len);

#endif /* EVE_GEN > 2 */


void EVE_cmd_getprops(uint32_t *pointer, uint32_t *width, uint32_t *height);
uint32_t EVE_cmd_getptr(void);
void EVE_cmd_inflate(uint32_t ptr, const uint8_t *data, uint32_t len);
void EVE_cmd_interrupt(uint32_t ms);
void EVE_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *data, uint32_t len);
void EVE_cmd_mediafifo(uint32_t ptr, uint32_t size);
void EVE_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num);
uint32_t EVE_cmd_memcrc(uint32_t ptr, uint32_t num);
void EVE_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num);
/*(void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *data); */
void EVE_cmd_memzero(uint32_t ptr, uint32_t num);
void EVE_cmd_playvideo(uint32_t options, const uint8_t *data, uint32_t len);
uint32_t EVE_cmd_regread(uint32_t ptr);
void EVE_cmd_setrotate(uint32_t r);
void EVE_cmd_snapshot(uint32_t ptr);
void EVE_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t x0, int16_t y0, int16_t w0, int16_t h0);
void EVE_cmd_track(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t tag);
void EVE_cmd_videoframe(uint32_t dest, uint32_t result_ptr);


/*----------------------------------------------------------------------------------------------------------------------------*/
/*------------- patching and initialisation ----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------*/

#if EVE_GEN > 2

uint8_t EVE_init_flash(void);

#endif /* EVE_GEN > 2 */

uint8_t EVE_init(void);


/*----------------------------------------------------------------------------------------------------------------------------*/
/*-------- functions for display lists ---------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------------*/

void EVE_start_cmd_burst(void);
void EVE_end_cmd_burst(void);




/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3



#endif /* EVE_GEN > 3 */


/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_animdraw(int32_t ch);
void EVE_cmd_animdraw_burst(int32_t ch);
void EVE_cmd_animframe(int16_t x0, int16_t y0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animframe_burst(int16_t x0, int16_t y0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animstart(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstart_burst(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstop(int32_t ch);
void EVE_cmd_animstop_burst(int32_t ch);
void EVE_cmd_animxy(int32_t ch, int16_t x0, int16_t y0);
void EVE_cmd_animxy_burst(int32_t ch, int16_t x0, int16_t y0);
void EVE_cmd_appendf(uint32_t ptr, uint32_t num);
void EVE_cmd_appendf_burst(uint32_t ptr, uint32_t num);
uint16_t EVE_cmd_bitmap_transform( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2);
void EVE_cmd_bitmap_transform_burst( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2);
void EVE_cmd_fillwidth(uint32_t s);
void EVE_cmd_fillwidth_burst(uint32_t s);
void EVE_cmd_gradienta(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void EVE_cmd_gradienta_burst(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void EVE_cmd_rotatearound(int32_t x0, int32_t y0, int32_t angle, int32_t scale);
void EVE_cmd_rotatearound_burst(int32_t x0, int32_t y0, int32_t angle, int32_t scale);

void EVE_cmd_button_var(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text, uint8_t num_args, ...);
void EVE_cmd_button_var_burst(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text, uint8_t num_args, ...);
void EVE_cmd_text_var(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text, uint8_t numargs, ...);
void EVE_cmd_text_var_burst(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text, uint8_t numargs, ...);
void EVE_cmd_toggle_var(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text, uint8_t num_args, ...);
void EVE_cmd_toggle_var_burst(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text, uint8_t num_args, ...);

#endif /* EVE_GEN > 2 */


void EVE_cmd_dl(uint32_t command);
void EVE_cmd_dl_burst(uint32_t command);

void EVE_cmd_append(uint32_t ptr, uint32_t num);
void EVE_cmd_append_burst(uint32_t ptr, uint32_t num);
void EVE_cmd_bgcolor(uint32_t color);
void EVE_cmd_bgcolor_burst(uint32_t color);
void EVE_cmd_button(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_button_burst(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_calibrate(void);
void EVE_cmd_clock(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs);
void EVE_cmd_clock_burst(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs);
void EVE_cmd_dial(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val);
void EVE_cmd_dial_burst(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val);
void EVE_cmd_fgcolor(uint32_t color);
void EVE_cmd_fgcolor_burst(uint32_t color);
void EVE_cmd_gauge(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_gauge_burst(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_getmatrix(int32_t *get_a, int32_t *get_b, int32_t *get_c, int32_t *get_d, int32_t *get_e, int32_t *get_f);
void EVE_cmd_gradcolor(uint32_t color);
void EVE_cmd_gradcolor_burst(uint32_t color);
void EVE_cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void EVE_cmd_gradient_burst(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void EVE_cmd_keys(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_keys_burst(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_number(int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number);
void EVE_cmd_number_burst(int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number);
void EVE_cmd_progress(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_progress_burst(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_romfont(uint32_t font, uint32_t romslot);
void EVE_cmd_romfont_burst(uint32_t font, uint32_t romslot);
void EVE_cmd_rotate(int32_t angle);
void EVE_cmd_rotate_burst(int32_t angle);
void EVE_cmd_scale(int32_t sx, int32_t sy);
void EVE_cmd_scale_burst(int32_t sx, int32_t sy);
void EVE_cmd_scrollbar(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_scrollbar_burst(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_setbase(uint32_t base);
void EVE_cmd_setbase_burst(uint32_t base);
void EVE_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void EVE_cmd_setbitmap_burst(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void EVE_cmd_setfont(uint32_t font, uint32_t ptr);
void EVE_cmd_setfont_burst(uint32_t font, uint32_t ptr);
void EVE_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setfont2_burst(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setscratch(uint32_t handle);
void EVE_cmd_setscratch_burst(uint32_t handle);
void EVE_cmd_sketch(int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format);
void EVE_cmd_sketch_burst(int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format);
void EVE_cmd_slider(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_slider_burst(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_spinner(int16_t x0, int16_t y0, uint16_t style, uint16_t scale);
void EVE_cmd_spinner_burst(int16_t x0, int16_t y0, uint16_t style, uint16_t scale);
void EVE_cmd_text(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_text_burst(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_toggle(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text);
void EVE_cmd_toggle_burst(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text);
void EVE_cmd_translate(int32_t tx, int32_t ty);
void EVE_cmd_translate_burst(int32_t tx, int32_t ty);

void EVE_color_rgb(uint32_t color);
void EVE_color_rgb_burst(uint32_t color);


/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------- special purpose functions ------------------- --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

void EVE_calibrate_manual(uint16_t height);


#endif /* EVE_COMMANDS_H_ */
