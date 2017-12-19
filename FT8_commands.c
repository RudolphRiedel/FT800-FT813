/*
@file    FT8_commands.c
@brief   Contains Functions for using the FT8xx
@version 3.6
@date    2017-12-19
@author  Rudolph Riedel

This file needs to be renamed to FT8_command.cpp for use with Arduino. 

@section History

2.1
- removed the REG_ROTATE line from FT8_init()
- simplified FT8_busy() slightly
- implemented FT8_cmd_mediafifo(), FT8_cmd_setrotate(), FT8_cmd_setbitmap() (FT81x)

2.2
- moved hardware abstraction functions over to FT8_config.c

2.3
- changes to this header

2.3
- added "FT8_cmdWrite(FT8_CLKINT);" to top of FT8_init() to switch easier
  between displays with and without crystal populated
  note: need to add a define for internal/external clock to

- added a FT8_get_cmdoffset() call to the end of FT8_init() as it must be called
  after init anyways

2.4
- added a few lines in FT8_init() for the new config define FT8_HAS_CRYSTAL
- FT8_init() does a timeout now instead of an endless loop when waiting for the FT8xx to complete
  it's power-on selftest and returns 0x00 in case of failure'
- added FT8_cmd_romfont() (FT81x only)
- added millis option to FT8_cmd_clock()
- switched to standard-C compliant comment-style

2.5
- added FT8_memWrite_flash_buffer()
- modified FT8_cmd_loadimage() to ignore data when FT8_OPT_MEDIAFIFO is given (FT81x only)

2.6
- deleted a "#define FT8_HAS_CRYSTAL 0" line from FT8_init() which should have been long gone...

2.7
- modified FT8_cmd_loadimage() to load bigger files in chunks of 4000

3.0
- renamed from FT800_commands.c to FT8_commands.c
- changed FT_ prefixes to FT8_
- changed ft800_ prefixes to FT8_
- removed test-function FT8_cmd_loadimage_mf()
- added while (FT8_busy()); to the end of FT8_cmd_execute()
- removed while (FT8_busy()); from FT8_cmd_loadimage()

3.1
- added FT8_cmd_setfont() and FT8_cmd_setfont2() as pointed out missing by Rafael Drzewiecki

3.2
- Bugfix: FT8_cmd_interrupt() was using CMD_SNAPSHOT instead of CMD_INTERRUPT
- removed several functions for commands that do not need a function of their own:
	FT8_cmd_stop(), FT8_cmd_loadidentity(), FT8_cmd_setmatrix(), FT8_cmd_screensaver(),
	FT8_cmd_logo(), FT8_cmd_coldstart()
	These all do not have any arguments and can be used with FT8_cmd_dl(), for example:
	FT8_cmd_dl(CMD_SCREENSAVER);
	FT8_cmd_dl(CMD_SETMATRIX);
- added FT8_cmd_snapshot2()
- added FT8_cmd_setscratch()

3.3
- implemented FT8_cmd_memcrc(), FT8_cmd_getptr(), FT8_cmd_regread(), FT8_cmd_getprops()

3.4
- implemented functions FT8_start_cmd_burst() and FT8_end_cmd_burst()

3.5
- Bugifx: FT8_cmd_setbase() was incrementing the command-offset by 12 instead of 4

3.6
- Bugifix: FT8_cmd_getptr() was using CMD_MEMCRC instead of CMD_GETPTR

*/

#include "FT8.h"
#include "FT8_config.h"


/* FT8xx Memory Commands - use with FT8_memWritexx and FT8_memReadxx */
#define MEM_WRITE	0x80	/* FT8xx Host Memory Write */
#define MEM_READ	0x00	/* FT8xx Host Memory Read */

volatile uint16_t cmdOffset = 0x0000;	/* used to navigate command ring buffer */

volatile uint8_t cmd_burst = 0; /* flag to indicate cmd-burst is active */


void FT8_cmdWrite(uint8_t data)
{
	FT8_cs_set();
	spi_transmit(data);
	spi_transmit(0x00);
	spi_transmit(0x00);
	FT8_cs_clear();
}


uint8_t FT8_memRead8(uint32_t ftAddress)
{
	uint8_t ftData8 = 0;
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_READ);	/* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));	/* send low address byte */
	spi_transmit(0x00);	/* send dummy byte */
	ftData8 = spi_receive(ftData8);	/* read data byte */
	FT8_cs_clear();
	return ftData8;	/* return byte read */
}


uint16_t FT8_memRead16(uint32_t ftAddress)
{
	uint16_t ftData16 = 0;
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_READ);	/* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));	/* send low address byte */
	spi_transmit(0x00);	/* send dummy byte */
	ftData16 = (spi_receive(0x00));	/* read low byte */
	ftData16 = (spi_receive(0x00) << 8) | ftData16;	/* read high byte */
	FT8_cs_clear();
	return ftData16;	/* return integer read */
}


