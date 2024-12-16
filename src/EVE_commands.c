/*
@file    EVE_commands.c
@brief   contains FT8xx / BT8xx functions
@version 5.0
@date    2024-12-16
@author  Rudolph Riedel

@section info

At least for Arm Cortex-M0 and Cortex-M4 I have fastest execution with -O2.
The c-standard is C99.


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
- added EVE_cmd_pclkfreq()
- put back writing of REG_CSSPREAD as it needs to be deactivated for higher frequencies
- added the configuration of the second PLL for the pixel clock in BT817/BT818 to EVE_init() in case the display config
has EVE_PCLK_FREQ defined
- replaced BT81X_ENABLE with "EVE_GEN > 2"
- removed FT81X_ENABLE as FT81x already is the lowest supported chip revision now
- removed the formerly as deprected marked EVE_get_touch_tag()
- changed EVE_color_rgb() to use a 32 bit value like the rest of the color commands
- removed the meta-commands EVE_cmd_point(), EVE_cmd_line() and EVE_cmd_rect()
- split all display-list commands into two functions: EVE_cmd_XXX() and EVE_cmd_XXX_burst()
- switched from using EVE_RAM_CMD + cmdOffset to REG_CMDB_WRITE
- as a side effect from switching to REG_CMDB_WRITE, every coprocessor command is automatically executed now
- renamed EVE_LIB_GetProps() back to EVE_cmd_getprops() since it does not do anything special to justify a special name
- added helper function EVE_memWrite_sram_buffer()
- added EVE_cmd_bitmap_transform() and EVE_cmd_bitmap_transform_burst()
- added zero-pointer protection to commands using block_transfer()
- added EVE_cmd_playvideo()
- changed EVE_cmd_setfont() to a display-list command and added EVE_cmd_setfont_burst()
- changed EVE_cmd_setfont2() to a display-list command and added EVE_cmd_setfont2_burst()
- added EVE_cmd_videoframe()
- restructured: functions are sorted by chip-generation and within their group in alphabetical order
- reimplementedEVE_cmd_getmatrix() again, it needs to read values, not write them
- added EVE_cmd_fontcache() and EVE_cmd_fontcachequery()
- added EVE_cmd_calibratesub()
- added EVE_cmd_animframeram(), EVE_cmd_animframeram_burst(), EVE_cmd_animstartram(), EVE_cmd_animstartram_burst()
- added EVE_cmd_apilevel(), EVE_cmd_apilevel_burst()
- added EVE_cmd_calllist(), EVE_cmd_calllist_burst()
- added EVE_cmd_hsf(), EVE_cmd_hsf_burst()
- added EVE_cmd_linetime()
- added EVE_cmd_newlist(), EVE_cmd_newlist_burst()
- added EVE_cmd_runanim(), EVE_cmd_runanim_burst()
- added a safeguard to EVE_start_cmd_burst() to protect it from overlapping transfers with DMA and segmented lists
- used spi_transmit_32() to shorten this file by around 600 lines with no functional change
- removed the history from before 4.0
- removed a couple of spi_transmit_32() calls from EVE_cmd_getptr() to make it work again
- Bugfix: EVE_cmd_setfont2_burst() was using CMD_SETFONT instead of CMD_SETFONT2
- removed a check for cmd_burst from EVE_cmd_getimage() as it is in the group of commands that are not used for display
lists
- moved EVE_cmd_newlist() to the group of commands that are not used for display lists
- removed EVE_cmd_newlist_burst()
- renamed spi_flash_write() to private_block_write() and made it static
- renamed EVE_write_string() to private_string_write() and made it static
- made EVE_start_command() static
- Bugfix: ESP8266 needs 32 bit alignment for 32 bit pointers,
    changed private_string_write() for burst-mode to read 8-bit values
- Bugfix: somehow messed up private_string_write() for burst-mode
    but only for 8-Bit controllers
- changed EVE_memRead8(), EVE_memRead16() and EVE_memRead32() to use
    spi_transmit_32() for the initial address+zero byte transfer
    This speeds up ESP32/ESP8266 by several us, has no measureable effect
    for ATSAMD51 and is a little slower for AVR.
- Bugfix: not sure why but setting private_block_write() to static broke it, without "static" it works
- Bugfix: EVE_cmd_flashspirx() was using CMD_FLASHREAD
- fixed a warning in EVE_init() when compiling for EVE4
- renamed internal function EVE_begin_cmd() to eve_begin_cmd() and made it static
- changed all the EVE_start_command() calls to eve_begin_cmd() calls following the report on Github from
  Michael Wachs that these are identical - they weren't prior to V5
- removed EVE_start_command()
- Bugfix: EVE_init() was only checking the first two bits of REG_CPURESET and ignored the bit for the audio-engine, not
an issue but not correct either.
- fixed a few clang-tidy warnings
- fixed a few cppcheck warnings
- fixed a few CERT warnings
- converted all TABs to SPACEs
- made EVE_TOUCH_RZTHRESH in EVE_init() optional to a) remove it from EVE_config.h and b) make it configureable
externally
- changed EVE_init() to write 1200U to REG_TOUCH_RZTHRESH if EVE_TOUCH_RZTHRESH is not defined
- changed EVE_init() to return E_OK = 0x00 in case of success and more meaningfull values in case of failure
- changed EVE_busy() to return EVE_IS_BUSY if EVE is busy and E_OK = 0x00 if EVE is not busy - no real change in
functionality
- finally removed EVE_cmd_start() after setting it to deprecatd with the first 5.0 release
- renamed EVE_cmd_execute() to EVE_execute_cmd() to be more consistent, this is is not an EVE command
- changed EVE_init_flash() to return E_OK in case of success and more meaningfull values in case of failure
- added the return-value of EVE_FIFO_HALF_EMPTY to EVE_busy() to indicate there is more than 2048 bytes available
- minor cleanup, less break and else statements
- added the burst code back into all the functions for which there is a _burst version, this allows to use the version
without the traling _burst in the name when exceution speed is not an issue - e.g. with all targets supporting DMA
- removed the 4.0 history
- added the optional parameter EVE_ROTATE as define to EVE_init() to allow for screen rotation during init
    thanks for the idea to AndrejValand on Github!
- added the optional parameter EVE_BACKLIGHT_PWM to EVE_init() to allow setting the backlight during init
- modified EVE_calibrate_manual() to work better with bar type displays
- fixed a large number of MISRA-C issues - mostly more casts for explicit type conversion and more brackets
- changed the varargs versions of cmd_button, cmd_text and cmd_toggle to use an array of uint32_t values to comply with MISRA-C
- basic maintenance: checked for violations of white space and indent rules
- more linter fixes for minor issues like variables shorter than 3 characters
- added EVE_color_a() / EVE_color_a_burst()
- more minor tweaks and fixes to make the static analyzer happy
- changed the burst variant of private_string_write() back to the older and faster version
- refactoring of EVE_init() to single return
- added prototype for EVE_write_display_parameters()
- added EVE_memRead_sram_buffer()
- Bugfix issue #81: neither DISP or the pixel clock are enabled for EVE4 configurations not using EVE_PCLK_FREQ.
    thanks for the report to grados73 on Github!
- added a few support lines for the Gameduino GD3X to EVE_init()
- switched from using CMD_PCLKFREQ to writing to REG_PCLK_FREQ directly
- added define EVE_SET_REG_PCLK_2X to set REG_PCLK_2X to 1 when necessary
- Bugfix: EVE_init() did not set the audio engine to "mute" as intended, but to "silent"
- Bugfix: EVE_busy() returns E_NOT_OK now on coprocessor faults.
    thanks for the report to Z0ld3n on Github!
- Fix: reworked EVE_busy() to return EVE_FAULT_RECOVERED on deteced coprocessor faults,
    removed the flash commands from the fault recovery sequence as these are project specific.
- added EVE_get_and_reset_fault_state() to check if EVE_busy() triggered a fault recovery
- added notes on how to use to EVE_cmd_setfont2() and EVE_cmd_romfont()
- new optional parameter in EVE_init(): EVE_BACKLIGHT_FREQ
- fixed a couple of minor issues from static code analysis
- reworked the burst part of private_string_write() to be less complex
- renamed chipid references to regid as suggested by #93 on github
- Bugfix: broke transfers of buffers larger than 3840 when fixing issues from static code analysis
- changed a number of function parameters from signed to unsigned following the
    updated BT81x series programming guide V2.4
- did another linter pass and fixed some things
- started to improve the embedded documentation
- added more documentation
- removed EVE_cmd_hsf_burst()
- new parameter for EVE_init(): EVE_SOFT_RESET
- Bugfix: while this worked, the PLL range for BT81x was configured incorrectly
- Compliance: fixed BARR-C:2018 Rule 6.2c violation in private_string_write()
- fix: added two EVE_cmd_memzero() calls to EVE_cmd_clearcache() to run CMD_CLEARCACHE on empty display lists
- cleanup: moved most of the type casts to static inline functions: i16_i16_to_u32(), u16_u16_to_u32() and i32_to_u32()
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
- added EVE_point_size() / EVE_point_size_burst()
- added EVE_cmd_stop() / EVE_cmd_stop_burst()
- Bugfix: broke GT911 support for EVE2 and AVR almost two years ago...
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

#include "EVE_commands.h"

/* EVE Memory Commands - used with EVE_memWritexx and EVE_memReadxx */
#define MEM_WRITE 0x80U /* EVE Host Memory Write */
/* #define MEM_READ 0x00U */ /* EVE Host Memory Read */

#define DUMMY_BYTE ((uint8_t) 0x00U)

/* define NULL if it not already is */
#ifndef NULL
#include <stdio.h>
#endif

static volatile uint8_t cmd_burst = 0U; /* flag to indicate cmd-burst is active */
static volatile uint8_t fault_recovered = E_OK; /* flag to indicate if EVE_busy triggered a fault recovery */

/* ##################################################################
    helper functions
##################################################################### */

/**
 * @brief Send a host command.
 */
void EVE_cmdWrite(uint8_t const command, uint8_t const parameter)
{
    EVE_cs_set();
    spi_transmit(command);
    spi_transmit(parameter);
    spi_transmit(DUMMY_BYTE);
    EVE_cs_clear();
}

/**
 * @brief Implementation of rd8() function, reads 8 bits.
 */
uint8_t EVE_memRead8(uint32_t const ft_address)
{
    uint8_t data;
    EVE_cs_set();
    spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));
    data = spi_receive(DUMMY_BYTE); /* read data byte by sending another dummy byte */
    EVE_cs_clear();
    return (data);
}

/**
 * @brief Implementation of rd16() function, reads 16 bits.
 */
uint16_t EVE_memRead16(uint32_t const ft_address)
{
    uint16_t data;

    EVE_cs_set();
    spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));
    uint8_t const lowbyte = spi_receive(DUMMY_BYTE); /* read low byte */
    uint8_t const hibyte = spi_receive(DUMMY_BYTE); /* read high byte */
    data = ((uint16_t) hibyte * 256U) | lowbyte;
    EVE_cs_clear();
    return (data);
}

/**
 * @brief Implementation of rd32() function, reads 32 bits.
 */
uint32_t EVE_memRead32(uint32_t const ft_address)
{
    uint32_t data;
    EVE_cs_set();
    spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));
    data = ((uint32_t) spi_receive(DUMMY_BYTE)); /* read low byte */
    data = ((uint32_t) spi_receive(DUMMY_BYTE) << 8U) | data;
    data = ((uint32_t) spi_receive(DUMMY_BYTE) << 16U) | data;
    data = ((uint32_t) spi_receive(DUMMY_BYTE) << 24U) | data; /* read high byte */
    EVE_cs_clear();
    return (data);
}

/**
 * @brief Implementation of wr8() function, writes 8 bits.
 */
void EVE_memWrite8(uint32_t const ft_address, uint8_t const ft_data)
{
    EVE_cs_set();
    spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE);
    spi_transmit((uint8_t) (ft_address >> 8U));
    spi_transmit((uint8_t) (ft_address & 0x000000ffUL));
    spi_transmit(ft_data);
    EVE_cs_clear();
}

/**
 * @brief Implementation of wr16() function, writes 16 bits.
 */
void EVE_memWrite16(uint32_t const ft_address, uint16_t const ft_data)
{
    EVE_cs_set();
    spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE); /* send Memory Write plus high address byte */
    spi_transmit((uint8_t) (ft_address >> 8U));              /* send middle address byte */
    spi_transmit((uint8_t) (ft_address & 0x000000ffUL));     /* send low address byte */
    spi_transmit((uint8_t) (ft_data & 0x00ffU));             /* send data low byte */
    spi_transmit((uint8_t) (ft_data >> 8U));                 /* send data high byte */
    EVE_cs_clear();
}

/**
 * @brief Implementation of wr32() function, writes 32 bits.
 */
void EVE_memWrite32(uint32_t const ft_address, uint32_t const ft_data)
{
    EVE_cs_set();
    spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE); /* send Memory Write plus high address byte */
    spi_transmit((uint8_t) (ft_address >> 8U));              /* send middle address byte */
    spi_transmit((uint8_t) (ft_address & 0x000000ffUL));     /* send low address byte */
    spi_transmit_32(ft_data);
    EVE_cs_clear();
}

/**
 * @brief Helper function, write a block of memory from the FLASH of the host controller to EVE.
 */
void EVE_memWrite_flash_buffer(uint32_t const ft_address, const uint8_t * const p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE);
        spi_transmit((uint8_t) (ft_address >> 8U));
        spi_transmit((uint8_t) (ft_address & 0x000000ffUL));

    //    uint32_t length = (len + 3U) & (~3U);

        for (uint32_t count = 0U; count < len; count++)
        {
            spi_transmit(fetch_flash_byte(&p_data[count]));
        }

        EVE_cs_clear();
    }
}

/**
 * @brief Helper function, write a block of memory from the SRAM of the host controller to EVE.
 */
void EVE_memWrite_sram_buffer(uint32_t const ft_address, const uint8_t * const p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit((uint8_t) (ft_address >> 16U) | MEM_WRITE);
        spi_transmit((uint8_t) (ft_address >> 8U));
        spi_transmit((uint8_t) (ft_address & 0x000000ffUL));

    //    uint32_t length = (len + 3U) & (~3U);

        for (uint32_t count = 0U; count < len; count++)
        {
            spi_transmit(p_data[count]);
        }

        EVE_cs_clear();
    }
}

/**
 * @brief Helper function, read a block of memory from EVE to the SRAM of the host controller.
 */
void EVE_memRead_sram_buffer(uint32_t const ft_address, uint8_t * const p_data, uint32_t const len)
{
    if (p_data != NULL)
    {
        EVE_cs_set();
        spi_transmit_32(((ft_address >> 16U) & 0x0000007fUL) + (ft_address & 0x0000ff00UL) + ((ft_address & 0x000000ffUL) << 16U));

        for (uint32_t count = 0U; count < len; count++)
        {
            p_data[count] = spi_receive(0U); /* read data byte by sending another dummy byte */
        }

        EVE_cs_clear();
    }
}

static void CoprocessorFaultRecover(void)
{
#if EVE_GEN > 2
        uint16_t copro_patch_pointer;
        copro_patch_pointer = EVE_memRead16(REG_COPRO_PATCH_PTR);
#endif

        EVE_memWrite8(REG_CPURESET, 1U); /* hold coprocessor engine in the reset condition */
        EVE_memWrite16(REG_CMD_READ, 0U); /* set REG_CMD_READ to 0 */
        EVE_memWrite16(REG_CMD_WRITE, 0U); /* set REG_CMD_WRITE to 0 */
        EVE_memWrite16(REG_CMD_DL, 0U); /* reset REG_CMD_DL to 0 as required by the BT81x programming guide, should not hurt FT8xx */

#if EVE_GEN > 2
        EVE_memWrite16(REG_COPRO_PATCH_PTR, copro_patch_pointer);

        /* restore REG_PCLK in case it was set to zero by an error */
#if (EVE_GEN > 3) && (defined EVE_PCLK_FREQ)
        EVE_memWrite16(REG_PCLK_FREQ, (uint16_t) EVE_PCLK_FREQ);
        EVE_memWrite8(REG_PCLK, 1U); /* enable extsync mode */
#else
        EVE_memWrite8(REG_PCLK, EVE_PCLK);
#endif

#endif
        EVE_memWrite8(REG_CPURESET, 0U); /* set REG_CPURESET to 0 to restart the coprocessor engine*/
        DELAY_MS(10U);                   /* just to be safe */
}

