/*
@file    tft.c
@brief   TFT handling functions for EVE_Test project, for FT81x only, probably works with BT81x
@version 1.10
@date    2019-01-04
@author  Rudolph Riedel

@section History

1.0
- initial release

1.1
- added a simple .png image that is drawn beneath the clock just as example of how it could be done

1.2
- replaced the clock with a scrolling line-strip
- moved the .png image over to the font-test section to make room
- added a scrolling bar display
- made scrolling depending on the state of the on/off button
- reduced the precision of VERTEX2F to 1 Pixel with VERTEXT_FORMAT() (FT81x only) to avoid some pointless "*16" multiplications

1.3
- adapted to release 3 of Ft8xx library with EVE_ and ft_ prefixes changed to EVE_
- removed "while (EVE_busy());" lines after EVE_cmd_execute() since it does that by itself now
- removed "EVE_cmd_execute();" line after EVE_cmd_loadimage(MEM_PIC1, EVE_OPT_NODL, pngpic, pngpic_size); as EVE_cmd_loadimage() executes itself now

1.4
- added num_profile_a and num_profile_b for simple profiling
- utilised EVE_cmd_start()
- some general cleanup and house-keeping to make it fit for release

1.5
- simplified the example layout a lot and made it to scale with all display-sizes

1.6
- a bit of house-keeping after trying a couple of things

1.7
- added a display for the amount of bytes generated in the display-list by the command co-pro

1.8
- moved the color settings from EVE_config.h to here

1.9
- changed FT8_ prefixes to EVE_

1.10
- some cleanup
 */

#include "EVE.h"
#include "EVE_commands.h"
#include "tft_data.h"

/* some pre-definded colors */
#define RED		0xff0000UL
#define ORANGE	0xffa500UL
#define GREEN	0x00ff00UL
#define BLUE	0x0000ffUL
#define BLUE_1	0x5dade2L
#define YELLOW	0xffff00UL
#define PINK	0xff00ffUL
#define PURPLE	0x800080UL
#define WHITE	0xffffffUL
#define BLACK	0x000000UL

/* memory-map defines */
#define MEM_LOGO 0x00000000 /* start-address of logo, needs 2242 bytes of memory */
#define MEM_PIC1 0x00000900 /* start of 100x100 pixel test image, ARGB565, needs 20000 bytes of memory */

#define MEM_DL_STATIC (EVE_RAM_G_SIZE - 4096) /* start-address of the static part of the display-list, upper 4k of gfx-mem */

uint32_t num_dl_static; /* amount of bytes in the static part of our display-list */
uint8_t tft_active = 0;
uint16_t num_profile_a, num_profile_b;

#define LAYOUT_Y1 66


