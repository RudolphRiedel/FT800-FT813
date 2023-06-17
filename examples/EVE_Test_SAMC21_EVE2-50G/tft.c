/*
@file    tft.c
@brief   TFT handling functions for EVE_Test project
@version 1.22
@date    2023-03-24
@author  Rudolph Riedel

@section History

1.17
- replaced the UTF-8 font with a freshly generated one and adjusted the parameters for the .xfont file

1.18
- several minor changes

1.19
- removed most of the history
- changed a couple of "while (EVE_busy()) {};" lines to "EVE_execute_cmd();"
- renamed PINK to MAGENTA

1.20
- several minor changes

1.21
- several minor changes

1.22
- added touch calibration values for EVE_GD3X

 */

#include "EmbeddedVideoEngine/EVE.h"
#include "tft_data.h"
#include "tft.h"


#define TEST_UTF8 0


/* some pre-definded colors */
#define RED     0xff0000UL
#define ORANGE  0xffa500UL
#define GREEN   0x00ff00UL
#define BLUE    0x0000ffUL
#define BLUE_1  0x5dade2L
#define YELLOW  0xffff00UL
#define MAGENTA 0xff00ffUL
#define PURPLE  0x800080UL
#define WHITE   0xffffffUL
#define BLACK   0x000000UL

/* memory-map defines */
#define MEM_FONT 0x000f7e00 /* the .xfont file for the UTF-8 font is copied here */
#define MEM_LOGO 0x000f8000 /* start-address of logo, needs 6272 bytes of memory */
#define MEM_PIC1 0x000fa000 /* start of 100x100 pixel test image, ARGB565, needs 20000 bytes of memory */

#define MEM_DL_STATIC (EVE_RAM_G_SIZE - 4096) /* 0xff000 - start-address of the static part of the display-list, upper 4k of gfx-mem */

uint32_t num_dl_static = 0; /* amount of bytes in the static part of our display-list */
uint8_t tft_active = 0;
uint16_t num_profile_a = 0;
uint16_t num_profile_b = 0;

#define LAYOUT_Y1 66


void touch_calibrate(void);
void initStaticBackground(void);


void touch_calibrate(void)
{

/* send pre-recorded touch calibration values, depending on the display the code is compiled for */

#if defined (EVE_CFAF240400C1_030SC)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000ed11);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00001139);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff76809);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00000000);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010690);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffadf2e);
#endif

#if defined (EVE_CFAF320240F_035T)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00005614);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0000009e);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff43422);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000001d);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0xffffbda4);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x00f8f2ef);
#endif

#if defined (EVE_CFAF480128A0_039TC)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00010485);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0000017f);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffb0bd3);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00000073);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000e293);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x00069904);
#endif

#if defined (EVE_CFAF800480E0_050SC)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000107f9);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xffffff8c);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff451ae);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000000d2);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000feac);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffcfaaf);
#endif

#if defined (EVE_CFAF800480E1_050SC_A2)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00010603);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0000007A);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFFF0525);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000060B);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010E13);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFE8813F);
#endif

#if defined (EVE_PAF90)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00000159);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0001019c);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff93625);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00010157);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00000000);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0000c101);
#endif

#if defined (EVE_RiTFT43)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000062cd);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xfffffe45);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff45e0a);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001a3);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00005b33);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFFbb870);
#endif

#if defined (EVE_EVE2_38)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00007bed);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000001b0);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff60aa5);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00000095);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0xffffdcda);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x00829c08);
#endif

#if defined (EVE_EVE2_35G) ||  defined (EVE_EVE3_35G)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined (EVE_EVE2_43G) ||  defined (EVE_EVE3_43G)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

#if defined (EVE_EVE2_50G) || defined (EVE_EVE3_50G)
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

#if defined (EVE_RVT70)
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

#if defined (EVE_GD3X)
    EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000D09D);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xFFFFFE27);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFF0838A);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF72);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0xFFFF7D2B);
    EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x01F3096A);
#endif

/* activate this if you are using a module for the first time or if you need to re-calibrate it */
/* write down the numbers on the screen and either place them in one of the pre-defined blocks above or make a new block */
#if 0
    /* calibrate touch and displays values to screen */

