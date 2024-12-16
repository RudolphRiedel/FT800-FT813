/*
@file    EVE_commands.h
@brief   contains FT8xx / BT8xx function prototypes
@version 5.0
@date    2024-12-16
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2024 Rudolph Riedel

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
- added prototype for EVE_cmd_plkfreq()
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
- added prototypes for EVE_cmd_fontcache() and EVE_cmd_fontcachequery()
- added prototype for EVE_cmd_flashprogram()
- added prototype for EVE_cmd_calibratesub()
- added prototypes for EVE_cmd_animframeram(), EVE_cmd_animframeram_burst(), EVE_cmd_animstartram(),
EVE_cmd_animstartram_burst()
- added prototypes for EVE_cmd_apilevel(), EVE_cmd_apilevel_burst()
- added prototypes for EVE_cmd_calllist(), EVE_cmd_calllist_burst()
- added prototype for EVE_cmd_getimage()
- added prototypes for EVE_cmd_hsf(), EVE_cmd_hsf_burst()
- added prototype for EVE_cmd_linetime()
- added prototypes for EVE_cmd_newlist(), EVE_cmd_newlist_burst()
- added prototypes for EVE_cmd_runanim(), EVE_cmd_runanim_burst()
- added prototype for EVE_cmd_wait()
- removed the history from before 4.0
- added an enum with return codes to have the functions return something more meaningfull
- finally removed EVE_cmd_start() after setting it to deprecatd with the first 5.0 release
- renamed EVE_cmd_execute() to EVE_execute_cmd() to be more consistent, this is is not an EVE command
- added the return-value of EVE_FIFO_HALF_EMPTY to EVE_busy() to indicate there is more than 2048 bytes available
- removed the 4.0 history
- added parameter width to EVE_calibrate_manual()
- changed the varargs versions of cmd_button, cmd_text and cmd_toggle to use an array of uint32_t values to comply with MISRA-C
- fixed some MISRA-C issues
- basic maintenance: checked for violations of white space and indent rules
- more linter fixes for minor issues like variables shorter than 3 characters
- added EVE_color_a() / EVE_color_a_burst()
- removed EVE_cmd_newlist_burst() prototype as the function got removed earlier
- added prototype for EVE_write_display_parameters()
- added EVE_memRead_sram_buffer()
- added EVE_FAULT_RECOVERED to the list of return codes
- added defines for the state of the external flash
- added protype for EVE_get_and_reset_fault_state()
- put E_OK and E_NOT_OK in #ifndef/#endif guards as these are usually defined
  already in AUTOSAR projects
- renamed EVE_FAIL_CHIPID_TIMEOUT to EVE_FAIL_REGID_TIMEOUT as suggested by #93 on github
- changed a number of function parameters from signed to unsigned following the
    updated BT81x series programming guide V2.4
- commented out EVE_cmd_regread() prototype
- removed prototype for EVE_cmd_hsf_burst()
- added static inline functions: i16_i16_to_u32(), u16_u16_to_u32() and i32_to_u32()
- added EVE_cmd_dlstart() / EVE_cmd_dlstart_burst()
- added EVE_clear_color_rgb() / EVE_clear_color_rgb_burst()
- added EVE_clear() / EVE_clear_burst()
- moved EVE_calibrate_manual() to EVE_supplemental
- added EVE_tag() / EVE_tag_burst()
- added EVE_save_context() / EVE_save_context_burst()
- added EVE_cmd_loadidentity() / EVE_cmd_loadidentity_burst()
- added EVE_cmd_setmatrix() / EVE_cmd_setmatrix_burst()
- added EVE_begin() / EVE_begin_burst()
- added EVE_end() / EVE_end_burst()
- added EVE_cmd_swap() / EVE_cmd_swap_burst()
- added EVE_restore_context() / EVE_restore_context_burst()
- added EVE_display() / EVE_display_burst()
- added EVE_vertex2f() / EVE_vertex2f_burst(), EVE_vertex2ii() / EVE_vertex2ii_burst()
- added EVE_vertex_format() / EVE_vertex_format_burst()
- added EVE_line_width() / EVE_line_width_burst()
- added EVE_point_size() / EVE_point_size_burst()
- added EVE_cmd_stop() / EVE_cmd_stop_burst()
- added EVE_macro() / EVE_macro_burst()
- added EVE_cmd_screensaver() / EVE_cmd_screensaver_burst()
- added EVE_cmd_logo(), EVE_cmd_coldstart(), EVE_cmd_videostart(), EVE_cmd_videostartf()
- added EVE_cmd_sync() / EVE_cmd_sync_burst(), EVE_cmd_resetfonts()
- reworked EVE_cmd_memcpy(), added EVE_cmd_memcpy_burst()
- added EVE_cmd_testcard(), EVE_cmd_endlist(), EVE_cmd_return(), EVE_cmd_return_burst()
- commented out EVE_cmd_linetime()
- added EVE_bitmap_ext_format() / EVE_bitmap_ext_format_burst()
- added EVE_bitmap_swizzle(), EVE_bitmap_swizzle_burst()
- added EVE_alpha_func(), EVE_alpha_func_burst()
- dropped CMD_SYNC a tier from EVE3 to EVE2
- added EVE_bitmap_handle() / EVE_bitmap_handle_burst()
- added EVE_bitmap_layout() / EVE_bitmap_layout_burst()
- added EVE_bitmap_layout_h() / EVE_bitmap_layout_h_burst()
- added EVE_bitmap_size() / EVE_bitmap_size_burst()
- added EVE_bitmap_size_h() / EVE_bitmap_size_h_burst()
- added EVE_bitmap_source() / EVE_bitmap_source_burst()
- added EVE_blend_func() / EVE_blend_func_burst(), EVE_call(), EVE_call_burst()
- added EVE_cell() / EVE_cell_burst(), EVE_clear_color_a() / EVE_clear_color_a_burst()
- added EVE_clear_stencil() / EVE_clear_stencil_burst()
- added EVE_clear_tag() / EVE_clear_tag_burst()
- added EVE_color_mask() / EVE_color_mask_burst()
- added EVE_jump() / EVE_jump_burst(), EVE_nop() / EVE_nop_burst()
- added EVE_palette_source() / EVE_palette_source_burst()
- added EVE_return() / EVE_return_burst()
- added EVE_scissor_size() / EVE_scissor_size_burst()
- added EVE_scissor_xy() / EVE_scissor_xy_burst()
- added EVE_stencil_func() / EVE_stencil_func_burst()
- added EVE_stencil_mask() / EVE_stencil_mask_burst()
- added EVE_stencil_op() / EVE_stencil_op_burst()
- added EVE_tag_mask() / EVE_tag_mask_burst()
- added EVE_vertex_translate_x() / EVE_vertex_translate_x_burst()
- added EVE_vertex_translate_y() / EVE_vertex_translate_y_burst()
- added "const" statements for BARR-C:2018 / CERT C compliance

*/