/**
 * @brief Check if the coprocessor completed executing the current command list.
 * @return - E_OK - if EVE is not busy (no DMA transfer active and REG_CMDB_SPACE has the value 0xffc, meaning the CMD-FIFO is empty
 * @return - EVE_IS_BUSY - if a DMA transfer is active or REG_CMDB_SPACE has a value smaller than 0xffc
 * @return - EVE_FIFO_HALF_EMPTY - if no DMA transfer is active and REG_CMDB_SPACE shows more than 2048 bytes available
 * @return - E_NOT_OK - if there was a coprocessor fault and the recovery sequence was executed
 * @note - if there is a coprocessor fault the external flash is not reinitialized by EVE_busy()
 */
uint8_t EVE_busy(void)
{
    uint16_t space;
    uint8_t ret = EVE_IS_BUSY;

#if defined (EVE_DMA)
    if (0 == EVE_dma_busy)
    {
#endif

    space = EVE_memRead16(REG_CMDB_SPACE);

    /* (REG_CMDB_SPACE & 0x03) != 0 -> we have a coprocessor fault */
    if ((space & 3U) != 0U) /* we have a coprocessor fault, make EVE play with us again */
    {
        ret = EVE_FAULT_RECOVERED;
        fault_recovered = EVE_FAULT_RECOVERED; /* save fault recovery state */
        CoprocessorFaultRecover();
    }
    else
    {
        if (0xffcU == space)
        {
            ret = E_OK;
        }
        else if (space > 0x800U)
        {
            ret = EVE_FIFO_HALF_EMPTY;
        }
        else
        {
            ret = EVE_IS_BUSY;
        }
    }

#if defined (EVE_DMA)
    }
#endif

    return (ret);
}

/**
 * @brief Helper function to check if EVE_busy() tried to recover from a coprocessor fault.
 * The internal fault indicator is cleared so it could be set by EVE_busy() again.
 * @return - EVE_FAULT_RECOVERED - if EVE_busy() detected a coprocessor fault
 * @return - E_OK - if EVE_busy() did not detect a coprocessor fault
 */
uint8_t EVE_get_and_reset_fault_state(void)
{
    uint8_t ret = E_OK;

    if (EVE_FAULT_RECOVERED == fault_recovered)
    {
        ret = EVE_FAULT_RECOVERED;
        fault_recovered = E_OK;
    }
    return (ret);
}

/**
 * @brief Helper function, wait for the coprocessor to complete the FIFO queue.
 */
void EVE_execute_cmd(void)
{
    while (EVE_busy() != E_OK)
    {
    }
}

/* begin a coprocessor command, this is used for non-display-list and non-burst-mode commands.*/
static void eve_begin_cmd(const uint32_t command)
{
    EVE_cs_set();
    spi_transmit((uint8_t) 0xB0U); /* high-byte of REG_CMDB_WRITE + MEM_WRITE */
    spi_transmit((uint8_t) 0x25U); /* middle-byte of REG_CMDB_WRITE */
    spi_transmit((uint8_t) 0x78U); /* low-byte of REG_CMDB_WRITE */
    spi_transmit_32(command);
}

void private_block_write(const uint8_t * const p_data, const uint16_t len); /* prototype to comply with MISRA */

void private_block_write(const uint8_t * const p_data, const uint16_t len)
{
    uint8_t padding;

    padding = (uint8_t) (len & 3U); /* 0, 1, 2 or 3 */
    padding = 4U - padding;         /* 4, 3, 2 or 1 */
    padding &= 3U;                  /* 3, 2 or 1 */

    for (uint16_t count = 0U; count < len; count++)
    {
        spi_transmit(fetch_flash_byte(&p_data[count]));
    }

    while (padding > 0U)
    {
        spi_transmit(0U);
        padding--;
    }
}

void block_transfer(const uint8_t * const p_data, const uint32_t len); /* prototype to comply with MISRA */

void block_transfer(const uint8_t * const p_data, const uint32_t len)
{
    uint32_t bytes_left;
    uint32_t offset = 0U;

    bytes_left = len;
    while (bytes_left > 0U)
    {
        uint32_t block_len;

        block_len = (bytes_left > 3840UL) ? 3840UL : bytes_left;

        EVE_cs_set();
        spi_transmit((uint8_t) 0xB0U); /* high-byte of REG_CMDB_WRITE + MEM_WRITE */
        spi_transmit((uint8_t) 0x25U); /* middle-byte of REG_CMDB_WRITE */
        spi_transmit((uint8_t) 0x78U); /* low-byte of REG_CMDB_WRITE */
        private_block_write(&p_data[offset], (uint16_t) block_len);
        EVE_cs_clear();
        offset += block_len;
        bytes_left -= block_len;
        EVE_execute_cmd();
    }
}

/* ##################################################################
    coprocessor commands that are not used in displays lists,
    most of these are not to be used with burst transfers
################################################################### */

/* BT817 / BT818 */
#if EVE_GEN > 3

/**
 * @brief Terminates the compilation of a command list into RAM_G.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_endlist(void)
{
    eve_begin_cmd(CMD_ENDLIST);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Write "num" bytes from src in RAM_G to the previously erased external flash of a BT81x at address dest.
 * @note - dest must be 4096-byte aligned, src must be 4-byte aligned, num must be a multiple of 4096
 * @note - EVE will not do anything if the alignment requirements are not met
 * @note - the address ptr is relative to the flash so the first address is 0x000000 not 0x800000
 * @note - this looks exactly the same as EVE_cmd_flashupdate() but it needs the flash to be empty
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashprogram(const uint32_t dest, const uint32_t src, const uint32_t num)
{
    eve_begin_cmd(CMD_FLASHPROGRAM);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Enable the font cache.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_fontcache(const uint32_t font, const uint32_t ptr, const uint32_t num)
{
    eve_begin_cmd(CMD_FONTCACHE);
    spi_transmit_32(font);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Queries the capacity and utilization of the font cache.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_fontcachequery(uint32_t * const p_total, uint32_t * const p_used)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_FONTCACHEQUERY);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();

    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */

    if (p_total != NULL)
    {
        *p_total = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
    }
    if (p_used != NULL)
    {
        *p_used = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
    }
}

/**
 * @brief Returns all the attributes of the bitmap made by the previous CMD_LOADIMAGE, CMD_PLAYVIDEO, CMD_VIDEOSTART or CMD_VIDEOSTARTF.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_getimage(uint32_t * const p_source, uint32_t * const p_fmt, uint32_t * const p_width, uint32_t * const p_height, uint32_t * const p_palette)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_GETIMAGE);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();

    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */

    if (p_palette != NULL)
    {
        *p_palette = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
    }
    if (p_height != NULL)
    {
        *p_height = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
    }
    if (p_width != NULL)
    {
        *p_width = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL));
    }
    if (p_fmt != NULL)
    {
        *p_fmt = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 16UL) & 0xfffUL));
    }
    if (p_source != NULL)
    {
        *p_source = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 20UL) & 0xfffUL));
    }
}

/**
 * @brief Undocumented command.
 * @note - Commented out as it not known what this does.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
#if 0
void EVE_cmd_linetime(uint32_t dest)
{
    eve_begin_cmd(CMD_LINETIME);
    spi_transmit_32(dest);
    EVE_cs_clear();
    EVE_execute_cmd();
}
#endif

/**
 * @brief Starts the compilation of a command list into RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_newlist(const uint32_t adr)
{
    eve_begin_cmd(CMD_NEWLIST);
    spi_transmit_32(adr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Sets REG_PCLK_FREQ to generate the closest possible frequency to the one requested.
 * @return - the frequency achieved or zero if no frequency was found
 * @note - When using this command, the flash BLOB is required.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_pclkfreq(const uint32_t ftarget, const int32_t rounding)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_PCLKFREQ);
    spi_transmit_32(ftarget);
    spi_transmit_32(i32_to_u32(rounding));
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Display a tescard graphic.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_testcard(void)
{
    eve_begin_cmd(CMD_TESTCARD);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Waits for a specified number of microseconds.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_wait(const uint32_t usec)
{
    eve_begin_cmd(CMD_WAIT);
    spi_transmit_32(usec);
    EVE_cs_clear();
    EVE_execute_cmd();
}

#endif /* EVE_GEN > 3 */

/* BT815 / BT816 */
#if EVE_GEN > 2

/**
 * @brief Clears the graphics engine’s internal flash cache.
 * @note - This function includes clearing out the display list.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_clearcache(void)
{
    EVE_cmd_memzero(EVE_RAM_DL, 16);
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(CMD_SWAP);
    EVE_execute_cmd();

    EVE_cmd_memzero(EVE_RAM_DL, 16);
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(CMD_SWAP);
    EVE_execute_cmd();

    EVE_cmd_dl(CMD_CLEARCACHE);
    EVE_execute_cmd();
}

/**
 * @brief Re-connect to the attached SPI flash storage.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashattach(void)
{
    eve_begin_cmd(CMD_FLASHATTACH);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Dis-connect from the attached SPI flash storage.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashdetach(void)
{
    eve_begin_cmd(CMD_FLASHDETACH);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Erases the attached SPI flash storage.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flasherase(void)
{
    eve_begin_cmd(CMD_FLASHERASE);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Drive the attached SPI flash storage in full-speed mode, if possible.
 * @return - Zero on success, error code on failure
 * @note - When using this command, the flash BLOB is required.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_flashfast(void)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_FLASHFAST);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief De-asserts the SPI CS signal of the attached SPI flash storage.
 * @note - Only works when the attached SPI flash storage has been detached.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashspidesel(void)
{
    eve_begin_cmd(CMD_FLASHSPIDESEL);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Copies "num" bytes from "src" in attached SPI flash storage to "dest" in RAM_G.
 * @note - src must be 64-byte aligned, dest must be 4-byte aligned, num must be a multiple of 4
 * @note - EVE will not do anything if the alignment requirements are not met
 * @note - The src pointer is relative to the flash so the first address is 0x000000 not 0x800000.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashread(const uint32_t dest, const uint32_t src, const uint32_t num)
{
    eve_begin_cmd(CMD_FLASHREAD);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Set the source address for flash data loaded by the CMD_LOADIMAGE, CMD_PLAYVIDEO, CMD_VIDEOSTARTF and CMD_INFLATE2 commands with the OPT_FLASH option.
 * @note - Address must be 64-byte aligned.
 * @note - EVE will not do anything if the alignment requirements are not met.
 * @note - The pointer is relative to the flash, so the first address is 0x000000 not 0x800000.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashsource(const uint32_t ptr)
{
    eve_begin_cmd(CMD_FLASHSOURCE);
    spi_transmit_32(ptr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Receives bytes from the flash SPI interface and writes them to main memory.
 * @note - Only works when the attached SPI flash storage has been detached.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashspirx(const uint32_t dest, const uint32_t num)
{
    eve_begin_cmd(CMD_FLASHSPIRX);
    spi_transmit_32(dest);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Transmits bytes over the flash SPI interface.
 * @note - Only works when the attached SPI flash storage has been detached.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashspitx(const uint32_t num, const uint8_t * const p_data)
{
    eve_begin_cmd(CMD_FLASHSPITX);
    spi_transmit_32(num);
    EVE_cs_clear();
    block_transfer(p_data, num);
}

/**
 * @brief Write "num" bytes from src in RAM_G to the attached SPI flash storage at address dest.
 * @note - dest must be 4096-byte aligned, src must be 4-byte aligned, num must be a multiple of 4096
 * @note - EVE will not do anything if the alignment requirements are not met.
 * @note - The address ptr is relative to the flash so the first address is 0x000000 not 0x800000.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashupdate(const uint32_t dest, const uint32_t src, const uint32_t num)
{
    eve_begin_cmd(CMD_FLASHUPDATE);
    spi_transmit_32(dest);
    spi_transmit_32(src);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Write "num" bytes to the attached SPI flash storage at address dest.
 * @note - dest must be 256-byte aligned, num must be a multiple of 256
 * @note - EVE will not do anything if the alignment requirements are not met.
 * @note - The address ptr is relative to the flash so the first address is 0x000000 not 0x800000.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_flashwrite(const uint32_t ptr, const uint32_t num, const uint8_t * const p_data)
{
    eve_begin_cmd(CMD_FLASHWRITE);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    EVE_cs_clear();
    if (p_data != NULL)
    {
        block_transfer(p_data, num);
    }
}

/**
 * @brief Decompress data into RAM_G.
 * @note - The data must be correct and complete.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_inflate2(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_INFLATE2);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

    if (0UL == options) /* direct data, not by Media-FIFO or Flash */
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
    }
}

/**
 * @brief Load bitmap handles 16-31 with their default fonts.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_resetfonts(void)
{
    eve_begin_cmd(CMD_RESETFONTS);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Initialize video frame decoder for video from the flash memory.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_videostartf(void)
{
    eve_begin_cmd(CMD_VIDEOSTARTF);
    EVE_cs_clear();
    EVE_execute_cmd();
}

#endif /* EVE_GEN > 2 */

/**
 * @brief Set the coprocessor engine to default reset states.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 *
 */
void EVE_cmd_coldstart(void)
{
    eve_begin_cmd(CMD_COLDSTART);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Returns the source address and size of the bitmap loaded by the previous CMD_LOADIMAGE.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_getprops(uint32_t * const p_pointer, uint32_t * const p_width, uint32_t * const p_height)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_GETPROPS);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */

    if (p_pointer != NULL)
    {
        *p_pointer = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL));
    }
    if (p_width != NULL)
    {
        *p_width = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
    }
    if (p_height != NULL)
    {
        *p_height = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
    }
}

/**
 * @brief Returns the next address after a CMD_INFLATE and other commands.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_getptr(void)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_GETPTR);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Decompress data into RAM_G.
 * @note - The data must be correct and complete.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_inflate(const uint32_t ptr, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_INFLATE);
    spi_transmit_32(ptr);
    EVE_cs_clear();
    if (p_data != NULL)
    {
        block_transfer(p_data, len);
    }
}

/**
 * @brief Trigger interrupt INT_CMDFLAG.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_interrupt(const uint32_t msec)
{
    eve_begin_cmd(CMD_INTERRUPT);
    spi_transmit_32(msec);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Loads and decodes a JPEG/PNG image into RAM_G.
 * @note - Decoding PNG images takes significantly more time than decoding JPEG images.
 * @note - In doubt use the EVE Asset Builder to check if PNG/JPEG files are compatible.
 * @note - If the image is in PNG format, the top 42kiB of RAM_G will be overwritten.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_loadimage(const uint32_t ptr, const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_LOADIMAGE);
    spi_transmit_32(ptr);
    spi_transmit_32(options);
    EVE_cs_clear();

#if EVE_GEN > 2
    if ((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
        (0UL == (options & EVE_OPT_FLASH))) /* direct data, neither by Media-FIFO or from Flash */
#else
    if (0UL == (options & EVE_OPT_MEDIAFIFO))  /* direct data, not by Media-FIFO */
#endif
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
    }
}