#if 1
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(DL_CLEAR_COLOR_RGB | BLACK);
    EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
    EVE_cmd_text((EVE_HSIZE/2), 50, 26, EVE_OPT_CENTER, "Please tap on the dot.");
    EVE_cmd_calibrate();
    EVE_cmd_dl(DL_DISPLAY);
    EVE_cmd_dl(CMD_SWAP);
    EVE_execute_cmd();
#else
    EVE_calibrate_manual(EVE_HSIZE, EVE_VSIZE);
#endif

    uint32_t touch_a = EVE_memRead32(REG_TOUCH_TRANSFORM_A);
    uint32_t touch_b = EVE_memRead32(REG_TOUCH_TRANSFORM_B);
    uint32_t touch_c = EVE_memRead32(REG_TOUCH_TRANSFORM_C);
    uint32_t touch_d = EVE_memRead32(REG_TOUCH_TRANSFORM_D);
    uint32_t touch_e = EVE_memRead32(REG_TOUCH_TRANSFORM_E);
    uint32_t touch_f = EVE_memRead32(REG_TOUCH_TRANSFORM_F);

    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(DL_CLEAR_COLOR_RGB | BLACK);
    EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
    EVE_cmd_dl(TAG(0));

    EVE_cmd_text(5, 15, 26, 0, "TOUCH_TRANSFORM_A:");
    EVE_cmd_text(5, 30, 26, 0, "TOUCH_TRANSFORM_B:");
    EVE_cmd_text(5, 45, 26, 0, "TOUCH_TRANSFORM_C:");
    EVE_cmd_text(5, 60, 26, 0, "TOUCH_TRANSFORM_D:");
    EVE_cmd_text(5, 75, 26, 0, "TOUCH_TRANSFORM_E:");
    EVE_cmd_text(5, 90, 26, 0, "TOUCH_TRANSFORM_F:");

    EVE_cmd_setbase(16L);
    EVE_cmd_number(310, 15, 26, EVE_OPT_RIGHTX|8, touch_a);
    EVE_cmd_number(310, 30, 26, EVE_OPT_RIGHTX|8, touch_b);
    EVE_cmd_number(310, 45, 26, EVE_OPT_RIGHTX|8, touch_c);
    EVE_cmd_number(310, 60, 26, EVE_OPT_RIGHTX|8, touch_d);
    EVE_cmd_number(310, 75, 26, EVE_OPT_RIGHTX|8, touch_e);
    EVE_cmd_number(310, 90, 26, EVE_OPT_RIGHTX|8, touch_f);

    EVE_cmd_dl(DL_DISPLAY); /* instruct the co-processor to show the list */
    EVE_cmd_dl(CMD_SWAP); /* make this list active */
    EVE_execute_cmd();

    while(1);
#endif
}


