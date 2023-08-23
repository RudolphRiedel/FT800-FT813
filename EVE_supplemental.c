/*
@file    EVE_supplemental.h
@brief   supplemental functions
@version 5.0
@date    2023-08-22
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


*/

#include "EVE_suppplemental.h"

/**
@brief widget function to draw a circle
@param xc0 The x-coordinate of the circle center in pixels.
@param yc0 The y-coordinate of the circle center in pixels.
@param radius the circile radius in pixels.
@param border The width of the circile's border in pixels. Set to 0 for no border.
@param bordercolor The color of the circile's border. Set to 0 for no border.
@param bgcolor The background color of the circle.
*/
void EVE_widget_circle(int16_t xc0, int16_t yc0, uint16_t radius, uint16_t border, uint32_t bordercolor, uint32_t bgcolor)
{
    EVE_cmd_dl_burst(DL_SAVE_CONTEXT);
    EVE_cmd_dl(DL_BEGIN | EVE_POINTS);
    if (border)
    {
        EVE_color_rgb(bordercolor);
        EVE_cmd_dl(POINT_SIZE(radius * 16 + border * 16));
        EVE_cmd_dl(VERTEX2F(xc0, yc0));
    }
    EVE_color_rgb(bgcolor);
    EVE_cmd_dl(POINT_SIZE(radius * 16));
    EVE_cmd_dl(VERTEX2F(xc0, yc0));
    EVE_cmd_dl(DL_END);
    EVE_cmd_dl_burst(DL_RESTORE_CONTEXT);
}

/**
@brief Draw a rectangle-shaped widget.
@param xc0 The x-coordinate of the upper-left corner of the rectangle in pixels.
@param yc0 The y-coordinate of the upper-left corner of the rectangle in pixels.
@param wid The width of the rectangle in pixels.
@param hgt The height of the rectangle in pixels.
@param border The width of the rectangle's border in pixels. Set to 0 for no border.
@param bordercolor The color of the rectangle's border. Set to 0 for no border.
@param linewidth The width of the lines used to draw the rectangle and its border.
@param bgcolor The background color of the rectangle.
*/
void EVE_widget_rectangle(int16_t xc0, int16_t yc0, int16_t wid, int16_t hgt, int16_t border, uint32_t bordercolor, uint16_t linewidth, uint32_t bgcolor)
{
    EVE_cmd_dl_burst(DL_SAVE_CONTEXT);
    EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
    EVE_cmd_dl(LINE_WIDTH(linewidth));
    if (border)
    {
        EVE_color_rgb(bordercolor);
        EVE_cmd_dl(VERTEX2F(xc0 + wid + border, yc0 - border));
        EVE_cmd_dl(VERTEX2F(xc0 - border, yc0 + hgt + border));
    }
    EVE_color_rgb(bgcolor);
    EVE_cmd_dl(VERTEX2F(xc0 + wid, yc0));
    EVE_cmd_dl(VERTEX2F(xc0, yc0 + hgt));
    EVE_cmd_dl(DL_END);
    EVE_cmd_dl_burst(DL_RESTORE_CONTEXT);
}
