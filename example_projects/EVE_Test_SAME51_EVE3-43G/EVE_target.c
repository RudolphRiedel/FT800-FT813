/*
@file    EVE_target.c
@brief   target specific functions
@version 5.0
@date    2020-10-29
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2020 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

4.0
- added support for MSP432
- moved the two include lines out of reach for Arduino to increase compatibility with Arduino
- removed preceding "__" from two CMSIS functions that were not necessary and maybe even wrong
- moved the very basic DELAY_MS() function for ATSAM to EVE_target.c and therefore removed the unneceesary inlining for this function
- added DMA support for ATSAME51
- started to implement DMA support for STM32
- added a few more controllers as examples from the ATSAMC2x and ATSAMx5x family trees

5.0
- changed the DMA buffer from uin8_t to uint32_t


 */

#if !defined (ARDUINO)

  #include "EVE_target.h"
  #include "EVE_commands.h"

	#if defined (__GNUC__)
		#if defined (__SAMC21E18A__) || (__SAMC21J18A__) || (__SAME51J19A__) || (__SAMD51P20A__) || (__SAMD51J19A__) || (__SAMD51G18A__)
		/* note: target as set by AtmelStudio, valid  are all from the same family, ATSAMC2x and ATSAMx5x use the same SERCOM units */

		void DELAY_MS(uint16_t val)
		{
			uint16_t counter;

			while(val > 0)
			{
				for(counter=0; counter < EVE_DELAY_1MS;counter++)
				{
					__asm__ volatile ("nop");
				}
				val--;
			}
		}

		#if defined (EVE_DMA)

			static DmacDescriptor dmadescriptor __attribute__((aligned(16)));
			static DmacDescriptor dmawriteback __attribute__((aligned(16)));
			uint32_t EVE_dma_buffer[1025];
			volatile uint16_t EVE_dma_buffer_index;

			volatile uint8_t EVE_dma_busy = 0;

			#if defined  (__SAMC21E18A__) || (__SAMC21J18A__)

			void EVE_init_dma(void)
			{
				DMAC->CTRL.reg = 0;
				while(DMAC->CTRL.bit.DMAENABLE);
				DMAC->CTRL.bit.SWRST = 1;
				while(DMAC->CTRL.bit.SWRST); /* wait for the software-reset to be complete */

				DMAC->BASEADDR.reg = (uint32_t) &dmadescriptor;
				DMAC->WRBADDR.reg = (uint32_t) &dmawriteback;

				DMAC->CHCTRLB.reg = DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_TRIGSRC(EVE_SPI_DMA_TRIGGER); /* beat-transfer, SERCOM0 TX Trigger, level 0, channel-event input / output disabled */
				DMAC->CHID.reg = EVE_DMA_CHANNEL; /* select channel */
				DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */

				dmadescriptor.BTCTRL.reg = DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID; /* increase source-address, beat-size = 8-bit */
				dmadescriptor.DSTADDR.reg = (uint32_t) &EVE_SPI->SPI.DATA.reg;
				dmadescriptor.DESCADDR.reg = 0; /* no next descriptor */

				DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
				NVIC_SetPriority(DMAC_IRQn, 0);
				NVIC_EnableIRQ(DMAC_IRQn);
			}

			void EVE_start_dma_transfer(void)
			{
				dmadescriptor.BTCNT.reg = (EVE_dma_buffer_index*4)-1;
				dmadescriptor.SRCADDR.reg = (uint32_t) &EVE_dma_buffer[EVE_dma_buffer_index]; /* note: last entry in array + 1 */
				EVE_SPI->SPI.CTRLB.bit.RXEN = 0; /* switch receiver off by setting RXEN to 0 which is not enable-protected */
				EVE_cs_set();
				DMAC->CHCTRLA.bit.ENABLE = 1; /* start sending out EVE_dma_buffer ?*/
				EVE_dma_busy = 42;
			}

			/* executed at the end of the DMA transfer */
			void DMAC_Handler()
			{
				DMAC->CHID.reg = EVE_DMA_CHANNEL; /* we only use one channel, so this should not even change */
				DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL; /* ack irq */
				while(EVE_SPI->SPI.INTFLAG.bit.TXC == 0); /* wait for the SPI to be done transmitting */
				EVE_SPI->SPI.CTRLB.bit.RXEN = 1; /* switch receiver on by setting RXEN to 1 which is not enable protected */
				EVE_cs_clear();
				EVE_dma_busy = 0;
			}

			#endif /* DMA functions SAMC2x */

			#if defined (__SAME51J19A__) || (__SAMD51P20A__) || (__SAMD51J19A__) || (__SAMD51G18A__)

			void EVE_init_dma(void)
			{
				DMAC->CTRL.reg = 0;
				while(DMAC->CTRL.bit.DMAENABLE);
				DMAC->CTRL.bit.SWRST = 1;
				while(DMAC->CTRL.bit.SWRST); /* wait for the software-reset to be complete */

				DMAC->BASEADDR.reg = (uint32_t) &dmadescriptor;
				DMAC->WRBADDR.reg = (uint32_t) &dmawriteback;
				DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */
				DMAC->Channel[EVE_DMA_CHANNEL].CHCTRLA.reg =
					DMAC_CHCTRLA_BURSTLEN_SINGLE |
					DMAC_CHCTRLA_TRIGACT_BURST |
					DMAC_CHCTRLA_TRIGSRC(EVE_SPI_DMA_TRIGGER);

				dmadescriptor.BTCTRL.reg = DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID; /* increase source-address, beat-size = 8-bit */
				dmadescriptor.DSTADDR.reg = (uint32_t) &EVE_SPI->SPI.DATA.reg;
				dmadescriptor.DESCADDR.reg = 0; /* no next descriptor */

				DMAC->Channel[EVE_DMA_CHANNEL].CHINTENSET.bit.TCMPL = 1; /* enable transfer complete interrupt */
				DMAC->CTRL.reg = DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE; /* enable level 0 transfers, enable DMA */

				NVIC_SetPriority(DMAC_0_IRQn, 0);
				NVIC_EnableIRQ(DMAC_0_IRQn);
			}

			void EVE_start_dma_transfer(void)
			{
				dmadescriptor.BTCNT.reg = (EVE_dma_buffer_index*4)-1;
				dmadescriptor.SRCADDR.reg = (uint32_t) &EVE_dma_buffer[EVE_dma_buffer_index]; /* note: last entry in array + 1 */
				EVE_SPI->SPI.CTRLB.bit.RXEN = 0; /* switch receiver off by setting RXEN to 0 which is not enable-protected */
				EVE_cs_set();
				DMAC->Channel[EVE_DMA_CHANNEL].CHCTRLA.bit.ENABLE = 1; /* start sending out EVE_dma_buffer */
				EVE_dma_busy = 42;
			}

			/* executed at the end of the DMA transfer */
			void DMAC_0_Handler()
			{
				DMAC->Channel[EVE_DMA_CHANNEL].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL; /* ack irq */
				while(EVE_SPI->SPI.INTFLAG.bit.TXC == 0); /* wait for the SPI to be done transmitting */
				EVE_SPI->SPI.CTRLB.bit.RXEN = 1; /* switch receiver on by setting RXEN to 1 which is not enable protected */
				EVE_dma_busy = 0;
				EVE_cs_clear();
			}

		#endif /* DMA functions SAMx5x */

		#endif /* DMA */

        #endif /* ATSAM */

		#if defined (STM32L073xx) || (STM32F1) || (STM32F207xx) || (STM32F3) || (STM32F4)

		#if defined (EVE_DMA)
			uint32_t EVE_dma_buffer[1025];
			volatile uint16_t EVE_dma_buffer_index;
			volatile uint8_t EVE_dma_busy = 0;

			volatile DMA_HandleTypeDef EVE_dma_tx;

			void EVE_init_dma(void)
			{

			}

			void EVE_start_dma_transfer(void)
			{

				EVE_cs_set();
				EVE_dma_busy = 42;
			}

			/* DMA-done-Interrupt-Handler */