/**
 * @brief Ask the coprocessor engine to play back a short animation of the FTDI or Bridgetek logo.
 * @note - Takes 2.5s to complete during which RAM_CMD and RAM_DL must not be written to.
 * @note - Use EVE_execute_cmd() for a blocking wait for completion.
 * @note - Or call EVE_busy() in your state-machine untill it returns E_OK.
 * @note - Meant to be called outside display-list building.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_logo(void)
{
    eve_begin_cmd(CMD_LOGO);
    EVE_cs_clear();
}

/**
 * @brief Set up a streaming media FIFO in RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_mediafifo(const uint32_t ptr, const uint32_t size)
{
    eve_begin_cmd(CMD_MEDIAFIFO);
    spi_transmit_32(ptr);
    spi_transmit_32(size);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Copy a block of memory with the coprocessor.
 * @note - Meant to be called outside display-list building.
 * @note - For use with CMD_SYNC for example.
 */
void EVE_cmd_memcpy(const uint32_t dest, const uint32_t src, const uint32_t num)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_MEMCPY);
        spi_transmit_32(dest);
        spi_transmit_32(src);
        spi_transmit_32(num);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_MEMCPY);
        spi_transmit_burst(dest);
        spi_transmit_burst(src);
        spi_transmit_burst(num);
    }
}

/**
 * @brief Copy a block of memory with the coprocessor, only works in burst-mode.
 */
void EVE_cmd_memcpy_burst(const uint32_t dest, const uint32_t src, const uint32_t num)
{
    spi_transmit_burst(CMD_MEMCPY);
    spi_transmit_burst(dest);
    spi_transmit_burst(src);
    spi_transmit_burst(num);
}

/**
 * @brief Compute a CRC-32 for RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_memcrc(const uint32_t ptr, const uint32_t num)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_MEMCRC);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Fill RAM_G with a byte value.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memset(const uint32_t ptr, const uint8_t value, const uint32_t num)
{
    eve_begin_cmd(CMD_MEMSET);
    spi_transmit_32(ptr);
    spi_transmit_32((uint32_t)value);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Write bytes into memory using the coprocessor.
 * @note - Commented out, just use one of the EVE_memWrite* helper functions to directly write to EVEs memory.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
#if 0
void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *p_data)
{
    eve_begin_cmd(CMD_MEMWRITE);
    spi_transmit_32(dest);
    spi_transmit_32(num);

    num = (num + 3U) & (~3U);

    for (uint32_t count = 0U; count<len; count++)
    {
        spi_transmit(pgm_read_byte_far(p_data + count));
    }

    EVE_cs_clear();
    EVE_execute_cmd();
}
#endif

/**
 * @brief Write zero to RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memzero(const uint32_t ptr, const uint32_t num)
{
    eve_begin_cmd(CMD_MEMZERO);
    spi_transmit_32(ptr);
    spi_transmit_32(num);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Play back motion-JPEG encoded AVI video.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command.
 * @note - Does not support burst-mode.
 * @note - Does not wait for completion in order to allow the video to be paused or terminated by REG_PLAY_CONTROL
 */
void EVE_cmd_playvideo(const uint32_t options, const uint8_t * const p_data, const uint32_t len)
{
    eve_begin_cmd(CMD_PLAYVIDEO);
    spi_transmit_32(options);
    EVE_cs_clear();

#if EVE_GEN > 2
    if ((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
        (0UL == (options & EVE_OPT_FLASH))) /* direct data, neither by Media-FIFO or from Flash */
#else
    if (0UL == (options & EVE_OPT_MEDIAFIFO))  /* direct data, not by Media-FIFO */
#endif
    {
        if (p_data != NULL)
        {
            block_transfer(p_data, len);
        }
    }
}

/**
 * @brief Read a register value using the coprocessor.
 * @note - Commented out, just read the register directly.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
#if 0
uint32_t EVE_cmd_regread(uint32_t ptr)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_REGREAD);
    spi_transmit_32(ptr);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}
#endif

/**
 * @brief Rotate the screen and set up transform matrix accordingly.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_setrotate(const uint32_t rotation)
{
    eve_begin_cmd(CMD_SETROTATE);
    spi_transmit_32(rotation);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Take a snapshot of the current screen.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_snapshot(const uint32_t ptr)
{
    eve_begin_cmd(CMD_SNAPSHOT);
    spi_transmit_32(ptr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Take a snapshot of part of the current screen with format option.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_snapshot2(const uint32_t fmt, const uint32_t ptr, const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt)
{
    eve_begin_cmd(CMD_SNAPSHOT2);
    spi_transmit_32(fmt);
    spi_transmit_32(ptr);
    spi_transmit_32(i16_i16_to_u32(xc0, yc0));
    spi_transmit_32(u16_u16_to_u32(wid, hgt));
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Wait for the end of the video scan out period.
 * @note - Meant to be called outside display-list building.
 * @note - This does not go into the display list, this is for the coprocessor.
 */
void EVE_cmd_sync(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SYNC);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SYNC);
    }
}

/**
 * @brief Wait for the end of the video scan out period, only works in burst-mode.
 */
void EVE_cmd_sync_burst(void)
{
    spi_transmit_burst(CMD_SYNC);
}

/**
 * @brief Track touches for a graphics object.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_track(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt, const uint16_t tag)
{
    eve_begin_cmd(CMD_TRACK);
    spi_transmit_32(i16_i16_to_u32(xc0, yc0));
    spi_transmit_32(u16_u16_to_u32(wid, hgt));
    spi_transmit_32(u16_u16_to_u32(tag, 0x0000));
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Load the next frame of a video.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_videoframe(const uint32_t dest, const uint32_t result_ptr)
{
    eve_begin_cmd(CMD_VIDEOFRAME);
    spi_transmit_32(dest);
    spi_transmit_32(result_ptr);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/**
 * @brief Initialize video frame decoder for video provided using the media FIFO.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_videostart(void)
{
    eve_begin_cmd(CMD_VIDEOSTART);
    EVE_cs_clear();
    EVE_execute_cmd();
}

/* ##################################################################
        patching and initialization
#################################################################### */

#if EVE_GEN > 2

/**
 * @brief EVE flash initialization for BT81x, switches the FLASH attached to a BT81x to full-speed mode
 * @return Returns E_OK in case of success, EVE_FAIL_FLASH_STATUS_INIT if the status remains init,
 * EVE_FAIL_FLASH_STATUS_DETACHED if no flash chip was found, a number of different values for failures with
 * cmd_flashfast and E_NOT_OK if a not supported status is returned in REG_FLASH_STATUS.
 */
uint8_t EVE_init_flash(void)
{
    uint8_t timeout = 0U;
    uint8_t status;
    uint8_t ret_val = E_NOT_OK;

    status = EVE_memRead8(REG_FLASH_STATUS); /* should be 0x02 - FLASH_STATUS_BASIC, power-up is done and the attached flash is detected */

     /* we are somehow still in init, give it a litte more time, this should never happen */
    while (EVE_FLASH_STATUS_INIT == status)
    {
        status = EVE_memRead8(REG_FLASH_STATUS);
        DELAY_MS(1U);
        timeout++;
        if (timeout > 100U) /* 100ms and still in init, lets call quits now and exit with an error */
        {
            ret_val = EVE_FAIL_FLASH_STATUS_INIT;
            break;
        }
    }

    /* no flash was found during init, no flash present or the detection failed, give it another try */
    if (EVE_FLASH_STATUS_DETACHED == status)
    {
        EVE_cmd_dl(CMD_FLASHATTACH);
        EVE_execute_cmd();
        status = EVE_memRead8(REG_FLASH_STATUS);
        if (status != 2U) /* still not in FLASH_STATUS_BASIC, time to give up */
        {
            ret_val = EVE_FAIL_FLASH_STATUS_DETACHED;
        }
    }

    /* flash detected and ready for action, move it up to FLASH_STATUS_FULL */
    if (EVE_FLASH_STATUS_BASIC == status)
    {
        uint32_t result;

        result = EVE_cmd_flashfast();

        switch (result)
        {
            case 0x0000UL:
                ret_val = E_OK;
            break;

            case 0xE001UL:
                ret_val = EVE_FAIL_FLASHFAST_NOT_SUPPORTED;
            break;

            case 0xE002UL:
                ret_val = EVE_FAIL_FLASHFAST_NO_HEADER_DETECTED;
            break;

            case 0xE003UL:
                ret_val = EVE_FAIL_FLASHFAST_SECTOR0_FAILED;
            break;

            case 0xE004UL:
                ret_val = EVE_FAIL_FLASHFAST_BLOB_MISMATCH;
            break;

            case 0xE005UL:
                ret_val = EVE_FAIL_FLASHFAST_SPEED_TEST;
            break;

            default: /* we have an unknown error, so just return failure */
                ret_val = E_NOT_OK;
            break;
        }
    }

    if (EVE_FLASH_STATUS_FULL == status) /* we are already there, why has this function been called? */
    {
        ret_val = E_OK;
    }

    return (ret_val);
}

#endif /* EVE_GEN > 2 */

#if defined (EVE_HAS_GT911)

#if EVE_GEN < 3
#if defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

/* FT811 / FT813 binary-blob from FTDIs AN_336 to patch the touch-engine for Goodix GT911 / GT9271 touch controllers */
const uint8_t eve_gt911_data[1184U] PROGMEM =
{
    26,  255, 255, 255, 32,  32,  48,  0,   4,   0,   0,   0,   2,   0,   0,   0,   34,  255, 255, 255, 0,   176, 48,
    0,   120, 218, 237, 84,  221, 111, 84,  69,  20,  63,  51,  179, 93,  160, 148, 101, 111, 76,  5,   44,  141, 123,
    111, 161, 11,  219, 154, 16,  9,   16,  17,  229, 156, 75,  26,  11,  13,  21,  227, 3,   16,  252, 184, 179, 45,
    219, 143, 45,  41,  125, 144, 72,  67,  100, 150, 71,  189, 113, 18,  36,  17,  165, 100, 165, 198, 16,  32,  17,
    149, 196, 240, 128, 161, 16,  164, 38,  54,  240, 0,   209, 72,  130, 15,  38,  125, 48,  66,  82,  30,  76,  19,
    31,  172, 103, 46,  139, 24,  255, 4,   227, 157, 204, 156, 51,  115, 102, 206, 231, 239, 220, 5,   170, 94,  129,
    137, 75,  194, 216, 98,  94,  103, 117, 115, 121, 76,  131, 177, 125, 89,  125, 82,  123, 60,  243, 58,  142, 242,
    204, 185, 243, 188, 118, 156, 227, 155, 203, 238, 238, 195, 251, 205, 229, 71,  92,  28,  169, 190, 184, 84,  143,
    113, 137, 53,  244, 103, 181, 237, 87,  253, 113, 137, 233, 48,  12,  198, 165, 181, 104, 139, 25,  84,  253, 155,
    114, 74,  191, 0,   54,  138, 163, 12,  62,  131, 207, 129, 23,  217, 34,  91,  31,  128, 65,  246, 163, 175, 213,
    8,   147, 213, 107, 35,  203, 94,  108, 3,   111, 40,  171, 83,  24,  15,  165, 177, 222, 116, 97,  23,  188, 140,
    206, 150, 42,  102, 181, 87,  78,  86,  182, 170, 134, 215, 241, 121, 26,  243, 252, 2,   76,  115, 217, 139, 222,
    206, 173, 136, 132, 81,  61,  35,  185, 39,  113, 23,  46,  199, 76,  178, 54,  151, 183, 224, 0,   40,  189, 28,
    149, 182, 58,  131, 79,  152, 30,  76,  34,  98,  234, 162, 216, 133, 141, 102, 39,  170, 40,  192, 101, 53,  201,
    146, 191, 37,  77,  44,  177, 209, 74,  211, 5,   206, 187, 5,   6,   216, 47,  53,  96,  123, 22,  50,  103, 251,
    192, 84,  17,  74,  227, 185, 56,  106, 51,  91,  161, 96,  182, 163, 48,  171, 141, 139, 65,  152, 66,  66,  11,
    102, 43,  158, 75,  36,  80,  147, 184, 147, 139, 112, 17,  235, 216, 103, 111, 239, 245, 92,  10,  175, 194, 40,
    44,  58,  125, 5,   59,  112, 50,  103, 245, 4,   78,  192, 5,   156, 194, 51,  60,  191, 134, 75,  110, 173, 237,
    46,  192, 121, 156, 192, 115, 184, 218, 120, 67,  63,  115, 46,  11,  102, 10,  97,  232, 50,  235, 114, 182, 148,
    118, 178, 41,  188, 12,  135, 77,  202, 124, 12,  96,  238, 35,  161, 234, 189, 129, 23,  249, 212, 139, 230, 25,
    53,  48,  205, 52,  93,  163, 117, 53,  154, 170, 81,  85,  163, 178, 70,  69,  66,  167, 241, 14,  46,  241, 1,
    226, 136, 152, 179, 197, 59,  184, 148, 254, 49,  132, 48,  15,  176, 137, 192, 76,  131, 196, 105, 104, 162, 86,
    81,  160, 165, 255, 26,  173, 162, 137, 86,  145, 210, 183, 192, 55,  175, 194, 211, 60,  91,  120, 230, 184, 174,
    27,  41,  131, 155, 40,  224, 29,  87,  179, 232, 16,  55,  55,  7,   165, 147, 81,  23,  165, 49,  101, 54,  224,
    75,  180, 81,  108, 18,  29,  226, 69,  225, 110, 175, 224, 42,  212, 25,  47,  130, 193, 110, 234, 192, 215, 252,
    56,  74,  162, 24,  46,  251, 174, 54,  106, 68,  245, 14,  9,   155, 160, 22,  120, 207, 104, 240, 29,  90,  178,
    140, 28,  24,  220, 47,  166, 112, 61,  251, 208, 192, 111, 56,  239, 238, 93,  255, 251, 62,  99,  32,  193, 75,
    61,  190, 235, 123, 229, 110, 218, 194, 85,  79,  225, 59,  98,  20,  238, 227, 235, 220, 11,  221, 149, 25,  180,
    116, 194, 159, 111, 96,  192, 24,  213, 59,  139, 179, 156, 215, 69,  230, 19,  24,  35,  135, 117, 206, 171, 206,
    162, 67,  129, 234, 61,  235, 11,  104, 103, 84,  64,  223, 167, 254, 40,  163, 101, 92,  84,  43,  150, 46,  249,
    219, 205, 7,   116, 11,  91,  104, 61,  57,  75,  223, 8,   48,  25,  28,  119, 252, 222, 113, 49,  86,  249, 74,
    180, 211, 156, 181, 61,  215, 168, 157, 7,   251, 199, 150, 242, 250, 91,  58,  132, 94,  121, 7,   53,  151, 139,
    98,  6,   165, 153, 69,  214, 32,  110, 211, 100, 101, 31,  89,  45,  81,  98,  23,  205, 205, 197, 209, 109, 186,
    198, 35,  141, 191, 249, 25,  60,  132, 223, 153, 251, 98,  20,  239, 146, 139, 20,  217, 250, 41,  250, 137, 58,
    177, 90,  57,  79,  51,  108, 233, 20,  253, 194, 187, 49,  222, 205, 114, 141, 96,  48,  175, 219, 107, 54,  111,
    138, 22,  154, 103, 108, 79,  58,  252, 179, 178, 79,  164, 195, 2,   153, 36,  39,  170, 199, 201, 167, 197, 85,
    106, 8,   59,  177, 81,  46,  56,  2,   230, 75,  114, 17,  55,  112, 188, 65,  208, 137, 77,  114, 10,  115, 55,
    58,  208, 197, 173, 122, 87,  6,   140, 110, 42,  208, 124, 163, 70,  108, 241, 104, 18,  245, 98,  214, 187, 134,
    53,  42,  221, 22,  182, 133, 211, 116, 148, 177, 194, 209, 192, 85,  90,  199, 58,  55,  203, 2,   229, 19,  137,
    187, 161, 228, 154, 112, 203, 145, 125, 244, 188, 220, 118, 228, 41,  201, 181, 41,  195, 144, 215, 183, 51,  80,
    250, 21,  217, 16,  217, 200, 235, 109, 227, 188, 122, 218, 142, 60,  170, 224, 112, 240, 184, 130, 229, 224, 113,
    5,   223, 148, 163, 80,  165, 183, 130, 187, 132, 116, 64,  238, 161, 85,  220, 115, 139, 205, 98,  227, 244, 29,
    102, 125, 7,   37,  243, 123, 223, 11,  26,  92,  63,  243, 116, 61,  191, 138, 123, 244, 160, 84,  186, 74,  31,
    5,   174, 247, 119, 135, 199, 248, 253, 135, 242, 97,  102, 145, 190, 144, 14,  85,  238, 221, 231, 193, 158, 48,
    205, 25,  120, 248, 15,  220, 29,  158, 9,   70,  185, 30,  103, 229, 33,  254, 23,  237, 160, 172, 62,  193, 90,
    222, 224, 232, 14,  200, 56,  90,  104, 142, 227, 120, 110, 6,   21,  211, 203, 65,  150, 99,  151, 220, 247, 87,
    164, 50,  159, 49,  239, 234, 58,  142, 0,   109, 108, 123, 18,  79,  227, 36,  100, 248, 222, 205, 96,  127, 120,
    26,  171, 228, 69,  63,  36,  17,  252, 200, 17,  116, 242, 187, 227, 88,  143, 247, 2,   75,  191, 6,   130, 59,
    188, 11,  55,  240, 31,  243, 122, 152, 226, 183, 207, 154, 73,  188, 39,  219, 43,  105, 222, 87,  41,  143, 141,
    140, 175, 73,  112, 184, 252, 61,  184, 16,  90,  250, 35,  168, 82,  119, 176, 57,  116, 94,  200, 150, 22,  190,
    179, 44,  104, 12,  235, 84,  149, 102, 252, 89,  154, 193, 99,  228, 106, 242, 125, 248, 64,  194, 255, 223, 127,
    242, 83,  11,  255, 2,   70,  214, 226, 128, 0,   0
};
#endif

