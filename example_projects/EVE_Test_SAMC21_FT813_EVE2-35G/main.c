/*
@file    main.c
@brief   main
@version 1.1
@date    2020-02-08
@author  Rudolph Riedel

@section History

1.0
- restart with SAMC21 and EVE 4.0

1.1
- switched to hardware-SPI using SERCOM0
- added using Timer4 for the time spent in TFT_loop()

1.2
- updated to be more similar to what I am currently using in projects

 */


#include "sam.h"

//#include "EVE_commands.h"
#include "tft.h"


volatile uint8_t system_tick = 0;

void SysTick_Handler(void)
{
	system_tick = 42;
}


void init_clock(void)
{
	REG_NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS(2);	// Set the NVM Read Wait States to 2, Since the operating frequency will be 48 MHz

	REG_OSCCTRL_XOSCCTRL =  OSCCTRL_XOSCCTRL_STARTUP(6) |		// 1,953 ms
							OSCCTRL_XOSCCTRL_RUNSTDBY |
							OSCCTRL_XOSCCTRL_AMPGC |
							OSCCTRL_XOSCCTRL_GAIN(3) |
							OSCCTRL_XOSCCTRL_XTALEN |
							OSCCTRL_XOSCCTRL_ENABLE;

	while((REG_OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY) == 0);

	/* configure the PLL, source = XOSC, pre-scaler = 8, multiply by 24 -> 48MHz clock from 16MHz input */
	REG_OSCCTRL_DPLLCTRLB = OSCCTRL_DPLLCTRLB_DIV(3) | OSCCTRL_DPLLCTRLB_REFCLK(1); // setup PLL to use XOSC input
	REG_OSCCTRL_DPLLRATIO = OSCCTRL_DPLLRATIO_LDRFRAC(0x0) | OSCCTRL_DPLLRATIO_LDR(23);
	REG_OSCCTRL_DPLLCTRLA = OSCCTRL_DPLLCTRLA_RUNSTDBY | OSCCTRL_DPLLCTRLA_ENABLE;
	while((REG_OSCCTRL_DPLLSTATUS & OSCCTRL_DPLLSTATUS_CLKRDY) != 0); // wait for the pll to be ready

	REG_GCLK_GENCTRL0 = GCLK_GENCTRL_DIV(0) |
						GCLK_GENCTRL_RUNSTDBY |
						GCLK_GENCTRL_GENEN |
						//GCLK_GENCTRL_SRC_XOSC |
						GCLK_GENCTRL_SRC_DPLL96M |
						GCLK_GENCTRL_IDC ;

	while((REG_GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0) != 0);	/* wait for the synchronization between clock domain to be complete */

	REG_OSCCTRL_OSC48MDIV = OSCCTRL_OSC48MDIV_DIV(0);	// set 48MHz Oscillator to 48MHz ouput

	REG_GCLK_GENCTRL1 = GCLK_GENCTRL_DIV(0) |
	GCLK_GENCTRL_RUNSTDBY |
	GCLK_GENCTRL_GENEN |
	GCLK_GENCTRL_SRC_OSC48M |
	//		GCLK_GENCTRL_SRC_DPLL96M |
	GCLK_GENCTRL_IDC ;

	while((REG_GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL1) != 0);	// wait for the synchronization between clock domains is complete
}


void set_spi_speed(uint8_t baudval)
{
	if(REG_SERCOM0_SPI_CTRLA & SERCOM_SPI_CTRLA_ENABLE) /* SPI already is active, need to disable first and re-activate */
	{
		REG_SERCOM0_SPI_CTRLA &= ~SERCOM_SPI_CTRLA_ENABLE; /* disable SERCOM0 -> enable config */
		while(REG_SERCOM0_SPI_SYNCBUSY & SERCOM_SPI_SYNCBUSY_ENABLE); /* wait for SERCOM0 to be ready */
		REG_SERCOM0_SPI_BAUD = baudval; /* 48 / (2 * (baudval + 1)) -> @48Mhz: 0 = 24MHz, 1 = 12MHz, 2 = 8MHz, 3 = 6MHz */
		REG_SERCOM0_SPI_CTRLA |= SERCOM_SPI_CTRLA_ENABLE; /* activate SERCOM0 */
		while(REG_SERCOM0_SPI_SYNCBUSY & SERCOM_SPI_SYNCBUSY_ENABLE); /* wait for SERCOM0 to be ready */
	}
	else
	{
		REG_SERCOM0_SPI_BAUD = baudval; /* 48 / (2 * (baudval + 1)) -> @48Mhz: 0 = 24MHz, 1 = 12MHz, 2 = 8MHz, 3 = 6MHz */
	}
}


