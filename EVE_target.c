/*
@file    EVE_target.c
@brief   target specific functions
@version 4.0
@date    2019-05-11
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2019 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include "EVE_config.h"
#include "EVE_commands.h"

#if !defined (ARDUINO)
	#if defined (__GNUC__)
		#if defined (__SAMC21E18A__)

		#if defined (EVE_DMA)

			static DmacDescriptor dmadescriptor __attribute__((aligned(16))) SECTION_DMAC_DESCRIPTOR;
			static DmacDescriptor dmawriteback __attribute__((aligned(16))) SECTION_DMAC_DESCRIPTOR;
			uint8_t EVE_dma_buffer[4100];
			volatile uint16_t EVE_dma_buffer_index;

			volatile uint8_t EVE_dma_busy = 0;

			#define EVE_dma_channel 0

			void EVE_init_dma(void)
			{
				REG_DMAC_BASEADDR = (uint32_t) &dmadescriptor;
				REG_DMAC_WRBADDR = (uint32_t) &dmawriteback;
				REG_DMAC_PRICTRL0 = 0; /* all off, reset-default */

				REG_DMAC_CHCTRLB = DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_TRIGSRC(SERCOM0_DMAC_ID_TX); /* beat-transfer, SERCOM0 TX Trigger, level 0, channel-event input / output disabled */
				REG_DMAC_CTRL = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */
				REG_DMAC_CHID = EVE_dma_channel; /* select channel 0, reset-default */

				dmadescriptor.BTCTRL.reg = DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID; /* increase source-address, beat-size = 8-bit */
				dmadescriptor.DSTADDR.reg = (uint32_t) &REG_SERCOM0_SPI_DATA;
				dmadescriptor.DESCADDR.reg = 0; /* no next descriptor */

				REG_DMAC_CHINTENSET = DMAC_CHINTENSET_TCMPL;
				__NVIC_SetPriority(DMAC_IRQn, 0);
				__NVIC_EnableIRQ(DMAC_IRQn);
			}


			void EVE_start_dma_transfer(void)
			{
				dmadescriptor.BTCNT.reg = EVE_dma_buffer_index;
				dmadescriptor.SRCADDR.reg = (uint32_t) &EVE_dma_buffer[EVE_dma_buffer_index]; /* note: last entry in array + 1 */
				REG_SERCOM0_SPI_CTRLB = 0; /* switch receiver off by setting RXEN to 0 which is not enable-protected */
				EVE_cs_set();
				REG_DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE; /* start sending out EVE_dma_buffer ´*/
				EVE_dma_busy = 42;
			}

			/* executed at the end of the DMA transfer */
			void DMAC_Handler()
			{
				REG_DMAC_CHID = EVE_dma_channel; /* we only use one channel, so this should not even change */
				REG_DMAC_CHINTFLAG = DMAC_CHINTFLAG_TCMPL; /* ack irq */
				while((REG_SERCOM0_SPI_INTFLAG & SERCOM_SPI_INTFLAG_TXC) == 0); /* wait for the SPI to be done transmitting */
				REG_SERCOM0_SPI_CTRLB = SERCOM_SPI_CTRLB_RXEN; /* switch receiver on by setting RXEN to 1 which is not enable protected */
				EVE_dma_busy = 0;
				EVE_cs_clear();
				EVE_cmd_start(); /* order the command co-processor to start processing its FIFO queue but do not wait for completion */
			}

		#endif


		#endif
	#endif
#endif