void touch_calibrate(void)
{

/* send pre-recorded touch calibration values, depending on the display the code is compiled for */

#if defined (EVE_EVE2_38)
	// no values yet, calibration does not work with it
	//	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
	//	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
	//	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
	//	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
	//	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
	//	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

#if defined (EVE_EVE2_35G)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined (EVE_EVE2_43G)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

#if defined (EVE_EVE2_50G)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined (EVE_EVE2_70G)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000105BC);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xFFFFFA8A);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0x00004670);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF75);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010074);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFFF14C8);
#endif

#if defined (EVE_NHD_35)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000f78b);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000427);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffcedf8);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xfffffba4);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000f756);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0009279e);
#endif

#if defined (EVE_RVT70AQ)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000074df);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000000e6);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffd5474);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001af);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00007e79);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xffe9a63c);
#endif

#if defined (EVE_FT811CB_HY50HD)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 66353);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 712);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 4293876677);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 4294966157);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 67516);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 418276);
#endif

#if defined (EVE_ADAM101)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000101E3);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000114);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFF5EEBA);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF5E);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010226);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0000C783);
#endif

/* activate this if you are using a module for the first time or if you need to re-calibrate it */
/* write down the numbers on the screen and either place them in one of the pre-defined blocks above or make a new block */
#if 0
	/* calibrate touch and displays values to screen */
	EVE_cmd_dl(CMD_DLSTART);
	EVE_cmd_dl(DL_CLEAR_RGB | BLACK);
	EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
	EVE_cmd_text((EVE_HSIZE/2), 50, 26, EVE_OPT_CENTER, "Please tap on the dot.");
	EVE_cmd_calibrate();
	EVE_cmd_dl(DL_DISPLAY);
	EVE_cmd_dl(CMD_SWAP);
	EVE_cmd_execute();

	uint32_t touch_a, touch_b, touch_c, touch_d, touch_e, touch_f;

	touch_a = EVE_memRead32(REG_TOUCH_TRANSFORM_A);
	touch_b = EVE_memRead32(REG_TOUCH_TRANSFORM_B);
	touch_c = EVE_memRead32(REG_TOUCH_TRANSFORM_C);
	touch_d = EVE_memRead32(REG_TOUCH_TRANSFORM_D);
	touch_e = EVE_memRead32(REG_TOUCH_TRANSFORM_E);
	touch_f = EVE_memRead32(REG_TOUCH_TRANSFORM_F);

	EVE_cmd_dl(CMD_DLSTART);
	EVE_cmd_dl(DL_CLEAR_RGB | BLACK);
	EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
	EVE_cmd_dl(TAG(0));

	EVE_cmd_text(5, 15, 26, 0, "TOUCH_TRANSFORM_A:");
	EVE_cmd_text(5, 30, 26, 0, "TOUCH_TRANSFORM_B:");
	EVE_cmd_text(5, 45, 26, 0, "TOUCH_TRANSFORM_C:");
	EVE_cmd_text(5, 60, 26, 0, "TOUCH_TRANSFORM_D:");
	EVE_cmd_text(5, 75, 26, 0, "TOUCH_TRANSFORM_E:");
	EVE_cmd_text(5, 90, 26, 0, "TOUCH_TRANSFORM_F:");

#if defined (FT81X_ENABLE)
	EVE_cmd_setbase(16L); /* FT81x only */
	EVE_cmd_number(310, 15, 26, EVE_OPT_RIGHTX|8, touch_a);
	EVE_cmd_number(310, 30, 26, EVE_OPT_RIGHTX|8, touch_b);
	EVE_cmd_number(310, 45, 26, EVE_OPT_RIGHTX|8, touch_c);
	EVE_cmd_number(310, 60, 26, EVE_OPT_RIGHTX|8, touch_d);
	EVE_cmd_number(310, 75, 26, EVE_OPT_RIGHTX|8, touch_e);
	EVE_cmd_number(310, 90, 26, EVE_OPT_RIGHTX|8, touch_f);
#else
	EVE_cmd_number(310, 15, 26, EVE_OPT_RIGHTX, touch_a);
	EVE_cmd_number(310, 30, 26, EVE_OPT_RIGHTX, touch_b);
	EVE_cmd_number(310, 45, 26, EVE_OPT_RIGHTX, touch_c);
	EVE_cmd_number(310, 60, 26, EVE_OPT_RIGHTX, touch_d);
	EVE_cmd_number(310, 75, 26, EVE_OPT_RIGHTX, touch_e);
	EVE_cmd_number(310, 90, 26, EVE_OPT_RIGHTX, touch_f);
#endif

	EVE_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
	EVE_cmd_dl(CMD_SWAP); /* make this list active */
	EVE_cmd_execute();

	while(1);
#endif
}


