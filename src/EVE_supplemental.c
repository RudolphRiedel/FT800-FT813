/*
@file    EVE_supplemental.h
@brief   supplemental functions
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
- added EVE_polar_cartesian()
- fix: EVE_cmd_dl_burst() -> EVE_cmd_dl()
- Compliance: fixed linter violation in EVE_widget_circle()
- moved EVE_calibrate_manual() over from EVE_commands
- added EVE_calibrate_write() and EVE_calibrate_read()
- replaced several EVE_cmd_dl() calls with calls to dedicated functions
- added "const" statements for BARR-C:2018 / CERT C compliance

*/

#include "EVE_supplemental.h"

/* define NULL if it not already is */
#ifndef NULL
#include <stdio.h>
#endif

#if defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

/*
 * @brief widget function to draw a circle
 */
void EVE_widget_circle(const int16_t xc0, const int16_t yc0, const uint16_t radius, const uint16_t border, const uint32_t bgcolor)
{
    EVE_save_context();
    EVE_begin(EVE_POINTS);
    EVE_point_size(radius);
    EVE_vertex2f(xc0, yc0);
    EVE_color_rgb(bgcolor);
    EVE_point_size((uint16_t) (radius - border));
    EVE_vertex2f(xc0, yc0);
    EVE_end();
    EVE_restore_context();
}

/*
 * @brief widget function to draw a rectangle
 */
void EVE_widget_rectangle(const int16_t xc0, const int16_t yc0, const int16_t wid, const int16_t hgt, const int16_t border, const uint16_t linewidth, const uint32_t bgcolor)
{
    EVE_save_context();
    EVE_begin(EVE_RECTS);
    EVE_line_width(linewidth);
    EVE_vertex2f(xc0, yc0);
    EVE_vertex2f(xc0 + wid, yc0 + hgt);
    EVE_color_rgb(bgcolor);
    EVE_vertex2f(xc0 + border, yc0 + border);
    EVE_vertex2f(xc0 + wid - border, yc0 + hgt - border);
    EVE_end();
    EVE_restore_context();
}

static const int8_t sine_table[360] PROGMEM =
{
    0, 2, 4, 7, 9, 11, 13, 15, 18, 20, 22, 24, 26, 29, 31, 33, 35, 37, 39, 41,
    43, 46, 48, 50, 52, 54, 56, 58, 60, 62, 63, 65, 67, 69, 71, 73, 75, 76, 78,
    80, 82, 83, 85, 87, 88, 90, 91, 93, 94, 96, 97, 99, 100, 101, 103, 104, 105,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 119, 120,
    121, 121, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 125,
    124, 124, 123, 123, 122, 121, 121, 120, 119, 119, 118, 117, 116, 115, 114,
    113, 112, 111, 110, 109, 108, 107, 105, 104, 103, 101, 100, 99, 97, 96, 94,
    93, 91, 90, 88, 87, 85, 83, 82, 80, 78, 76, 75, 73, 71, 69, 67, 65, 63, 62,
    60, 58, 56, 54, 52, 50, 48, 46, 43, 41, 39, 37, 35, 33, 31, 29, 26, 24, 22,
    20, 18, 15, 13, 11, 9, 7, 4, 2, 0, -2, -4, -7, -9, -11, -13, -15, -18, -20,
    -22, -24, -26, -29, -31, -33, -35, -37, -39, -41, -43, -46, -48, -50, -52,
    -54, -56, -58, -60, -62, -63, -65, -67, -69, -71, -73, -75, -76, -78, -80,
    -82, -83, -85, -87, -88, -90, -91, -93, -94, -96, -97, -99,-100,-101,-103,
    -104, -105, -107, -108, -109, -110, -111, -112, -113, -114, -115, -116,
    -117, -118, -119, -119, -120, -121, -121, -122, -123, -123, -124, -124,
    -125, -125, -125, -126, -126, -126, -127, -127, -127, -127, -127, -127,
    -127, -127, -127, -127, -127, -126, -126, -126, -125, -125, -125, -124,
    -124, -123, -123, -122, -121, -121, -120, -119, -119, -118, -117, -116,
    -115, -114, -113, -112, -111, -110, -109, -108, -107, -105, -104, -103,
    -101, -100, -99, -97, -96, -94, -93, -91, -90, -88, -87, -85, -83, -82,
    -80, -78, -76, -75, -73, -71, -69, -67, -65, -63, -62, -60, -58, -56, -54,
    -52, -50, -48, -46, -43, -41, -39, -37, -35, -33, -31, -29, -26, -24,
    -22, -20, -18, -15, -13, -11, -9, -7, -4, -2
};