void initStaticBackground(void)
{
    EVE_cmd_dl(CMD_DLSTART); /* Start the display list */
    EVE_cmd_dl(DL_TAG); /* no tag = 0 - do not use the following objects for touch-detection */

    EVE_cmd_bgcolor(0x00c0c0c0UL); /* light grey */

    EVE_cmd_dl(DL_VERTEX_FORMAT); /* set to 0 - reduce precision for VERTEX2F to 1 pixel instead of 1/16 pixel default */

    /* draw a rectangle on top */
    EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
    EVE_cmd_dl(LINE_WIDTH(1U*16U)); /* size is in 1/16 pixel */

    EVE_color_rgb(BLUE_1);
    EVE_cmd_dl(DL_VERTEX2F); /* set to 0 / 0 */
    EVE_cmd_dl(VERTEX2F(EVE_HSIZE,LAYOUT_Y1-2));
    EVE_cmd_dl(DL_END);

    /* display the logo */
    EVE_color_rgb(WHITE);
    EVE_cmd_dl(DL_BEGIN | EVE_BITMAPS);
    EVE_cmd_setbitmap(MEM_LOGO, EVE_ARGB1555, 56U, 56U);
    EVE_cmd_dl(VERTEX2F(EVE_HSIZE - 58, 5));
    EVE_cmd_dl(DL_END);

    /* draw a black line to separate things */
    EVE_color_rgb(BLACK);
    EVE_cmd_dl(DL_BEGIN | EVE_LINES);
    EVE_cmd_dl(VERTEX2F(0,LAYOUT_Y1-2));
    EVE_cmd_dl(VERTEX2F(EVE_HSIZE,LAYOUT_Y1-2));
    EVE_cmd_dl(DL_END);

#if (TEST_UTF8 != 0) && (EVE_GEN > 2)
    EVE_cmd_setfont2(12,MEM_FONT,32); /* assign bitmap handle to a custom font */
    EVE_cmd_text(EVE_HSIZE/2, 15, 12, EVE_OPT_CENTERX, "EVE Demo");
#else
    EVE_cmd_text(EVE_HSIZE/2, 15, 29, EVE_OPT_CENTERX, "EVE Demo");
#endif

    /* add the static text to the list */
#if defined (EVE_DMA)
    EVE_cmd_text(10, EVE_VSIZE - 65, 26, 0, "Bytes:");
#endif
    EVE_cmd_text(10, EVE_VSIZE - 50, 26, 0, "DL-size:");
    EVE_cmd_text(10, EVE_VSIZE - 35, 26, 0, "Time1:");
    EVE_cmd_text(10, EVE_VSIZE - 20, 26, 0, "Time2:");

    EVE_cmd_text(105, EVE_VSIZE - 35, 26, 0, "us");
    EVE_cmd_text(105, EVE_VSIZE - 20, 26, 0, "us");

    EVE_execute_cmd();

    num_dl_static = EVE_memRead16(REG_CMD_DL);

    EVE_cmd_memcpy(MEM_DL_STATIC, EVE_RAM_DL, num_dl_static);
    EVE_execute_cmd();
}


void TFT_init(void)
{
    if(E_OK == EVE_init())
    {
        tft_active = 1;

        EVE_memWrite8(REG_PWM_DUTY, 0x30);  /* setup backlight, range is from 0 = off to 0x80 = max */
        touch_calibrate();

#if (TEST_UTF8 != 0) && (EVE_GEN > 2)   /* we need a BT81x for this */
    #if 0
        /* this is only needed once to transfer the flash-image to the external flash */
        uint32_t datasize;

        EVE_cmd_inflate(0, flash, sizeof(flash)); /* de-compress flash-image to RAM_G */
        datasize = EVE_cmd_getptr(); /* we unpacked to RAM_G address 0x0000, so the first address after the unpacked data also is the size */
        EVE_cmd_flashupdate(0,0,4096); /* write blob first */
        if (E_OK == EVE_init_flash())
        {
            EVE_cmd_flashupdate(0,0,(datasize|4095)+1); /* size must be a multiple of 4096, so set the lower 12 bits and add 1 */
        }
    #endif

    if (E_OK == EVE_init_flash())
    {
        EVE_cmd_flashread(MEM_FONT, 61376, 320); /* copy .xfont from FLASH to RAM_G, offset and length are from the .map file */
    }

#endif /* TEST_UTF8 */

        EVE_cmd_inflate(MEM_LOGO, logo, sizeof(logo)); /* load logo into gfx-memory and de-compress it */
        EVE_cmd_loadimage(MEM_PIC1, EVE_OPT_NODL, pic, sizeof(pic));

        initStaticBackground();
    }
}

uint16_t toggle_state = 0;
uint16_t display_list_size = 0;

/* check for touch events and setup vars for TFT_display() */
void TFT_touch(void)
{
    if(tft_active != 0)
    {
        uint8_t tag;
        static uint8_t toggle_lock = 0;

        if(EVE_IS_BUSY == EVE_busy()) /* is EVE still processing the last display list? */
        {
            return;
        }

        display_list_size = EVE_memRead16(REG_CMD_DL); /* debug-information, get the size of the last generated display-list */

        tag = EVE_memRead8(REG_TOUCH_TAG); /* read the value for the first touch point */

        switch(tag)
        {
            case 0:
                toggle_lock = 0;
                break;

            case 10: /* use button on top as on/off toggle-switch */
                if(0 == toggle_lock)
                {
                    toggle_lock = 42;
                    if(0 == toggle_state)
                    {
                        toggle_state = EVE_OPT_FLAT;
                    }
                    else
                    {
                        toggle_state = 0;
                    }
                }
                break;
            default:
                break;
        }
    }
}

