/*
@file    tft.c
@brief   TFT handling functions for FT8xx_Test project, the layout is for 800x480 displays
@version 1.6
@date    2018-07-08
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
- adapted to release 3 of Ft8xx library with FT8_ and ft_ prefixes changed to FT8_
- removed "while (FT8_busy());" lines after FT8_cmd_execute() since it does that by itself now
- removed "FT8_cmd_execute();" line after FT8_cmd_loadimage(MEM_PIC1, FT8_OPT_NODL, pngpic, pngpic_size); as FT8_cmd_loadimage() executes itself now

1.4
- added num_profile_a and num_profile_b for simple profiling
- utilised FT8_cmd_start()
- some general cleanup and house-keeping to make it fit for release

1.5
- simplified the example layout a lot and made it to scale with all display-sizes

1.6
- a bit of house-keeping after trying a couple of things

 */ 


#include "FT8.h"
#include "FT8_commands.h"
#include "tft_data.h"

#define BLUE_1	0x5dade2L

/* memory-map defines */
#define MEM_LOGO 0x00000000 /* start-address of logo, needs 2242 bytes of memory */
#define MEM_PIC1 0x00000900 /* start of 100x100 pixel test image, ARGB565, needs 20000 bytes of memory */ 

#define MEM_DL_STATIC 0x000ff000 /* start-address of the static part of the display-list, upper 4k of gfx-mem */

uint32_t num_dl_static; /* amount of bytes in the static part of our display-list */
uint8_t tft_active = 0;
uint16_t num_profile_a, num_profile_b;

#define LAYOUT_Y1 66


void initStaticBackground()
{
	FT8_cmd_dl(CMD_DLSTART); /* Start the display list */

	FT8_cmd_dl(TAG(0)); /* none that follows is an object for touch-detection */

	FT8_cmd_bgcolor(0x00c0c0c0); /* light grey */
	
	FT8_cmd_dl(VERTEX_FORMAT(0)); /* reduce precision for VERTEX2F to 1 pixel instead of 1/16 pixel default */

	/* draw a rectangle on top */				
	FT8_cmd_dl(DL_BEGIN | FT8_RECTS);
	FT8_cmd_dl(LINE_WIDTH(1*16)); /* size is in 1/16 pixel */

	FT8_cmd_dl(DL_COLOR_RGB | BLUE_1);
	FT8_cmd_dl(VERTEX2F(0,0));
	FT8_cmd_dl(VERTEX2F(FT8_HSIZE,LAYOUT_Y1-2));
	FT8_cmd_dl(DL_END);

	/* display the logo */
	FT8_cmd_dl(DL_COLOR_RGB | WHITE);
	FT8_cmd_dl(DL_BEGIN | FT8_BITMAPS);
	FT8_cmd_setbitmap(MEM_LOGO, FT8_L8, 38, 59);
	FT8_cmd_dl(VERTEX2F(FT8_HSIZE - 50, 5));
	FT8_cmd_dl(DL_END);

	/* draw a black line to separate things */
	FT8_cmd_dl(DL_COLOR_RGB | BLACK);
	FT8_cmd_dl(DL_BEGIN | FT8_LINES);
	FT8_cmd_dl(VERTEX2F(0,LAYOUT_Y1-2));
	FT8_cmd_dl(VERTEX2F(FT8_HSIZE,LAYOUT_Y1-2));
	FT8_cmd_dl(DL_END);

	/* add the static text to the list */
	FT8_cmd_text(10, FT8_VSIZE - 65, 27, 0, "Bytes:");
	FT8_cmd_text(10, FT8_VSIZE - 45, 27, 0, "Time1:");
	FT8_cmd_text(10, FT8_VSIZE - 25, 27, 0, "Time2:");

	FT8_cmd_text(125, FT8_VSIZE - 45, 27, 0, "us");
	FT8_cmd_text(125, FT8_VSIZE - 25, 27, 0, "us");

	FT8_cmd_execute();
	
	num_dl_static = FT8_memRead16(REG_CMD_DL);

	FT8_cmd_memcpy(MEM_DL_STATIC, FT8_RAM_DL, num_dl_static);
	FT8_cmd_execute();
}


