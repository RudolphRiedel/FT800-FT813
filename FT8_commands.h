/*
@file    FT8_commands.h
@brief   Contains FT8xx Function Prototypes
@version 3.6
@date    2018-04-14
@author  Rudolph Riedel

@section History

2.2
- changes to this header

2.3

- changed type of result-value for FT8_init from void to uint8_t
- added prototype for FT8_cmd_romfont()
- added millis option to FT8_cmd_clock()
- switched to standard-C compliant comment-style

2.4
- added prototype for FT8_memWrite_flash_buffer()

3.0
- renamed from FT800_commands.h to FT8_commands.h
- changed FT_ prefixes to FT8_
- changed ft800_ prefixes to FT8_
- removed test-function FT8_cmd_loadimage_mf()

3.1
- added prototypes for FT8_cmd_setfont() and FT8_cmd_setfont2()

3.2
- removed several prototypes for commands that do not need a function of their own:
	FT8_cmd_stop(), FT8_cmd_loadidentity(), FT8_cmd_setmatrix(), FT8_cmd_screensaver(),
	FT8_cmd_logo(), FT8_cmd_coldstart()
	These all do not have any arguments and can be used with FT8_cmd_dl(), for example:
	FT8_cmd_dl(CMD_SCREENSAVER);
	FT8_cmd_dl(CMD_SETMATRIX);
- added prototype for FT8_cmd_snapshot2()
- added prototype for FT8_cmd_setscratch()

3.3
- added prototypes for FT8_cmd_memcrc(), FT8_cmd_getptr(), FT8_cmd_regread() and FT8_cmd_getprops()

3.4
- added protoypes for FT8_start_cmd_burst() and FT8_end_cmd_burst()

3.5
- added prototype for FT8_cmd_start()

3.6
- added prototype for FT8_report_cmdoffset()
- removed exporting var cmdOffset

*/

#ifndef FT8_COMMANDS_H_
#define FT8_COMMANDS_H_


void FT8_cmdWrite(uint8_t data);

uint8_t FT8_memRead8(uint32_t ftAddress);
uint16_t FT8_memRead16(uint32_t ftAddress);
uint32_t FT8_memRead32(uint32_t ftAddress);
void FT8_memWrite8(uint32_t ftAddress, uint8_t ftData8);
void FT8_memWrite16(uint32_t ftAddress, uint16_t ftData16);
void FT8_memWrite32(uint32_t ftAddress, uint32_t ftData32);
void FT8_memWrite_flash_buffer(uint32_t ftAddress, const uint8_t *data, uint16_t len);
uint8_t FT8_busy(void);
void FT8_cmd_dl(uint32_t command);

void FT8_get_cmdoffset(void);
uint16_t FT8_report_cmdoffset(void);
uint32_t FT8_get_touch_tag(void);
void FT8_cmd_start(void);
void FT8_cmd_execute(void);

void FT8_start_cmd_burst(void);
void FT8_end_cmd_burst(void);

/* commands to draw graphics objects: */
void FT8_cmd_text(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text);
void FT8_cmd_button(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void FT8_cmd_clock(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs);
void FT8_cmd_bgcolor(uint32_t color);
void FT8_cmd_fgcolor(uint32_t color);
void FT8_cmd_gradcolor(uint32_t color);
void FT8_cmd_gauge(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void FT8_cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void FT8_cmd_keys(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text);
void FT8_cmd_progress(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range);
void FT8_cmd_scrollbar(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void FT8_cmd_slider(int16_t x1, int16_t y1, int16_t w1, int16_t h1, uint16_t options, uint16_t val, uint16_t range);
void FT8_cmd_dial(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val);
void FT8_cmd_toggle(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text);
void FT8_cmd_number(int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number);

#ifdef FT8_81X_ENABLE
void FT8_cmd_setbase(uint32_t base);
void FT8_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
#endif

/* commands to operate on memory: */
void FT8_cmd_memzero(uint32_t ptr, uint32_t num);
void FT8_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num);
/*(void FT8_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *data); */
void FT8_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num);
void FT8_cmd_append(uint32_t ptr, uint32_t num);


/* commands for loading image data into FT8xx memory: */
void FT8_cmd_inflate(uint32_t ptr, const uint8_t *data, uint16_t len);
void FT8_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len);
#ifdef FT8_81X_ENABLE
void FT8_cmd_mediafifo(uint32_t ptr, uint32_t size);
#endif

/* commands for setting the bitmap transform matrix: */
void FT8_cmd_translate(int32_t tx, int32_t ty);
void FT8_cmd_scale(int32_t sx, int32_t sy);
void FT8_cmd_rotate(int32_t ang);
void FT8_cmd_getmatrix(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f);


/* other commands: */
void FT8_cmd_calibrate(void);
void FT8_cmd_interrupt(uint32_t ms);
void FT8_cmd_setfont(uint32_t font, uint32_t ptr);
#ifdef FT8_81X_ENABLE
void FT8_cmd_romfont(uint32_t font, uint32_t romslot);
void FT8_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void FT8_cmd_setrotate(uint32_t r);
void FT8_cmd_setscratch(uint32_t handle);
#endif
void FT8_cmd_sketch(int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format);
void FT8_cmd_snapshot(uint32_t ptr);
#ifdef FT8_81X_ENABLE
void FT8_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t x0, int16_t y0, int16_t w0, int16_t h0);
#endif
void FT8_cmd_spinner(int16_t x0, int16_t y0, uint16_t style, uint16_t scale);
void FT8_cmd_track(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t tag);


/* commands that return values by writing to the command-fifo */
uint16_t FT8_cmd_memcrc(uint32_t ptr, uint32_t num);
uint16_t FT8_cmd_getptr(void);
uint16_t FT8_cmd_regread(uint32_t ptr);
uint16_t FT8_cmd_getprops(uint32_t ptr);


/* meta-commands, sequences of several display-list entries condensed into simpler to use functions at the price of some overhead */
void FT8_cmd_point(int16_t x0, int16_t y0, uint16_t size);
void FT8_cmd_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t w0);
void FT8_cmd_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t corner);


/* startup FT8xx: */
uint8_t FT8_init(void);

#endif /* FT8_COMMANDS_H_ */