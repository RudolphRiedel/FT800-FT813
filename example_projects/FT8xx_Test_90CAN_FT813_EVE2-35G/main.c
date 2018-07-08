/*
@file    main.c
@brief   main file for FT8xx_Test project, version for AVR controller
@version 1.1
@date    2017-04-29
@author  Rudolph Riedel

@section History

1.0
- initial release

1.1
- added simple profiling

 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "FT8_commands.h"

#include "tft.h"

volatile uint8_t roundtrip=0;

/* executed every 2.5 ms */
ISR (TIMER1_COMPA_vect)
{
	roundtrip++;
	roundtrip &= 0x03;	/* limit to 4 time-slots */
}


void init_timer1(void)
{
	/* Timer1 - 2.5ms @ 16 Mhz cpu-clock */
	TCCR1B = 0x00;	/* stop */
	TCCR1A = 0x00;
	TCCR1C = 0x00;
	OCR1A = 4999;	/* TOP - timer counts to TOP and generates IRQ */
	TIMSK1 |= (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS11);	/* mode = CTC, prescaler = 8 -> 2 MHz */
}


void init_timer3(void)
{
	/* Timer3 - just running for profiling */
	TCCR3B = 0x00;	/* stop */
	TCCR3A = 0x00; /* mode: normal */
	TCCR1C = 0x00;
}


void init_ports(void)
{
	DDRA = 0x00;	/* all pins set to input */
	PORTA = 0xff;	/* pullup-resistors activated */

	DDRB = (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB4) | (1<<PB5);	/* SS (not used), SCK, MOSI, PB4=CS, PB5=PowerDown set to Output, others to set to input */
	PORTB = (1<<PB0) | (1<PB5) | (1<<PB6) | (1<<PB7);	/* SS (not used) set to high, CS set to high, PowerDown set to low, pullup-resistors for unused pins activated */

	DDRC = 0x00;
	PORTC = 0xff;

	DDRD = 0x00;
	PORTD = 0x9f;

	DDRE = 0x00;
	PORTE = 0xff;

	DDRF = 0x00;
	PORTF = 0xfe;

	DDRG = 0x00;
	PORTG = 0xff;
}


void init_spi(void)
{
	SPCR = (1<<SPE) | (1<<MSTR) | (0<<SPR1) | (0<<SPR0);	/* SPI on, MSB first, Mode 0, Master, Fosc/4 -> 4MHz @ 16MHz CPU */
	SPSR = (1<<SPI2X);	/* Fosc/2 -> 8 MHz @ 16MHz CPU */
}


int main(void)
{
	uint8_t triplock;
	uint16_t counter_val;
	uint8_t toggle = 0;

	uint16_t timeout = 0;

	cli();
	init_ports();
	init_timer1();
	init_timer3();
	init_spi();
	sei();

	TFT_init();

	set_sleep_mode(SLEEP_MODE_IDLE);

	while(1)
	{
		triplock = roundtrip;

		if(roundtrip == 0) /* executed every 10 ms */
		{
			TCNT3 = 0;
			TCCR3B = (1<<CS1); /* start timer, prescaler = 8 -> 500ns per tick */
			TFT_loop();
			TCCR3B = 0; /* stop timer */
			
			counter_val = TCNT3;
			counter_val /= 2; /* 1µs */
			
			if(toggle == 0)
			{
				num_profile_a = counter_val;
				toggle++;
			}
			else
			{
				num_profile_b = counter_val;
				toggle = 0;
			}
		}

		if(roundtrip == 1)
		{
		}

		if(roundtrip == 2)
		{
		}

		if(roundtrip == 3)
		{
			timeout++;
			if(timeout > 600) // 6 secs
			{
//					TFT_init();
//					FT8_init();
					timeout = 0;
			}
			
		}

		while(triplock == roundtrip)
		{
			cli();
			sleep_enable();
			sei();
			sleep_cpu();
			sleep_disable();
		}
	} /* while */
}