void init_spi(void)
{
	REG_PORT_DIRSET0 = PORT_PA03; /* PD_TFT */
	REG_PORT_DIRSET0 = PORT_PA05; /* CS_TFT */
	REG_PORT_OUTCLR0 = PORT_PA03; /* PD_TFT */
	REG_PORT_OUTSET0 = PORT_PA05; /* CS_TFT */

	/* configure SERCOM0 MISO on PA04 */
	REG_PORT_WRCONFIG0 =
	PORT_WRCONFIG_WRPINCFG |
	PORT_WRCONFIG_WRPMUX |
	PORT_WRCONFIG_PMUX(3) |		/* SERCOM0 */
	PORT_WRCONFIG_INEN |
	PORT_WRCONFIG_PINMASK(0x10) |	/* PA04 */
	PORT_WRCONFIG_PMUXEN;

	/* configure SERCOM0 MOSI on PA06 and SERCOM0 SCK on PA07 */
	REG_PORT_WRCONFIG0 =
	PORT_WRCONFIG_WRPINCFG |
	PORT_WRCONFIG_WRPMUX |
	PORT_WRCONFIG_PMUX(3) |		/* SERCOM0 */
	PORT_WRCONFIG_DRVSTR |
	PORT_WRCONFIG_PINMASK(0xc0) | /* PA06 + PA07 */
	PORT_WRCONFIG_PMUXEN;

	REG_MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM0;
	REG_GCLK_PCHCTRL19 = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN; /* setup SERCOM0 to use GLCK0 -> 48MHz */

	REG_SERCOM0_SPI_CTRLA = 0x00; /* disable SPI -> enable config */
	REG_SERCOM0_SPI_CTRLA = SERCOM_SPI_CTRLA_MODE(3) | SERCOM_SPI_CTRLA_DOPO(1); /* MSB first, CPOL = 0, CPHA = 0, SPI frame, master mode, PAD0 = MISO, PAD2 = MOSI, PAD3 = SCK */
	set_spi_speed(2); /* 48 / (2 * (2 + 1)) -> 48 / 6 -> 8MHZ clock from 48MHz core-clock */
	REG_SERCOM0_SPI_CTRLB = SERCOM_SPI_CTRLB_RXEN; /* receiver enabled, no hardware select, 8-bit */
	REG_SERCOM0_SPI_CTRLA |= SERCOM_SPI_CTRLA_ENABLE; /* activate SERCOM0 */
	while(REG_SERCOM0_SPI_SYNCBUSY & SERCOM_SPI_SYNCBUSY_ENABLE); /* wait for SERCOM0 to be ready */
}


void init_timer(void)
{
	REG_MCLK_APBCMASK |= MCLK_APBCMASK_TC4;
	REG_GCLK_PCHCTRL32 = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN; /* setup TC4 to use GLCK0 -> 48MHz */
	
	REG_TC4_CTRLA = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV16 | TC_CTRLA_ENABLE;
	while(REG_TC4_SYNCBUSY & TC_SYNCBUSY_ENABLE); /* wait for TC4 to be ready */
}


int main(void)
{
	uint8_t display_delay = 0;
	uint8_t led_delay = 0;
		
	init_clock();
	init_spi();
	init_timer();
	SysTick_Config(48000000 / 200); // configure and Enable Systick for 5 ms ticks

	REG_PORT_DIRSET0 = PORT_PA27; /* Debug-LED */

	TFT_init();
	set_spi_speed(1); /* speed up to 12MHz after init, with 24MHz touch is not working on my HW... */

	while (1)
	{
		if(system_tick)
		{
			system_tick = 0;

			led_delay++;
			if(led_delay > 39)
			{
				led_delay = 0;
				REG_PORT_OUTTGL0 = PORT_PA27;
			}

			REG_TC4_COUNT16_COUNT = 0;
			REG_TC4_CTRLBSET = TC_CTRLBSET_CMD_UPDATE;
			while((REG_TC4_CTRLBSET & TC_CTRLBSET_MASK) == 0);

			TFT_touch();
			
			REG_TC4_CTRLBSET = TC_CTRLBSET_CMD_READSYNC;
			while((REG_TC4_CTRLBSET & TC_CTRLBSET_MASK) == 0);

			num_profile_b = REG_TC4_COUNT16_COUNT;
			num_profile_b /= 3; /* 1?s */

			display_delay++;
			if(display_delay > 3)
			{
				display_delay = 0;

				REG_TC4_COUNT16_COUNT = 0;
				REG_TC4_CTRLBSET = TC_CTRLBSET_CMD_UPDATE;
				while((REG_TC4_CTRLBSET & TC_CTRLBSET_MASK) == 0);

				TFT_display();

				REG_TC4_CTRLBSET = TC_CTRLBSET_CMD_READSYNC;
				while((REG_TC4_CTRLBSET & TC_CTRLBSET_MASK) == 0);

				num_profile_a = REG_TC4_COUNT16_COUNT;
				num_profile_a /= 3; /* 1?s */
			}
		}
	}
}