void TFT_init(void)
{
	if(FT8_init() != 0)
	{
		tft_active = 1;
		
//		FT8_memWrite8(REG_PWM_DUTY, 0x20);	/* reduce the current necessary for the backlight -> weak psu.. */

		/* send pre-recorded touch calibration values, depending on the display the code is compiled for */

#if defined (FT8_EVE2_38)
// no values yet, calibration does not work with it
//	FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
//	FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
//	FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
//	FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
//	FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
//	FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

		
#if defined (FT8_EVE2_35G)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif


#if defined (FT8_EVE2_43G)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif


#if defined (FT8_EVE2_50G)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined (FT8_NHD_35)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000f78b);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000427);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffcedf8);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0xfffffba4);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000f756);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0009279e);
#endif

#if defined (FT8_RVT70AQ)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000074df);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000000e6);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffd5474);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001af);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00007e79);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0xffe9a63c);
#endif

#if defined (FT8_FT811CB_HY50HD)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 66353);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 712);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 4293876677);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 4294966157);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 67516);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 418276);
#endif

#if defined (FT8_ADAM101)
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000101E3);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000114);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFF5EEBA);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF5E);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010226);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0000C783);
#endif


#if 0
 		/* calibrate touch and displays values to screen */
		FT8_cmd_dl(CMD_DLSTART);
		FT8_cmd_dl(DL_CLEAR_RGB | BLACK);
		FT8_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
		FT8_cmd_text((FT8_HSIZE/2), 50, 26, FT8_OPT_CENTER, "Please tap on the dot.");
		FT8_cmd_calibrate();
		FT8_cmd_dl(DL_DISPLAY);
		FT8_cmd_dl(CMD_SWAP);
		FT8_cmd_execute();

		uint32_t touch_a, touch_b, touch_c, touch_d, touch_e, touch_f;

		touch_a = FT8_memRead32(REG_TOUCH_TRANSFORM_A);
		touch_b = FT8_memRead32(REG_TOUCH_TRANSFORM_B);
		touch_c = FT8_memRead32(REG_TOUCH_TRANSFORM_C);
		touch_d = FT8_memRead32(REG_TOUCH_TRANSFORM_D);
		touch_e = FT8_memRead32(REG_TOUCH_TRANSFORM_E);
		touch_f = FT8_memRead32(REG_TOUCH_TRANSFORM_F);

		FT8_cmd_dl(CMD_DLSTART);
		FT8_cmd_dl(DL_CLEAR_RGB | BLACK);
		FT8_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
		FT8_cmd_dl(TAG(0));

		FT8_cmd_text(5, 15, 26, 0, "TOUCH_TRANSFORM_A:");
		FT8_cmd_text(5, 30, 26, 0, "TOUCH_TRANSFORM_B:");
		FT8_cmd_text(5, 45, 26, 0, "TOUCH_TRANSFORM_C:");
		FT8_cmd_text(5, 60, 26, 0, "TOUCH_TRANSFORM_D:");
		FT8_cmd_text(5, 75, 26, 0, "TOUCH_TRANSFORM_E:");
		FT8_cmd_text(5, 90, 26, 0, "TOUCH_TRANSFORM_F:");

#ifdef FT8_81X_ENABLE
		FT8_cmd_setbase(16L); /* FT81x only */
		FT8_cmd_number(310, 15, 26, FT8_OPT_RIGHTX|8, touch_a);
		FT8_cmd_number(310, 30, 26, FT8_OPT_RIGHTX|8, touch_b);
		FT8_cmd_number(310, 45, 26, FT8_OPT_RIGHTX|8, touch_c);
		FT8_cmd_number(310, 60, 26, FT8_OPT_RIGHTX|8, touch_d);
		FT8_cmd_number(310, 75, 26, FT8_OPT_RIGHTX|8, touch_e);
		FT8_cmd_number(310, 90, 26, FT8_OPT_RIGHTX|8, touch_f);
#else
		FT8_cmd_number(310, 15, 26, FT8_OPT_RIGHTX, touch_a);
		FT8_cmd_number(310, 30, 26, FT8_OPT_RIGHTX, touch_b);
		FT8_cmd_number(310, 45, 26, FT8_OPT_RIGHTX, touch_c);
		FT8_cmd_number(310, 60, 26, FT8_OPT_RIGHTX, touch_d);
		FT8_cmd_number(310, 75, 26, FT8_OPT_RIGHTX, touch_e);
		FT8_cmd_number(310, 90, 26, FT8_OPT_RIGHTX, touch_f);
#endif

		FT8_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
		FT8_cmd_dl(CMD_SWAP); /* make this list active */
		FT8_cmd_execute();
		
		while(1);
