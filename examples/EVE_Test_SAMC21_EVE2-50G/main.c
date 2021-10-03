/*
@file    main.c
@brief   main
@version 1.2
@date    2020-04-15
@author  Rudolph Riedel

@section History

1.0
- restart with SAMC21 and EVE 4.0

1.1
- switched to hardware-SPI using SERCOM0
- added using Timer4 for the time spent in TFT_loop()

1.2
- generell cleanup, switching from legacy register names


 */


#include "sam.h"
#include "tft.h"


volatile uint8_t system_tick = 0;

void SysTick_Handler(void)
{
	system_tick = 42;
}


void init_clock(void)
{
	NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_RWS(2);	/* set the NVM Read Wait States to 2, since the operating frequency will be 48 MHz */
	
	OSCCTRL->XOSCCTRL.reg =
		OSCCTRL_XOSCCTRL_STARTUP(6) |		/* 1,953 ms */
		OSCCTRL_XOSCCTRL_RUNSTDBY |
		OSCCTRL_XOSCCTRL_AMPGC |
		OSCCTRL_XOSCCTRL_GAIN(3) |
		OSCCTRL_XOSCCTRL_XTALEN |
		OSCCTRL_XOSCCTRL_ENABLE;
	while(0 == OSCCTRL->STATUS.bit.XOSCRDY);

	/* configure the PLL, source = XOSC, pre-scaler = 8, multiply by 24 -> 48MHz clock from 16MHz input */
	OSCCTRL->DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_DIV(3) | OSCCTRL_DPLLCTRLB_REFCLK(1);
	OSCCTRL->DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(0x0) | OSCCTRL_DPLLRATIO_LDR(23);
	OSCCTRL->DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_RUNSTDBY | OSCCTRL_DPLLCTRLA_ENABLE;
	while(0 == OSCCTRL->DPLLSTATUS.bit.CLKRDY); /* wait for the pll to be ready */

	/* configure the clock-generator for the core to use the PLL -> run at 48MHz */
	GCLK->GENCTRL[0].reg =
		GCLK_GENCTRL_DIV(0) |
		GCLK_GENCTRL_RUNSTDBY |
		GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_SRC_DPLL96M |
		GCLK_GENCTRL_IDC ;
	while(1 == GCLK->SYNCBUSY.bit.GENCTRL0); /* wait for the synchronization between clock domains to be complete */

	OSCCTRL->OSC48MDIV.reg = OSCCTRL_OSC48MDIV_DIV(0);	/* set 48MHz Oscillator to 48MHz output */
	
	/* configure the clock-generator 1 to use the 48MHz Oscillator */
	GCLK->GENCTRL[1].reg =
		GCLK_GENCTRL_DIV(0) |
		GCLK_GENCTRL_RUNSTDBY |
		GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_SRC_OSC48M |
		GCLK_GENCTRL_IDC ;
	while(1 == GCLK->SYNCBUSY.bit.GENCTRL1); /* wait for the synchronization between clock domains to be complete */
}


void set_spi_speed(uint8_t baudval)
{
	if(SERCOM0->SPI.CTRLA.bit.ENABLE)  /* SPI already is active, need to disable first and re-activate */
	{
		SERCOM0->SPI.CTRLA.bit.ENABLE = 0; /* disable SERCOM -> enable config */
		while(SERCOM0->SPI.SYNCBUSY.bit.ENABLE);
		SERCOM0->SPI.BAUD.reg = baudval; /* 48 / (2 * (baudval + 1)) -> @48Mhz: 0 = 24MHz, 1 = 12MHz, 2 = 8MHz, 3 = 6MHz */
		SERCOM0->SPI.CTRLA.bit.ENABLE = 1; /* activate SERCOM */
		while(SERCOM0->SPI.SYNCBUSY.bit.ENABLE); /* wait for SERCOM to be ready */
	}
	else
	{
		SERCOM0->SPI.BAUD.reg = baudval; /* 48 / (2 * (baudval + 1)) -> @48Mhz: 0 = 24MHz, 1 = 12MHz, 2 = 8MHz, 3 = 6MHz */
	}
}


