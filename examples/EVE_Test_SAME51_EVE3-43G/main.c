/*
@file    main.c
@brief   Bare-Metall example main.c for ATSAMD51/ATSAME51
@date    2020-05-01
@author  Rudolph Riedel
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
	/* AUTOWS is enabled by default in REG_NVMCTRL_CTRLA - no need to change the number of wait states when changing the core clock */

	/* configure XOSC1 for a 16MHz crystal connected to XIN1/XOUT1 */
	OSCCTRL->XOSCCTRL[1].reg =
		OSCCTRL_XOSCCTRL_STARTUP(6) |		// 1,953 ms
		OSCCTRL_XOSCCTRL_RUNSTDBY |
		OSCCTRL_XOSCCTRL_ENALC |
		OSCCTRL_XOSCCTRL_IMULT(4) |
		OSCCTRL_XOSCCTRL_IPTAT(3) |
		OSCCTRL_XOSCCTRL_XTALEN |
		OSCCTRL_XOSCCTRL_ENABLE;
	while(0 == OSCCTRL->STATUS.bit.XOSCRDY1);

	/* configure DPLL0, source = XOSC1, pre-scaler = 8, multiplied by 60 -> 120MHz clock from 16MHz input */
	OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_DIV(3) | OSCCTRL_DPLLCTRLB_REFCLK(OSCCTRL_DPLLCTRLB_REFCLK_XOSC1_Val); /* setup PLL to use XOSC1 input, divided by 8 */
	OSCCTRL->Dpll[0].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(0x0) | OSCCTRL_DPLLRATIO_LDR(59); /* multiply by 60 */
	OSCCTRL->Dpll[0].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_RUNSTDBY | OSCCTRL_DPLLCTRLA_ENABLE;
	while(0 == OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY);
	
	/* configure clock-generator 0 to use DPLL0 as source -> GCLK0 is used for the core */
	GCLK->GENCTRL[0].reg =
		GCLK_GENCTRL_DIV(0) |
		GCLK_GENCTRL_RUNSTDBY |
		GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_SRC_DPLL0 |	/* DPLL0 */
		GCLK_GENCTRL_IDC ;
	while(1 == GCLK->SYNCBUSY.bit.GENCTRL0); /* wait for the synchronization between clock domains to be complete */

	/* configure clock-generator 1 to use  48MHz DFLL as source -> GCLK1 is used for the timer0 */
	GCLK->GENCTRL[1].reg =
		GCLK_GENCTRL_DIV(0) |
		GCLK_GENCTRL_RUNSTDBY |
		GCLK_GENCTRL_GENEN |
		GCLK_GENCTRL_SRC_DFLL |	/* 48MHz DFLL */
		GCLK_GENCTRL_IDC ;
	while(1 == GCLK->SYNCBUSY.bit.GENCTRL1); /* wait for the synchronization between clock domains to be complete */
}


void set_spi_speed(uint8_t baudval)
{
	if(SERCOM5->SPI.CTRLA.bit.ENABLE)  /* SPI already is active, need to disable first and re-activate */
	{
		SERCOM5->SPI.CTRLA.bit.ENABLE = 0; /* disable SERCOM5 -> enable config */
		while(SERCOM5->SPI.SYNCBUSY.bit.ENABLE);
		SERCOM5->SPI.BAUD.reg = baudval; /* 120 / (2 * (baudval + 1)) -> @120Mhz: 0 = 60MHz, 1 = 30MHz, 2 = 20MHz, 3 = 15MHz, 4 = 12MHz, 5 = 10MHz */
		SERCOM5->SPI.CTRLA.bit.ENABLE = 1; /* activate SERCOM5 */
		while(SERCOM5->SPI.SYNCBUSY.bit.ENABLE); /* wait for SERCOM5 to be ready */
	}
	else
	{
		SERCOM5->SPI.BAUD.reg = baudval; /* 120 / (2 * (baudval + 1)) -> @120Mhz: 0 = 60MHz, 1 = 30MHz, 2 = 20MHz, 3 = 15MHz, 4 = 12MHz, 5 = 10MHz */
	}
}