uint32_t FT8_memRead32(uint32_t ftAddress)
{
	uint32_t ftData32= 0;
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_READ);	/* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));	/* send low address byte */
	spi_transmit(0x00);	/* send dummy byte */
	ftData32 = ((uint32_t)spi_receive(0x00));		/* read low byte */
	ftData32 = ((uint32_t)spi_receive(0x00) << 8) | ftData32;
	ftData32 = ((uint32_t)spi_receive(0x00) << 16) | ftData32;
	ftData32 = ((uint32_t)spi_receive(0x00) << 24) | ftData32; /* read high byte */
	FT8_cs_clear();
	return ftData32;	/* return long read */
}


void FT8_memWrite8(uint32_t ftAddress, uint8_t ftData8)
{
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE);
	spi_transmit((uint8_t)(ftAddress >> 8));
	spi_transmit((uint8_t)(ftAddress));
	spi_transmit(ftData8);
	FT8_cs_clear();
}


void FT8_memWrite16(uint32_t ftAddress, uint16_t ftData16)
{
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));		/* send low address byte */
	spi_transmit((uint8_t)(ftData16));		/* send data low byte */
	spi_transmit((uint8_t)(ftData16 >> 8));	/* send data high byte */
	FT8_cs_clear();
}


void FT8_memWrite32(uint32_t ftAddress, uint32_t ftData32)
{
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));		/* send low address byte */
	spi_transmit((uint8_t)(ftData32));		/* send data low byte */
	spi_transmit((uint8_t)(ftData32 >> 8));
	spi_transmit((uint8_t)(ftData32 >> 16));
	spi_transmit((uint8_t)(ftData32 >> 24));	/* send data high byte */
	FT8_cs_clear();
}


void FT8_memWrite_flash_buffer(uint32_t ftAddress, const uint8_t *data, uint16_t len)
{
	uint16_t count;

	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE);
	spi_transmit((uint8_t)(ftAddress >> 8));
	spi_transmit((uint8_t)(ftAddress));

	len = (len + 3)&(~3);

	for(count=0;count<len;count++)
	{
		spi_transmit(fetch_flash_byte(data+count));
	}

	FT8_cs_clear();
}



/* Check if the graphics processor completed executing the current command list. */
/* This is the case when REG_CMD_READ matches cmdOffset, indicating that all commands have been executed. */
uint8_t FT8_busy(void)
{
	uint16_t cmdBufferRead;

	cmdBufferRead = FT8_memRead16(REG_CMD_READ);	/* read the graphics processor read pointer */

	if(cmdOffset != cmdBufferRead)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


uint32_t FT8_get_touch_tag(void)
{
	uint32_t value;

	value = FT8_memRead32(REG_TOUCH_TAG);
	return value;
}


/* order the command co-prozessor to start processing its FIFO que */
void FT8_cmd_execute(void)
{
	uint32_t ftAddress;

	ftAddress = REG_CMD_WRITE;

	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));			/* send low address byte */
	spi_transmit((uint8_t)(cmdOffset));			/* send data low byte */
	spi_transmit((uint8_t)(cmdOffset >> 8));	/* send data high byte */
	FT8_cs_clear();
	while (FT8_busy());
}


void FT8_get_cmdoffset(void)
{
	cmdOffset = FT8_memRead16(REG_CMD_WRITE);
}


void FT8_inc_cmdoffset(uint16_t increment)
{
	cmdOffset += increment;
	cmdOffset &= 0x0fff;
}


/*
These eliminate the overhead of transmitting the command-fifo address with every single command, just wrap a sequence of commands
with these and the address is only transmitted once at the start of the block.
Be careful to not use any functions in the sequence that do not address the command-fifo as for example any FT8_mem...() function.
*/
void FT8_start_cmd_burst(void)
{
	uint32_t ftAddress;
	
	cmd_burst = 42;
	ftAddress = FT8_RAM_CMD + cmdOffset;
	FT8_cs_set();

	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));		/* send low address byte */
}

void FT8_end_cmd_burst(void)
{
	cmd_burst = 0;
	FT8_cs_clear();
}
/* ---------------------- */


/* Beginn a co-prozessor command */
void FT8_start_cmd(uint32_t command)
{
	uint32_t ftAddress;
	
	if(cmd_burst == 0)
	{
		ftAddress = FT8_RAM_CMD + cmdOffset;
		FT8_cs_set();
		spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
		spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
		spi_transmit((uint8_t)(ftAddress));		/* send low address byte */
	}

	spi_transmit((uint8_t)(command));		/* send data low byte */
	spi_transmit((uint8_t)(command >> 8));
	spi_transmit((uint8_t)(command >> 16));
	spi_transmit((uint8_t)(command >> 24));		/* Send data high byte */
	FT8_inc_cmdoffset(4);			/* update the command-ram pointer */
}