#ifndef EVE_COMMANDS_H
#define EVE_COMMANDS_H

#include "EVE.h"

#if !defined E_OK
#define E_OK 0U
#endif

#if !defined E_NOT_OK
#define E_NOT_OK 1U
#endif

#define EVE_FAIL_REGID_TIMEOUT 2U
#define EVE_FAIL_RESET_TIMEOUT 3U
#define EVE_FAIL_PCLK_FREQ 4U
#define EVE_FAIL_FLASH_STATUS_INIT 5U
#define EVE_FAIL_FLASH_STATUS_DETACHED 6U
#define EVE_FAIL_FLASHFAST_NOT_SUPPORTED 7U
#define EVE_FAIL_FLASHFAST_NO_HEADER_DETECTED 8U
#define EVE_FAIL_FLASHFAST_SECTOR0_FAILED 9U
#define EVE_FAIL_FLASHFAST_BLOB_MISMATCH 10U
#define EVE_FAIL_FLASHFAST_SPEED_TEST 11U
#define EVE_IS_BUSY 12U
#define EVE_FIFO_HALF_EMPTY 13U
#define EVE_FAULT_RECOVERED 14U

#define EVE_FLASH_STATUS_INIT 0U
#define EVE_FLASH_STATUS_DETACHED 1U
#define EVE_FLASH_STATUS_BASIC 2U
#define EVE_FLASH_STATUS_FULL 3U