void use_gt911(void);

void use_gt911(void)
{
#if EVE_GEN > 2
    EVE_memWrite16(REG_TOUCH_CONFIG, 0x05d0U); /* switch to Goodix touch controller */
#else
    EVE_cs_set();
    spi_transmit((uint8_t) 0xB0U); /* high-byte of REG_CMDB_WRITE + MEM_WRITE */
    spi_transmit((uint8_t) 0x25U); /* middle-byte of REG_CMDB_WRITE */
    spi_transmit((uint8_t) 0x78U); /* low-byte of REG_CMDB_WRITE */
    private_block_write(eve_gt911_data, sizeof(eve_gt911_data));
    EVE_cs_clear();
    EVE_execute_cmd();

    EVE_memWrite8(REG_TOUCH_OVERSAMPLE, 0x0fU); /* setup oversample to 0x0f as "hidden" in binary-blob for AN_336 */
    EVE_memWrite16(REG_TOUCH_CONFIG, 0x05D0U);  /* write magic cookie as requested by AN_336 */

    /* specific to the EVE2 modules from Matrix-Orbital we have to use GPIO3 to reset GT911 */
    EVE_memWrite16(REG_GPIOX_DIR, 0x8008U); /* Reset-Value is 0x8000, adding 0x08 sets GPIO3 to output, default-value
                                              for REG_GPIOX is 0x8000 -> Low output on GPIO3 */
    DELAY_MS(1U);                           /* wait more than 100us */
    EVE_memWrite8(REG_CPURESET, 0U);        /* clear all resets */
    DELAY_MS(110U); /* wait more than 55ms - does not work with multitouch, for some reason a minimum delay of 108ms is
                      required */
    EVE_memWrite16(REG_GPIOX_DIR, 0x8000U); /* setting GPIO3 back to input */
#endif
}
#endif

/**
 * @brief Waits for either reading REG_ID with a value of 0x7c, indicating that
 *  an EVE chip is present and ready to communicate, or untill a timeout of 400ms has passed.
 * @return Returns E_OK in case of success, EVE_FAIL_REGID_TIMEOUT if the
 * value of 0x7c could not be read.
 */
static uint8_t wait_regid(void)
{
    uint8_t ret = EVE_FAIL_REGID_TIMEOUT;
    uint8_t regid = 0U;

    for (uint16_t timeout = 0U; timeout < 400U; timeout++)
    {
        DELAY_MS(1U);

        regid = EVE_memRead8(REG_ID);
        if (0x7cU == regid) /* EVE is up and running */
        {
            ret = E_OK;
            break;
        }
    }

    return (ret);
}

/**
 * @brief Waits for either REG_CPURESET to indicate that the audio, touch and
 * coprocessor units finished their respective reset cycles,
 * or untill a timeout of 50ms has passed.
 * @return Returns E_OK in case of success, EVE_FAIL_RESET_TIMEOUT if either the
 * audio, touch or coprocessor unit indicate a fault by not returning from reset.
 */
static uint8_t wait_reset(void)
{
    uint8_t ret = EVE_FAIL_RESET_TIMEOUT;
    uint8_t reset = 0U;

    for (uint16_t timeout = 0U; timeout < 50U; timeout++)
    {
        DELAY_MS(1U);

        reset = EVE_memRead8(REG_CPURESET) & 7U;
        if (0U == reset) /* EVE reports all units running */
        {
            ret = E_OK;
            break;
        }
    }

    return (ret);
}

/**
 * @brief Writes all parameters defined for the display selected in EVE_config.h.
 * to the corresponding registers.
 * It is used by EVE_init() and can be used to refresh the register values if needed.
 */
void EVE_write_display_parameters(void)
{
    /* Initialize Display */
    EVE_memWrite16(REG_HSIZE, EVE_HSIZE);     /* active display width */
    EVE_memWrite16(REG_HCYCLE, EVE_HCYCLE);   /* total number of clocks per line, incl front/back porch */
    EVE_memWrite16(REG_HOFFSET, EVE_HOFFSET); /* start of active line */
    EVE_memWrite16(REG_HSYNC0, EVE_HSYNC0);   /* start of horizontal sync pulse */
    EVE_memWrite16(REG_HSYNC1, EVE_HSYNC1);   /* end of horizontal sync pulse */
    EVE_memWrite16(REG_VSIZE, EVE_VSIZE);     /* active display height */
    EVE_memWrite16(REG_VCYCLE, EVE_VCYCLE);   /* total number of lines per screen, including pre/post */
    EVE_memWrite16(REG_VOFFSET, EVE_VOFFSET); /* start of active screen */
    EVE_memWrite16(REG_VSYNC0, EVE_VSYNC0);   /* start of vertical sync pulse */
    EVE_memWrite16(REG_VSYNC1, EVE_VSYNC1);   /* end of vertical sync pulse */
    EVE_memWrite8(REG_SWIZZLE, EVE_SWIZZLE);  /* FT8xx output to LCD - pin order */
    EVE_memWrite8(REG_PCLK_POL, EVE_PCLKPOL); /* LCD data is clocked in on this PCLK edge */
    EVE_memWrite8(REG_CSPREAD, EVE_CSPREAD);  /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */

    /* configure Touch */
    EVE_memWrite8(REG_TOUCH_MODE, EVE_TMODE_CONTINUOUS); /* enable touch */
#if defined (EVE_TOUCH_RZTHRESH)
    EVE_memWrite16(REG_TOUCH_RZTHRESH, EVE_TOUCH_RZTHRESH); /* configure the sensitivity of resistive touch */
#else
    EVE_memWrite16(REG_TOUCH_RZTHRESH, 1200U); /* set a reasonable default value if none is given */
#endif

#if defined (EVE_ROTATE)
    EVE_memWrite8(REG_ROTATE, EVE_ROTATE & 7U); /* bit0 = invert, bit2 = portrait, bit3 = mirrored */
    /* reset default value is 0x0 - not inverted, landscape, not mirrored */
#endif
}

static void enable_pixel_clock(void)
{
    EVE_memWrite8(REG_GPIO, 0x80U); /* enable the DISP signal to the LCD panel, it is set to output in REG_GPIO_DIR by default */

#if (EVE_GEN > 3) && (defined EVE_PCLK_FREQ)
    EVE_memWrite16(REG_PCLK_FREQ, (uint16_t) EVE_PCLK_FREQ);

#if defined (EVE_SET_REG_PCLK_2X)
    EVE_memWrite8(REG_PCLK_2X, 1U);
#endif

    EVE_memWrite8(REG_PCLK, 1U); /* enable extsync mode */
#else
    EVE_memWrite8(REG_PCLK, EVE_PCLK); /* start clocking data to the LCD panel */
#endif
}

/**
 * @brief Initializes EVE according to the selected configuration from EVE_config.h.
 * @return E_OK in case of success
 * @note - Has to be executed with the SPI setup to 11 MHz or less as required by FT8xx / BT8xx!
 * @note - Additional settings can be made through extra macros.
 * @note - EVE_TOUCH_RZTHRESH - configure the sensitivity of resistive touch, defaults to 1200.
 * @note - EVE_ROTATE - set the screen rotation: bit0 = invert, bit1 = portrait, bit2 = mirrored.
 * @note - needs a set of calibration values for the selected rotation since this rotates before calibration!
 * @note - EVE_BACKLIGHT_FREQ - configure the backlight frequency, default is not writing it which results in 250Hz.
 * @note - EVE_BACKLIGHT_PWM - configure the backlight pwm, defaults to 0x20 / 25%.
 * @note - EVE_SOFT_RESET - if defined the host command RST_PULSE is send
 */
uint8_t EVE_init(void)
{
    uint8_t ret;

    EVE_pdn_set();
    DELAY_MS(6U); /* minimum time for power-down is 5ms */
    EVE_pdn_clear();
    DELAY_MS(21U); /* minimum time to allow from rising PD_N to first access is 20ms */

#if defined (EVE_SOFT_RESET)
    EVE_cmdWrite(EVE_RST_PULSE,0U); /* reset, only required for warm-start if PowerDown line is not used */
#endif

#if defined (EVE_HAS_CRYSTAL)
    EVE_cmdWrite(EVE_CLKEXT, 0U); /* setup EVE for external clock */
#else
    EVE_cmdWrite(EVE_CLKINT, 0U); /* setup EVE for internal clock */
#endif

#if EVE_GEN > 2
    EVE_cmdWrite(EVE_CLKSEL, 0x86U); /* set clock to 72 MHz */
#endif

    EVE_cmdWrite(EVE_ACTIVE, 0U); /* start EVE */
    DELAY_MS(40U); /* give EVE a moment of silence to power up */

    ret = wait_regid();
    if (E_OK == ret)
    {
        ret = wait_reset();
        if (E_OK == ret)
        {
/* tell EVE that we changed the frequency from default to 72MHz for BT8xx */
#if EVE_GEN > 2
            EVE_memWrite32(REG_FREQUENCY, 72000000UL);
#endif

/* we have a display with a Goodix GT911 / GT9271 touch-controller on it,
 so we patch our FT811 or FT813 according to AN_336 or setup a BT815 / BT817 accordingly */
#if defined (EVE_HAS_GT911)
            use_gt911();
#endif

#if defined (EVE_ADAM101)
            EVE_memWrite8(REG_PWM_DUTY, 0x80U); /* turn off backlight for Glyn ADAM101 module, it uses inverted values */
#else
            EVE_memWrite8(REG_PWM_DUTY, 0U); /* turn off backlight for any other module */
#endif
            EVE_write_display_parameters();

            /* disable Audio for now */
            EVE_memWrite8(REG_VOL_PB, 0U);      /* turn recorded audio volume down, reset-default is 0xff */
            EVE_memWrite8(REG_VOL_SOUND, 0U);   /* turn synthesizer volume down, reset-default is 0xff */
            EVE_memWrite16(REG_SOUND, EVE_MUTE); /* set synthesizer to mute */

            /* write a basic display-list to get things started */
            EVE_memWrite32(EVE_RAM_DL, DL_CLEAR_COLOR_RGB);
            EVE_memWrite32(EVE_RAM_DL + 4U, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
            EVE_memWrite32(EVE_RAM_DL + 8U, DL_DISPLAY); /* end of display list */
            EVE_memWrite32(REG_DLSWAP, EVE_DLSWAP_FRAME);
            /* nothing is being displayed yet... the pixel clock is still 0x00 */

#if defined (EVE_GD3X)
            EVE_memWrite16(REG_OUTBITS,0x01B6U); /* the GD3X is only using 6 bits per color */
#endif

            enable_pixel_clock();

#if defined (EVE_BACKLIGHT_FREQ)
            EVE_memWrite16(REG_PWM_HZ, EVE_BACKLIGHT_FREQ); /* set backlight frequency to configured value */
#endif

#if defined (EVE_BACKLIGHT_PWM)
            EVE_memWrite8(REG_PWM_DUTY, EVE_BACKLIGHT_PWM); /* set backlight pwm to user requested level */
#else
#if defined (EVE_ADAM101)
            EVE_memWrite8(REG_PWM_DUTY, 0x60U); /* turn on backlight pwm to 25% for Glyn ADAM101 module, it uses inverted values */
#else
            EVE_memWrite8(REG_PWM_DUTY, 0x20U); /* turn on backlight pwm to 25% for any other module */
#endif
#endif
            DELAY_MS(1U);
            EVE_execute_cmd(); /* just to be safe, wait for EVE to not be busy */

#if defined (EVE_DMA)
            EVE_init_dma(); /* prepare DMA */
#endif
        }
    }

    return (ret);
}

/* ##################################################################
    functions for display lists
##################################################################### */

/**
 * @brief Begin a sequence of commands or prepare a DMA transfer if applicable.
 * @note - Needs to be used with EVE_end_cmd_burst().
 * @note - Do not use any functions in the sequence that do not address the command-fifo as for example any of EVE_mem...() functions.
 * @note - Do not use any of the functions that do not support burst-mode.
 */
void EVE_start_cmd_burst(void)
{
#if defined (EVE_DMA)
    if (EVE_dma_busy)
    {
        EVE_execute_cmd(); /* this is a safe-guard to protect segmented display-list building with DMA from overlapping */
    }
#endif

    cmd_burst = 42U;

#if defined (EVE_DMA)
    EVE_dma_buffer[0U] = 0x7825B000UL; /* REG_CMDB_WRITE + MEM_WRITE low mid hi 00 */
    EVE_dma_buffer_index = 1U;
#else
    EVE_cs_set();
    spi_transmit((uint8_t) 0xB0U); /* high-byte of REG_CMDB_WRITE + MEM_WRITE */
    spi_transmit((uint8_t) 0x25U); /* middle-byte of REG_CMDB_WRITE */
    spi_transmit((uint8_t) 0x78U); /* low-byte of REG_CMDB_WRITE */
#endif
}

/**
 * @brief End a sequence of commands or trigger a prepared DMA transfer if applicable.
 * @note - Needs to be used with EVE_start_cmd_burst().
 */
void EVE_end_cmd_burst(void)
{
    cmd_burst = 0U;

#if defined (EVE_DMA)
    EVE_start_dma_transfer(); /* begin DMA transfer */
#else
    EVE_cs_clear();
#endif
}

/* write a string to coprocessor memory in context of a command: */
/* no chip-select, just plain SPI-transfers */
static void private_string_write(const char * const p_text)
{
    /* treat the array as bunch of bytes */
    const uint8_t *const p_bytes = (const uint8_t *)p_text;

    if (0U == cmd_burst)
    {
        uint8_t textindex = 0U;
        uint8_t padding;

        /* either leave on Zero or when the string is too long */
        while ((textindex < 249U) && (p_bytes[textindex] != 0U))
        {
            spi_transmit(p_bytes[textindex]);
            textindex++;
        }

        /* transmit at least one 0x00 byte */
        /* and up to four if the string happens to be 4-byte aligned already */
        padding = textindex & 3U; /* 0, 1, 2 or 3 */
        padding = 4U - padding;   /* 4, 3, 2 or 1 */

        while (padding > 0U)
        {
            spi_transmit(0U);
            padding--;
        }
    }
    else /* we are in burst mode, so every transfer is 32 bits */
    {
        uint8_t exit_flag = 0U;

        for (uint8_t textindex = 0U; (textindex < 249U) && (0U == exit_flag); textindex += 4U)
        {
            uint32_t calc = 0U;

            for (uint8_t index = 0U; index < 4U; index++)
            {
                uint8_t data;

                data = p_bytes[textindex + index];

                if (0U == data)
                {
                    exit_flag = 1U; /* leave outer loop */
                    break; /* leave inner loop */
                }
                calc += ((uint32_t)data) << (index * 8U);
            }

            spi_transmit_burst(calc);
        }

        if(0U == exit_flag) /* left outer loop because the string is too long, send zeroes to terminate the string */
        {
            spi_transmit_burst(0U);
        }
    }
}

/* BT817 / BT818 */
#if EVE_GEN > 3

/**
 * @brief Render one frame in RAM_G of an animation.
 */
void EVE_cmd_animframeram(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMFRAMERAM);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(aoptr);
        spi_transmit_32(frame);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMFRAMERAM);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(aoptr);
        spi_transmit_burst(frame);
    }
}