#endif


		FT8_cmd_inflate(MEM_LOGO, logo, sizeof(logo)); /* load logo into gfx-memory and de-compress it */
		FT8_cmd_loadimage(MEM_PIC1, FT8_OPT_NODL, pic, sizeof(pic));

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

	if(tft_active != 0)
	{
	  	switch(touch_or_list)
		{
			case 0: /* handling of touch-events */
			
				touch_or_list = 1; /* build display-list with next call */
			
				if(FT8_busy() == 0) /* is the FT8xx executing the display list?  note: may be working as intended - or not all to indicate the FT8xx is still up and running */
				{
					alive_counter++;
				}

				calc = FT8_get_touch_tag();
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
								toggle_state = FT8_OPT_FLAT;
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
				old_offset =  FT8_report_cmdoffset(); /* used to calculate the amount of cmd-fifo bytes necessary */
			
				FT8_start_cmd_burst(); /* start writing to the cmd-fifo as one stream of bytes, only sending the address once */				
			
				FT8_cmd_dl(CMD_DLSTART); /* start the display list */
				FT8_cmd_dl(DL_CLEAR_RGB | WHITE); /* set the default clear color to white */
				FT8_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG); /* clear the screen - this and the previous prevent artifacts between lists, Attributes are the color, stencil and tag buffers */
				FT8_cmd_dl(TAG(0));

				FT8_cmd_append(MEM_DL_STATIC, num_dl_static); /* insert static part of display-list from copy in gfx-mem */

				/* display a button */
				FT8_cmd_dl(DL_COLOR_RGB | WHITE);
				FT8_cmd_fgcolor(0x00c0c0c0); /* some grey */
				FT8_cmd_dl(TAG(1));
				FT8_cmd_button(20,20,80,30, 28, toggle_state,"Touch!");
				FT8_cmd_dl(TAG(0));

				/* display a picture and rotate it when the button on top is activated */
				FT8_cmd_dl(DL_COLOR_RGB | WHITE);
				FT8_cmd_setbitmap(MEM_PIC1, FT8_RGB565, 100, 100);
				
				FT8_cmd_dl(CMD_LOADIDENTITY);
				FT8_cmd_translate(65536 * 70, 65536 * 50); /* shift off-center */
				FT8_cmd_rotate(rotate);
				FT8_cmd_translate(65536 * -70, 65536 * -50); /* shift back */
				FT8_cmd_dl(CMD_SETMATRIX);

				if(toggle_state != 0)
				{
					rotate += 256;
				}

				FT8_cmd_dl(DL_BEGIN | FT8_BITMAPS);
				FT8_cmd_dl(VERTEX2F(FT8_HSIZE - 105, (LAYOUT_Y1)));
				FT8_cmd_dl(DL_END);
				
				/* reset the transformation matrix to default values */
				FT8_cmd_getmatrix(BITMAP_TRANSFORM_A(256),BITMAP_TRANSFORM_B(0),BITMAP_TRANSFORM_C(0),BITMAP_TRANSFORM_D(0),BITMAP_TRANSFORM_E(256),BITMAP_TRANSFORM_F(0));

				/* print profiling values */
				FT8_cmd_dl(DL_COLOR_RGB | BLACK);

				FT8_cmd_number(120, FT8_VSIZE - 45, 27, FT8_OPT_RIGHTX|5, num_profile_a); /* duration in µs of TFT_loop() for the touch-even part */
				FT8_cmd_number(120, FT8_VSIZE - 25, 27, FT8_OPT_RIGHTX|5, num_profile_b); /* duration in µs of TFT_loop() for the display-list part */

				new_offset =  FT8_report_cmdoffset();
				if(old_offset > new_offset)
				{
					new_offset+=4096;
				}
				calc = new_offset-old_offset;
				calc += 24; /* adjust for the commands that follow before the end */
				FT8_cmd_number(120, FT8_VSIZE - 65, 27, FT8_OPT_RIGHTX, calc); /* number of bytes written to the cmd-fifo over the spi without adressing overhead */

				FT8_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
				FT8_cmd_dl(CMD_SWAP); /* make this list active */

				FT8_end_cmd_burst(); /* stop writing to the cmd-fifo */
				FT8_cmd_start(); /* order the command co-processor to start processing its FIFO queue but do not wait for completion */
				break;
		}
	}
}
