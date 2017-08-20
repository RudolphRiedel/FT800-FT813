/*
@file    tft.c
@brief   TFT handling functions for FT8xx_Test project, the layout is for 800x480 displays
@version 1.3
@date    2017-04-02
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

 */ 


#include "FT8.h"
#include "FT8_commands.h"

#include "tft_data.h"


#define BLUE_1	0x5dade2L
#define BLUE_2	0x3498dbL
#define BLUE_3	0x2e86c1L
#define BLUE_4	0x2874a6L
#define BLUE_5	0x21618cL
#define ORANGE_1	0xe6962cL
#define GREEN_1	0x91c60eL

/* memory-map defines */
#define MEM_LOGO 0x00000000 /* start-address of logo, needs 2242 bytes of memory */
#define MEM_PIC1 0x00000900 /* start of 100x100 pixel test image, needs 20000 bytes of memory */ 
#define MEM_DL_STATIC 0x000ff000 /* start-address of the static part of the display-list, upper 4k of gfx-mem */

uint32_t num_dl_static;

uint8_t tft_active = 0;

/* just some silly static test-data for the line-strip */
uint8_t scope_data[64] =
{
	0x80, 0x80, 0x80, 0x60, 0xa0, 0x80, 0x00, 0xff, 0xa0, 0x60, 0xa0, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x7e, 0x7c, 0x7a, 0x78, 0x76, 0x74, 0x72, 0x70, 0x6e, 0x6c, 0x6a, 0x68, 0x80, 0x80,
	0x80, 0x99, 0xa4, 0xad, 0xb1, 0xb1, 0xad, 0xa4, 0x99, 0x8e, 0x85, 0x81, 0x81, 0x85, 0x8e, 0x80,
};


#define LAYOUT_W 266
#define LAYOUT_X1 0
#define LAYOUT_X2 267
#define LAYOUT_X3 534
#define LAYOUT_H 379
#define LAYOUT_Y1 101