/**
 * @brief Render one frame in RAM_G of an animation, only works in burst-mode.
 */
void EVE_cmd_animframeram_burst(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame)
{
    spi_transmit_burst(CMD_ANIMFRAMERAM);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(aoptr);
    spi_transmit_burst(frame);
}

/**
 * @brief Start an animation in RAM_G.
 */
void EVE_cmd_animstartram(const int32_t chnl, const uint32_t aoptr, const uint32_t loop)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMSTARTRAM);
        spi_transmit_32(i32_to_u32(chnl));
        spi_transmit_32(aoptr);
        spi_transmit_32(loop);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMSTARTRAM);
        spi_transmit_burst(i32_to_u32(chnl));
        spi_transmit_burst(aoptr);
        spi_transmit_burst(loop);
    }
}

/**
 * @brief Start an animation in RAM_G, only works in burst-mode.
 */
void EVE_cmd_animstartram_burst(const int32_t chnl, const uint32_t aoptr, const uint32_t loop)
{
    spi_transmit_burst(CMD_ANIMSTARTRAM);
    spi_transmit_burst(i32_to_u32(chnl));
    spi_transmit_burst(aoptr);
    spi_transmit_burst(loop);
}

/**
 * @brief Sets the API level used by the coprocessor.
 */
void EVE_cmd_apilevel(const uint32_t level)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_APILEVEL);
        spi_transmit_32(level);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_APILEVEL);
        spi_transmit_burst(level);
    }
}

/**
 * @brief Sets the API level used by the coprocessor, only works in burst-mode.
 */
void EVE_cmd_apilevel_burst(const uint32_t level)
{
    spi_transmit_burst(CMD_APILEVEL);
    spi_transmit_burst(level);
}

/**
 * @brief Execute the touch screen calibration routine for a sub-window.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_calibratesub(const uint16_t xc0, const uint16_t yc0, const uint16_t width, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CALIBRATESUB);
        spi_transmit_32(u16_u16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(width, height));
        EVE_cs_clear();
    }
}

/**
 * @brief Calls a command list in RAM_G.
 */
void EVE_cmd_calllist(const uint32_t adr)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CALLLIST);
        spi_transmit_32(adr);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_CALLLIST);
        spi_transmit_burst(adr);
    }
}

/**
 * @brief Calls a command list in RAM_G, only works in burst-mode.
 */
void EVE_cmd_calllist_burst(const uint32_t adr)
{
    spi_transmit_burst(CMD_CALLLIST);
    spi_transmit_burst(adr);
}

/**
 * @brief Indicate the end of a command list.
 */
void EVE_cmd_return(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_RETURN);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_RETURN);
    }
}

/**
 * @brief Indicate the end of a command list, only works in burst-mode.
 */
void EVE_cmd_return_burst(void)
{
    spi_transmit_burst(CMD_RETURN);
}

/**
 * @brief Setup the Horizontal Scan out Filter for non-square pixel LCD support.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_hsf(const uint32_t hsf)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_HSF);
        spi_transmit_32(hsf);
        EVE_cs_clear();
    }
}

/**
 * @brief Play/run animations until complete.
 */
void EVE_cmd_runanim(const uint32_t waitmask, const uint32_t play)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_RUNANIM);
        spi_transmit_32(waitmask);
        spi_transmit_32(play);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_RUNANIM);
        spi_transmit_burst(waitmask);
        spi_transmit_burst(play);
    }
}

/**
 * @brief Play/run animations until complete, only works in burst-mode.
 */
void EVE_cmd_runanim_burst(const uint32_t waitmask, const uint32_t play)
{
    spi_transmit_burst(CMD_RUNANIM);
    spi_transmit_burst(waitmask);
    spi_transmit_burst(play);
}

#endif /* EVE_GEN > 3 */

/* BT815 / BT816 */
#if EVE_GEN > 2

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */



/* ##################################################################
    functions for display lists
##################################################################### */

/**
 * @brief Draw one or more active animations.
 */
void EVE_cmd_animdraw(const int32_t chnl)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMDRAW);
        spi_transmit_32(i32_to_u32(chnl));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMDRAW);
        spi_transmit_burst(i32_to_u32(chnl));
    }
}

/**
 * @brief Draw one or more active animations, only works in burst-mode.
 */
void EVE_cmd_animdraw_burst(const int32_t chnl)
{
    spi_transmit_burst(CMD_ANIMDRAW);
    spi_transmit_burst(i32_to_u32(chnl));
}

/**
 * @brief Draw the specified frame of an animation.
 */
void EVE_cmd_animframe(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMFRAME);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(aoptr);
        spi_transmit_32(frame);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMFRAME);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(aoptr);
        spi_transmit_burst(frame);
    }
}

/**
 * @brief Draw the specified frame of an animation, only works in burst-mode.
 */
void EVE_cmd_animframe_burst(const int16_t xc0, const int16_t yc0, const uint32_t aoptr, const uint32_t frame)
{
    spi_transmit_burst(CMD_ANIMFRAME);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(aoptr);
    spi_transmit_burst(frame);
}

/**
 * @brief Start an animation.
 */
void EVE_cmd_animstart(const int32_t chnl, const uint32_t aoptr, const uint32_t loop)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMSTART);
        spi_transmit_32(i32_to_u32(chnl));
        spi_transmit_32(aoptr);
        spi_transmit_32(loop);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMSTART);
        spi_transmit_burst(i32_to_u32(chnl));
        spi_transmit_burst(aoptr);
        spi_transmit_burst(loop);
    }
}

/**
 * @brief Start an animation, only works in burst-mode.
 */
void EVE_cmd_animstart_burst(const int32_t chnl, const uint32_t aoptr, const uint32_t loop)
{
    spi_transmit_burst(CMD_ANIMSTART);
    spi_transmit_burst(i32_to_u32(chnl));
    spi_transmit_burst(aoptr);
    spi_transmit_burst(loop);
}

/**
 * @brief Stops one or more active animations.
 */
void EVE_cmd_animstop(const int32_t chnl)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMSTOP);
        spi_transmit_32(i32_to_u32(chnl));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMSTOP);
        spi_transmit_burst(i32_to_u32(chnl));
    }
}

/**
 * @brief Stops one or more active animations, only works in burst-mode.
 */
void EVE_cmd_animstop_burst(const int32_t chnl)
{
    spi_transmit_burst(CMD_ANIMSTOP);
    spi_transmit_burst(i32_to_u32(chnl));
}

/**
 * @brief Sets the coordinates of an animation.
 */
void EVE_cmd_animxy(const int32_t chnl, const int16_t xc0, const int16_t yc0)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ANIMXY);
        spi_transmit_32(i32_to_u32(chnl));
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ANIMXY);
        spi_transmit_burst(i32_to_u32(chnl));
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    }
}

/**
 * @brief Sets the coordinates of an animation, only works in burst-mode.
 */
void EVE_cmd_animxy_burst(const int32_t chnl, const int16_t xc0, const int16_t yc0)
{
    spi_transmit_burst(CMD_ANIMXY);
    spi_transmit_burst(i32_to_u32(chnl));
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
}

/**
 * @brief Append flash data to the display list.
 */
void EVE_cmd_appendf(const uint32_t ptr, const uint32_t num)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_APPENDF);
        spi_transmit_32(ptr);
        spi_transmit_32(num);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_APPENDF);
        spi_transmit_burst(ptr);
        spi_transmit_burst(num);
    }
}

/**
 * @brief Append flash data to the display list, only works in burst-mode.
 */
void EVE_cmd_appendf_burst(const uint32_t ptr, const uint32_t num)
{
    spi_transmit_burst(CMD_APPENDF);
    spi_transmit_burst(ptr);
    spi_transmit_burst(num);
}

/**
 * @brief Computes a bitmap transform and appends commands BITMAP_TRANSFORM_A...BITMAP_TRANSFORM_F to the display list.
 */
uint16_t EVE_cmd_bitmap_transform(const int32_t xc0, const int32_t yc0, const int32_t xc1,
                                    const int32_t yc1, const int32_t xc2, const int32_t yc2,
                                    const int32_t tx0, const int32_t ty0, const int32_t tx1,
                                    const int32_t ty1, const int32_t tx2,const int32_t ty2)
{
    uint16_t ret_val = 0U;

    if (0U == cmd_burst)
    {
        uint16_t cmdoffset;

        eve_begin_cmd(CMD_BITMAP_TRANSFORM);
        spi_transmit_32(i32_to_u32(xc0));
        spi_transmit_32(i32_to_u32(yc0));
        spi_transmit_32(i32_to_u32(xc1));
        spi_transmit_32(i32_to_u32(yc1));
        spi_transmit_32(i32_to_u32(xc2));
        spi_transmit_32(i32_to_u32(yc2));
        spi_transmit_32(i32_to_u32(tx0));
        spi_transmit_32(i32_to_u32(ty0));
        spi_transmit_32(i32_to_u32(tx1));
        spi_transmit_32(i32_to_u32(ty1));
        spi_transmit_32(i32_to_u32(tx2));
        spi_transmit_32(i32_to_u32(ty2));
        spi_transmit_32(0UL);
        EVE_cs_clear();
        EVE_execute_cmd();
        cmdoffset = EVE_memRead16(REG_CMD_WRITE);
        cmdoffset -= 4U;
        cmdoffset &= 0x0fffU;
        ret_val = (uint16_t) EVE_memRead32(EVE_RAM_CMD + cmdoffset);
    }
    else /* note: the result parameter is ignored in burst mode */
    {
        spi_transmit_burst(CMD_BITMAP_TRANSFORM);
        spi_transmit_burst(i32_to_u32(xc0));
        spi_transmit_burst(i32_to_u32(yc0));
        spi_transmit_burst(i32_to_u32(xc1));
        spi_transmit_burst(i32_to_u32(yc1));
        spi_transmit_burst(i32_to_u32(xc2));
        spi_transmit_burst(i32_to_u32(yc2));
        spi_transmit_burst(i32_to_u32(tx0));
        spi_transmit_burst(i32_to_u32(ty0));
        spi_transmit_burst(i32_to_u32(tx1));
        spi_transmit_burst(i32_to_u32(ty1));
        spi_transmit_burst(i32_to_u32(tx2));
        spi_transmit_burst(i32_to_u32(ty2));
        spi_transmit_burst(0UL);
    }
    return (ret_val);
}

/**
 * @brief Computes a bitmap transform and appends commands BITMAP_TRANSFORM_A...BITMAP_TRANSFORM_F to the display list.
 * @note - Only works in burst-mode, the result parameter is ignored.
 */
void EVE_cmd_bitmap_transform_burst(const int32_t xc0, const int32_t yc0, const int32_t xc1,
                                    const int32_t yc1, const int32_t xc2, const int32_t yc2,
                                    const int32_t tx0, const int32_t ty0, const int32_t tx1,
                                    const int32_t ty1, const int32_t tx2, const int32_t ty2)
{
    spi_transmit_burst(CMD_BITMAP_TRANSFORM);
    spi_transmit_burst(i32_to_u32(xc0));
    spi_transmit_burst(i32_to_u32(yc0));
    spi_transmit_burst(i32_to_u32(xc1));
    spi_transmit_burst(i32_to_u32(yc1));
    spi_transmit_burst(i32_to_u32(xc2));
    spi_transmit_burst(i32_to_u32(yc2));
    spi_transmit_burst(i32_to_u32(tx0));
    spi_transmit_burst(i32_to_u32(ty0));
    spi_transmit_burst(i32_to_u32(tx1));
    spi_transmit_burst(i32_to_u32(ty1));
    spi_transmit_burst(i32_to_u32(tx2));
    spi_transmit_burst(i32_to_u32(ty2));
    spi_transmit_burst(0UL);
}

/**
 * @brief Sets the pixel fill width for CMD_TEXT,CMD_BUTTON,CMD_BUTTON with the OPT_FILL option.
 */
void EVE_cmd_fillwidth(const uint32_t pixel)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_FILLWIDTH);
        spi_transmit_32(pixel);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FILLWIDTH);
        spi_transmit_burst(pixel);
    }
}

/**
 * @brief Sets the pixel fill width for CMD_TEXT,CMD_BUTTON,CMD_BUTTON with the OPT_FILL option.
 * @note - Only works in burst-mode.
 */
void EVE_cmd_fillwidth_burst(const uint32_t pixel)
{
    spi_transmit_burst(CMD_FILLWIDTH);
    spi_transmit_burst(pixel);
}

/**
 * @brief Draw a smooth color gradient with transparency.
 */
void EVE_cmd_gradienta(const int16_t xc0, const int16_t yc0, const uint32_t argb0, const int16_t xc1, const int16_t yc1, const uint32_t argb1)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GRADIENTA);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(argb0);
        spi_transmit_32(i16_i16_to_u32(xc1, yc1));
        spi_transmit_32(argb1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRADIENTA);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(argb0);
        spi_transmit_burst(i16_i16_to_u32(xc1, yc1));
        spi_transmit_burst(argb1);
    }
}

/**
 * @brief Draw a smooth color gradient with transparency, only works in burst-mode.
 */
void EVE_cmd_gradienta_burst(const int16_t xc0, const int16_t yc0, const uint32_t argb0, const int16_t xc1, const int16_t yc1, const uint32_t argb1)
{
    spi_transmit_burst(CMD_GRADIENTA);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(argb0);
    spi_transmit_burst(i16_i16_to_u32(xc1, yc1));
    spi_transmit_burst(argb1);
}

/**
 * @brief Apply a rotation and scale around a specified coordinate.
 */
void EVE_cmd_rotatearound(const int32_t xc0, const int32_t yc0, const uint32_t angle, const int32_t scale)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ROTATEAROUND);
        spi_transmit_32(i32_to_u32(xc0));
        spi_transmit_32(i32_to_u32(yc0));
        spi_transmit_32(angle & 0xFFFFUL);
        spi_transmit_32(i32_to_u32(scale));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ROTATEAROUND);
        spi_transmit_burst(i32_to_u32(xc0));
        spi_transmit_burst(i32_to_u32(yc0));
        spi_transmit_burst(angle & 0xFFFFUL);
        spi_transmit_burst(i32_to_u32(scale));
    }
}

/**
 * @brief Apply a rotation and scale around a specified coordinate, only works in burst-mode.
 */
void EVE_cmd_rotatearound_burst(const int32_t xc0, const int32_t yc0, const uint32_t angle, const int32_t scale)
{
    spi_transmit_burst(CMD_ROTATEAROUND);
    spi_transmit_burst(i32_to_u32(xc0));
    spi_transmit_burst(i32_to_u32(yc0));
    spi_transmit_burst(angle & 0xFFFFUL);
    spi_transmit_burst(i32_to_u32(scale));
}