void init_spi(void)
{
	PORT->Group[1].DIRSET.reg = PORT_PB31; /* PD_TFT */
	PORT->Group[1].DIRSET.reg = PORT_PB01; /* CS_TFT */
	PORT->Group[1].OUTCLR.reg = PORT_PB31; /* PD_TFT */
	PORT->Group[1].OUTSET.reg = PORT_PB01; /* CS_TFT */

	/* configure SERCOM5 MISO on PB00 */
	PORT->Group[1].WRCONFIG.reg =
		PORT_WRCONFIG_WRPINCFG |
		PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(MUX_PB00D_SERCOM5_PAD2) |
		PORT_WRCONFIG_INEN |
		PORT_WRCONFIG_PINMASK(0x0001) |	/* PB00 */
		PORT_WRCONFIG_PMUXEN;

	/* configure SERCOM5 MOSI on PB02 and SERCOM5 SCK on PB03 */
	PORT->Group[1].WRCONFIG.reg =
		PORT_WRCONFIG_WRPINCFG |
		PORT_WRCONFIG_WRPMUX |
		PORT_WRCONFIG_PMUX(MUX_PB02D_SERCOM5_PAD0) |
		PORT_WRCONFIG_DRVSTR |
		PORT_WRCONFIG_PINMASK(0x000c) | /* PB02 + PB03 */
		PORT_WRCONFIG_PMUXEN;

	MCLK->APBDMASK.bit.SERCOM5_ = 1;
	GCLK->PCHCTRL[35].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN; /* setup SERCOM5 to use GLCK0 -> 120MHz */

	SERCOM5->SPI.CTRLA.bit.ENABLE = 0; /* disable SERCOM5 -> enable config */
	while(SERCOM5->SPI.SYNCBUSY.bit.ENABLE);
	SERCOM5->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE(3) | SERCOM_SPI_CTRLA_DOPO(0) | SERCOM_SPI_CTRLA_DIPO(2); /* MSB first, CPOL = 0, CPHA = 0, SPI frame, master mode, PAD0 = MOSI, PAD2 = MISI, PAD1 = SCK */
	set_spi_speed(5); /* 120 / (2 * (5 + 1)) -> 120 / 12 -> 10MHZ clock from 120MHz core-clock */
	SERCOM5->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN; /* receiver enabled, no hardware select, 8-bit */
	SERCOM5->SPI.CTRLA.bit.ENABLE = 1; /* activate SERCOM */
	while(SERCOM5->SPI.SYNCBUSY.bit.ENABLE);
}


void init_timer(void)
{
	MCLK->APBCMASK.bit.TC4_ = 1;
	GCLK->PCHCTRL[30].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN; /* setup TC4 to use GLCK1 -> 48MHz */
	TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV16;
	TC4->COUNT16.CTRLA.bit.ENABLE = 1;
	while(1 == TC4->COUNT16.SYNCBUSY.bit.ENABLE); /* wait for TC4 to be ready */
}


void init_io(void)
{
	PORT->Group[0].DIRSET.reg = PORT_PA02; /* Debug-LED */
}


int main(void)
{
	uint8_t led_delay = 0;
	uint8_t display_delay = 0;
	
	init_clock();
	init_io();
	init_spi();
	init_timer();
	
	SysTick_Config(120000000 / 200); /* configure and enable Systick for 5ms ticks */

#if 1
	if(0 == CMCC->SR.bit.CSTS) /* if cache controller is disabled */
	{
		CMCC->CTRL.bit.CEN = 1;	/* enable it */
	}
#endif

	TFT_init();
	set_spi_speed(3); /* speed up to 15MHz after init, with 20MHz touch is not working on my HW... */
	
    while (1) 
    {
		if(system_tick)
		{
			system_tick = 0;

			led_delay++;
			if(led_delay > 49)
			{
				led_delay = 0;
				PORT->Group[0].OUTTGL.reg = PORT_PA02;
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

				num_profile_a = TC4->COUNT16.COUNT.reg;
				num_profile_a /= 3; /* 1µs */
			}
		}
	}
}