/* ##################################################################
    functions that convert to uint32
##################################################################### */

static inline uint32_t i16_i16_to_u32(const int16_t arg1, const int16_t arg2)
{
    return (((uint32_t) ((uint16_t) arg1)) + (((uint32_t) ((uint16_t) arg2)) << 16U));
}

static inline uint32_t u16_u16_to_u32(const uint16_t arg1, const uint16_t arg2)
{
    return (((uint32_t) arg1) + (((uint32_t) arg2) << 16U));
}

static inline uint32_t i32_to_u32(const int32_t arg1)
{
    return ((uint32_t) arg1);
}

/* ##################################################################
    helper functions
##################################################################### */

void EVE_cmdWrite(uint8_t const command, uint8_t const parameter);

uint8_t EVE_memRead8(uint32_t const ft_address);
uint16_t EVE_memRead16(uint32_t const ft_address);
uint32_t EVE_memRead32(uint32_t const ft_address);
void EVE_memWrite8(uint32_t const ft_address, uint8_t const ft_data);
void EVE_memWrite16(uint32_t const ft_address, uint16_t const ft_data);
void EVE_memWrite32(uint32_t const ft_address, uint32_t const ft_data);
void EVE_memWrite_flash_buffer(uint32_t const ft_address, const uint8_t * const p_data, uint32_t const len);
void EVE_memWrite_sram_buffer(uint32_t const ft_address, const uint8_t * const p_data, uint32_t const len);
void EVE_memRead_sram_buffer(uint32_t const ft_address, uint8_t * const p_data, uint32_t const len);
uint8_t EVE_busy(void);
uint8_t EVE_get_and_reset_fault_state(void);
void EVE_execute_cmd(void);

/* ##################################################################
    commands and functions to be used outside of display-lists
##################################################################### */

/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3

void EVE_cmd_endlist(void);
void EVE_cmd_flashprogram(const uint32_t dest, const uint32_t src, const uint32_t num);
void EVE_cmd_fontcache(const uint32_t font, const uint32_t ptr, const uint32_t num);
void EVE_cmd_fontcachequery(uint32_t * const p_total, uint32_t * const p_used);
void EVE_cmd_getimage(uint32_t * const p_source, uint32_t * const p_fmt, uint32_t * const p_width, uint32_t * const p_height, uint32_t * const p_palette);
/*void EVE_cmd_linetime(uint32_t dest);*/
void EVE_cmd_newlist(const uint32_t adr);
uint32_t EVE_cmd_pclkfreq(const uint32_t ftarget, const int32_t rounding);
void EVE_cmd_testcard(void);
void EVE_cmd_wait(const uint32_t usec);

#endif /* EVE_GEN > 3 */