/*
    dynamic portion of display-handling, meant to be called every 20ms or more
*/
void TFT_display(void)
{
    static int32_t rotate = 0;

    if(tft_active != 0U)
    {
        #if defined (EVE_DMA)
            uint16_t cmd_fifo_size;
            cmd_fifo_size = EVE_dma_buffer_index*4; /* without DMA there is no way to tell how many bytes are written to the cmd-fifo */
        #endif

        EVE_start_cmd_burst(); /* start writing to the cmd-fifo as one stream of bytes, only sending the address once */

        EVE_cmd_dl_burst(CMD_DLSTART); /* start the display list */
        EVE_cmd_dl_burst(DL_CLEAR_COLOR_RGB | WHITE); /* set the default clear color to white */
        EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG); /* clear the screen - this and the previous prevent artifacts between lists, Attributes are the color, stencil and tag buffers */
        EVE_cmd_dl_burst(DL_TAG);

        EVE_cmd_append_burst(MEM_DL_STATIC, num_dl_static); /* insert static part of display-list from copy in gfx-mem */
        /* display a button */
        EVE_color_rgb_burst(WHITE);
        EVE_cmd_fgcolor_burst(0x00c0c0c0UL); /* some grey */
        EVE_cmd_dl_burst(DL_TAG+10U); /* assign tag-value '10' to the button that follows */
        EVE_cmd_button_burst(20,20,80,30, 28, toggle_state,"Touch!");
        EVE_cmd_dl_burst(DL_TAG); /* no touch */

        /* display a picture and rotate it when the button on top is activated */
        EVE_cmd_setbitmap_burst(MEM_PIC1, EVE_RGB565, 100U, 100U);

        EVE_cmd_dl_burst(DL_SAVE_CONTEXT);
        EVE_cmd_dl_burst(CMD_LOADIDENTITY);
        EVE_cmd_translate_burst(65536 * 70, 65536 * 50); /* shift off-center */
        EVE_cmd_rotate_burst(rotate);
        EVE_cmd_translate_burst(65536 * -70, 65536 * -50); /* shift back */
        EVE_cmd_dl_burst(CMD_SETMATRIX);

        if(toggle_state != 0U)
        {
            rotate += 256;
        }

        EVE_cmd_dl_burst(DL_BEGIN | EVE_BITMAPS);
        EVE_cmd_dl_burst(VERTEX2F(EVE_HSIZE - 100, (LAYOUT_Y1)));
        EVE_cmd_dl_burst(DL_END);
        EVE_cmd_dl_burst(DL_RESTORE_CONTEXT);

        /* print profiling values */
        EVE_color_rgb_burst(BLACK);

        #if defined (EVE_DMA)
        EVE_cmd_number_burst(100, EVE_VSIZE - 65, 26, EVE_OPT_RIGHTX, cmd_fifo_size); /* number of bytes written to the cmd-fifo */
        #endif
        EVE_cmd_number_burst(100, EVE_VSIZE - 50, 26, EVE_OPT_RIGHTX, display_list_size); /* number of bytes written to the display-list by the command co-pro */
        EVE_cmd_number_burst(100, EVE_VSIZE - 35, 26, EVE_OPT_RIGHTX|4U, num_profile_a); /* duration in us of TFT_loop() for the touch-event part */
        EVE_cmd_number_burst(100, EVE_VSIZE - 20, 26, EVE_OPT_RIGHTX|4U, num_profile_b); /* duration in us of TFT_loop() for the display-list part */

        EVE_cmd_dl_burst(DL_DISPLAY); /* instruct the co-processor to show the list */
        EVE_cmd_dl_burst(CMD_SWAP); /* make this list active */

        EVE_end_cmd_burst(); /* stop writing to the cmd-fifo, the cmd-FIFO will be executed automatically after this or when DMA is done */
    }
}