void init_spi(void)
{
	PORT->Group[0].DIRSET.reg = PORT_PA03; /* PD_TFT */
	PORT->Group[0].DIRSET.reg = PORT_PA05; /* CS_TFT */
	PORT->Group[0].OUTCLR.reg = PORT_PA03; /* PD_TFT */
	PORT->Group[0].OUTSET.reg = PORT_PA05; /* CS_TFT */

	/* configure SERCOM0 MISO on PA04 */
	PORT->Group[0].WRCONFIG.reg =
		PORT_WRCONFIG_WRPINCFG |
		PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(MUX_PA04D_SERCOM0_PAD0) |
		PORT_WRCONFIG_INEN |
		PORT_WRCONFIG_PINMASK(0x10) |	/* PA04 */
		PORT_WRCONFIG_PMUXEN;

	/* configure SERCOM0 MOSI on PA06 and SERCOM0 SCK on PA07 */
	PORT->Group[0].WRCONFIG.reg =
		PORT_WRCONFIG_WRPINCFG |
		PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(MUX_PA06D_SERCOM0_PAD2) |
		PORT_WRCONFIG_DRVSTR |
		PORT_WRCONFIG_PINMASK(0xc0) | /* PA06 + PA07 */
		PORT_WRCONFIG_PMUXEN;
	
	MCLK->APBCMASK.bit.SERCOM0_ = 1;
	GCLK->PCHCTRL[19].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN; /* setup SERCOM0 to use GLCK0 -> 48MHz */

	SERCOM0->SPI.CTRLA.reg = 0x00; /* disable SPI -> enable config */
	while(SERCOM0->SPI.SYNCBUSY.bit.ENABLE);
	SERCOM0->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE(3) | SERCOM_SPI_CTRLA_DOPO(1); /* MSB first, CPOL = 0, CPHA = 0, SPI frame, master mode, PAD0 = MISO, PAD2 = MOSI, PAD3 = SCK */
	set_spi_speed(2); /* 48 / (2 * (2 + 1)) -> 48 / 6 -> 8MHZ clock from 48MHz core-clock */
	SERCOM0->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN; /* receiver enabled, no hardware select, 8-bit */
	SERCOM0->SPI.CTRLA.bit.ENABLE = 1; /* activate SERCOM0 */
	while(SERCOM0->SPI.SYNCBUSY.bit.ENABLE); /* wait for SERCOM to be ready */
}


void init_timer(void)
{
	MCLK->APBCMASK.bit.TC4_ = 1;
	GCLK->PCHCTRL[32].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN; /* setup TC4 to use GLCK1 -> 48MHz */
	TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV16;
	TC4->COUNT16.CTRLA.bit.ENABLE = 1;
	while(1 == TC4->COUNT16.SYNCBUSY.bit.ENABLE); /* wait for TC4 to be ready */
}


int main(void)
{
	uint8_t display_delay = 0;
	uint8_t led_delay = 0;
	
	init_clock();
	init_spi();
	init_timer();
	SysTick_Config(48000000 / 200); // configure and Enable Systick for 5 ms ticks
	
	PORT->Group[0].DIRSET.reg = PORT_PA27; /* Debug-LED */
	
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
				PORT->Group[0].OUTTGL.reg = PORT_PA27;
			}

			TC4->COUNT16.COUNT.reg = 0;
			TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_UPDATE;
			while(TC4->COUNT16.CTRLBSET.reg);

			TFT_touch();

			TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
			while(TC4->COUNT16.CTRLBSET.reg);
			num_profile_b =	TC4->COUNT16.COUNT.reg;
			num_profile_b /= 3; /* 1µs */

			display_delay++;
			if(display_delay > 3)
			{
				display_delay = 0;

				TC4->COUNT16.COUNT.reg = 0;
				TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_UPDATE;
				while(TC4->COUNT16.CTRLBSET.reg);
			
				TFT_display();

				TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
				while(TC4->COUNT16.CTRLBSET.reg);
				num_profile_a =	TC4->COUNT16.COUNT.reg;
				num_profile_a /= 3; /* 1µs */
			}
		}
	}
}