/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_clearcache(void);
void EVE_cmd_flashattach(void);
void EVE_cmd_flashdetach(void);
void EVE_cmd_flasherase(void);
uint32_t EVE_cmd_flashfast(void);
void EVE_cmd_flashspidesel(void);
void EVE_cmd_flashread(const uint32_t dest, const uint32_t src, const uint32_t num);
void EVE_cmd_flashsource(const uint32_t ptr);
void EVE_cmd_flashspirx(const uint32_t dest, const uint32_t num);
void EVE_cmd_flashspitx(const uint32_t num, const uint8_t * const p_data);
void EVE_cmd_flashupdate(const uint32_t dest, const uint32_t src, const uint32_t num);
void EVE_cmd_flashwrite(const uint32_t ptr, const uint32_t num, const uint8_t * const p_data);
void EVE_cmd_inflate2(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_resetfonts(void);
void EVE_cmd_videostartf(void);

#endif /* EVE_GEN > 2 */

void EVE_cmd_coldstart(void);
void EVE_cmd_getprops(uint32_t * const p_pointer, uint32_t * const p_width, uint32_t * const p_height);
uint32_t EVE_cmd_getptr(void);
void EVE_cmd_inflate(const uint32_t ptr, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_interrupt(const uint32_t msec);
void EVE_cmd_loadimage(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_logo(void);
void EVE_cmd_mediafifo(const uint32_t ptr, const uint32_t size);
void EVE_cmd_memcpy(const uint32_t dest, const uint32_t src, const uint32_t num);
void EVE_cmd_memcpy_burst(const uint32_t dest, const uint32_t src, const uint32_t num);
uint32_t EVE_cmd_memcrc(const uint32_t ptr, const uint32_t num);
void EVE_cmd_memset(const uint32_t ptr, const uint8_t value, const uint32_t num);
void EVE_cmd_memzero(const uint32_t ptr, const uint32_t num);
void EVE_cmd_playvideo(const uint32_t options, const uint8_t * const p_data, const uint32_t len);
void EVE_cmd_setrotate(const uint32_t rotation);
void EVE_cmd_snapshot(const uint32_t ptr);
void EVE_cmd_snapshot2(const uint32_t fmt, const uint32_t ptr, const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt);
void EVE_cmd_sync(void);
void EVE_cmd_sync_burst(void);
void EVE_cmd_track(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt, const uint16_t tag);
void EVE_cmd_videoframe(const uint32_t dest, const uint32_t result_ptr);
void EVE_cmd_videostart(void);
/*void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *p_data);*/
/*uint32_t EVE_cmd_regread(uint32_t ptr);*/

/* ##################################################################
    patching and initialization
##################################################################### */

#if EVE_GEN > 2
uint8_t EVE_init_flash(void);
#endif /* EVE_GEN > 2 */

void EVE_write_display_parameters(void);
uint8_t EVE_init(void);

/* ##################################################################
    functions for display lists
##################################################################### */

void EVE_start_cmd_burst(void);
void EVE_end_cmd_burst(void);

/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3

void EVE_cmd_animframeram(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame);
void EVE_cmd_animframeram_burst(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame);
void EVE_cmd_animstartram(const int32_t chnl, const uint32_t aoptr, const uint32_t loop);
void EVE_cmd_animstartram_burst(const int32_t chnl, const uint32_t aoptr, const uint32_t loop);
void EVE_cmd_apilevel(const uint32_t level);
void EVE_cmd_apilevel_burst(const uint32_t level);
void EVE_cmd_calibratesub(const uint16_t xc0, const uint16_t yc0, const uint16_t width, const uint16_t height);
void EVE_cmd_calllist(const uint32_t adr);
void EVE_cmd_calllist_burst(const uint32_t adr);
void EVE_cmd_return(void);
void EVE_cmd_return_burst(void);
void EVE_cmd_hsf(const uint32_t hsf);
void EVE_cmd_runanim(const uint32_t waitmask, const uint32_t play);
void EVE_cmd_runanim_burst(const uint32_t waitmask, const uint32_t play);

#endif /* EVE_GEN > 3 */

/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_animdraw(const int32_t chnl);
void EVE_cmd_animdraw_burst(const int32_t chnl);
void EVE_cmd_animframe(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame);
void EVE_cmd_animframe_burst(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame);
void EVE_cmd_animstart(const int32_t chnl, const uint32_t aoptr, const uint32_t loop);
void EVE_cmd_animstart_burst(const int32_t chnl, const uint32_t aoptr, const uint32_t loop);
void EVE_cmd_animstop(const int32_t chnl);
void EVE_cmd_animstop_burst(const int32_t chnl);
void EVE_cmd_animxy(const int32_t chnl, const int16_t xc0, const int16_t yc0);
void EVE_cmd_animxy_burst(const int32_t chnl, const int16_t xc0, const int16_t yc0);
void EVE_cmd_appendf(const uint32_t ptr, const uint32_t num);
void EVE_cmd_appendf_burst(const uint32_t ptr, const uint32_t num);
uint16_t EVE_cmd_bitmap_transform(const int32_t xc0, const int32_t yc0, const int32_t xc1, const int32_t yc1,
                                    const int32_t xc2, const int32_t yc2, const int32_t tx0, const int32_t ty0,
                                    const int32_t tx1, const int32_t ty1, const int32_t tx2, const int32_t ty2);
void EVE_cmd_bitmap_transform_burst(const int32_t xc0, const int32_t yc0, const int32_t xc1, const int32_t yc1,
                                    const int32_t xc2, const int32_t yc2, const int32_t tx0, const int32_t ty0,
                                    const int32_t tx1, const int32_t ty1, const int32_t tx2, const int32_t ty2);
void EVE_cmd_fillwidth(const uint32_t pixel);
void EVE_cmd_fillwidth_burst(const uint32_t pixel);
void EVE_cmd_gradienta(const int16_t xc0, const int16_t yc0, const uint32_t argb0, const int16_t xc1, const int16_t yc1, const uint32_t argb1);
void EVE_cmd_gradienta_burst(const int16_t xc0, const int16_t yc0, const uint32_t argb0, const int16_t xc1, const int16_t yc1, const uint32_t argb1);
void EVE_cmd_rotatearound(const int32_t xc0, const int32_t yc0, const uint32_t angle, const int32_t scale);
void EVE_cmd_rotatearound_burst(const int32_t xc0, const int32_t yc0, const uint32_t angle, const int32_t scale);

void EVE_cmd_button_var(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t font, const uint16_t options, const char * const p_text,
                        const uint8_t num_args, const uint32_t * const p_arguments);
void EVE_cmd_button_var_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                                const uint16_t font, const uint16_t options, const char * const p_text,
                                const uint8_t num_args, const uint32_t * const p_arguments);
void EVE_cmd_text_var(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options,
                        const char * const p_text, const uint8_t num_args, const uint32_t * const p_arguments);
void EVE_cmd_text_var_burst(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options,
                            const char * const p_text, const uint8_t num_args, const uint32_t * const p_arguments);
void EVE_cmd_toggle_var(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                        const uint16_t options, const uint16_t state, const char * const p_text,
                        const uint8_t num_args, const uint32_t * const p_arguments);
void EVE_cmd_toggle_var_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                                const uint16_t options, const uint16_t state, const char * const p_text,
                                const uint8_t num_args, const uint32_t * const p_arguments);

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */

void EVE_bitmap_ext_format(const uint16_t format);
void EVE_bitmap_ext_format_burst(const uint16_t format);
void EVE_bitmap_swizzle(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
void EVE_bitmap_swizzle_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);


#endif /* EVE_GEN > 2 */

/* ##################################################################
    functions for display lists
##################################################################### */

void EVE_cmd_append(const uint32_t ptr, const uint32_t num);
void EVE_cmd_append_burst(const uint32_t ptr, const uint32_t num);
void EVE_cmd_bgcolor(const uint32_t color);
void EVE_cmd_bgcolor_burst(const uint32_t color);
void EVE_cmd_button(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_button_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_calibrate(void);
void EVE_cmd_clock(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                    const uint16_t hours, const uint16_t mins, const uint16_t secs, const uint16_t msecs);
void EVE_cmd_clock_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                            const uint16_t hours, const uint16_t mins, const uint16_t secs, const uint16_t msecs);
void EVE_cmd_dial(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options, const uint16_t val);
void EVE_cmd_dial_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options, const uint16_t val);
void EVE_cmd_dlstart(void);
void EVE_cmd_dlstart_burst(void);
void EVE_cmd_fgcolor(const uint32_t color);
void EVE_cmd_fgcolor_burst(const uint32_t color);
void EVE_cmd_gauge(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                    const uint16_t major, const uint16_t minor, const uint16_t val, const uint16_t range);
void EVE_cmd_gauge_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                            const uint16_t major, const uint16_t minor, const uint16_t val, const uint16_t range);
void EVE_cmd_getmatrix(int32_t * const p_a, int32_t * const p_b, int32_t * const p_c, int32_t * const p_d, int32_t * const p_e, int32_t * const p_f);
void EVE_cmd_gradcolor(const uint32_t color);
void EVE_cmd_gradcolor_burst(const uint32_t color);
void EVE_cmd_gradient(const int16_t xc0, const int16_t yc0, const uint32_t rgb0, const int16_t xc1, const int16_t yc1, const uint32_t rgb1);
void EVE_cmd_gradient_burst(const int16_t xc0, const int16_t yc0, const uint32_t rgb0, const int16_t xc1, const int16_t yc1, const uint32_t rgb1);
void EVE_cmd_keys(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_keys_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_loadidentity(void);
void EVE_cmd_loadidentity_burst(void);
void EVE_cmd_number(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const int32_t number);
void EVE_cmd_number_burst(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const int32_t number);
void EVE_cmd_progress(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t options, const uint16_t val, const uint16_t range);
void EVE_cmd_progress_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint16_t options, const uint16_t val, const uint16_t range);
void EVE_cmd_romfont(const uint32_t font, const uint32_t romslot);
void EVE_cmd_romfont_burst(const uint32_t font, const uint32_t romslot);
void EVE_cmd_rotate(const uint32_t angle);
void EVE_cmd_rotate_burst(const uint32_t angle);
void EVE_cmd_scale(const int32_t scx, const int32_t scy);
void EVE_cmd_scale_burst(const int32_t scx, const int32_t scy);
void EVE_cmd_screensaver(void);
void EVE_cmd_screensaver_burst(void);
void EVE_cmd_scrollbar(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t options, const uint16_t val, const uint16_t size, const uint16_t range);
void EVE_cmd_scrollbar_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                                const uint16_t options, const uint16_t val, const uint16_t size, const uint16_t range);
void EVE_cmd_setbase(const uint32_t base);
void EVE_cmd_setbase_burst(const uint32_t base);
void EVE_cmd_setbitmap(const uint32_t addr, const uint16_t fmt, const uint16_t width, const uint16_t height);
void EVE_cmd_setbitmap_burst(const uint32_t addr, const uint16_t fmt, const uint16_t width, const uint16_t height);
void EVE_cmd_setfont(const uint32_t font, const uint32_t ptr);
void EVE_cmd_setfont_burst(const uint32_t font, const uint32_t ptr);
void EVE_cmd_setfont2(const uint32_t font, const uint32_t ptr, const uint32_t firstchar);
void EVE_cmd_setfont2_burst(const uint32_t font, const uint32_t ptr, const uint32_t firstchar);
void EVE_cmd_setmatrix(void);
void EVE_cmd_setmatrix_burst(void);
void EVE_cmd_setscratch(const uint32_t handle);
void EVE_cmd_setscratch_burst(const uint32_t handle);
void EVE_cmd_sketch(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint32_t ptr, const uint16_t format);
void EVE_cmd_sketch_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint32_t ptr, const uint16_t format);
void EVE_cmd_slider(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint16_t options, const uint16_t val, const uint16_t range);
void EVE_cmd_slider_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint16_t options, const uint16_t val, const uint16_t range);
void EVE_cmd_spinner(const int16_t xc0, const int16_t yc0, const uint16_t style, const uint16_t scale);
void EVE_cmd_spinner_burst(const int16_t xc0, const int16_t yc0, const uint16_t style, const uint16_t scale);
void EVE_cmd_stop(void);
void EVE_cmd_stop_burst(void);
void EVE_cmd_swap(void);
void EVE_cmd_swap_burst(void);
void EVE_cmd_text(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_text_burst(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const char * const p_text);
void EVE_cmd_toggle(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                    const uint16_t options, const uint16_t state, const char * const p_text);
void EVE_cmd_toggle_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                            const uint16_t options, const uint16_t state, const char * const p_text);
void EVE_cmd_translate(const int32_t tr_x, const int32_t tr_y);
void EVE_cmd_translate_burst(const int32_t tr_x, const int32_t tr_y);

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */

