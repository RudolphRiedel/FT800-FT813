/*
@file    EVE_commands.h
@brief   contains FT8xx / BT8xx function prototypes
@version 4.0
@date    2019-08-31
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2019 Rudolph Riedel

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

*/

#ifndef EVE_COMMANDS_H_
#define EVE_COMMANDS_H_


void EVE_cmdWrite(uint8_t command, uint8_t parameter);

uint8_t EVE_memRead8(uint32_t ftAddress);
uint16_t EVE_memRead16(uint32_t ftAddress);
uint32_t EVE_memRead32(uint32_t ftAddress);
void EVE_memWrite8(uint32_t ftAddress, uint8_t ftData8);
void EVE_memWrite16(uint32_t ftAddress, uint16_t ftData16);
void EVE_memWrite32(uint32_t ftAddress, uint32_t ftData32);
void EVE_memWrite_flash_buffer(uint32_t ftAddress, const uint8_t *data, uint16_t len);
uint8_t EVE_busy(void);
void EVE_get_cmdoffset(void);
uint16_t EVE_report_cmdoffset(void);
uint32_t EVE_get_touch_tag(uint8_t num);


/* commands to operate on memory: */
void EVE_cmd_memzero(uint32_t ptr, uint32_t num);
void EVE_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num);
/*(void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *data); */
void EVE_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num);


/* commands for loading image data into FT8xx memory: */
void EVE_cmd_inflate(uint32_t ptr, const uint8_t *data, uint16_t len);
void EVE_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len);

#if defined (FT81X_ENABLE)
void EVE_cmd_mediafifo(uint32_t ptr, uint32_t size);
#endif


void EVE_cmd_start(void);
void EVE_cmd_execute(void);

void EVE_start_cmd_burst(void);
void EVE_end_cmd_burst(void);

void EVE_cmd_dl(uint32_t command);


/* EVE3 commands */
#if defined (BT81X_ENABLE)

void EVE_cmd_flashwrite(uint32_t ptr, uint32_t num, const uint8_t *data);
void EVE_cmd_flashread(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashupdate(uint32_t dest, uint32_t src, uint32_t num);
uint32_t EVE_cmd_flashfast(void);
void EVE_cmd_flashspitx(uint32_t num, const uint8_t *data);
void EVE_cmd_flashspirx(uint32_t dest, uint32_t num);
void EVE_cmd_flashsource(uint32_t ptr);

void EVE_cmd_inflate2(uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len);
void EVE_cmd_rotatearound(int32_t x0, int32_t y0, int32_t angle, int32_t scale);
void EVE_cmd_animstart(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstop(int32_t ch);
void EVE_cmd_animxy(int32_t ch, int16_t x0, int16_t y0);
void EVE_cmd_animdraw(int32_t ch);
void EVE_cmd_animframe(int16_t x0, int16_t y0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_gradienta(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void EVE_cmd_fillwidth(uint32_t s);
void EVE_cmd_appendf(uint32_t ptr, uint32_t num);

uint8_t EVE_init_flash(void);
#endif


/* commands to draw graphics objects: */

#if defined (BT81X_ENABLE)
void EVE_cmd_text_var(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text, uint8_t numargs, ...);
void EVE_cmd_button_var(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text, uint8_t num_args, ...);
void EVE_cmd_toggle_var(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text, uint8_t num_args, ...);
#endif

void EVE_cmd_text(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_button(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_clock(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs);
void EVE_cmd_bgcolor(uint32_t color);
void EVE_cmd_fgcolor(uint32_t color);
void EVE_cmd_gradcolor(uint32_t color);
void EVE_cmd_gauge(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void EVE_cmd_keys(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void EVE_cmd_progress(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_scrollbar(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_slider(int16_t x1, int16_t y1, int16_t w1, int16_t h1, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_dial(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val);
void EVE_cmd_toggle(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text);
void EVE_cmd_number(int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number);

#if defined (FT81X_ENABLE)
void EVE_cmd_setbase(uint32_t base);
void EVE_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
#endif


void EVE_cmd_append(uint32_t ptr, uint32_t num);


/* commands for setting the bitmap transform matrix: */
void EVE_cmd_translate(int32_t tx, int32_t ty);
void EVE_cmd_scale(int32_t sx, int32_t sy);
void EVE_cmd_rotate(int32_t ang);


/* other commands: */
void EVE_cmd_calibrate(void);
void EVE_cmd_interrupt(uint32_t ms);
void EVE_cmd_setfont(uint32_t font, uint32_t ptr);
#if defined (FT81X_ENABLE)
void EVE_cmd_romfont(uint32_t font, uint32_t romslot);
void EVE_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setrotate(uint32_t r);
void EVE_cmd_setscratch(uint32_t handle);
#endif
void EVE_cmd_sketch(int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format);
void EVE_cmd_snapshot(uint32_t ptr);
#if defined (FT81X_ENABLE)
void EVE_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t x0, int16_t y0, int16_t w0, int16_t h0);
#endif
void EVE_cmd_spinner(int16_t x0, int16_t y0, uint16_t style, uint16_t scale);
void EVE_cmd_track(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t tag);


/* commands that return values by writing to the command-fifo */
uint32_t EVE_cmd_memcrc(uint32_t ptr, uint32_t num);
uint32_t EVE_cmd_getptr(void);
uint32_t EVE_cmd_regread(uint32_t ptr);

struct EVE_struct_getprops
{
	uint32_t ptr;
	uint32_t width;
	uint32_t height;
};

struct EVE_struct_getprops EVE_cmd_getprops(void);

struct EVE_struct_getmatrix
{
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
	uint32_t e;
	uint32_t f;
};

extern struct EVE_struct_getmatrix EVE_cmd_getmatrix(void);


/* meta-commands, sequences of several display-list entries condensed into simpler to use functions at the price of some overhead */
void EVE_cmd_point(int16_t x0, int16_t y0, uint16_t size);
void EVE_cmd_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t w0);
void EVE_cmd_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t corner);


/* startup FT8xx: */
uint8_t EVE_init(void);

#endif /* EVE_COMMANDS_H_ */