void initStaticBackground()
{
	EVE_cmd_dl(CMD_DLSTART); /* Start the display list */

	EVE_cmd_dl(TAG(0)); /* do not use the following objects for touch-detection */

	EVE_cmd_bgcolor(0x00c0c0c0); /* light grey */

	EVE_cmd_dl(VERTEX_FORMAT(0)); /* reduce precision for VERTEX2F to 1 pixel instead of 1/16 pixel default */

	/* draw a rectangle on top */
	EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
	EVE_cmd_dl(LINE_WIDTH(1*16)); /* size is in 1/16 pixel */

	EVE_cmd_dl(DL_COLOR_RGB | BLUE_1);
	EVE_cmd_dl(VERTEX2F(0,0));
	EVE_cmd_dl(VERTEX2F(EVE_HSIZE,LAYOUT_Y1-2));
	EVE_cmd_dl(DL_END);

	/* display the logo */
	EVE_cmd_dl(DL_COLOR_RGB | WHITE);
	EVE_cmd_dl(DL_BEGIN | EVE_BITMAPS);
	EVE_cmd_setbitmap(MEM_LOGO, EVE_L8, 38, 59);
	EVE_cmd_dl(VERTEX2F(EVE_HSIZE - 50, 5));
	EVE_cmd_dl(DL_END);

	/* draw a black line to separate things */
	EVE_cmd_dl(DL_COLOR_RGB | BLACK);
	EVE_cmd_dl(DL_BEGIN | EVE_LINES);
	EVE_cmd_dl(VERTEX2F(0,LAYOUT_Y1-2));
	EVE_cmd_dl(VERTEX2F(EVE_HSIZE,LAYOUT_Y1-2));
	EVE_cmd_dl(DL_END);

	/* add the static text to the list */
	EVE_cmd_text(10, EVE_VSIZE - 65, 26, 0, "DL-size:");
	EVE_cmd_text(10, EVE_VSIZE - 50, 26, 0, "Bytes:");
	EVE_cmd_text(10, EVE_VSIZE - 35, 26, 0, "Time1:");
	EVE_cmd_text(10, EVE_VSIZE - 20, 26, 0, "Time2:");

	EVE_cmd_text(125, EVE_VSIZE - 35, 26, 0, "us");
	EVE_cmd_text(125, EVE_VSIZE - 20, 26, 0, "us");

	EVE_cmd_execute();

	num_dl_static = EVE_memRead16(REG_CMD_DL);

	EVE_cmd_memcpy(MEM_DL_STATIC, EVE_RAM_DL, num_dl_static);
	EVE_cmd_execute();
}


void TFT_init(void)
{
	if(EVE_init() != 0)
	{
		tft_active = 1;

		EVE_memWrite8(REG_PWM_DUTY, 0x30);	/* setup backlight, range is from 0 = off to 0x80 = max */

		touch_calibrate();

		EVE_cmd_inflate(MEM_LOGO, logo, sizeof(logo)); /* load logo into gfx-memory and de-compress it */
		EVE_cmd_loadimage(MEM_PIC1, EVE_OPT_NODL, pic, sizeof(pic));

		initStaticBackground();
	}
}