/**
 * @brief Draw a button with a label, varargs version.
 * @param p_arguments[] pointer to an array of values converted to uint32_t to be used when using EVE_OPT_FORMAT
 * @param num_args the number of elements provided in p_arguments[]
 */
void EVE_cmd_button_var(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t font, const uint16_t options, const char * const p_text,
                        const uint8_t num_args, const uint32_t * const p_arguments)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_BUTTON);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);

        if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_32(p_arguments[counter]);
                }
            }
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BUTTON);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);

        if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_burst(p_arguments[counter]);
                }
            }
        }
    }
}

/**
 * @brief Draw a button with a label, varargs version, only works in burst-mode.
 * @param p_arguments[] pointer to an array of values converted to uint32_t to be used when using EVE_OPT_FORMAT
 * @param num_args the number of elements provided in p_arguments[]
 */
void EVE_cmd_button_var_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                              const uint16_t font, const uint16_t options, const char * const p_text,
                              const uint8_t num_args, const uint32_t * const p_arguments)
{
    spi_transmit_burst(CMD_BUTTON);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);

    if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
    {
        if (p_arguments != NULL)
        {
            for (uint8_t counter = 0U; counter < num_args; counter++)
            {
                spi_transmit_burst(p_arguments[counter]);
            }
        }
    }
}

/**
 * @brief Draw a text string, varargs version.
 * @param p_arguments[] pointer to an array of values converted to uint32_t to be used when using EVE_OPT_FORMAT
 * @param num_args the number of elements provided in p_arguments[]
 */
void EVE_cmd_text_var(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options,
                        const char * const p_text, const uint8_t num_args, const uint32_t * const p_arguments)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TEXT);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);

        if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_32(p_arguments[counter]);
                }
            }
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXT);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);

        if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_burst(p_arguments[counter]);
                }
            }
        }
    }
}

/**
 * @brief Draw a text string, varargs version.
 * @param p_arguments[] pointer to an array of values converted to uint32_t to be used when using EVE_OPT_FORMAT
 * @param num_args the number of elements provided in p_arguments[]
 */
void EVE_cmd_text_var_burst(const int16_t xc0, const int16_t yc0, const uint16_t font,
                            const uint16_t options, const char * const p_text,
                            const uint8_t num_args, const uint32_t * const p_arguments)
{
    spi_transmit_burst(CMD_TEXT);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);

    if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
    {
        if (p_arguments != NULL)
        {
            for (uint8_t counter = 0U; counter < num_args; counter++)
            {
                spi_transmit_burst(p_arguments[counter]);
            }
        }
    }
}

/**
 * @brief Draw a toggle switch with labels, varargs version.
 * @param p_arguments[] pointer to an array of values converted to uint32_t to be used when using EVE_OPT_FORMAT
 * @param num_args the number of elements provided in p_arguments[]
 */
void EVE_cmd_toggle_var(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                        const uint16_t options, const uint16_t state, const char * const p_text,
                        const uint8_t num_args, const uint32_t * const p_arguments)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TOGGLE);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, font));
        spi_transmit_32(u16_u16_to_u32(options, state));
        private_string_write(p_text);

        if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_32(p_arguments[counter]);
                }
            }
        }
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOGGLE);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, font));
        spi_transmit_burst(u16_u16_to_u32(options, state));
        private_string_write(p_text);

        if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
        {
            if (p_arguments != NULL)
            {
                for (uint8_t counter = 0U; counter < num_args; counter++)
                {
                    spi_transmit_burst(p_arguments[counter]);
                }
            }
        }
    }
}

/**
 * @brief Draw a toggle switch with labels, varargs version, only works in burst-mode.
 * @param p_arguments[] pointer to an array of values converted to uint32_t to be used when using EVE_OPT_FORMAT
 * @param num_args the number of elements provided in p_arguments[]
 */
void EVE_cmd_toggle_var_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                            const uint16_t options, const uint16_t state, const char * const p_text,
                            const uint8_t num_args, const uint32_t * const p_arguments)
{
    spi_transmit_burst(CMD_TOGGLE);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, font));
    spi_transmit_burst(u16_u16_to_u32(options, state));
    private_string_write(p_text);

    if (((uint16_t) (options & EVE_OPT_FORMAT)) != 0U)
    {
        if (p_arguments != NULL)
        {
            for (uint8_t counter = 0U; counter < num_args; counter++)
            {
                spi_transmit_burst(p_arguments[counter]);
            }
        }
    }
}

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */

/**
 * @brief Specify the extended format of the bitmap.
 */
void EVE_bitmap_ext_format(const uint16_t format)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_EXT_FORMAT(format));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_EXT_FORMAT(format));
    }
}

/**
 * @brief Specify the extended format of the bitmap, only works in burst-mode.
 */
void EVE_bitmap_ext_format_burst(const uint16_t format)
{
    spi_transmit_burst(BITMAP_EXT_FORMAT(format));
}

/**
 * @brief Set the source for the red, green, blue and alpha channels of a bitmap.
 */
void EVE_bitmap_swizzle(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_SWIZZLE(red, green, blue, alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SWIZZLE(red, green, blue, alpha));
    }
}

/**
 * @brief Set the source for the red, green, blue and alpha channels of a bitmap, only works in burst-mode.
 */
void EVE_bitmap_swizzle_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    spi_transmit_burst(BITMAP_SWIZZLE(red, green, blue, alpha));
}

#endif /* EVE_GEN > 2 */

/* ##################################################################
    functions for display lists
##################################################################### */

/**
 * @brief Appends commands from RAM_G to the display list.
 */
void EVE_cmd_append(const uint32_t ptr, const uint32_t num)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_APPEND);
        spi_transmit_32(ptr);
        spi_transmit_32(num);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_APPEND);
        spi_transmit_burst(ptr);
        spi_transmit_burst(num);
    }
}

/**
 * @brief Appends commands from RAM_G to the display list, only works in burst-mode.
 */
void EVE_cmd_append_burst(const uint32_t ptr, const uint32_t num)
{
    spi_transmit_burst(CMD_APPEND);
    spi_transmit_burst(ptr);
    spi_transmit_burst(num);
}

/**
 * @brief Set the background color.
 */
void EVE_cmd_bgcolor(const uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_BGCOLOR);
        spi_transmit_32(color);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BGCOLOR);
        spi_transmit_burst(color);
    }
}

/**
 * @brief Set the background color, only works in burst-mode.
 */
void EVE_cmd_bgcolor_burst(const uint32_t color)
{
    spi_transmit_burst(CMD_BGCOLOR);
    spi_transmit_burst(color);
}

/**
 * @brief Draw a button with a label.
 */
void EVE_cmd_button(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint16_t font, const uint16_t options, const char * const p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_BUTTON);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_BUTTON);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);
    }
}

/**
 * @brief Draw a button with a label, only works in burst-mode.
 */
void EVE_cmd_button_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint16_t font, const uint16_t options, const char * const p_text)
{
    spi_transmit_burst(CMD_BUTTON);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);
}

/**
 * @brief Execute the touch screen calibration routine.
 * @note - does not support burst-mode
 */
void EVE_cmd_calibrate(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CALIBRATE);
        spi_transmit_32(0UL);
        EVE_cs_clear();
    }
}

/**
 * @brief Draw an analog clock.
 */
void EVE_cmd_clock(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                    const uint16_t hours, const uint16_t mins, const uint16_t secs, const uint16_t msecs)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_CLOCK);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(rad, options));
        spi_transmit_32(u16_u16_to_u32(hours, mins));
        spi_transmit_32(u16_u16_to_u32(secs, msecs));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_CLOCK);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(rad, options));
        spi_transmit_burst(u16_u16_to_u32(hours, mins));
        spi_transmit_burst(u16_u16_to_u32(secs, msecs));
    }
}

/**
 * @brief Draw an analog clock, only works in burst-mode.
 */
void EVE_cmd_clock_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                            const uint16_t hours, const uint16_t mins, const uint16_t secs, const uint16_t msecs)
{
    spi_transmit_burst(CMD_CLOCK);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(rad, options));
    spi_transmit_burst(u16_u16_to_u32(hours, mins));
    spi_transmit_burst(u16_u16_to_u32(secs, msecs));
}

/**
 * @brief Draw a rotary dial control.
 */
void EVE_cmd_dial(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options, const uint16_t val)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_DIAL);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(rad, options));
        spi_transmit_32(u16_u16_to_u32(val, 0x0000));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_DIAL);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(rad, options));
        spi_transmit_burst(u16_u16_to_u32(val, 0x0000));
    }
}

/**
 * @brief Draw a rotary dial control, only works in burst-mode.
 */
void EVE_cmd_dial_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options, const uint16_t val)
{
    spi_transmit_burst(CMD_DIAL);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(rad, options));
    spi_transmit_burst(u16_u16_to_u32(val, 0x0000));
}

/**
 * @brief Start a new display list.
 */
void EVE_cmd_dlstart(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_DLSTART);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_DLSTART);
    }
}

/**
 * @brief Start a new display list, only works in burst-mode.
 */
void EVE_cmd_dlstart_burst(void)
{
    spi_transmit_burst(CMD_DLSTART);
}

/**
 * @brief Set the foreground color.
 */
void EVE_cmd_fgcolor(const uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_FGCOLOR);
        spi_transmit_32(color);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_FGCOLOR);
        spi_transmit_burst(color);
    }
}

/**
 * @brief Set the foreground color, only works in burst-mode.
 */
void EVE_cmd_fgcolor_burst(const uint32_t color)
{
    spi_transmit_burst(CMD_FGCOLOR);
    spi_transmit_burst(color);
}

/**
 * @brief Draw a gauge.
 */
void EVE_cmd_gauge(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                    const uint16_t major, const uint16_t minor, const uint16_t val, const uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GAUGE);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(rad, options));
        spi_transmit_32(u16_u16_to_u32(major, minor));
        spi_transmit_32(u16_u16_to_u32(val, range));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GAUGE);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(rad, options));
        spi_transmit_burst(u16_u16_to_u32(major, minor));
        spi_transmit_burst(u16_u16_to_u32(val, range));
    }
}

/**
 * @brief Draw a gauge, only works in burst-mode.
 */
void EVE_cmd_gauge_burst(const int16_t xc0, const int16_t yc0, const uint16_t rad, const uint16_t options,
                            const uint16_t major, const uint16_t minor, const uint16_t val, const uint16_t range)
{
    spi_transmit_burst(CMD_GAUGE);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(rad, options));
    spi_transmit_burst(u16_u16_to_u32(major, minor));
    spi_transmit_burst(u16_u16_to_u32(val, range));
}

/**
 * @brief Retrieves the current matrix within the context of the coprocessor engine.
 * @note - waits for completion and reads values from RAM_CMD after completion
 * @note - can not be used with cmd-burst
 */
void EVE_cmd_getmatrix(int32_t * const p_a, int32_t * const p_b, int32_t * const p_c, int32_t * const p_d, int32_t * const p_e, int32_t * const p_f)
{
    if (0U == cmd_burst)
    {
        uint16_t cmdoffset;
        uint32_t address;

        eve_begin_cmd(CMD_GETMATRIX);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        spi_transmit_32(0UL);
        EVE_cs_clear();
        EVE_execute_cmd();
        cmdoffset = EVE_memRead16(REG_CMD_WRITE);

        if (p_f != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL);
            *p_f = (int32_t) EVE_memRead32(address);
        }
        if (p_e != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL);
            *p_e = (int32_t) EVE_memRead32(address);
        }
        if (p_d != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL);
            *p_d = (int32_t) EVE_memRead32(address);
        }
        if (p_c != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 16UL) & 0xfffUL);
            *p_c = (int32_t) EVE_memRead32(address);
        }
        if (p_b != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 20UL) & 0xfffUL);
            *p_b = (int32_t) EVE_memRead32(address);
        }
        if (p_a != NULL)
        {
            address = EVE_RAM_CMD + ((cmdoffset - 24UL) & 0xfffUL);
            *p_a = (int32_t) EVE_memRead32(address);
        }
    }
}

/**
 * @brief Set up the highlight color used in 3D effects for CMD_BUTTON and CMD_KEYS.
 */
void EVE_cmd_gradcolor(const uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GRADCOLOR);
        spi_transmit_32(color);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRADCOLOR);
        spi_transmit_burst(color);
    }
}

/**
 * @brief Set up the highlight color used in 3D effects for CMD_BUTTON and CMD_KEYS, only works in burst-mode.
 */
void EVE_cmd_gradcolor_burst(const uint32_t color)
{
    spi_transmit_burst(CMD_GRADCOLOR);
    spi_transmit_burst(color);
}

/**
 * @brief Draw a smooth color gradient.
 */
void EVE_cmd_gradient(const int16_t xc0, const int16_t yc0, const uint32_t rgb0, const int16_t xc1, const int16_t yc1, const uint32_t rgb1)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_GRADIENT);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(rgb0);
        spi_transmit_32(i16_i16_to_u32(xc1, yc1));
        spi_transmit_32(rgb1);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_GRADIENT);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(rgb0);
        spi_transmit_burst(i16_i16_to_u32(xc1, yc1));
        spi_transmit_burst(rgb1);
    }
}

/**
 * @brief Draw a smooth color gradient, only works in burst-mode.
 */
void EVE_cmd_gradient_burst(const int16_t xc0, const int16_t yc0, const uint32_t rgb0, const int16_t xc1, const int16_t yc1, const uint32_t rgb1)
{
    spi_transmit_burst(CMD_GRADIENT);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(rgb0);
    spi_transmit_burst(i16_i16_to_u32(xc1, yc1));
    spi_transmit_burst(rgb1);
}

/**
 * @brief Draw a row of key buttons with labels.
 * @note - The tag value of each button is set to the ASCII value of its label.
 * @note - Does not work with UTF-8.
 */
void EVE_cmd_keys(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint16_t font, const uint16_t options, const char * const p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_KEYS);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_KEYS);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);
    }
}

/**
 * @brief Draw a row of key buttons with labels, only works in burst-mode.
 * @note - The tag value of each button is set to the ASCII value of its label.
 * @note - Does not work with UTF-8.
 */
void EVE_cmd_keys_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t font, const uint16_t options, const char * const p_text)
{
    spi_transmit_burst(CMD_KEYS);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);
}

/**
 * @brief Set the current matrix to the identity matrix.
 */
void EVE_cmd_loadidentity(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_LOADIDENTITY);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_LOADIDENTITY);
    }
}

/**
 * @brief Set the current matrix to the identity matrix, only works in burst-mode.
 */
void EVE_cmd_loadidentity_burst(void)
{
    spi_transmit_burst(CMD_LOADIDENTITY);
}

/**
 * @brief Draw a number.
 */
void EVE_cmd_number(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const int32_t number)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_NUMBER);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(font, options));
        spi_transmit_32(i32_to_u32(number));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_NUMBER);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        spi_transmit_burst(i32_to_u32(number));
    }
}

/**
 * @brief Draw a number, only works in burst-mode.
 */
void EVE_cmd_number_burst(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const int32_t number)
{
    spi_transmit_burst(CMD_NUMBER);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    spi_transmit_burst(i32_to_u32(number));
}

/**
 * @brief Draw a progress bar.
 */
void EVE_cmd_progress(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t options, const uint16_t val, const uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_PROGRESS);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(options, val));
        spi_transmit_32(u16_u16_to_u32(range, 0x0000)); /* dummy word for 4-byte alignment */
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_PROGRESS);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(options, val));
        spi_transmit_burst(u16_u16_to_u32(range, 0x0000));
    }
}

/**
 * @brief Draw a progress bar, only works in burst-mode.
 */