/**
 * @brief Calculate coordinates from an angle and a length.
 * @param length distance from coordinate origin (0,0)
 * @param angle rotation in degrees
 * @return signed X/Y coordinates for use with VERTEX2F
 * @note - resolution for angle is 1° and rotation is clockwise
 * @note - angle should be limited to a (n*360)-1
 */
void EVE_polar_cartesian(const uint16_t length, const uint16_t angle, int16_t * const p_xc0, int16_t * const p_yc0)
{
    uint16_t anglev;
    anglev = angle % 360U;

    if (p_xc0 != NULL)
    {
        int32_t calc = (int16_t) length;
        calc = ((calc * (sine_table[anglev])) + 64) / 128;
        *p_xc0 = (int16_t) calc;
    }

    if (p_yc0 != NULL)
    {
        anglev = anglev + 270U;
        anglev = anglev % 360U;

        int32_t calc = (int16_t) length;
        calc = ((calc * (sine_table[anglev])) + 64) / 128;
        *p_yc0 = (int16_t) calc;
    }
}

/**
 * @brief Helper function to write the touch calibration values.
 */
void EVE_calibrate_write(const uint32_t tta, const uint32_t ttb, const uint32_t ttc, const uint32_t ttd, const uint32_t tte, const uint32_t ttf)
{
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, tta);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, ttb);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, ttc);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, ttd);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, tte);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, ttf);
}

/**
 * @brief Helper function to read the touch calibration values.
 */
void EVE_calibrate_read(uint32_t * const p_tta, uint32_t * const p_ttb, uint32_t * const p_ttc, uint32_t * const p_ttd, uint32_t * const p_tte, uint32_t * const p_ttf)
{
    if (p_tta != NULL)
    {
        *p_tta = EVE_memRead32(REG_TOUCH_TRANSFORM_A);
    }
    if (p_ttb != NULL)
    {
        *p_ttb = EVE_memRead32(REG_TOUCH_TRANSFORM_B);
    }
    if (p_ttc != NULL)
    {
        *p_ttc = EVE_memRead32(REG_TOUCH_TRANSFORM_C);
    }
    if (p_ttd != NULL)
    {
        *p_ttd = EVE_memRead32(REG_TOUCH_TRANSFORM_D);
    }
    if (p_tte != NULL)
    {
        *p_tte = EVE_memRead32(REG_TOUCH_TRANSFORM_E);
    }
    if (p_ttf != NULL)
    {
        *p_ttf = EVE_memRead32(REG_TOUCH_TRANSFORM_F);
    }
}

/* This is meant to be called outside display-list building. */
/* This function displays an interactive calibration screen, calculates the calibration values */
/* and writes the new values to the touch matrix registers of EVE.*/
/* Unlike the built-in cmd_calibrate() of EVE this also works with displays that are cut down from larger ones like
 * EVE2-38A / EVE2-38G. */
/* The dimensions are needed as parameter as EVE_VSIZE for the EVE2-38 is 272 but the visible size is only 116. */
/* So the call would be EVE_calibrate_manual(EVE_HSIZE, 116); for the EVE2-38A and EVE2-38G while for most other
 * displays */
/* using EVE_calibrate_manual(EVE_VSIZE, EVE_VSIZE) would work - but for normal displays the built-in cmd_calibrate
 * would work as expected anyways */
