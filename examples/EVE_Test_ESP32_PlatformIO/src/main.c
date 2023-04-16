/*
@file    main.c
@brief   Main file for ESP-IDF ESP32 example
@version 1.0
@date    2023-04-16
@author  Rudolph Riedel
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if ESP_IDF_VERSION_MAJOR == 5
#include "esp_timer.h"
#endif

#include "EVE.h"
#include "tft.h"

void app_main()
{
    uint32_t current_millis;
    uint32_t previous_millis = 0;
    uint8_t display_delay = 0;
    uint32_t micros_start, micros_end;

    EVE_init_spi();
    TFT_init();

    while(1)
    {
       	current_millis = (uint32_t) (esp_timer_get_time() / 1000ULL);

        if((current_millis - previous_millis) > 4) /* execute the code every 5 milli-seconds */
        {
            previous_millis = current_millis;

            micros_start = (uint32_t) (esp_timer_get_time());
            TFT_touch();
            micros_end = (uint32_t) (esp_timer_get_time());
            num_profile_b = (micros_end - micros_start); /* calculate the micro-seconds passed during the call to TFT_touch */

            display_delay++;
            if(display_delay > 3) /* refresh the display every 20ms */
            {
                display_delay = 0;
                micros_start = (uint32_t) (esp_timer_get_time());
                TFT_display();
                micros_end = (uint32_t) (esp_timer_get_time());
                num_profile_a = (micros_end - micros_start); /* calculate the micro-seconds passed during the call to TFT_display */
            }
        }

    vPortYield();
    }
}