void initStaticBackground()
{
	FT8_cmd_dl(CMD_DLSTART); /* Start the display list */

	FT8_cmd_bgcolor(0x00c0c0c0); /* light grey */
	
	FT8_cmd_dl(VERTEX_FORMAT(0)); /* reduce precision for VERTEX2F to 1 pixel instead of 1/16 pixel default */

	/* draw the thre rectangles on top */				
	FT8_cmd_dl(DL_BEGIN | FT8_RECTS);
	FT8_cmd_dl(LINE_WIDTH(1*16)); /* size is in 1/16 pixel */

	FT8_cmd_dl(DL_COLOR_RGB | BLUE_1);
	FT8_cmd_dl(VERTEX2F(0,0));
	FT8_cmd_dl(VERTEX2F(265,99));
	
	FT8_cmd_dl(DL_COLOR_RGB | BLUE_3);
	FT8_cmd_dl(VERTEX2F(267,0));
	FT8_cmd_dl(VERTEX2F(532,99));

	FT8_cmd_dl(DL_COLOR_RGB | BLUE_5);
	FT8_cmd_dl(VERTEX2F(534,0));
	FT8_cmd_dl(VERTEX2F(800,99));


	/* draw two border-frames, just a colored rectangle and a smaller rectangle on top with background-color */
	FT8_cmd_dl(DL_COLOR_RGB | BLUE_3);
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30),(LAYOUT_Y1+40)));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30+130),(LAYOUT_Y1+40+70)));

	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30),(LAYOUT_Y1+275)));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30+130),(LAYOUT_Y1+275+70)));

	FT8_cmd_dl(DL_COLOR_RGB | WHITE);
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30+5),(LAYOUT_Y1+45)));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30+130-5),(LAYOUT_Y1+45+60)));

	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30+5),(LAYOUT_Y1+280)));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+30+130-5),(LAYOUT_Y1+280+60)));

	FT8_cmd_dl(DL_END);

	/* display the logo */
	FT8_cmd_dl(DL_BEGIN | FT8_BITMAPS);
	FT8_cmd_setbitmap(MEM_LOGO, FT8_L8, 38, 59);
	FT8_cmd_dl(VERTEX2F((LAYOUT_X3 + ((LAYOUT_W - 38) / 2)) , 20));
	FT8_cmd_dl(DL_END);

	FT8_cmd_dl(DL_BEGIN | FT8_BITMAPS);
	FT8_cmd_setbitmap(MEM_PIC1, FT8_RGB565, 100, 100);
	FT8_cmd_dl(VERTEX2F((LAYOUT_X3 + 150) , (LAYOUT_Y1 + 50) ));
	FT8_cmd_dl(DL_END);

	FT8_cmd_text(LAYOUT_X2 + (LAYOUT_W / 2), 40, 29, FT8_OPT_CENTERX, "FT81x Test");

	/* display an invisible rectangle where the slider is displayed to improve the touch-handling */
	FT8_cmd_dl(TAG(10));
	FT8_cmd_rect(LAYOUT_X1+(5*(LAYOUT_W/6))-25,LAYOUT_Y1+40,LAYOUT_X1+(5*(LAYOUT_W/6))+25,LAYOUT_Y1+40+300,1);
	FT8_cmd_dl(TAG(0));

	/* some black lines to seperate things */
	FT8_cmd_dl(DL_COLOR_RGB | BLACK);
	FT8_cmd_dl(DL_BEGIN | FT8_LINES);
	FT8_cmd_dl(VERTEX2F(0,100));
	FT8_cmd_dl(VERTEX2F(800,100));

	FT8_cmd_dl(VERTEX2F(266,0));
	FT8_cmd_dl(VERTEX2F(266,480));

	FT8_cmd_dl(VERTEX2F(533,0));
	FT8_cmd_dl(VERTEX2F(533,480));

	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+(5*(LAYOUT_W/6))-45),(LAYOUT_Y1+40+150)));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+(5*(LAYOUT_W/6))+25),(LAYOUT_Y1+40+150)));

	FT8_cmd_dl(DL_END);

	/* display a "button" consisting of two overlapping points */
	FT8_cmd_dl(DL_BEGIN | FT8_POINTS);
	FT8_cmd_dl(DL_COLOR_RGB | 0x00aaaaaa);
	FT8_cmd_dl(POINT_SIZE(25*16));
	FT8_cmd_dl(TAG(11));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+(LAYOUT_W/2)),(LAYOUT_Y1+40+150)));
	FT8_cmd_dl(DL_COLOR_RGB | 0x00af00);
	FT8_cmd_dl(POINT_SIZE(20*16));
	FT8_cmd_dl(VERTEX2F((LAYOUT_X1+(LAYOUT_W/2)),(LAYOUT_Y1+40+150)));
	FT8_cmd_dl(DL_END);

	FT8_cmd_dl(TAG(0));

	FT8_cmd_dl(DL_COLOR_RGB | BLACK);
	
	/* add some static text to the list */
	FT8_cmd_text(LAYOUT_X1+(LAYOUT_W/3), LAYOUT_Y1+10, 28, FT8_OPT_CENTERX, "Unit 1");

	FT8_cmd_text(LAYOUT_X1+175,LAYOUT_Y1+6, 26, 0, "U =");
	FT8_cmd_text(LAYOUT_X1+175+20+1+32+4,LAYOUT_Y1+6, 26, 0, "V");

	FT8_cmd_text(LAYOUT_X2-30,LAYOUT_Y1+30, 26, 0, "70V");
	FT8_cmd_text(LAYOUT_X2-30,LAYOUT_Y1+30+310, 26, 0, "10V");

	FT8_cmd_text(LAYOUT_X1+40, LAYOUT_Y1+45, 27, 0, "U");
	FT8_cmd_text(LAYOUT_X1+40+11+1, LAYOUT_Y1+45+10, 26, 0, "X1");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1, LAYOUT_Y1+45, 27, 0, "=");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1+9+1+50+1, LAYOUT_Y1+45, 27, 0, "V");

	FT8_cmd_text(LAYOUT_X1+40, LAYOUT_Y1+45+30, 27, 0, "I");
	FT8_cmd_text(LAYOUT_X1+40+11+1, LAYOUT_Y1+45+30+10, 26, 0, "X1");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1, LAYOUT_Y1+45+30, 27, 0, "=");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1+9+1+50+1, LAYOUT_Y1+45+30, 27, 0, "A");

	FT8_cmd_text(LAYOUT_X1+40, LAYOUT_Y1+280, 27, 0, "U");
	FT8_cmd_text(LAYOUT_X1+40+11+1, LAYOUT_Y1+280+10, 26, 0, "X2");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1, LAYOUT_Y1+280, 27, 0, "=");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1+9+1+50+1, LAYOUT_Y1+280, 27, 0, "V");

	FT8_cmd_text(LAYOUT_X1+40, LAYOUT_Y1+280+30, 27, 0, "I");
	FT8_cmd_text(LAYOUT_X1+40+11+1, LAYOUT_Y1+280+30+10, 26, 0, "X2");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1, LAYOUT_Y1+280+30, 27, 0, "=");
	FT8_cmd_text(LAYOUT_X1+40+11+1+20+1+9+1+50+1, LAYOUT_Y1+280+30, 27, 0, "A");


	FT8_cmd_dl(DL_COLOR_RGB | BLACK);

	FT8_cmd_text(LAYOUT_X3+150,LAYOUT_Y1+20, 16, 0, "Font 16");
	FT8_cmd_text(LAYOUT_X3+150,LAYOUT_Y1+30, 18, 0, "Font 18");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+20, 20, 0, "Font 20");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+30, 21, 0, "Font 21");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+45, 22, 0, "Font 22");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+60, 23, 0, "Font 23");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+80, 24, 0, "Font 24");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+100, 25, 0, "Font 25");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+136, 26, 0, "Font 26");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+150, 27, 0, "Font 27");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+165, 28, 0, "Font 28");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+185, 29, 0, "Font 29");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+205, 30, 0, "Font 30");
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+230, 31, 0, "Font 31");

