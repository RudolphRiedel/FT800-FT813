/*

Test!
Just to see if it compiles, does not do anything usefull!

This is missing everything to make this a demo:

- init of the target-controller
- init of the I/O pins
- init of the SPI

*/

#include "EVE_target.h"
#include "tft.h"

#if 0

#if defined (STM32L0) /* set with "build_flags = -D STM32L0" in platformio.ini */
  #include "stm32l0xx.h"
#endif

#if defined (STM32F0) /* set with "build_flags = -D STM32F0" in platformio.ini */
  #include "stm32f0xx.h"
#endif

#if defined (STM32F1) /* set with "build_flags = -D STM32F1" in platformio.ini */
    #include "stm32f1xx.h"
#endif

#if defined (STM32F3) /* set with "build_flags = -D STM32F3" in platformio.ini */
    #include "stm32f3xx.h"
#endif

#if defined (STM32F4) /* set in platformio.ini */
  #include "stm32f4xx.h"
#endif


/* generated with STM32CubeMX for Nucleo-64 STM32F446RE */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

#endif

volatile uint8_t system_tick = 0;

void SysTick_Handler(void) /* the HAL definition for this is inconsistent across STM32 families, so we roll our own */ 
{
    system_tick = 42;
}



int main(void)
{
    uint8_t display_delay = 0;

    HAL_Init();
//  SystemClock_Config();
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/200U); /*Configure the SysTick to have interrupts in 5ms time basis*/

//  EVE_init_spi();
//  EVE_init_dma();

    TFT_init();
    
    while(1)
    {
        if(system_tick)
        {
            system_tick = 0;

            TFT_touch();

            display_delay++;
            if(display_delay > 3)
            {
                display_delay = 0;

                TFT_display();
            }
        }
    }
}