#if 0
			void some_name_handler()
			{


				EVE_dma_busy = 0;
				EVE_cs_clear();
				EVE_cmd_start(); /* order the command co-processor to start processing its FIFO queue but do not wait for completion */
			}
#endif

		#endif /* DMA */

		#endif /* STM32 */


    #endif /* __GNUC__ */

    #if defined (__TI_ARM__)

        #if defined (__MSP432P401R__)

			/* SPI Master Configuration Parameter */
			const eUSCI_SPI_MasterConfig EVE_Config =
			{
			        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
			        48000000,                                   // SMCLK  = 48MHZ
			        500000,                                    // SPICLK = 1Mhz
			        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
			        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,    // Phase
			        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, // High polarity
			        EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
			};

			void EVE_SPI_Init(void)
			{
			    GPIO_setAsOutputPin(EVE_CS_PORT,EVE_CS);
			    GPIO_setAsOutputPin(EVE_PDN_PORT,EVE_PDN);
//			    GPIO_setAsInputPinWithPullDownResistor(EVE_INT_PORT,EVE_INT);

			    GPIO_setOutputHighOnPin(EVE_CS_PORT,EVE_CS);
			    GPIO_setOutputHighOnPin(EVE_PDN_PORT,EVE_PDN);

			    GPIO_setAsPeripheralModuleFunctionInputPin(RIVERDI_PORT, RIVERDI_SIMO | RIVERDI_SOMI | RIVERDI_CLK, GPIO_PRIMARY_MODULE_FUNCTION);
			    SPI_initMaster(EUSCI_B0_BASE, &EVE_Config);
			    SPI_enableModule(EUSCI_B0_BASE);
			}

        #endif

	#endif
#endif
