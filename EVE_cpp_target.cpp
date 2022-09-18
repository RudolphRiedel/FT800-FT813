/*
@file    EVE_target.cpp
@brief   target specific functions for C++ targets, so far only Arduino targets
@version 5.0
@date    2022-09-18
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2022 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- changed the DMA buffer from uin8_t to uint32_t
- added a section for Arduino-ESP32
- corrected the clock-divider settings for ESP32
- added DMA to ARDUINO_METRO_M4 target
- added DMA to ARDUINO_NUCLEO_F446RE target
- added DMA to Arduino-ESP32 target
- added a native ESP32 target with DMA
- added an ARDUINO_TEENSY41 target with DMA support
- added DMA for the Raspberry Pi Pico - RP2040
- added ARDUINO_TEENSY35 to the ARDUINO_TEENSY41 target
- split up this file in EVE_target.c for the plain C targets and EVE_target.cpp for the Arduino C++ targets
- converted all TABs to SPACEs
- copied over the SPI and DMA support functions for the RP2040 baremetall target to be used under Arduino
- modified the WIZIOPICO target for Arduino RP2040 to also work with ArduinoCore-mbed
- fixed the ESP32 target to work with the ESP32-S3 as well

 */

#if defined (ARDUINO)

    #if defined (ARDUINO_METRO_M4)
        #include "EVE_target.h"
        #include "EVE_commands.h"

        #include <Adafruit_ZeroDMA.h>

        #if defined (EVE_DMA)
            uint32_t EVE_dma_buffer[1025];
            volatile uint16_t EVE_dma_buffer_index;
            volatile uint8_t EVE_dma_busy = 0;

            Adafruit_ZeroDMA myDMA;
            DmacDescriptor *desc;

            /* Callback for end-of-DMA-transfer */
            void dma_callback(Adafruit_ZeroDMA *dma)
            {
                while(SERCOM2->SPI.INTFLAG.bit.TXC == 0);
                SERCOM2->SPI.CTRLB.bit.RXEN = 1; /* switch receiver on by setting RXEN to 1 which is not enable protected */
                EVE_dma_busy = 0;
                EVE_cs_clear();
            }

            void EVE_init_dma(void)
            {
                myDMA.setTrigger(SERCOM2_DMAC_ID_TX);
                myDMA.setAction(DMA_TRIGGER_ACTON_BEAT);
                myDMA.allocate();
                myDMA.setCallback(dma_callback);
                desc = myDMA.addDescriptor(
                    NULL, /* from */
                    (void *) &SERCOM2->SPI.DATA.reg, /* to */
                    100, /* size */
                    DMA_BEAT_SIZE_BYTE, /* beat size -> byte */
                    true,   /* increment source */
                    false); /* increment dest */
            }

            void EVE_start_dma_transfer(void)
            {
                myDMA.changeDescriptor(desc, (void *) (((uint32_t) &EVE_dma_buffer[0])+1), NULL, (EVE_dma_buffer_index*4)-1);
                SERCOM2->SPI.CTRLB.bit.RXEN = 0; /* switch receiver off by setting RXEN to 0 which is not enable-protected */
                EVE_cs_set();
                EVE_dma_busy = 42;
                myDMA.startJob();
//              SPI.transfer( ((uint8_t *) &EVE_dma_buffer[0])+1, ((uint8_t *) &EVE_dma_buffer[0]), (((EVE_dma_buffer_index)*4)-1), false ); /* alternative to using ZeroDMA */
            }
        #endif
    #endif

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #if defined (ARDUINO_NUCLEO_F446RE)
        #include "EVE_target.h"
        #include "EVE_commands.h"

        SPI_HandleTypeDef eve_spi_handle;

        void EVE_init_spi(void)
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
            __HAL_RCC_SPI1_CLK_ENABLE();

            /* SPI1 GPIO Configuration: PA5 -> SPI1_SCK, PA6 -> SPI1_MISO, PA7 -> SPI1_MOSI */
            GPIO_InitTypeDef gpio_init;
            gpio_init.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
            gpio_init.Mode = GPIO_MODE_AF_PP;
            gpio_init.Pull = GPIO_NOPULL;
            gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            gpio_init.Alternate = GPIO_AF5_SPI1;
            HAL_GPIO_Init(GPIOA, &gpio_init);

            eve_spi_handle.Instance = EVE_SPI;
            eve_spi_handle.Init.Mode = SPI_MODE_MASTER;
            eve_spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
            eve_spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
            eve_spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
            eve_spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
            eve_spi_handle.Init.NSS = SPI_NSS_SOFT;
            eve_spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
            eve_spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
            eve_spi_handle.Init.TIMode = SPI_TIMODE_DISABLED;
            eve_spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
            eve_spi_handle.Init.CRCPolynomial = 3;
            HAL_SPI_Init(&eve_spi_handle);
            __HAL_SPI_ENABLE(&eve_spi_handle);
        }

        #if defined (EVE_DMA)
            uint32_t EVE_dma_buffer[1025];
            volatile uint16_t EVE_dma_buffer_index;
            volatile uint8_t EVE_dma_busy = 0;

            DMA_HandleTypeDef eve_dma_handle;

            extern "C" void DMA2_Stream3_IRQHandler(void)
            {
                HAL_DMA_IRQHandler(&eve_dma_handle);
            }

            /* Callback for end-of-DMA-transfer */
            void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
            {
                EVE_dma_busy = 0;
                EVE_cs_clear();
            }

            void EVE_init_dma(void)
            {
                __HAL_RCC_DMA2_CLK_ENABLE();
                eve_dma_handle.Instance = DMA2_Stream3;
                eve_dma_handle.Init.Channel = DMA_CHANNEL_3;
                eve_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
                eve_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
                eve_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
                eve_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
                eve_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
                eve_dma_handle.Init.Mode = DMA_NORMAL;
                eve_dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
                eve_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
                HAL_DMA_Init(&eve_dma_handle);
                __HAL_LINKDMA(&eve_spi_handle, hdmatx, eve_dma_handle);
                HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
                HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
            }

            void EVE_start_dma_transfer(void)
            {
                EVE_cs_set();
                if(HAL_OK == HAL_SPI_Transmit_DMA(&eve_spi_handle, ((uint8_t *) &EVE_dma_buffer[0])+1, ((EVE_dma_buffer_index)*4)-1))
                {
                    EVE_dma_busy = 42;
                }
            }
        #endif
    #endif

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #if defined (ESP32)
    /* note: this is using the ESP-IDF driver as the Arduino class and driver does not allow DMA for SPI */
        #include "EVE_target.h"

        spi_device_handle_t EVE_spi_device = {0};
        spi_device_handle_t EVE_spi_device_simple = {0};

        static void eve_spi_post_transfer_callback(void)
        {
            digitalWrite(EVE_CS, HIGH); /* tell EVE to stop listen */
            #if defined (EVE_DMA)
                EVE_dma_busy = 0;
            #endif
            }

        void EVE_init_spi(void)
        {
            spi_bus_config_t buscfg = {0};
            spi_device_interface_config_t devcfg = {0};

            buscfg.mosi_io_num = EVE_MOSI;
            buscfg.miso_io_num = EVE_MISO;
            buscfg.sclk_io_num = EVE_SCK;
            buscfg.quadwp_io_num = -1;
            buscfg.quadhd_io_num = -1;
            buscfg.max_transfer_sz= 4088;

            devcfg.clock_speed_hz = 16 * 1000 * 1000; /* clock = 16 MHz */
            devcfg.mode = 0;                          /* SPI mode 0 */
            devcfg.spics_io_num = -1;                 /* CS pin operated by app */
            devcfg.queue_size = 3;                    /* we need only one transaction in the que */
            devcfg.address_bits = 24;                 /* 24 bits for the address */
            devcfg.command_bits = 0;                  /* command operated by app */
            devcfg.post_cb = (transaction_cb_t)eve_spi_post_transfer_callback;

            spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
            spi_bus_add_device(SPI2_HOST, &devcfg, &EVE_spi_device);

            devcfg.address_bits = 0;
            devcfg.post_cb = 0;
            devcfg.clock_speed_hz = 10 * 1000 * 1000; /* Clock = 10 MHz */
            spi_bus_add_device(SPI2_HOST, &devcfg, &EVE_spi_device_simple);
        }

        #if defined (EVE_DMA)
            uint32_t EVE_dma_buffer[1025];
            volatile uint16_t EVE_dma_buffer_index;
            volatile uint8_t EVE_dma_busy = 0;

            void EVE_init_dma(void)
            {
            }

            void EVE_start_dma_transfer(void)
            {
                spi_transaction_t EVE_spi_transaction = {0};
                digitalWrite(EVE_CS, LOW); /* make EVE listen */
                EVE_spi_transaction.tx_buffer = (uint8_t *) &EVE_dma_buffer[1];
                EVE_spi_transaction.length = (EVE_dma_buffer_index-1) * 4 * 8;
                EVE_spi_transaction.addr = 0x00b02578; /* WRITE + REG_CMDB_WRITE; */
                spi_device_queue_trans(EVE_spi_device, &EVE_spi_transaction, portMAX_DELAY);
                EVE_dma_busy = 42;
            }
        #endif
    #endif /* ESP32 */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

    #if defined (ARDUINO_TEENSY41) || (ARDUINO_TEENSY35)
        #include "EVE_target.h"
        #include <SPI.h>

        #if defined (EVE_DMA)
        uint32_t EVE_dma_buffer[1025];
        volatile uint16_t EVE_dma_buffer_index;
        volatile uint8_t EVE_dma_busy = 0;

        EventResponder EVE_spi_event;

        /* Callback for end-of-DMA-transfer */
        void dma_callback(EventResponderRef event_responder)
        {
            EVE_dma_busy = 0;
            EVE_cs_clear();
        }

        void EVE_init_dma(void)
        {
            EVE_spi_event.attachImmediate(&dma_callback);
        }

        void EVE_start_dma_transfer(void)
        {
            EVE_cs_set();
            EVE_dma_busy = 42;
            SPI.transfer( ((uint8_t *) &EVE_dma_buffer[0])+1, NULL, (((EVE_dma_buffer_index)*4)-1), EVE_spi_event);
        }
        #endif
    #endif /* Teensy 4.1 */