#ifdef FT8_81X_ENABLE
	FT8_cmd_romfont(1,32);
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+260, 1, 0, "Font 32");
	FT8_cmd_romfont(1,33);
	FT8_cmd_text(LAYOUT_X3+20,LAYOUT_Y1+300, 1, 0, "Font 33");
	FT8_cmd_romfont(1,34);
	FT8_cmd_text(LAYOUT_X3+150,LAYOUT_Y1+140, 1, 0, "34");
#endif


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

		/* send pre-recorded touch calibration values, RVT70 */
		FT8_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000074df);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000000e6);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffd5474);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001af);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00007e79);
		FT8_memWrite32(REG_TOUCH_TRANSFORM_F, 0xffe9a63c);

#if 0
 		/* calibrate touch and displays values to screen */
		FT8_cmd_dl(CMD_DLSTART);
		FT8_cmd_dl(DL_CLEAR_RGB | BLACK);
		FT8_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
		FT8_cmd_text((FT8_HSIZE/2), (FT8_VSIZE/2), 27, FT8_OPT_CENTER, "Please tap on the dot.");
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

		FT8_cmd_text(5, 30, 28, 0, "TOUCH_TRANSFORM_A:");
		FT8_cmd_text(5, 50, 28, 0, "TOUCH_TRANSFORM_B:");
		FT8_cmd_text(5, 70, 28, 0, "TOUCH_TRANSFORM_C:");
		FT8_cmd_text(5, 90, 28, 0, "TOUCH_TRANSFORM_D:");
		FT8_cmd_text(5, 110, 28, 0, "TOUCH_TRANSFORM_E:");
		FT8_cmd_text(5, 130, 28, 0, "TOUCH_TRANSFORM_F:");

#ifdef FT8_81X_ENABLE
		FT8_cmd_setbase(16L); /* FT81x only */
#endif
		FT8_cmd_number(250, 30, 28, 0, touch_a);
		FT8_cmd_number(250, 50, 28, 0, touch_b);
		FT8_cmd_number(250, 70, 28, 0, touch_c);
		FT8_cmd_number(250, 90, 28, 0, touch_d);
		FT8_cmd_number(250, 110, 28, 0, touch_e);
		FT8_cmd_number(250, 130, 28, 0, touch_f);

		FT8_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
		FT8_cmd_dl(CMD_SWAP); /* make this list active */
		FT8_cmd_execute();
#endif

		FT8_cmd_inflate(MEM_LOGO, logo, logo_size); /* load logo into gfx-memory and de-compress it */
		FT8_cmd_execute();

		FT8_cmd_loadimage(MEM_PIC1, FT8_OPT_NODL, pngpic, pngpic_size);

		initStaticBackground();
	}
}