void EVE_cmd_progress_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint16_t options, const uint16_t val, const uint16_t range)
{
    spi_transmit_burst(CMD_PROGRESS);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(options, val));
    spi_transmit_burst(u16_u16_to_u32(range, 0x0000));
}

/**
 * @brief Load a ROM font into bitmap handle.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_romfont(const uint32_t font, const uint32_t romslot)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ROMFONT);
        spi_transmit_32(font);
        spi_transmit_32(romslot);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ROMFONT);
        spi_transmit_burst(font);
        spi_transmit_burst(romslot);
    }
}

/**
 * @brief Load a ROM font into bitmap handle, only works in burst-mode.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_romfont_burst(const uint32_t font, const uint32_t romslot)
{
    spi_transmit_burst(CMD_ROMFONT);
    spi_transmit_burst(font);
    spi_transmit_burst(romslot);
}

/**
 * @brief Apply a rotation to the current matrix.
 * @note - range for angle is 0...65535
 */
void EVE_cmd_rotate(const uint32_t angle)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_ROTATE);
        spi_transmit_32(angle & 0xFFFFUL);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_ROTATE);
        spi_transmit_burst(angle & 0xFFFFUL);
    }
}

/**
 * @brief Apply a rotation to the current matrix, only works in burst-mode.
 */
void EVE_cmd_rotate_burst(const uint32_t angle)
{
    spi_transmit_burst(CMD_ROTATE);
    spi_transmit_burst(angle & 0xFFFFUL);
}

/**
 * @brief Apply a scale to the current matrix.
 */
void EVE_cmd_scale(const int32_t scx, const int32_t scy)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SCALE);
        spi_transmit_32(i32_to_u32(scx));
        spi_transmit_32(i32_to_u32(scy));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SCALE);
        spi_transmit_burst(i32_to_u32(scx));
        spi_transmit_burst(i32_to_u32(scy));
    }
}

/**
 * @brief Apply a scale to the current matrix, only works in burst-mode.
 */
void EVE_cmd_scale_burst(const int32_t scx, const int32_t scy)
{
    spi_transmit_burst(CMD_SCALE);
    spi_transmit_burst(i32_to_u32(scx));
    spi_transmit_burst(i32_to_u32(scy));
}

/**
 * @brief Start an animated screensaver.
 */
void EVE_cmd_screensaver(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SCREENSAVER);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SCREENSAVER);
    }
}

/**
 * @brief Start an animated screensaver, only works in burst-mode.
 */
void EVE_cmd_screensaver_burst(void)
{
    spi_transmit_burst(CMD_SCREENSAVER);
}

/**
 * @brief Draw a scroll bar.
 */
void EVE_cmd_scrollbar(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                        const uint16_t options, const uint16_t val, const uint16_t size, const uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SCROLLBAR);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(options, val));
        spi_transmit_32(u16_u16_to_u32(size, range));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SCROLLBAR);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(options, val));
        spi_transmit_burst(u16_u16_to_u32(size, range));
    }
}

/**
 * @brief Draw a scroll bar, only works in burst-mode.
 */
void EVE_cmd_scrollbar_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                                const uint16_t options, const uint16_t val, const uint16_t size, const uint16_t range)
{
    spi_transmit_burst(CMD_SCROLLBAR);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(options, val));
    spi_transmit_burst(u16_u16_to_u32(size, range));
}

/**
 * @brief Set the base for number output.
 */
void EVE_cmd_setbase(const uint32_t base)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETBASE);
        spi_transmit_32(base);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETBASE);
        spi_transmit_burst(base);
    }
}

/**
 * @brief Set the base for number output, only works in burst-mode.
 */
void EVE_cmd_setbase_burst(const uint32_t base)
{
    spi_transmit_burst(CMD_SETBASE);
    spi_transmit_burst(base);
}

/**
 * @brief Generate the corresponding display list commands for given bitmap information.
 * @note - when used with ASTC images from flash, the source-address is a block of 32 bytes
 */
void EVE_cmd_setbitmap(const uint32_t addr, const uint16_t fmt, const uint16_t width, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETBITMAP);
        spi_transmit_32(addr);
        spi_transmit_32(u16_u16_to_u32(fmt, width));
        spi_transmit_32(u16_u16_to_u32(height, 0x0000));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETBITMAP);
        spi_transmit_burst(addr);
        spi_transmit_burst(u16_u16_to_u32(fmt, width));
        spi_transmit_burst(u16_u16_to_u32(height, 0x0000));
    }
}

/**
 * @brief Generate the corresponding display list commands for given bitmap information, only works in burst-mode.
 * @note - when used with ASTC images from flash, the source-address is a block of 32 bytes
 */
void EVE_cmd_setbitmap_burst(const uint32_t addr, const uint16_t fmt, const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(CMD_SETBITMAP);
    spi_transmit_burst(addr);
    spi_transmit_burst(u16_u16_to_u32(fmt, width));
    spi_transmit_burst(u16_u16_to_u32(height, 0x0000));
}

/**
 * @brief Register one custom font into the coprocessor engine.
 * @note - does not set up the bitmap parameters of the font
 */
void EVE_cmd_setfont(const uint32_t font, const uint32_t ptr)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETFONT);
        spi_transmit_32(font);
        spi_transmit_32(ptr);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETFONT);
        spi_transmit_burst(font);
        spi_transmit_burst(ptr);
    }
}

/**
 * @brief Register one custom font into the coprocessor engine, only works in burst-mode.
 * @note - does not set up the bitmap parameters of the font
 */
void EVE_cmd_setfont_burst(const uint32_t font, const uint32_t ptr)
{
    spi_transmit_burst(CMD_SETFONT);
    spi_transmit_burst(font);
    spi_transmit_burst(ptr);
}

/**
 * @brief Set up a custom font for use by the coprocessor engine.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_setfont2(const uint32_t font, const uint32_t ptr, const uint32_t firstchar)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETFONT2);
        spi_transmit_32(font);
        spi_transmit_32(ptr);
        spi_transmit_32(firstchar);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETFONT2);
        spi_transmit_burst(font);
        spi_transmit_burst(ptr);
        spi_transmit_burst(firstchar);
    }
}

/**
 * @brief Set up a custom font for use by the coprocessor engine, only works in burst-mode.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_setfont2_burst(const uint32_t font, const uint32_t ptr, const uint32_t firstchar)
{
    spi_transmit_burst(CMD_SETFONT2);
    spi_transmit_burst(font);
    spi_transmit_burst(ptr);
    spi_transmit_burst(firstchar);
}

/**
 * @brief Assign the value of the current matrix to the bitmap transform matrix.
 */
void EVE_cmd_setmatrix(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETMATRIX);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETMATRIX);
    }
}

/**
 * @brief Assign the value of the current matrix to the bitmap transform matrix, only works in burst-mode.
 */
void EVE_cmd_setmatrix_burst(void)
{
    spi_transmit_burst(CMD_SETMATRIX);
}

/**
 * @brief Set the scratch bitmap for widget use.
 */
void EVE_cmd_setscratch(const uint32_t handle)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SETSCRATCH);
        spi_transmit_32(handle);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SETSCRATCH);
        spi_transmit_burst(handle);
    }
}

/**
 * @brief Set the scratch bitmap for widget use, only works in burst-mode.
 */
void EVE_cmd_setscratch_burst(const uint32_t handle)
{
    spi_transmit_burst(CMD_SETSCRATCH);
    spi_transmit_burst(handle);
}

/**
 * @brief Start a continuous sketch update.
 */
void EVE_cmd_sketch(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint32_t ptr, const uint16_t format)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SKETCH);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(ptr);
        spi_transmit_32(u16_u16_to_u32(format, 0x0000));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SKETCH);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(ptr);
        spi_transmit_burst(u16_u16_to_u32(format, 0x0000));
    }
}

/**
 * @brief Start a continuous sketch update, only works in burst-mode.
 */
void EVE_cmd_sketch_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint32_t ptr, const uint16_t format)
{
    spi_transmit_burst(CMD_SKETCH);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(ptr);
    spi_transmit_burst(u16_u16_to_u32(format, 0x0000));
}

/**
 * @brief Draw a slider.
 */
void EVE_cmd_slider(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                    const uint16_t options, const uint16_t val, const uint16_t range)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SLIDER);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, hgt));
        spi_transmit_32(u16_u16_to_u32(options, val));
        spi_transmit_32(u16_u16_to_u32(range, 0x0000));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SLIDER);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, hgt));
        spi_transmit_burst(u16_u16_to_u32(options, val));
        spi_transmit_burst(u16_u16_to_u32(range, 0x0000));
    }
}

/**
 * @brief Draw a slider, only works in burst-mode.
 */
void EVE_cmd_slider_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t hgt,
                            const uint16_t options, const uint16_t val, const uint16_t range)
{
    spi_transmit_burst(CMD_SLIDER);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, hgt));
    spi_transmit_burst(u16_u16_to_u32(options, val));
    spi_transmit_burst(u16_u16_to_u32(range, 0x0000));
}

/**
 * @brief Start an animated spinner.
 */
void EVE_cmd_spinner(const int16_t xc0, const int16_t yc0, const uint16_t style, const uint16_t scale)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SPINNER);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(style, scale));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SPINNER);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(style, scale));
    }
}

/**
 * @brief Start an animated spinner, only works in burst-mode.
 */
void EVE_cmd_spinner_burst(const int16_t xc0, const int16_t yc0, const uint16_t style, const uint16_t scale)
{
    spi_transmit_burst(CMD_SPINNER);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(style, scale));
}

/**
 * @brief Stop periodic operation of CMD_SKETCH, CMD_SPINNER or CMD_SCREENSAVER.
 */
void EVE_cmd_stop(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_STOP);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_STOP);
    }
}

/**
 * @brief Stop periodic operation of CMD_SKETCH, CMD_SPINNER or CMD_SCREENSAVER, only works in burst-mode.
 */
void EVE_cmd_stop_burst(void)
{
    spi_transmit_burst(CMD_STOP);
}

/**
 * @brief Swap the current display list.
 */
void EVE_cmd_swap(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_SWAP);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_SWAP);
    }
}

/**
 * @brief Swap the current display list, only works in burst-mode.
 */
void EVE_cmd_swap_burst(void)
{
    spi_transmit_burst(CMD_SWAP);
}

/**
 * @brief Draw a text string.
 */
void EVE_cmd_text(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const char * const p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TEXT);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(font, options));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TEXT);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(font, options));
        private_string_write(p_text);
    }
}

/**
 * @brief Draw a text string, only works in burst-mode.
 */
void EVE_cmd_text_burst(const int16_t xc0, const int16_t yc0, const uint16_t font, const uint16_t options, const char * const p_text)
{
    spi_transmit_burst(CMD_TEXT);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(font, options));
    private_string_write(p_text);
}

/**
 * @brief Draw a toggle switch with labels.
 */
void EVE_cmd_toggle(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                    const uint16_t options, const uint16_t state, const char * const p_text)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TOGGLE);
        spi_transmit_32(i16_i16_to_u32(xc0, yc0));
        spi_transmit_32(u16_u16_to_u32(wid, font));
        spi_transmit_32(u16_u16_to_u32(options, state));
        private_string_write(p_text);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TOGGLE);
        spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
        spi_transmit_burst(u16_u16_to_u32(wid, font));
        spi_transmit_burst(u16_u16_to_u32(options, state));
        private_string_write(p_text);
    }
}

/**
 * @brief Draw a toggle switch with labels, only works in burst-mode.
 */
void EVE_cmd_toggle_burst(const int16_t xc0, const int16_t yc0, const uint16_t wid, const uint16_t font,
                            const uint16_t options, const uint16_t state, const char * const p_text)
{
    spi_transmit_burst(CMD_TOGGLE);
    spi_transmit_burst(i16_i16_to_u32(xc0, yc0));
    spi_transmit_burst(u16_u16_to_u32(wid, font));
    spi_transmit_burst(u16_u16_to_u32(options, state));
    private_string_write(p_text);
}

/**
 * @brief Apply a translation to the current matrix.
 */
void EVE_cmd_translate(const int32_t tr_x, const int32_t tr_y)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CMD_TRANSLATE);
        spi_transmit_32(i32_to_u32(tr_x));
        spi_transmit_32(i32_to_u32(tr_y));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CMD_TRANSLATE);
        spi_transmit_burst(i32_to_u32(tr_x));
        spi_transmit_burst(i32_to_u32(tr_y));
    }
}

/**
 * @brief Apply a translation to the current matrix, only works in burst-mode.
 */
void EVE_cmd_translate_burst(const int32_t tr_x, const int32_t tr_y)
{
    spi_transmit_burst(CMD_TRANSLATE);
    spi_transmit_burst(i32_to_u32(tr_x));
    spi_transmit_burst(i32_to_u32(tr_y));
}

/* ##################################################################
    display list command functions for use with the coprocessor
##################################################################### */

/**
 * @brief Generic function for display-list and coprocessor commands with no arguments.
 * @note - EVE_cmd_dl(CMD_DLSTART);
 * @note - EVE_cmd_dl(CMD_SWAP);
 * @note - EVE_cmd_dl(CMD_SCREENSAVER);
 * @note - EVE_cmd_dl(VERTEX2F(0,0));
 * @note - EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
 * @note - use when keeping the binary size small is more important than beeing close to ESE / BRT_AN025
 */
void EVE_cmd_dl(const uint32_t command)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(command);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(command);
    }
}

/**
 * @brief Generic function for display-list and coprocessor commands with no arguments, only works in burst-mode.
 */
void EVE_cmd_dl_burst(const uint32_t command)
{
    spi_transmit_burst(command);
}

/**
 * @brief Specify the alpha test function.
 */
void EVE_alpha_func(const uint8_t func, const uint8_t ref)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(ALPHA_FUNC(func, ref));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(ALPHA_FUNC(func, ref));
    }
}

/**
 * @brief Specify the alpha test function, only works in burst-mode.
 */
void EVE_alpha_func_burst(const uint8_t func, const uint8_t ref)
{
    spi_transmit_burst(ALPHA_FUNC(func, ref));
}

/**
 * @brief Begin drawing a graphics primitive.
 */
void EVE_begin(const uint32_t prim)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_BEGIN | prim);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_BEGIN | prim);
    }
}

/**
 * @brief Begin drawing a graphics primitive, only works in burst-mode.
 */
void EVE_begin_burst(const uint32_t prim)
{
    spi_transmit_burst(DL_BEGIN | prim);
}

/**
 * @brief Specify the bitmap handle.
 */
void EVE_bitmap_handle(const uint8_t handle)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_HANDLE(handle));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_HANDLE(handle));
    }
}

/**
 * @brief Specify the bitmap handle, only works in burst-mode.
 */
void EVE_bitmap_handle_burst(const uint8_t handle)
{
    spi_transmit_burst(BITMAP_HANDLE(handle));
}

/**
 * @brief Specify the source bitmap memory format and layout for the current handle.
 */
void EVE_bitmap_layout(const uint8_t format, const uint16_t linestride, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_LAYOUT(format , linestride, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_LAYOUT(format , linestride, height));
    }
}

/**
 * @brief Specify the source bitmap memory format and layout for the current handle, only works in burst-mode.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_layout_burst(const uint8_t format, const uint16_t linestride, const uint16_t height)
{
    spi_transmit_burst(BITMAP_LAYOUT(format , linestride, height));
}

/**
 * @brief Specify the 2 most significant bits of the source bitmap memory format and layout for the current handle.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_layout_h(const uint16_t linestride, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_LAYOUT_H(linestride, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_LAYOUT_H(linestride, height));
    }
}

/**
 * @brief Specify the 2 most significant bits of the source bitmap memory format and layout for the current handle, only works in burst-mode.
 */
void EVE_bitmap_layout_h_burst(const uint16_t linestride, const uint16_t height)
{
    spi_transmit_burst(BITMAP_LAYOUT_H(linestride, height));
}

/**
 * @brief Specify the screen drawing of bitmaps for the current handle.
 */
