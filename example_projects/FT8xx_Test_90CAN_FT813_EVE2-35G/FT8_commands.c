/*
@file    FT8_commands.c
@brief   Contains Functions for using the FT8xx
@version 3.12
@date    2018-07-08
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

3.7
- Added FT8_cmd_start(), a non-blocking variant of FT8_cmd_execute() to be used at the end of a display-list update.
	Thanks for pointing out that oversight to user "Peter" of Mikrocontroller.net!

3.8
- Added setting of REG_CSPRED to FT8_init() as new parameter, some Matrix Orbital modules use '0' instead of the reset-default '1'.

3.9
- Added patching of the touch-interface for GT911 to FT8_init() if required by the module attached
- simplified FT8_init() a bit
- some house-keeping, fixing typos in comments left and right

3.10
- FT8_cmd_inflate() supports binaries >4k now and does not need to be executed anymore

3.11
- added fault-detection and co-processor reset to  FT8_busy(), this allows the FT8xx to continue to work even after beeing supplied with bad image data for example

3.12
- changed the way FT8_HAS_CRYSTAL works
- added special treatment for switching on and off the backlight in FT8_init() for ADAM101 modules.
- default backlight value after FT8_init() is 25% now

*/

#include "FT8.h"
#include "FT8_config.h"


/* FT8xx Memory Commands - used with FT8_memWritexx and FT8_memReadxx */
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
	uint8_t ftData8;
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_READ); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8)); /* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));	/* send low address byte */
	spi_transmit(0x00);	/* send dummy byte */
	ftData8 = spi_receive(0x00); /* read data byte by sending another dummy byte */
	FT8_cs_clear();
	return ftData8;	/* return byte read */
}


uint16_t FT8_memRead16(uint32_t ftAddress)
{
	uint16_t ftData16 = 0;
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_READ); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8)); /* send middle address byte */
	spi_transmit((uint8_t)(ftAddress)); /* send low address byte */
	spi_transmit(0x00);	/* send dummy byte */
	ftData16 = (spi_receive(0x00));	/* read low byte */
	ftData16 = (spi_receive(0x00) << 8) | ftData16;	/* read high byte */
	FT8_cs_clear();
	return ftData16; /* return integer read */
}


uint32_t FT8_memRead32(uint32_t ftAddress)
{
	uint32_t ftData32= 0;
	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_READ); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8)); /* send middle address byte */
	spi_transmit((uint8_t)(ftAddress));	/* send low address byte */
	spi_transmit(0x00);	/* send dummy byte */
	ftData32 = ((uint32_t)spi_receive(0x00)); /* read low byte */
	ftData32 = ((uint32_t)spi_receive(0x00) << 8) | ftData32;
	ftData32 = ((uint32_t)spi_receive(0x00) << 16) | ftData32;
	ftData32 = ((uint32_t)spi_receive(0x00) << 24) | ftData32; /* read high byte */
	FT8_cs_clear();
	return ftData32; /* return long read */
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
	spi_transmit((uint8_t)(ftAddress >> 8)); /* send middle address byte */
	spi_transmit((uint8_t)(ftAddress)); /* send low address byte */
	spi_transmit((uint8_t)(ftData16)); /* send data low byte */
	spi_transmit((uint8_t)(ftData16 >> 8));  /* send data high byte */
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

	if(cmdBufferRead == 0xFFF)
	{
		FT8_memWrite8(REG_CPURESET, 1);		/* hold co-processor engine in the reset condition */
		FT8_memWrite16(REG_CMD_READ, 0);	/* set REG_CMD_READ to 0 */
		FT8_memWrite16(REG_CMD_WRITE, 0);	/* set REG_CMD_WRITE to 0 */
		cmdOffset = 0;						/* reset cmdOffset */
		FT8_memWrite8(REG_CPURESET, 0);		/* set REG_CMD_WRITE to 0 to restart the co-processor engine*/
	}

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


/* order the command co-processor to start processing its FIFO queue and do not wait for completion */
void FT8_cmd_start(void)
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
}


/* order the command co-processor to start processing its FIFO queue and wait for completion */
void FT8_cmd_execute(void)
{
	FT8_cmd_start();
	while (FT8_busy());
}


void FT8_get_cmdoffset(void)
{
	cmdOffset = FT8_memRead16(REG_CMD_WRITE);
}