/*
	dynamic portion of display-handling, meant to be called every 10ms
	divided into two sections:
		- handling of touch-events and variables
		- sending a new display-list to the FT8xx
*/
void TFT_loop(void)
{
    static uint8_t delay;
    static uint8_t tag = 0;
    static uint16_t toggle_state = 0;
    static uint8_t toggle_lock = 0;

	static uint8_t track = 0;
	static uint32_t tracker_val = 0;
	static uint16_t slider10_val = 30;

	static uint16_t alive_counter = 0;

    uint32_t calc;

	static uint8_t u_unit1 = 0;
	
	uint8_t counter;
	static uint8_t strip_offset = 0;
	static uint8_t strip_delay = 0;

	if(tft_active != 0)
	{
	  	switch(delay)
		{
			case 0:
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
						if(track != 0 )
						{
							FT8_cmd_track(0, 0, 0, 0, 0); /* stop tracking */
							FT8_cmd_execute();
							track = 0;
						}
						break;

					case 8: /* master-switch on top as on/off toggle-button */
						if(toggle_lock == 0)
						{
							toggle_lock = 42;
							if(toggle_state == 0)
							{
								toggle_state = 0xffff;
							}
							else
							{
								toggle_state = 0;
								slider10_val = 30;
							}
						}
						break;
						
					case 11:
						if(toggle_state != 0) /* only execute when master-switch is set to on */
						{
							slider10_val = 30;
						}
						break;

					case 10:
						if(track == 0)
						{
							if(toggle_state != 0)
							{
								FT8_cmd_track(LAYOUT_X1+(5*(LAYOUT_W/6))-25,LAYOUT_Y1+40,50,300,10); /* start tracking */
								FT8_cmd_execute();
								track = 10;
							}
						}
						else
						{
							tracker_val = FT8_memRead32(REG_TRACKER);
							if((tracker_val & 0xff) == 10)
							{
								tracker_val = tracker_val >> 14; /* cutoff lower 8 bit and six lsb more */
								tracker_val = tracker_val / 4369; /* limit value to 60 max */
								slider10_val = tracker_val;
							}
						}
						break;
				}

				u_unit1 = 70 - slider10_val;

				break;

			case 1:
				FT8_cmd_dl(CMD_DLSTART); /* start the display list */
				FT8_cmd_dl(DL_CLEAR_RGB | WHITE); /* set the default clear color to white */
				FT8_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG); /* clear the screen - this and the previous prevent artifacts between lists, Attributes are the color, stencil and tag buffers */
				FT8_cmd_dl(TAG(0));

				FT8_cmd_append(MEM_DL_STATIC, num_dl_static); /* insert static part of display-list from copy in gfx-mem */

				/* unit 1*/
				FT8_cmd_number(LAYOUT_X1+175+20+1+32,LAYOUT_Y1+6, 26, FT8_OPT_RIGHTX, u_unit1); /* DC/DC Leistung */
				FT8_cmd_number(LAYOUT_X1+40+11+1+20+1+9+1+40, LAYOUT_Y1+45, 27, FT8_OPT_RIGHTX, 43); /* U_X1 */
				FT8_cmd_number(LAYOUT_X1+40+11+1+20+1+9+1+40, LAYOUT_Y1+45+30, 27, FT8_OPT_RIGHTX, 12); /* I_X1 */
				FT8_cmd_number(LAYOUT_X1+40+11+1+20+1+9+1+40, LAYOUT_Y1+280, 27, FT8_OPT_RIGHTX, 18); /* U_X2 */
				FT8_cmd_number(LAYOUT_X1+40+11+1+20+1+9+1+40, LAYOUT_Y1+280+30, 27, FT8_OPT_RIGHTX, 7); /* I_X2 */

				FT8_cmd_dl(TAG(8));
				FT8_cmd_toggle(50, 45, 90, 29, 0, toggle_state, "off" "\xff" "on");
				
				FT8_cmd_dl(DL_COLOR_RGB | 0x00c0c0c0);
				
				FT8_cmd_dl(TAG(10));
				FT8_cmd_slider(LAYOUT_X1+(5*(LAYOUT_W/6))-20,LAYOUT_Y1+40,20,300,0,slider10_val,60);

				/* unit 2*/
				FT8_cmd_dl(DL_COLOR_RGB | BLACK);
				FT8_cmd_dl(LINE_WIDTH(1 * 16));
	
				FT8_cmd_dl(DL_BEGIN | FT8_LINE_STRIP);


				if(toggle_state != 0) /* only execute when master-switch is set to on */
				{
					strip_delay++;
					if(strip_delay > 1)
					{
						strip_offset++;
						strip_offset &= 0x3f;
						strip_delay = 0;
					}
				}

				for(counter=0;counter<63;counter++)
				{
					FT8_cmd_dl(VERTEX2F( (LAYOUT_X2 + 12 + (counter * 4)) , (LAYOUT_Y1 + 20 + scope_data[(counter+strip_offset) & 0x3f]) )); /* x/y are in 1/16 pixel */
				}

				FT8_cmd_dl(DL_END);

				FT8_cmd_dl(DL_COLOR_RGB | 0x00aaaaaa);

				FT8_cmd_dl(DL_BEGIN | FT8_RECTS);

				for(counter=0;counter<15;counter++)
				{
					FT8_cmd_dl(VERTEX2F( (LAYOUT_X2 + 10 + (counter*16) ), ((400 + (scope_data[(counter+strip_offset) & 0x3f] & 0x3f)) )));
					FT8_cmd_dl(VERTEX2F( (LAYOUT_X2 + 10 + (counter*16) +16) ,480) );
				}
				
				FT8_cmd_dl(DL_END);

				FT8_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
				FT8_cmd_dl(CMD_SWAP); /* make this list active */

				FT8_cmd_execute();
				break;
		}

		delay++;
		delay &= 0x01; /* -> delay toggles between 0 and 1 */
	}
}