/*
	dynamic portion of display-handling, meant to be called every 10ms or more
	divided into two sections:
		- handling of touch-events and variables
		- sending a new display-list to the FT8xx
*/
void TFT_loop(void)
{
    static uint8_t touch_or_list = 0;

    static uint8_t tag = 0;
    static uint16_t toggle_state = 0;
    static uint8_t toggle_lock = 0;

	static uint16_t alive_counter = 0;
    uint32_t calc;
	uint16_t old_offset, new_offset;
	static int32_t rotate = 0;

	uint16_t display_list_size = 0;

	if(tft_active != 0)
	{
	  	switch(touch_or_list)
		{
			case 0: /* handling of touch-events */

				touch_or_list = 1; /* build display-list with next call */

				if(EVE_busy() == 0) /* is the FT8xx executing the display list?  note: may be working as intended - or not all to indicate the FT8xx is still up and running */
				{
					alive_counter++;
				}

				calc = EVE_get_touch_tag();
				tag = calc;

				switch(tag)
				{
					case 0:
						toggle_lock = 0;
						break;

					case 1: /* use button on top as on/off radio-switch */
						if(toggle_lock == 0)
						{
							toggle_lock = 42;
							if(toggle_state == 0)
							{
								toggle_state = EVE_OPT_FLAT;
							}
							else
							{
								toggle_state = 0;
							}
						}
						break;
				}
				break;

			case 1: /* building a new display-list and sending it */

				touch_or_list = 0; /* handle touch-events with next call */
				old_offset =  EVE_report_cmdoffset(); /* used to calculate the amount of cmd-fifo bytes necessary */
				display_list_size = EVE_memRead16(REG_CMD_DL);

				EVE_start_cmd_burst(); /* start writing to the cmd-fifo as one stream of bytes, only sending the address once */

				EVE_cmd_dl(CMD_DLSTART); /* start the display list */
				EVE_cmd_dl(DL_CLEAR_RGB | WHITE); /* set the default clear color to white */
				EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG); /* clear the screen - this and the previous prevent artifacts between lists, Attributes are the color, stencil and tag buffers */
				EVE_cmd_dl(TAG(0));

				EVE_cmd_append(MEM_DL_STATIC, num_dl_static); /* insert static part of display-list from copy in gfx-mem */

				/* display a button */
				EVE_cmd_dl(DL_COLOR_RGB | WHITE);
				EVE_cmd_fgcolor(0x00c0c0c0); /* some grey */
				EVE_cmd_dl(TAG(1)); /* assign tag-value '1' to the button that follows */
				EVE_cmd_button(20,20,80,30, 28, toggle_state,"Touch!");
				EVE_cmd_dl(TAG(0)); /* no touch */

				/* display a picture and rotate it when the button on top is activated */
				EVE_cmd_setbitmap(MEM_PIC1, EVE_RGB565, 100, 100);

				EVE_cmd_dl(CMD_LOADIDENTITY);
				EVE_cmd_translate(65536 * 70, 65536 * 50); /* shift off-center */
				EVE_cmd_rotate(rotate);
				EVE_cmd_translate(65536 * -70, 65536 * -50); /* shift back */
				EVE_cmd_dl(CMD_SETMATRIX);

				if(toggle_state != 0)
				{
					rotate += 256;
				}

				EVE_cmd_dl(DL_BEGIN | EVE_BITMAPS);
				EVE_cmd_dl(VERTEX2F(EVE_HSIZE - 105, (LAYOUT_Y1)));
				EVE_cmd_dl(DL_END);

				/* reset the transformation matrix to default values */
				EVE_cmd_getmatrix(BITMAP_TRANSFORM_A(256),BITMAP_TRANSFORM_B(0),BITMAP_TRANSFORM_C(0),BITMAP_TRANSFORM_D(0),BITMAP_TRANSFORM_E(256),BITMAP_TRANSFORM_F(0));

				/* print profiling values */
				EVE_cmd_dl(DL_COLOR_RGB | BLACK);

				EVE_cmd_number(120, EVE_VSIZE - 65, 26, EVE_OPT_RIGHTX, display_list_size); /* number of bytes written to the display-list by the command co-pro */
				EVE_cmd_number(120, EVE_VSIZE - 35, 26, EVE_OPT_RIGHTX|5, num_profile_a); /* duration in µs of TFT_loop() for the touch-event part */
				EVE_cmd_number(120, EVE_VSIZE - 20, 26, EVE_OPT_RIGHTX|5, num_profile_b); /* duration in µs of TFT_loop() for the display-list part */

				new_offset =  EVE_report_cmdoffset();
				if(old_offset > new_offset)
				{
					new_offset+=4096;
				}
				calc = new_offset-old_offset;
				calc += 24; /* adjust for the commands that follow before the end */
				EVE_cmd_number(120, EVE_VSIZE - 50, 26, EVE_OPT_RIGHTX, calc); /* number of bytes written to the cmd-fifo over the spi without adressing overhead */

				EVE_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
				EVE_cmd_dl(CMD_SWAP); /* make this list active */

				EVE_end_cmd_burst(); /* stop writing to the cmd-fifo */
				EVE_cmd_start(); /* order the command co-processor to start processing its FIFO queue but do not wait for completion */
				break;
		}
	}
}