/* generic function for all commands that have no arguments and all display-list specific control words */
/*
 examples:
 FT8_cmd_dl(CMD_DLSTART);
 FT8_cmd_dl(CMD_SWAP);
 FT8_cmd_dl(CMD_SCREENSAVER);
 FT8_cmd_dl(LINE_WIDTH(1*16));
 FT8_cmd_dl(VERTEX2F(0,0));
 FT8_cmd_dl(DL_BEGIN | FT8_RECTS);
*/
void FT8_cmd_dl(uint32_t command)
{
	FT8_start_cmd(command);
	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* Write a string to co-processor memory in context of a command: no chip-select, just plain spi-transfers */
void FT8_write_string(const char *text)
{
	uint8_t textindex = 0;
	uint8_t padding = 0;

	while(text[textindex] != 0)
	{
		spi_transmit(text[textindex]);
		textindex++;
	}

	padding = textindex % 4;  /* 0, 1, 2 oder 3 */
	padding = 4-padding; /* 4, 3, 2, 1 */

	while(padding > 0)
	{
		spi_transmit(0);
		padding--;
		textindex++;
	}

	FT8_inc_cmdoffset(textindex);
}

/* commands to draw graphics objects: */

void FT8_cmd_text(int16_t x0, int16_t y0, int16_t font, uint16_t options, const char* text)
{
	FT8_start_cmd(CMD_TEXT);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	FT8_inc_cmdoffset(8);
	FT8_write_string(text);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_button(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text)
{
	FT8_start_cmd(CMD_BUTTON);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	FT8_inc_cmdoffset(12);
	FT8_write_string(text);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* draw a clock */
void FT8_cmd_clock(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t millisecs)
{
	FT8_start_cmd(CMD_CLOCK);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(r0));
	spi_transmit((uint8_t)(r0 >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(hours));
	spi_transmit((uint8_t)(hours >> 8));

	spi_transmit((uint8_t)(minutes));
	spi_transmit((uint8_t)(minutes >> 8));

	spi_transmit((uint8_t)(seconds));
	spi_transmit((uint8_t)(seconds >> 8));

	spi_transmit((uint8_t)(millisecs));
	spi_transmit((uint8_t)(millisecs >> 8));

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_bgcolor(uint32_t color)
{
	FT8_start_cmd(CMD_BGCOLOR);

	spi_transmit((uint8_t)(color));
	spi_transmit((uint8_t)(color >> 8));
	spi_transmit((uint8_t)(color >> 16));
	spi_transmit(0x00);

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_fgcolor(uint32_t color)
{
	FT8_start_cmd(CMD_FGCOLOR);

	spi_transmit((uint8_t)(color));
	spi_transmit((uint8_t)(color >> 8));
	spi_transmit((uint8_t)(color >> 16));
	spi_transmit(0x00);

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_gradcolor(uint32_t color)
{
	FT8_start_cmd(CMD_GRADCOLOR);

	spi_transmit((uint8_t)(color));
	spi_transmit((uint8_t)(color >> 8));
	spi_transmit((uint8_t)(color >> 16));
	spi_transmit(0x00);

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_gauge(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
	FT8_start_cmd(CMD_GAUGE);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(r0));
	spi_transmit((uint8_t)(r0 >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(major));
	spi_transmit((uint8_t)(major >> 8));

	spi_transmit((uint8_t)(minor));
	spi_transmit((uint8_t)(minor >> 8));

	spi_transmit((uint8_t)(val));
	spi_transmit((uint8_t)(val >> 8));

	spi_transmit((uint8_t)(range));
	spi_transmit((uint8_t)(range >> 8));

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
	FT8_start_cmd(CMD_GRADIENT);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(rgb0));
	spi_transmit((uint8_t)(rgb0 >> 8));
	spi_transmit((uint8_t)(rgb0 >> 16));
	spi_transmit(0x00);

	spi_transmit((uint8_t)(x1));
	spi_transmit((uint8_t)(x1 >> 8));

	spi_transmit((uint8_t)(y1));
	spi_transmit((uint8_t)(y1 >> 8));

	spi_transmit((uint8_t)(rgb1));
	spi_transmit((uint8_t)(rgb1 >> 8));
	spi_transmit((uint8_t)(rgb1 >> 16));
	spi_transmit(0x00);

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_keys(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t font, uint16_t options, const char* text)
{
	FT8_start_cmd(CMD_KEYS);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	FT8_inc_cmdoffset(12);
	FT8_write_string(text);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_progress(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t range)
{
	FT8_start_cmd(CMD_PROGRESS);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(val));
	spi_transmit((uint8_t)(val >> 8));

	spi_transmit((uint8_t)(range));
	spi_transmit((uint8_t)(range >> 8));

	spi_transmit(0x00);	/* dummy byte for 4-byte alignment */
	spi_transmit(0x00); /* dummy byte for 4-byte alignment */

	FT8_inc_cmdoffset(16);	/* update the command-ram pointer */

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_scrollbar(int16_t x0, int16_t y0, int16_t w0, int16_t h0, uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
	FT8_start_cmd(CMD_SCROLLBAR);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(val));
	spi_transmit((uint8_t)(val >> 8));

	spi_transmit((uint8_t)(size));
	spi_transmit((uint8_t)(size >> 8));

	spi_transmit((uint8_t)(range));
	spi_transmit((uint8_t)(range >> 8));

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_slider(int16_t x1, int16_t y1, int16_t w1, int16_t h1, uint16_t options, uint16_t val, uint16_t range)
{
	FT8_start_cmd(CMD_SLIDER);

	spi_transmit((uint8_t)(x1));
	spi_transmit((uint8_t)(x1 >> 8));

	spi_transmit((uint8_t)(y1));
	spi_transmit((uint8_t)(y1 >> 8));

	spi_transmit((uint8_t)(w1));
	spi_transmit((uint8_t)(w1 >> 8));

	spi_transmit((uint8_t)(h1));
	spi_transmit((uint8_t)(h1 >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(val));
	spi_transmit((uint8_t)(val >> 8));

	spi_transmit((uint8_t)(range));
	spi_transmit((uint8_t)(range >> 8));

	spi_transmit(0x00); /* dummy byte for 4-byte alignment */
	spi_transmit(0x00); /* dummy byte for 4-byte alignment */

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_dial(int16_t x0, int16_t y0, int16_t r0, uint16_t options, uint16_t val)
{
	FT8_start_cmd(CMD_DIAL);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(r0));
	spi_transmit((uint8_t)(r0 >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(val));
	spi_transmit((uint8_t)(val >> 8));

	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_toggle(int16_t x0, int16_t y0, int16_t w0, int16_t font, uint16_t options, uint16_t state, const char* text)
{
	FT8_start_cmd(CMD_TOGGLE);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(state));
	spi_transmit((uint8_t)(state >> 8));

	FT8_inc_cmdoffset(12);
	FT8_write_string(text);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


#ifdef FT8_81X_ENABLE
void FT8_cmd_setbase(uint32_t base)
{
	FT8_start_cmd(CMD_SETBASE);

	spi_transmit((uint8_t)(base));		/* send data low byte */
	spi_transmit((uint8_t)(base >> 8));
	spi_transmit((uint8_t)(base >> 16));
	spi_transmit((uint8_t)(base >> 24));	/* send data high byte */

	FT8_inc_cmdoffset(4);	/* update the command-ram pointer */	

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif

#ifdef FT8_81X_ENABLE
void FT8_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
	FT8_start_cmd(CMD_SETBITMAP);

	spi_transmit((uint8_t)(addr));
	spi_transmit((uint8_t)(addr >> 8));
	spi_transmit((uint8_t)(addr >> 16));
	spi_transmit((uint8_t)(addr >> 24));
	
	spi_transmit((uint8_t)(fmt));
	spi_transmit((uint8_t)(fmt>> 8));

	spi_transmit((uint8_t)(width));
	spi_transmit((uint8_t)(width >> 8));

	spi_transmit((uint8_t)(height));
	spi_transmit((uint8_t)(height >> 8));

	spi_transmit(0);
	spi_transmit(0);	
	
	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif


void FT8_cmd_number(int16_t x0, int16_t y0, int16_t font, uint16_t options, int32_t number)
{
	FT8_start_cmd(CMD_NUMBER);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));

	spi_transmit((uint8_t)(number));
	spi_transmit((uint8_t)(number >> 8));
	spi_transmit((uint8_t)(number >> 16));
	spi_transmit((uint8_t)(number >> 24));

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* commands to operate on memory: */

void FT8_cmd_memzero(uint32_t ptr, uint32_t num)
{
	FT8_start_cmd(CMD_MEMZERO);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num)
{
	FT8_start_cmd(CMD_MEMSET);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit(value);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/*
void FT8_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *data)
{
	FT8_start_cmd(CMD_MEMWRITE);

	spi_transmit((uint8_t)(dest));
	spi_transmit((uint8_t)(dest >> 8));
	spi_transmit((uint8_t)(dest >> 16));
	spi_transmit((uint8_t)(dest >> 24));

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	num = (num + 3)&(~3);

	for(count=0;count<len;count++)
	{
		spi_transmit(pgm_read_byte_far(data+count));
	}

	FT8_inc_cmdoffset(8+len);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
*/


void FT8_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num)
{
	FT8_start_cmd(CMD_MEMCPY);

	spi_transmit((uint8_t)(dest));
	spi_transmit((uint8_t)(dest >> 8));
	spi_transmit((uint8_t)(dest >> 16));
	spi_transmit((uint8_t)(dest >> 24));

	spi_transmit((uint8_t)(src));
	spi_transmit((uint8_t)(src >> 8));
	spi_transmit((uint8_t)(src >> 16));
	spi_transmit((uint8_t)(src >> 24));

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_append(uint32_t ptr, uint32_t num)
{
	FT8_start_cmd(CMD_APPEND);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* commands for loading image data into FT8xx memory: */

void spi_flash_write(const uint8_t *data, uint16_t len)
{
	uint16_t count;

	len = (len + 3)&(~3);

	for(count=0;count<len;count++)
	{
		spi_transmit(fetch_flash_byte(data+count));
	}

	FT8_inc_cmdoffset(len);
}


void FT8_cmd_inflate(uint32_t ptr, const uint8_t *data, uint16_t len)
{
	FT8_start_cmd(CMD_INFLATE);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	FT8_inc_cmdoffset(4);

	spi_flash_write(data,len);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}

/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void FT8_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len)
{
	uint16_t bytes_left;
	uint16_t block_len;
	uint32_t ftAddress;
	
	FT8_start_cmd(CMD_LOADIMAGE);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(options));
	spi_transmit((uint8_t)(options >> 8));
	spi_transmit((uint8_t)(options >> 16));
	spi_transmit((uint8_t)(options >> 24));

	FT8_inc_cmdoffset(8);
	FT8_cs_clear();

#ifdef FT8_81X_ENABLE
	if((options & FT8_OPT_MEDIAFIFO) == 0) /* data is not transmitted thru the Media-FIFO */
#endif
	{
		bytes_left = len;
		while(bytes_left > 0)
		{
			block_len = bytes_left>4000 ? 4000:bytes_left;

			ftAddress = FT8_RAM_CMD + cmdOffset;
			FT8_cs_set();
			spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
			spi_transmit((uint8_t)(ftAddress >> 8));	/* send middle address byte */
			spi_transmit((uint8_t)(ftAddress));		/* send low address byte */
			spi_flash_write(data,block_len);
			FT8_cs_clear();
			data += block_len;
			bytes_left -= block_len;
			FT8_cmd_execute();
		}
	}
}


#ifdef FT8_81X_ENABLE
/* this is meant to be called outside display-list building, does not support cmd-burst */
void FT8_cmd_mediafifo(uint32_t ptr, uint32_t size)
{
	FT8_start_cmd(CMD_MEDIAFIFO);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(size));
	spi_transmit((uint8_t)(size >> 8));
	spi_transmit((uint8_t)(size >> 16));
	spi_transmit((uint8_t)(size >> 24));

	FT8_inc_cmdoffset(8);
	FT8_cs_clear();
}
#endif


/* commands for setting the bitmap transform matrix: */

void FT8_cmd_translate(int32_t tx, int32_t ty)
{
	FT8_start_cmd(CMD_TRANSLATE);

	spi_transmit((uint8_t)(tx));
	spi_transmit((uint8_t)(tx >> 8));
	spi_transmit((uint8_t)(tx >> 16));
	spi_transmit((uint8_t)(tx >> 24));

	spi_transmit((uint8_t)(ty));
	spi_transmit((uint8_t)(ty >> 8));
	spi_transmit((uint8_t)(ty >> 16));
	spi_transmit((uint8_t)(ty >> 24));

	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_scale(int32_t sx, int32_t sy)
{
	FT8_start_cmd(CMD_SCALE);

	spi_transmit((uint8_t)(sx));
	spi_transmit((uint8_t)(sx >> 8));
	spi_transmit((uint8_t)(sx >> 16));
	spi_transmit((uint8_t)(sx >> 24));

	spi_transmit((uint8_t)(sy));
	spi_transmit((uint8_t)(sy >> 8));
	spi_transmit((uint8_t)(sy >> 16));
	spi_transmit((uint8_t)(sy >> 24));

	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_rotate(int32_t ang)
{
	FT8_start_cmd(CMD_ROTATE);

	spi_transmit((uint8_t)(ang));
	spi_transmit((uint8_t)(ang >> 8));
	spi_transmit((uint8_t)(ang >> 16));
	spi_transmit((uint8_t)(ang >> 24));

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/*	the description in the programmers guide is strange for this funtion
	while it is named *get*matrix, parameters 'a' to 'f' are supplied to
	the function and described as "output parameter"
	best guess is that this one allows to setup the matrix coefficients manually
	instead automagically like with _translate, _scale and _rotate
	if this assumption is correct it rather should be named cmd_setupmatrix() */
void FT8_cmd_getmatrix(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
	FT8_start_cmd(CMD_SETMATRIX);

	spi_transmit((uint8_t)(a));
	spi_transmit((uint8_t)(a >> 8));
	spi_transmit((uint8_t)(a >> 16));
	spi_transmit((uint8_t)(a >> 24));

	spi_transmit((uint8_t)(b));
	spi_transmit((uint8_t)(b >> 8));
	spi_transmit((uint8_t)(b >> 16));
	spi_transmit((uint8_t)(b >> 24));

	spi_transmit((uint8_t)(c));
	spi_transmit((uint8_t)(c >> 8));
	spi_transmit((uint8_t)(c >> 16));
	spi_transmit((uint8_t)(c >> 24));

	spi_transmit((uint8_t)(d));
	spi_transmit((uint8_t)(d >> 8));
	spi_transmit((uint8_t)(d >> 16));
	spi_transmit((uint8_t)(d >> 24));

	spi_transmit((uint8_t)(e));
	spi_transmit((uint8_t)(e >> 8));
	spi_transmit((uint8_t)(e >> 16));
	spi_transmit((uint8_t)(e >> 24));

	spi_transmit((uint8_t)(f));
	spi_transmit((uint8_t)(f >> 8));
	spi_transmit((uint8_t)(f >> 16));
	spi_transmit((uint8_t)(f >> 24));

	FT8_inc_cmdoffset(24);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* other commands: */

void FT8_cmd_calibrate(void)
{
	FT8_start_cmd(CMD_CALIBRATE);

	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_interrupt(uint32_t ms)
{
	FT8_start_cmd(CMD_INTERRUPT);

	spi_transmit((uint8_t)(ms));
	spi_transmit((uint8_t)(ms >> 8));
	spi_transmit((uint8_t)(ms >> 16));
	spi_transmit((uint8_t)(ms >> 24));

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


#ifdef FT8_81X_ENABLE
void FT8_cmd_romfont(uint32_t font, uint32_t romslot)
{
	FT8_start_cmd(CMD_ROMFONT);
	
	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));
	spi_transmit(0x00);
	spi_transmit(0x00);
	
	spi_transmit((uint8_t)(romslot));
	spi_transmit((uint8_t)(romslot >> 8));
	spi_transmit(0x00);
	spi_transmit(0x00);	
	
	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif


void FT8_cmd_setfont(uint32_t font, uint32_t ptr)
{
	FT8_start_cmd(CMD_SETFONT);

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));
	spi_transmit((uint8_t)(font >> 16));
	spi_transmit((uint8_t)(font >> 24));

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


#ifdef FT8_81X_ENABLE
void FT8_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
	FT8_start_cmd(CMD_SETFONT2);

	spi_transmit((uint8_t)(font));
	spi_transmit((uint8_t)(font >> 8));
	spi_transmit((uint8_t)(font >> 16));
	spi_transmit((uint8_t)(font >> 24));

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(firstchar));
	spi_transmit((uint8_t)(firstchar >> 8));
	spi_transmit((uint8_t)(firstchar >> 16));
	spi_transmit((uint8_t)(firstchar >> 24));

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif


#ifdef FT8_81X_ENABLE
void FT8_cmd_setrotate(uint32_t r)
{
	FT8_start_cmd(CMD_SETROTATE);

	spi_transmit((uint8_t)(r));
	spi_transmit((uint8_t)(r >> 8));
	spi_transmit((uint8_t)(r >> 16));
	spi_transmit((uint8_t)(r >> 24));

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif


#ifdef FT8_81X_ENABLE
void FT8_cmd_setscratch(uint32_t handle)
{
	FT8_start_cmd(CMD_SETSCRATCH);

	spi_transmit((uint8_t)(handle));
	spi_transmit((uint8_t)(handle >> 8));
	spi_transmit((uint8_t)(handle >> 16));
	spi_transmit((uint8_t)(handle >> 24));

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif


void FT8_cmd_sketch(int16_t x0, int16_t y0, uint16_t w0, uint16_t h0, uint32_t ptr, uint16_t format)
{
	FT8_start_cmd(CMD_SKETCH);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(format));
	spi_transmit((uint8_t)(format >> 8));

	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_snapshot(uint32_t ptr)
{
	FT8_start_cmd(CMD_SNAPSHOT);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


#ifdef FT8_81X_ENABLE
void FT8_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t x0, int16_t y0, int16_t w0, int16_t h0)
{
	FT8_start_cmd(CMD_SNAPSHOT2);

	spi_transmit((uint8_t)(fmt));
	spi_transmit((uint8_t)(fmt >> 8));
	spi_transmit((uint8_t)(fmt >> 16));
	spi_transmit((uint8_t)(fmt >> 24));

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}
#endif


void FT8_cmd_spinner(int16_t x0, int16_t y0, uint16_t style, uint16_t scale)
{
	FT8_start_cmd(CMD_SPINNER);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(style));
	spi_transmit((uint8_t)(style >> 8));

	spi_transmit((uint8_t)(scale));
	spi_transmit((uint8_t)(scale >> 8));

	FT8_inc_cmdoffset(8);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_track(int16_t x0, int16_t y0, int16_t w0, int16_t h0, int16_t tag)
{
	FT8_start_cmd(CMD_TRACK);

	spi_transmit((uint8_t)(x0));
	spi_transmit((uint8_t)(x0 >> 8));

	spi_transmit((uint8_t)(y0));
	spi_transmit((uint8_t)(y0 >> 8));

	spi_transmit((uint8_t)(w0));
	spi_transmit((uint8_t)(w0 >> 8));

	spi_transmit((uint8_t)(h0));
	spi_transmit((uint8_t)(h0 >> 8));

	spi_transmit((uint8_t)(tag));
	spi_transmit((uint8_t)(tag >> 8));

	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* commands that return values by writing to the command-fifo */
/* this is handled by having this functions return the offset address on the command-fifo from
   which the results can be fetched after execution: FT8_memRead32(FT8_RAM_CMD + offset) */
/* note: yes, these are different than the functions in the Programmers Guide from FTDI,
	this is because I have no idea why anyone would want to pass "result" as an actual argument to these functions
	when this only marks the offset the command-prozessor is writing to, it may even be okay to not transfer anything at all,
	just advance the offset by 4 bytes */

/*
example of using FT8_cmd_memcrc:

 offset = FT8_cmd_memcrc(my_ptr_to_some_memory_region, some_amount_of_bytes);
 FT8_cmd_execute();
 crc32 = FT8_memRead32(FT8_RAM_CMD + offset);

*/

uint16_t FT8_cmd_memcrc(uint32_t ptr, uint32_t num)
{
	uint16_t offset;

	FT8_start_cmd(CMD_MEMCRC);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit((uint8_t)(num));
	spi_transmit((uint8_t)(num >> 8));
	spi_transmit((uint8_t)(num >> 16));
	spi_transmit((uint8_t)(num >> 24));

	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(8);
	offset = cmdOffset;
	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}

	return offset;
}


uint16_t FT8_cmd_getptr(void)
{
	uint16_t offset;

	FT8_start_cmd(CMD_GETPTR);

	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	offset = cmdOffset;
	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}

	return offset;
}


uint16_t FT8_cmd_regread(uint32_t ptr)
{
	uint16_t offset;

	FT8_start_cmd(CMD_REGREAD);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(4);
	offset = cmdOffset;
	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}

	return offset;
}


/* be aware that this returns the first offset pointing to "width", in order to also read
"height" you need to:

 offset = FT8_cmd_getprops(my_last_picture_pointer);
 FT8_cmd_execute();
 width = FT8_memRead32(FT8_RAM_CMD + offset);
 offset += 4;
 offset &= 0x0fff;
 height = FT8_memRead32(FT8_RAM_CMD + offset);
*/

uint16_t FT8_cmd_getprops(uint32_t ptr)
{
	uint16_t offset;

	FT8_start_cmd(CMD_REGREAD);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);
	spi_transmit(0);

	FT8_inc_cmdoffset(4);
	offset = cmdOffset;
	FT8_inc_cmdoffset(4);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}

	return offset;
}


/* meta-commands, sequences of several display-list entries condensed into simpler to use functions at the price of some overhead */

void FT8_cmd_point(int16_t x0, int16_t y0, uint16_t size)
{
	uint32_t calc;

	FT8_start_cmd((DL_BEGIN | FT8_POINTS));

	calc = POINT_SIZE(size*16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	calc = VERTEX2F(x0 * 16, y0 * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	spi_transmit((uint8_t)(DL_END));
	spi_transmit((uint8_t)(DL_END >> 8));
	spi_transmit((uint8_t)(DL_END >> 16));
	spi_transmit((uint8_t)(DL_END >> 24));

	FT8_inc_cmdoffset(12);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t width)
{
	uint32_t calc;

	FT8_start_cmd((DL_BEGIN | FT8_LINES));

	calc = LINE_WIDTH(width * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	calc = VERTEX2F(x0 * 16, y0 * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	calc = VERTEX2F(x1 * 16, y1 * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	spi_transmit((uint8_t)(DL_END));
	spi_transmit((uint8_t)(DL_END >> 8));
	spi_transmit((uint8_t)(DL_END >> 16));
	spi_transmit((uint8_t)(DL_END >> 24));

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


void FT8_cmd_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t corner)
{
	uint32_t calc;

	FT8_start_cmd((DL_BEGIN | FT8_RECTS));

	calc = LINE_WIDTH(corner * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	calc = VERTEX2F(x0 * 16, y0 * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	calc = VERTEX2F(x1 * 16, y1 * 16);
	spi_transmit((uint8_t)(calc));
	spi_transmit((uint8_t)(calc >> 8));
	spi_transmit((uint8_t)(calc >> 16));
	spi_transmit((uint8_t)(calc >> 24));

	spi_transmit((uint8_t)(DL_END));
	spi_transmit((uint8_t)(DL_END >> 8));
	spi_transmit((uint8_t)(DL_END >> 16));
	spi_transmit((uint8_t)(DL_END >> 24));

	FT8_inc_cmdoffset(16);

	if(cmd_burst == 0)
	{
		FT8_cs_clear();
	}
}


/* init, has to be executed with the SPI setup to 11 MHz or less as required by FT8xx */

uint8_t FT8_init(void)
{
	uint8_t gpio;
	uint8_t chipid;
	uint8_t timeout = 0;

	FT8_pdn_set();
	DELAY_MS(6);	/* minimum time for power-down is 5ms */
	FT8_pdn_clear();
	DELAY_MS(21);	/* minimum time to allow from rising PD_N to first access is 20ms */
	
/*	FT8_cmdWrite(FT8_CORERST);*/ /* reset, only required for warmstart if PowerDown line is not used */

	if(FT8_HAS_CRYSTAL != 0)
	{
		FT8_cmdWrite(FT8_CLKEXT);	/* setup FT8xx for external clock */
	}
	else
	{
		FT8_cmdWrite(FT8_CLKINT);	/* setup FT8xx for internal clock */
	}

	FT8_cmdWrite(FT8_ACTIVE);	/* start FT8xx */

	chipid = FT8_memRead8(REG_ID);	/* Read ID register */
	while(chipid != 0x7C)	/* if chipid is not 0x7c, continue to read it until it is, FT81x may need a moment for it's power on selftest */
	{
		chipid = FT8_memRead8(REG_ID);
		DELAY_MS(1);
		timeout++;
		if(timeout > 200)
		{
			return 0;
		}
	}

	FT8_memWrite8(REG_PCLK, 0x00);		/* set PCLK to zero - don't clock the LCD until later */
	FT8_memWrite8(REG_PWM_DUTY, 10);		/* turn off backlight */

	/* Initialize Display */
	FT8_memWrite16(REG_HSIZE,   FT8_HSIZE);	/* active display width */
	FT8_memWrite16(REG_HCYCLE,  FT8_HCYCLE);	/* total number of clocks per line, incl front/back porch */
	FT8_memWrite16(REG_HOFFSET, FT8_HOFFSET);	/* start of active line */
	FT8_memWrite16(REG_HSYNC0,  FT8_HSYNC0);	/* start of horizontal sync pulse */
	FT8_memWrite16(REG_HSYNC1,  FT8_HSYNC1);	/* end of horizontal sync pulse */
	FT8_memWrite16(REG_VSIZE,   FT8_VSIZE);	/* active display height */
	FT8_memWrite16(REG_VCYCLE,  FT8_VCYCLE);	/* total number of lines per screen, incl pre/post */
	FT8_memWrite16(REG_VOFFSET, FT8_VOFFSET);	/* start of active screen */
	FT8_memWrite16(REG_VSYNC0,  FT8_VSYNC0);	/* start of vertical sync pulse */
	FT8_memWrite16(REG_VSYNC1,  FT8_VSYNC1);	/* end of vertical sync pulse */
	FT8_memWrite8(REG_SWIZZLE,  FT8_SWIZZLE);	/* FT8xx output to LCD - pin order */
	FT8_memWrite8(REG_PCLK_POL, FT8_PCLKPOL);	/* LCD data is clocked in on this PCLK edge */
	/* Don't set PCLK yet - wait for just after the first display list */

	/* Configure Touch */
	FT8_memWrite8(REG_TOUCH_MODE, FT8_TMODE_CONTINUOUS);	/* enable touch */
	FT8_memWrite16(REG_TOUCH_RZTHRESH, FT8_TOUCH_RZTHRESH);	/* eliminate any false touches */

	/* Configure Audio - not used, so disable it */
	FT8_memWrite8(REG_VOL_PB, 0x00);	/* turn recorded audio volume down */
/*	FT8_memWrite8(REG_VOL_SOUND, 0xff); */	/* turn synthesizer volume on */
	FT8_memWrite8(REG_VOL_SOUND, 0x00);	/* turn synthesizer volume off */
	FT8_memWrite16(REG_SOUND, 0x6000);	/*	set synthesizer to mute */

	FT8_memWrite32(FT8_RAM_DL, DL_CLEAR_RGB);
	FT8_memWrite32(FT8_RAM_DL + 4, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
	FT8_memWrite32(FT8_RAM_DL + 8, DL_DISPLAY);	/* end of display list */
	FT8_memWrite32(REG_DLSWAP, FT8_DLSWAP_FRAME);

	/* nothing is being displayed yet... the pixel clock is still 0x00 */

	gpio = FT8_memRead8(REG_GPIO_DIR);
	gpio |= 0x82; /* set DISP to Output although it always is output, set GPIO1 to Output - Audio Enable on VM800B */
	FT8_memWrite8(REG_GPIO_DIR, gpio);

	gpio = FT8_memRead8(REG_GPIO);	/* read the FT8xx GPIO register for a read/modify/write operation */
/*	gpio |= 0x82; */					/* set bit 7 of FT8xx GPIO register (DISP), set GPIO1 to High to enable Audio - others are inputs */
	gpio |= 0x80;						/* set bit 7 of FT8xx GPIO register (DISP), others are inputs */
	FT8_memWrite8(REG_GPIO, gpio);	/* enable the DISP signal to the LCD panel */
	FT8_memWrite8(REG_PCLK, FT8_PCLK);	/* now start clocking data to the LCD panel */

	FT8_memWrite8(REG_PWM_DUTY, 70);	/* turn on backlight */

	DELAY_MS(2);	/* just to be safe */
	
	while(FT8_busy() == 1);
	FT8_get_cmdoffset();
	return 1;
}