/*----------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------*/

        #if defined (WIZIOPICO) || (PICOPI)
        /* note: set in platformio.ini by "build_flags = -D WIZIOPICO" */
        #include "EVE_target.h"
        #include "EVE_commands.h"

        void EVE_init_spi(void)
        {
            gpio_set_function(EVE_MISO, GPIO_FUNC_SPI);
            gpio_set_function(EVE_SCK, GPIO_FUNC_SPI);
            gpio_set_function(EVE_MOSI, GPIO_FUNC_SPI);
            #if defined (WIZIOPICO)
                spi_init(EVE_SPI, 8000000);
            #else
/* trap:
    ArduinoCore-mbed/cores/arduino/mbed/targets/TARGET_RASPBERRYPI/TARGET_RP2040/pico-sdk/rp2_common/hardware_spi/include/hardware/spi.h 
    is not the same as:
    pico-sdk/src/rp2_common/hardware_spi/include/hardware/spi.h 
    The function spi_init() was renamed to _spi_init().
*/
                _spi_init(EVE_SPI, 8000000);
            #endif
        }

        #if defined (EVE_DMA)

        #include "hardware/dma.h"
        #include "hardware/irq.h"

        uint32_t EVE_dma_buffer[1025];
        volatile uint16_t EVE_dma_buffer_index;
        volatile uint8_t EVE_dma_busy = 0;
        int dma_tx;
        dma_channel_config dma_tx_config;

        static void EVE_DMA_handler(void)
        {
            dma_hw->ints0 = 1u << dma_tx; /* ack irq */
            while((spi_get_hw(EVE_SPI)->sr & SPI_SSPSR_BSY_BITS) != 0); /* wait for the SPI to be done transmitting */
            EVE_dma_busy = 0;
            EVE_cs_clear();
        }

        void EVE_init_dma(void)
        {
            dma_tx = dma_claim_unused_channel(true);
            dma_tx_config = dma_channel_get_default_config(dma_tx);
            channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_8);
            channel_config_set_dreq(&dma_tx_config, spi_get_index(EVE_SPI) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
            dma_channel_set_irq0_enabled (dma_tx, true);
            irq_set_exclusive_handler(DMA_IRQ_0, EVE_DMA_handler);
            irq_set_enabled(DMA_IRQ_0, true);
        }

        void EVE_start_dma_transfer(void)
        {
            EVE_cs_set();
            dma_channel_configure(dma_tx, &dma_tx_config,
                &spi_get_hw(EVE_SPI)->dr, /* write address */
                ((uint8_t *) &EVE_dma_buffer[0])+1, /* read address */
                (((EVE_dma_buffer_index)*4)-1), /* element count (each element is of size transfer_data_size) */
                true); /* start transfer */
            EVE_dma_busy = 42;
        }
        #endif /* DMA */

        #endif /* WIZIOPICO */

#endif