/* make current value of cmdOffset available while limiting access to that var to the FT8_commands module */
uint16_t FT8_report_cmdoffset(void)
{
	return (cmdOffset);
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


/* Begin a co-processor command */
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


/* Write a string to co-processor memory in context of a command: no chip-select, just plain SPI-transfers */
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


void block_transfer(const uint8_t *data, uint16_t len)
{
	uint16_t bytes_left;
	uint16_t block_len;
	uint32_t ftAddress;	

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


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void FT8_cmd_inflate(uint32_t ptr, const uint8_t *data, uint16_t len)
{
	FT8_start_cmd(CMD_INFLATE);

	spi_transmit((uint8_t)(ptr));
	spi_transmit((uint8_t)(ptr >> 8));
	spi_transmit((uint8_t)(ptr >> 16));
	spi_transmit((uint8_t)(ptr >> 24));

	FT8_inc_cmdoffset(4);
	FT8_cs_clear();
	
	block_transfer(data, len);
}


/* this is meant to be called outside display-list building, it includes executing the command and waiting for completion, does not support cmd-burst */
void FT8_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *data, uint16_t len)
{
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
		block_transfer(data, len);
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


/*	the description in the programmers guide is strange for this function
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
	when this only marks the offset the command-processor is writing to, it may even be okay to not transfer anything at all,
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


/* FT811 / FT813 binary-blob from FTDIs AN_336 to patch the touch-engine for Goodix GT911 / GT9271 touch controllers */
#ifdef FT8_HAS_GT911

#if	defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

const uint16_t FT8_GT911_len = 1184;
const uint8_t FT8_GT911_data[1184] PROGMEM =
{
 26,255,255,255,32,32,48,0,4,0,0,0,2,0,0,0,
 34,255,255,255,0,176,48,0,120,218,237,84,221,111,84,69,20,63,51,179,93,160,148,101,111,76,5,44,141,123,111,161,11,219,154,16,9,16,17,229,156,75,26,11,13,21,227,3,16,252,184,179,
 45,219,143,45,41,125,144,72,67,100,150,71,189,113,18,36,17,165,100,165,198,16,32,17,149,196,240,128,161,16,164,38,54,240,0,209,72,130,15,38,125,48,66,82,30,76,19,31,172,103,46,
 139,24,255,4,227,157,204,156,51,115,102,206,231,239,220,5,170,94,129,137,75,194,216,98,94,103,117,115,121,76,131,177,125,89,125,82,123,60,243,58,142,242,204,185,243,188,118,156,
 227,155,203,238,238,195,251,205,229,71,92,28,169,190,184,84,143,113,137,53,244,103,181,237,87,253,113,137,233,48,12,198,165,181,104,139,25,84,253,155,114,74,191,0,54,138,163,
 12,62,131,207,129,23,217,34,91,31,128,65,246,163,175,213,8,147,213,107,35,203,94,108,3,111,40,171,83,24,15,165,177,222,116,97,23,188,140,206,150,42,102,181,87,78,86,182,170,134,
 215,241,121,26,243,252,2,76,115,217,139,222,206,173,136,132,81,61,35,185,39,113,23,46,199,76,178,54,151,183,224,0,40,189,28,149,182,58,131,79,152,30,76,34,98,234,162,216,133,141,
 102,39,170,40,192,101,53,201,146,191,37,77,44,177,209,74,211,5,206,187,5,6,216,47,53,96,123,22,50,103,251,192,84,17,74,227,185,56,106,51,91,161,96,182,163,48,171,141,139,65,152,
 66,66,11,102,43,158,75,36,80,147,184,147,139,112,17,235,216,103,111,239,245,92,10,175,194,40,44,58,125,5,59,112,50,103,245,4,78,192,5,156,194,51,60,191,134,75,110,173,237,46,192,
 121,156,192,115,184,218,120,67,63,115,46,11,102,10,97,232,50,235,114,182,148,118,178,41,188,12,135,77,202,124,12,96,238,35,161,234,189,129,23,249,212,139,230,25,53,48,205,52,93,
 163,117,53,154,170,81,85,163,178,70,69,66,167,241,14,46,241,1,226,136,152,179,197,59,184,148,254,49,132,48,15,176,137,192,76,131,196,105,104,162,86,81,160,165,255,26,173,162,137,
 86,145,210,183,192,55,175,194,211,60,91,120,230,184,174,27,41,131,155,40,224,29,87,179,232,16,55,55,7,165,147,81,23,165,49,101,54,224,75,180,81,108,18,29,226,69,225,110,175,224,
 42,212,25,47,130,193,110,234,192,215,252,56,74,162,24,46,251,174,54,106,68,245,14,9,155,160,22,120,207,104,240,29,90,178,140,28,24,220,47,166,112,61,251,208,192,111,56,239,238,
 93,255,251,62,99,32,193,75,61,190,235,123,229,110,218,194,85,79,225,59,98,20,238,227,235,220,11,221,149,25,180,116,194,159,111,96,192,24,213,59,139,179,156,215,69,230,19,24,35,
 135,117,206,171,206,162,67,129,234,61,235,11,104,103,84,64,223,167,254,40,163,101,92,84,43,150,46,249,219,205,7,116,11,91,104,61,57,75,223,8,48,25,28,119,252,222,113,49,86,249,
 74,180,211,156,181,61,215,168,157,7,251,199,150,242,250,91,58,132,94,121,7,53,151,139,98,6,165,153,69,214,32,110,211,100,101,31,89,45,81,98,23,205,205,197,209,109,186,198,35,
 141,191,249,25,60,132,223,153,251,98,20,239,146,139,20,217,250,41,250,137,58,177,90,57,79,51,108,233,20,253,194,187,49,222,205,114,141,96,48,175,219,107,54,111,138,22,154,103,
 108,79,58,252,179,178,79,164,195,2,153,36,39,170,199,201,167,197,85,106,8,59,177,81,46,56,2,230,75,114,17,55,112,188,65,208,137,77,114,10,115,55,58,208,197,173,122,87,6,140,
 110,42,208,124,163,70,108,241,104,18,245,98,214,187,134,53,42,221,22,182,133,211,116,148,177,194,209,192,85,90,199,58,55,203,2,229,19,137,187,161,228,154,112,203,145,125,244,
 188,220,118,228,41,201,181,41,195,144,215,183,51,80,250,21,217,16,217,200,235,109,227,188,122,218,142,60,170,224,112,240,184,130,229,224,113,5,223,148,163,80,165,183,130,187,
 132,116,64,238,161,85,220,115,139,205,98,227,244,29,102,125,7,37,243,123,223,11,26,92,63,243,116,61,191,138,123,244,160,84,186,74,31,5,174,247,119,135,199,248,253,135,242,97,
 102,145,190,144,14,85,238,221,231,193,158,48,205,25,120,248,15,220,29,158,9,70,185,30,103,229,33,254,23,237,160,172,62,193,90,222,224,232,14,200,56,90,104,142,227,120,110,6,
 21,211,203,65,150,99,151,220,247,87,164,50,159,49,239,234,58,142,0,109,108,123,18,79,227,36,100,248,222,205,96,127,120,26,171,228,69,63,36,17,252,200,17,116,242,187,227,88,143,
 247,2,75,191,6,130,59,188,11,55,240,31,243,122,152,226,183,207,154,73,188,39,219,43,105,222,87,41,143,141,140,175,73,112,184,252,61,184,16,90,250,35,168,82,119,176,57,116,94,
 200,150,22,190,179,44,104,12,235,84,149,102,252,89,154,193,99,228,106,242,125,248,64,194,255,223,127,242,83,11,255,2,70,214,226,128,0,0
};
#endif


/* init, has to be executed with the SPI setup to 11 MHz or less as required by FT8xx */
uint8_t FT8_init(void)
{
	uint8_t chipid;
	uint8_t timeout = 0;

	FT8_pdn_set();
	DELAY_MS(6);	/* minimum time for power-down is 5ms */
	FT8_pdn_clear();
	DELAY_MS(21);	/* minimum time to allow from rising PD_N to first access is 20ms */
	
/*	FT8_cmdWrite(FT8_CORERST);*/ /* reset, only required for warmstart if PowerDown line is not used */

#if defined (FT8_HAS_CRYSTAL)
	FT8_cmdWrite(FT8_CLKEXT);	/* setup FT8xx for external clock */
#else
	FT8_cmdWrite(FT8_CLKINT);	/* setup FT8xx for internal clock */
#endif

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

/* we have a display with a Goodix GT911 / GT9271 touch-controller on it, so we patch our FT811 or FT813 according to AN_336 */
#if defined (FT8_HAS_GT911)
	uint32_t ftAddress;

	FT8_get_cmdoffset();
	ftAddress = FT8_RAM_CMD + cmdOffset;

	FT8_cs_set();
	spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
	spi_transmit((uint8_t)(ftAddress >> 8)); /* send middle address byte */
	spi_transmit((uint8_t)(ftAddress)); /* send low address byte */
	spi_flash_write(FT8_GT911_data, FT8_GT911_len);
	FT8_cs_clear();
	FT8_cmd_execute();

	FT8_memWrite8(REG_TOUCH_OVERSAMPLE, 0x0f); /* setup oversample to 0x0f as "hidden" in binary-blob for AN_336 */
	FT8_memWrite16(REG_TOUCH_CONFIG, 0x05D0); /* write magic cookie as requested by AN_336 */

	/* specific to the EVE2 modules from Matrix-Orbital we have to use GPIO3 to reset GT911 */
	FT8_memWrite16(REG_GPIOX_DIR,0x8008); /* Reset-Value is 0x8000, adding 0x08 sets GPIO3 to output, default-value for REG_GPIOX is 0x8000 -> Low output on GPIO3 */
	DELAY_MS(1); /* wait more than 100µs */
	FT8_memWrite8(REG_CPURESET, 0x00); /* clear all resets */
	DELAY_MS(56); /* wait more than 55ms */
	FT8_memWrite16(REG_GPIOX_DIR,0x8000); /* setting GPIO3 back to input */
#endif

/*	FT8_memWrite8(REG_PCLK, 0x00);	*/	/* set PCLK to zero - don't clock the LCD until later, line disabled because zero is reset-default and we just did a reset */

#if defined (FT8_ADAM101)
	FT8_memWrite8(REG_PWM_DUTY, 0x80);	/* turn off backlight for Glyn ADAM101 module, it uses inverted values */
#else
	FT8_memWrite8(REG_PWM_DUTY, 0);		/* turn off backlight for any other module */
#endif

	/* Initialize Display */
	FT8_memWrite16(REG_HSIZE,   FT8_HSIZE);		/* active display width */
	FT8_memWrite16(REG_HCYCLE,  FT8_HCYCLE);	/* total number of clocks per line, incl front/back porch */
	FT8_memWrite16(REG_HOFFSET, FT8_HOFFSET);	/* start of active line */
	FT8_memWrite16(REG_HSYNC0,  FT8_HSYNC0);	/* start of horizontal sync pulse */
	FT8_memWrite16(REG_HSYNC1,  FT8_HSYNC1);	/* end of horizontal sync pulse */
	FT8_memWrite16(REG_VSIZE,   FT8_VSIZE);		/* active display height */
	FT8_memWrite16(REG_VCYCLE,  FT8_VCYCLE);	/* total number of lines per screen, incl pre/post */
	FT8_memWrite16(REG_VOFFSET, FT8_VOFFSET);	/* start of active screen */
	FT8_memWrite16(REG_VSYNC0,  FT8_VSYNC0);	/* start of vertical sync pulse */
	FT8_memWrite16(REG_VSYNC1,  FT8_VSYNC1);	/* end of vertical sync pulse */
	FT8_memWrite8(REG_SWIZZLE,  FT8_SWIZZLE);	/* FT8xx output to LCD - pin order */
	FT8_memWrite8(REG_PCLK_POL, FT8_PCLKPOL);	/* LCD data is clocked in on this PCLK edge */
	FT8_memWrite8(REG_CSPREAD,	FT8_CSPREAD);	/* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
	
	/* Don't set PCLK yet - wait for just after the first display list */

	/* configure Touch */
	FT8_memWrite8(REG_TOUCH_MODE, FT8_TMODE_CONTINUOUS);	/* enable touch */
	FT8_memWrite16(REG_TOUCH_RZTHRESH, FT8_TOUCH_RZTHRESH);	/* eliminate any false touches */

	/* disable Audio for now */
	FT8_memWrite8(REG_VOL_PB, 0x00);	/* turn recorded audio volume down */
	FT8_memWrite8(REG_VOL_SOUND, 0x00);	/* turn synthesizer volume off */
	FT8_memWrite16(REG_SOUND, 0x6000);	/*	set synthesizer to mute */

	/* write a basic display-list to get things started */
	FT8_memWrite32(FT8_RAM_DL, DL_CLEAR_RGB);
	FT8_memWrite32(FT8_RAM_DL + 4, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
	FT8_memWrite32(FT8_RAM_DL + 8, DL_DISPLAY);	/* end of display list */
	FT8_memWrite32(REG_DLSWAP, FT8_DLSWAP_FRAME);

	/* nothing is being displayed yet... the pixel clock is still 0x00 */
	FT8_memWrite8(REG_GPIO, 0x80); /* enable the DISP signal to the LCD panel, it is set to output in REG_GPIO_DIR by default */
	FT8_memWrite8(REG_PCLK, FT8_PCLK); /* now start clocking data to the LCD panel */

#if defined (FT8_ADAM101)
	FT8_memWrite8(REG_PWM_DUTY, 0x60);	/* turn on backlight to 25% for Glyn ADAM101 module, it uses inverted values */
#else
	FT8_memWrite8(REG_PWM_DUTY, 0x20);	/* turn on backlight to 25% for any other module */
#endif

	DELAY_MS(2); /* just to be safe */
	while(FT8_busy() == 1); /* just to be safe, should return immediately */
	FT8_get_cmdoffset(); /* just to be safe */
	return 1;
}