/* This code was taken from the MatrixOrbital EVE2-Library on Github, adapted and modified */
void EVE_calibrate_manual(const uint16_t width, const uint16_t height)
{
    int32_t display_x[3U];
    int32_t display_y[3U];
    int32_t touch_x[3U];
    int32_t touch_y[3U];
    uint32_t touch_value;
    int32_t tmp;
    int32_t divi;
    int32_t trans_matrix[6U];
    uint8_t count = 0U;
    uint8_t calc = 0U;
    uint32_t calc32 = 0U;
    char num[4U];
    uint8_t touch_lock = 1U;

    /* these values determine where your calibration points will be drawn on your display */
    display_x[0U] = (int32_t) width / 6;
    display_y[0U] = (int32_t) height / 6;

    display_x[1U] = (int32_t) width - ((int32_t) width / 8);
    display_y[1U] = (int32_t) height / 2;

    display_x[2U] = (int32_t) width / 2;
    display_y[2U] = (int32_t) height - ((int32_t) height / 8);

    while (count < 3U)
    {
        EVE_cmd_dlstart();
        EVE_clear_color_rgb(0UL);
        EVE_clear(1, 1, 1);
        EVE_vertex_format(0);; /* set to 0 - reduce precision for VERTEX2F to 1 pixel instead of 1/16 pixel default */

        /* draw Calibration Point on screen */
        EVE_color_rgb(0x0000ffUL);
        EVE_point_size(15U * 16U);
        EVE_begin(EVE_POINTS);

        int16_t xc0;
        int16_t yc0;

        xc0 = (int16_t) display_x[count];
        yc0 = (int16_t) display_y[count];
        EVE_vertex2f(xc0, yc0);
        EVE_end();
        EVE_color_rgb(0xffffffUL);
        EVE_cmd_text((int16_t) width / 2, 20, 26U, EVE_OPT_CENTER, "tap on the dot");
        calc = count + 0x31U;
        num[0U] = (char) calc;
        num[1U] = (char) 0U; /* null terminated string of one character */
        EVE_cmd_text((int16_t) display_x[count], (int16_t) display_y[count], 27U, EVE_OPT_CENTER, num);

        EVE_display();
        EVE_cmd_swap();
        EVE_execute_cmd();

        for (;;)
        {
            touch_value = EVE_memRead32(REG_TOUCH_DIRECT_XY); /* read for any new touch tag inputs */

            if (touch_lock != 0U)
            {
                if ((touch_value & 0x80000000UL) != 0UL) /* check if we have no touch */
                {
                    touch_lock = 0U;
                }
            }
            else
            {
                if (0UL == (touch_value & 0x80000000UL)) /* check if a touch is detected */
                {
                    calc32 = ((touch_value >> 16U) & 0x03FFUL);
                    touch_x[count] = (int32_t) calc32; /* raw Touchscreen X coordinate */
                    calc32 = touch_value & 0x03FFUL;
                    touch_y[count] = (int32_t) calc32; /* raw Touchscreen Y coordinate */
                    touch_lock = 1U;
                    count++;
                    break; /* leave for (;;) */
                }
            }
        }
    }

    divi = ((touch_x[0U] - touch_x[2U]) * (touch_y[1U] - touch_y[2U])) - ((touch_x[1U] - touch_x[2U]) * (touch_y[0U] - touch_y[2U]));

    tmp = (((display_x[0U] - display_x[2U]) * (touch_y[1U] - touch_y[2U])) -
           ((display_x[1U] - display_x[2U]) * (touch_y[0U] - touch_y[2U])));
    trans_matrix[0U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = (((touch_x[0U] - touch_x[2U]) * (display_x[1U] - display_x[2U])) -
           ((display_x[0U] - display_x[2U]) * (touch_x[1U] - touch_x[2U])));
    trans_matrix[1U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = ((touch_y[0U] * (((touch_x[2U] * display_x[1U]) - (touch_x[1U] * display_x[2U])))) +
           (touch_y[1U] * (((touch_x[0U] * display_x[2U]) - (touch_x[2U] * display_x[0U])))) +
           (touch_y[2U] * (((touch_x[1U] * display_x[0U]) - (touch_x[0U] * display_x[1U])))));
    trans_matrix[2U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = (((display_y[0U] - display_y[2U]) * (touch_y[1U] - touch_y[2U])) -
           ((display_y[1U] - display_y[2U]) * (touch_y[0U] - touch_y[2U])));
    trans_matrix[3U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = (((touch_x[0U] - touch_x[2U]) * (display_y[1U] - display_y[2U])) -
           ((display_y[0U] - display_y[2U]) * (touch_x[1U] - touch_x[2U])));
    trans_matrix[4U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    tmp = ((touch_y[0U] * (((touch_x[2U] * display_y[1U]) - (touch_x[1U] * display_y[2U])))) +
           (touch_y[1U] * (((touch_x[0U] * display_y[2U]) - (touch_x[2U] * display_y[0U])))) +
           (touch_y[2U] * (((touch_x[1U] * display_y[0U]) - (touch_x[0U] * display_y[1U])))));
    trans_matrix[5U] = (int32_t) (((int64_t) tmp * 65536) / divi);

    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, i32_to_u32(trans_matrix[0U]));
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, i32_to_u32(trans_matrix[1U]));
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, i32_to_u32(trans_matrix[2U]));
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, i32_to_u32(trans_matrix[3U]));
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, i32_to_u32(trans_matrix[4U]));
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, i32_to_u32(trans_matrix[5U]));
}
