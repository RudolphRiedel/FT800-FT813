/*
 * Clockconfig.c
 *
 * Created: 17.09.2013 11:57:38
 *  Author: Robert
 */ 

#include <avr/io.h>


// This function set the system clock to 32 MHz with automatic calibration.
void clk_init(void)
{	
	// This function also enables the 2 MHz internal RC oscillator and its DFLL, even it isn't used.
	// Thats because of a BUG in the H revision of the XMEGA:
	/*	20. Both DFLLs and both oscillators have to be enabled for one to work
	 *	In order to use the automatic runtime calibration for the 2 MHz or the 32 MHz internal oscillators, the DFLL for both
	 *	oscillators and both oscillators have to be enabled for one to work.
	 *	Problem fix/Workaround
	 *	Enable both DFLLs and both oscillators when using automatic runtime calibration for either of the internal
	 *	oscillators
	 */
	
	OSC.CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm | OSC_RC2MEN_bm;	/* Enable all internal oscillators */
	while(!(OSC.STATUS & OSC_RC32KRDY_bm));							/* Wait for 32Khz oscillator to stabilize */
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));							/* Wait for 32MHz oscillator to stabilize */
	while(!(OSC.STATUS & OSC_RC2MRDY_bm));							/* Wait for 2MHz oscillator to stabilize */
	DFLLRC2M.CTRL = DFLL_ENABLE_bm ;								/* Enable DFLL also for 2 MHz Oscillator */
	DFLLRC32M.CTRL = DFLL_ENABLE_bm ;								/* Enable DFLL  */
	CCP = CCP_IOREG_gc;												/* Disable register security for clock update */
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;								/* Switch to 32MHz clock */
}