void EVE_cmd_dl(const uint32_t command);
void EVE_cmd_dl_burst(const uint32_t command);

void EVE_alpha_func(const uint8_t func, const uint8_t ref);
void EVE_alpha_func_burst(const uint8_t func, const uint8_t ref);
void EVE_begin(const uint32_t prim);
void EVE_begin_burst(const uint32_t prim);
void EVE_bitmap_handle(const uint8_t handle);
void EVE_bitmap_handle_burst(const uint8_t handle);
void EVE_bitmap_layout(const uint8_t format, const uint16_t linestride, const uint16_t height);
void EVE_bitmap_layout_burst(const uint8_t format, const uint16_t linestride, const uint16_t height);
void EVE_bitmap_layout_h(const uint16_t linestride, const uint16_t height);
void EVE_bitmap_layout_h_burst(const uint16_t linestride, const uint16_t height);
void EVE_bitmap_size(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height);
void EVE_bitmap_size_burst(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height);
void EVE_bitmap_size_h(const uint16_t width, const uint16_t height);
void EVE_bitmap_size_h_burst(const uint16_t width, const uint16_t height);
void EVE_bitmap_source(const uint32_t addr);
void EVE_bitmap_source_burst(const uint32_t addr);
void EVE_blend_func(const uint8_t src, const uint8_t dst);
void EVE_blend_func_burst(const uint8_t src, const uint8_t dst);
void EVE_call(const uint16_t dest);
void EVE_call_burst(const uint16_t dest);
void EVE_cell(const uint8_t cell);
void EVE_cell_burst(const uint8_t cell);
void EVE_clear(const uint8_t color, const uint8_t stencil, const uint8_t tag);
void EVE_clear_burst(const uint8_t color, const uint8_t stencil, const uint8_t tag);
void EVE_clear_color_a(const uint8_t alpha);
void EVE_clear_color_a_burst(const uint8_t alpha);
void EVE_clear_color_rgb(const uint32_t color);
void EVE_clear_color_rgb_burst(const uint32_t color);
void EVE_clear_stencil(const uint8_t val);
void EVE_clear_stencil_burst(const uint8_t val);
void EVE_clear_tag(const uint8_t val);
void EVE_clear_tag_burst(const uint8_t val);
void EVE_color_rgb(const uint32_t color);
void EVE_color_rgb_burst(const uint32_t color);
void EVE_color_a(const uint8_t alpha);
void EVE_color_a_burst(const uint8_t alpha);
void EVE_color_mask(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
void EVE_color_mask_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
void EVE_display(void);
void EVE_display_burst(void);
void EVE_end(void);
void EVE_end_burst(void);
void EVE_jump(const uint16_t dest);
void EVE_jump_burst(const uint16_t dest);
void EVE_line_width(const uint16_t width);
void EVE_line_width_burst(const uint16_t width);
void EVE_macro(const uint8_t macro);
void EVE_macro_burst(const uint8_t macro);
void EVE_nop(void);
void EVE_nop_burst(void);
void EVE_palette_source(const uint32_t addr);
void EVE_palette_source_burst(const uint32_t addr);
void EVE_point_size(const uint16_t size);
void EVE_point_size_burst(const uint16_t size);
void EVE_restore_context(void);
void EVE_restore_context_burst(void);
void EVE_return(void);
void EVE_return_burst(void);
void EVE_save_context(void);
void EVE_save_context_burst(void);
void EVE_scissor_size(const uint16_t width, const uint16_t height);
void EVE_scissor_size_burst(const uint16_t width, const uint16_t height);
void EVE_scissor_xy(const uint16_t xc0, const uint16_t yc0);
void EVE_scissor_xy_burst(const uint16_t xc0, const uint16_t yc0);
void EVE_stencil_func(const uint8_t func, const uint8_t ref, const uint8_t mask);
void EVE_stencil_func_burst(const uint8_t func, const uint8_t ref, const uint8_t mask);
void EVE_stencil_mask(const uint8_t mask);
void EVE_stencil_mask_burst(const uint8_t mask);
void EVE_stencil_op(const uint8_t sfail, const uint8_t spass);
void EVE_stencil_op_burst(const uint8_t sfail, const uint8_t spass);
void EVE_tag(const uint8_t tag);
void EVE_tag_burst(const uint8_t tag);
void EVE_tag_mask(const uint8_t mask);
void EVE_tag_mask_burst(const uint8_t mask);
void EVE_vertex2f(const int16_t xc0, const int16_t yc0);
void EVE_vertex2f_burst(const int16_t xc0, const int16_t yc0);
void EVE_vertex2ii(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell);
void EVE_vertex2ii_burst(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell);
void EVE_vertex_format(const uint8_t frac);
void EVE_vertex_format_burst(const uint8_t frac);
void EVE_vertex_translate_x(const int32_t xco);
void EVE_vertex_translate_x_burst(const int32_t xco);
void EVE_vertex_translate_y(const int32_t yco);
void EVE_vertex_translate_y_burst(const int32_t yco);

#endif /* EVE_COMMANDS_H */