void EVE_bitmap_size(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_SIZE(filter, wrapx, wrapy, width, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SIZE(filter, wrapx, wrapy, width, height));
    }
}

/**
 * @brief Specify the screen drawing of bitmaps for the current handle, only works in burst-mode.
 */
void EVE_bitmap_size_burst(const uint8_t filter, const uint8_t wrapx, const uint8_t wrapy, const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(BITMAP_SIZE(filter, wrapx, wrapy, width, height));
}

/**
 * @brief Specify the 2 most significant bits of bitmaps dimension for the current handle.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_size_h(const uint16_t width, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_SIZE_H(width, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SIZE_H(width, height));
    }
}

/**
 * @brief Specify the 2 most significant bits of bitmaps dimension for the current handle, only works in burst-mode.
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 */
void EVE_bitmap_size_h_burst(const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(BITMAP_SIZE_H(width, height));
}

/**
 * @brief Specify the source address of bitmap data.
 */
void EVE_bitmap_source(const uint32_t addr)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BITMAP_SOURCE(addr));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BITMAP_SOURCE(addr));
    }
}

/**
 * @brief Specify the source address of bitmap data, only works in burst-mode.
 */
void EVE_bitmap_source_burst(const uint32_t addr)
{
    spi_transmit_burst(BITMAP_SOURCE(addr));
}

/**
 * @brief Specify how new color values are combined with the values already in the color buffer.
 */
void EVE_blend_func(const uint8_t src, const uint8_t dst)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(BLEND_FUNC(src, dst));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(BLEND_FUNC(src, dst));
    }
}

/**
 * @brief Specify how new color values are combined with the values already in the color buffer, only works in burst-mode.
 */
void EVE_blend_func_burst(const uint8_t src, const uint8_t dst)
{
    spi_transmit_burst(BLEND_FUNC(src, dst));
}

/**
 * @brief Execute a sequence of commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 */
void EVE_call(const uint16_t dest)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CALL(dest));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CALL(dest));
    }
}

/**
 * @brief Execute a sequence of commands at another location in the display list, only works in burst-mode.
 * @note valid range for dest is from zero to 2047
 */
void EVE_call_burst(const uint16_t dest)
{
    spi_transmit_burst(CALL(dest));
}

/**
 * @brief Set the bitmap cell number for the VERTEX2F command.
 */
void EVE_cell(const uint8_t cell)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CELL(cell));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CELL(cell));
    }
}

/**
 * @brief Set the bitmap cell number for the VERTEX2F command, only works in burst-mode.
 */
void EVE_cell_burst(const uint8_t cell)
{
    spi_transmit_burst(CELL(cell));
}

/**
 * @brief Clear buffers to preset values.
 */
void EVE_clear(const uint8_t color, const uint8_t stencil, const uint8_t tag)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CLEAR(color, stencil, tag));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR(color, stencil, tag));
    }
}

/**
 * @brief Clear buffers to preset values, only works in burst-mode.
 */
void EVE_clear_burst(const uint8_t color, const uint8_t stencil, const uint8_t tag)
{
    spi_transmit_burst(CLEAR(color, stencil, tag));
}

/**
 * @brief Set clear value for the alpha channel.
 */
void EVE_clear_color_a(const uint8_t alpha)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CLEAR_COLOR_A(alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR_COLOR_A(alpha));
    }
}

/**
 * @brief Set clear value for the alpha channel, only works in burst-mode.
 */
void EVE_clear_color_a_burst(const uint8_t alpha)
{
    spi_transmit_burst(CLEAR_COLOR_A(alpha));
}

/**
 * @brief Specify clear values for red, green and blue channels.
 */
void EVE_clear_color_rgb(const uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_CLEAR_COLOR_RGB | (color & 0x00ffffffUL));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_CLEAR_COLOR_RGB | (color & 0x00ffffffUL));
    }
}

/**
 * @brief Specify clear values for red, green and blue channels, only works in burst-mode.
 */
void EVE_clear_color_rgb_burst(const uint32_t color)
{
    spi_transmit_burst(DL_CLEAR_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Set clear value for the stencil buffer.
 */
void EVE_clear_stencil(const uint8_t val)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CLEAR_STENCIL(val));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR_STENCIL(val));
    }
}

/**
 * @brief Set clear value for the stencil buffer, only works in burst-mode.
 */
void EVE_clear_stencil_burst(const uint8_t val)
{
    spi_transmit_burst(CLEAR_STENCIL(val));
}

/**
 * @brief Set clear value for the tag buffer.
 */
void EVE_clear_tag(const uint8_t val)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(CLEAR_TAG(val));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(CLEAR_TAG(val));
    }
}

/**
 * @brief Set clear value for the tag buffer, only works in burst-mode.
 */
void EVE_clear_tag_burst(const uint8_t val)
{
    spi_transmit_burst(CLEAR_TAG(val));
}


/**
 * @brief Set the current color red, green and blue.
 */
void EVE_color_rgb(const uint32_t color)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_COLOR_RGB | (color & 0x00ffffffUL));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_COLOR_RGB | (color & 0x00ffffffUL));
    }
}

/**
 * @brief Set the current color red, green and blue, only works in burst-mode.
 */
void EVE_color_rgb_burst(const uint32_t color)
{
    spi_transmit_burst(DL_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Set the current color alpha, green and blue.
 */
void EVE_color_a(const uint8_t alpha)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_COLOR_A | ((uint32_t) alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_COLOR_A | ((uint32_t) alpha));
    }
}

/**
 * @brief Set the current color alpha, green and blue, only works in burst-mode.
 */
void EVE_color_a_burst(const uint8_t alpha)
{
    spi_transmit_burst(DL_COLOR_A | ((uint32_t) alpha));
}

/**
 * @brief Enable or disable writing of color components.
 */
void EVE_color_mask(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(COLOR_MASK(red, green, blue, alpha));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(COLOR_MASK(red, green, blue, alpha));
    }
}

/**
 * @brief Enable or disable writing of color components, only works in burst-mode.
 */
void EVE_color_mask_burst(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    spi_transmit_burst(COLOR_MASK(red, green, blue, alpha));
}

/**
 * @brief End the display list.
 */
void EVE_display(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_DISPLAY);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_DISPLAY);
    }
}

/**
 * @brief End the display list, only works in burst-mode.
 */
void EVE_display_burst(void)
{
    spi_transmit_burst(DL_DISPLAY);
}

/**
 * @brief End drawing a graphics primitive.
 */
void EVE_end(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_END);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_END);
    }
}

/**
 * @brief End drawing a graphics primitive, only works in burst-mode.
 */
void EVE_end_burst(void)
{
    spi_transmit_burst(DL_END);
}

/**
 * @brief Execute commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 */
void EVE_jump(const uint16_t dest)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(JUMP(dest));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(JUMP(dest));
    }
}

/**
 * @brief Execute commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 */
void EVE_jump_burst(const uint16_t dest)
{
    spi_transmit_burst(JUMP(dest));
}

/**
 * @brief Specify the width of lines to be drawn with primitive LINES in 1/16 pixel precision.
 */
void EVE_line_width(const uint16_t width)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(LINE_WIDTH(width));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(LINE_WIDTH(width));
    }
}

/**
 * @brief Specify the width of lines to be drawn with primitive LINES in 1/16 pixel precision, only works in burst-mode.
 */
void EVE_line_width_burst(const uint16_t width)
{
    spi_transmit_burst(LINE_WIDTH(width));
}

/**
 * @brief Execute a single command from a macro register.
 * @param macro Macro registers to read. 0 for REG_MACRO_0, 1 for REG_MACRO_1.
 */
void EVE_macro(const uint8_t macro)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(MACRO(macro));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(MACRO(macro));
    }
}

/**
 * @brief Execute a single command from a macro register, only works in burst-mode.
 * @param macro Macro registers to read. 0 for REG_MACRO_0, 1 for REG_MACRO_1.
 */
void EVE_macro_burst(const uint8_t macro)
{
    spi_transmit_burst(MACRO(macro));
}

/**
 * @brief No operation.
 * @note Does nothing. May be used as a spacer in display lists, if required.
 */
void EVE_nop(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_NOP);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_NOP);
    }
}

/**
 * @brief No operation, only works in burst-mode.
 */
void EVE_nop_burst(void)
{
    spi_transmit_burst(DL_NOP);
}

/**
 * @brief Set the base address of the palette.
 * @note 2-byte alignment is required if pixel format is PALETTE4444 or PALETTE565.
 */
void EVE_palette_source(const uint32_t addr)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(PALETTE_SOURCE(addr));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(PALETTE_SOURCE(addr));
    }
}

/**
 * @brief Set the base address of the palette, only works in burst-mode.
 */
void EVE_palette_source_burst(const uint32_t addr)
{
    spi_transmit_burst(PALETTE_SOURCE(addr));
}

/**
 * @brief Specify the radius of points in 1/16 pixel precision.
 */
void EVE_point_size(const uint16_t size)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(POINT_SIZE(size));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(POINT_SIZE(size));
    }
}

/**
 * @brief Specify the radius of points in 1/16 pixel precision, only works in burst-mode.
 */
void EVE_point_size_burst(const uint16_t size)
{
    spi_transmit_burst(POINT_SIZE(size));
}

/**
 * @brief Restore the current graphics context from the context stack.
 */
void EVE_restore_context(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_RESTORE_CONTEXT);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_RESTORE_CONTEXT);
    }
}

/**
 * @brief Restore the current graphics context from the context stack, only works in burst-mode.
 */
void EVE_restore_context_burst(void)
{
    spi_transmit_burst(DL_RESTORE_CONTEXT);
}

/**
 * @brief Return from a previous CALL command..
 */
void EVE_return(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_RETURN);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_RETURN);
    }
}

/**
 * @brief Return from a previous CALL command..
 */
void EVE_return_burst(void)
{
    spi_transmit_burst(DL_RETURN);
}

/**
 * @brief Push the current graphics context on the context stack.
 */
void EVE_save_context(void)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_SAVE_CONTEXT);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_SAVE_CONTEXT);
    }
}

/**
 * @brief Push the current graphics context on the context stack, only works in burst-mode.
 */
void EVE_save_context_burst(void)
{
    spi_transmit_burst(DL_SAVE_CONTEXT);
}

/**
 * @brief Set the size of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2048
 */
void EVE_scissor_size(const uint16_t width, const uint16_t height)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(SCISSOR_SIZE(width, height));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(SCISSOR_SIZE(width, height));
    }
}

/**
 * @brief Set the size of the scissor clip rectangle, only works in burst-mode.
 */
void EVE_scissor_size_burst(const uint16_t width, const uint16_t height)
{
    spi_transmit_burst(SCISSOR_SIZE(width, height));
}

/**
 * @brief Specify the top left corner of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2047
 */
void EVE_scissor_xy(const uint16_t xc0, const uint16_t yc0)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(SCISSOR_XY(xc0, yc0));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(SCISSOR_XY(xc0, yc0));
    }
}

/**
 * @brief Specify the top left corner of the scissor clip rectangle, only works in burst-mode.
 */
void EVE_scissor_xy_burst(const uint16_t xc0, const uint16_t yc0)
{
    spi_transmit_burst(SCISSOR_XY(xc0, yc0));
}

/**
 * @brief Set function and reference value for stencil testing.
 */
void EVE_stencil_func(const uint8_t func, const uint8_t ref, const uint8_t mask)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(STENCIL_FUNC(func, ref, mask));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(STENCIL_FUNC(func, ref, mask));
    }
}

/**
 * @brief Set function and reference value for stencil testing, only works in burst-mode.
 */
void EVE_stencil_func_burst(const uint8_t func, const uint8_t ref, const uint8_t mask)
{
    spi_transmit_burst(STENCIL_FUNC(func, ref, mask));
}

/**
 * @brief Control the writing of individual bits in the stencil planes.
 */
void EVE_stencil_mask(const uint8_t mask)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(STENCIL_MASK(mask));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(STENCIL_MASK(mask));
    }
}

/**
 * @brief Control the writing of individual bits in the stencil planes, only works in burst-mode.
 */
void EVE_stencil_mask_burst(const uint8_t mask)
{
    spi_transmit_burst(STENCIL_MASK(mask));
}

/**
 * @brief Set stencil test actions.
 */
void EVE_stencil_op(const uint8_t sfail, const uint8_t spass)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(STENCIL_OP(sfail, spass));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(STENCIL_OP(sfail, spass));
    }
}

/**
 * @brief Set stencil test actions, only works in burst-mode.
 */
void EVE_stencil_op_burst(const uint8_t sfail, const uint8_t spass)
{
    spi_transmit_burst(STENCIL_OP(sfail, spass));
}

/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen.
 */
void EVE_tag(const uint8_t tag)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(DL_TAG | tag);
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(DL_TAG | tag);
    }
}

/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen, only works in burst-mode.
 */
void EVE_tag_burst(const uint8_t tag)
{
    spi_transmit_burst(DL_TAG | tag);
}

/**
 * @brief Control the writing of the tag buffer.
 */
void EVE_tag_mask(const uint8_t mask)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(TAG_MASK(mask));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(TAG_MASK(mask));
    }
}

/**
 * @brief Control the writing of the tag buffer, only works in burst-mode.
 */
void EVE_tag_mask_burst(const uint8_t mask)
{
    spi_transmit_burst(TAG_MASK(mask));
}

/**
 * @brief Set coordinates for graphics primitve.
 */
void EVE_vertex2f(const int16_t xc0, const int16_t yc0)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(VERTEX2F(xc0, yc0));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX2F(xc0, yc0));
    }
}

/**
 * @brief Set coordinates for graphics primitve, only works in burst-mode.
 */
void EVE_vertex2f_burst(const int16_t xc0, const int16_t yc0)
{
    spi_transmit_burst(VERTEX2F(xc0, yc0));
}

/**
 * @brief Set coordinates for graphics primitve.
 */
void EVE_vertex2ii(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(VERTEX2II(xc0, yc0, handle, cell));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX2II(xc0, yc0, handle, cell));
    }
}

/**
 * @brief Set coordinates for graphics primitve, only works in burst-mode.
 */
void EVE_vertex2ii_burst(const uint16_t xc0, const uint16_t yc0, const uint8_t handle, const uint8_t cell)
{
    spi_transmit_burst(VERTEX2II(xc0, yc0, handle, cell));
}

/**
 * @brief Set the precision of VERTEX2F coordinates.
 */
void EVE_vertex_format(const uint8_t frac)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(VERTEX_FORMAT(frac));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX_FORMAT(frac));
    }
}

/**
 * @brief Set the precision of VERTEX2F coordinates, only works in burst-mode.
 */
void EVE_vertex_format_burst(const uint8_t frac)
{
    spi_transmit_burst(VERTEX_FORMAT(frac));
}

/**
 * @brief Set the vertex transformations X translation component.
 */
void EVE_vertex_translate_x(const int32_t xco)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(VERTEX_TRANSLATE_X(xco));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX_TRANSLATE_X(xco));
    }
}

/**
 * @brief Set the vertex transformations X translation component, only works in burst-mode.
 */
void EVE_vertex_translate_x_burst(const int32_t xco)
{
    spi_transmit_burst(VERTEX_TRANSLATE_X(xco));
}

/**
 * @brief Set the vertex transformations Y translation component.
 */
void EVE_vertex_translate_y(const int32_t yco)
{
    if (0U == cmd_burst)
    {
        eve_begin_cmd(VERTEX_TRANSLATE_Y(yco));
        EVE_cs_clear();
    }
    else
    {
        spi_transmit_burst(VERTEX_TRANSLATE_Y(yco));
    }
}

/**
 * @brief Set the vertex transformations Y translation component, only works in burst-mode.
 */
void EVE_vertex_translate_y_burst(const int32_t yco)
{
    spi_transmit_burst(VERTEX_TRANSLATE_Y(yco));
}
