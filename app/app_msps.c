/**------------------------------------------------------------------------------------------------
 * @brief Archivo con la funciones de las incilaizaciones auxiliares de la libreria
-------------------------------------------------------------------------------------------------*/
#include "app_bsp.h"
#include "app_msps.h"
#include "app_serial.h"
#include "app_clock.h"
#include "hel_lcd.h"
void HAL_MspInit( void )
{

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; 
  
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /** Configure the main internal regulator output voltage*/
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure
  The frequency set is 64MHz with the internal 16MHz HSI oscilator. According to the formulas:
  fVCO = fPLLIN x ( N / M ) = 16MHz x (8 / 1) = 128MHz
  fPLLP = fVCO / P = 128MHz / 2 = 64MHz
  fPLLQ = fVCO / Q = 128MHz / 2 = 64MHz
  fPLLR = fVCO / R = 128MHz / 2 = 64MHz
  */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN            = 8;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
  HAL_RCC_OscConfig( &RCC_OscInitStruct );
  
  /** Initializes the CPU, AHB and APB buses clocks*/
  RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_2 );
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct; /* Structure for GPIO initialization*/
  GPIO_InitStruct.Pin   =  GPIO_PIN_14;             
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;   
  GPIO_InitStruct.Pull  = GPIO_NOPULL;           
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  
  HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
  GPIO_InitStruct.Pin   = GPIO_PIN_7;             /*pines a configurar*/
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;  /*interrupcion por flanco*/
  GPIO_InitStruct.Pull  = GPIO_NOPULL;           /*pin sin pull-up ni pull-down*/
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  /*pin a baja velocidad*/
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_NVIC_SetPriority( EXTI4_15_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ( EXTI4_15_IRQn );
 
}
/**
 * @brief  Configure the Gpio resources used for can communication
*/
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
    (void)hfdcan;
    GPIO_InitTypeDef GpioCanStruct;

    
    __HAL_RCC_FDCAN_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    GpioCanStruct.Mode = GPIO_MODE_AF_PP;
    GpioCanStruct.Alternate = GPIO_AF3_FDCAN1;
    GpioCanStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GpioCanStruct.Pull = GPIO_NOPULL;
    GpioCanStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( GPIOD, &GpioCanStruct );
    HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
    
}

/**
 * @brief  Makes the necessary configuration to use the rtc
 */
void HAL_RTC_MspInit( RTC_HandleTypeDef *rtc ) 
{
    (void)rtc;
    RCC_OscInitTypeDef        RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct = {0};

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /*Eanlble backup domain*/
    HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG( RCC_LSEDRIVE_LOW );

    /*reset previous RTC source clock*/
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_NONE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
    
    /* Configure LSE/LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
     RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    HAL_RCC_OscConfig( &RCC_OscInitStruct );

    /*Set LSE as source clock*/
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );
      
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
    HAL_NVIC_SetPriority(RTC_TAMP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_TAMP_IRQn);
}
void HAL_SPI_MspInit( SPI_HandleTypeDef *hspi )
{
    (void)hspi;
    /*pines B13, B14 y B15 en funcion alterna spi1 */
    GPIO_InitTypeDef GPIO_InitStruct;
    __GPIOB_CLK_ENABLE();
    __SPI2_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd ) /* cppcheck-suppress misra-c2012-8.6 ; Bug with weekly linked fuctions this is a false/positive */
{
  (void)hlcd;
  __GPIOC_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct; /* Structure for GPIO initialization*/
  GPIO_InitStruct.Pin   =  GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_12;             
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;   
  GPIO_InitStruct.Pull  = GPIO_NOPULL;           
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  
  HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
  GPIO_InitStruct.Pin   = GPIO_PIN_15;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,SET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,RESET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,SET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,SET);
}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef* hwwdg)
{
  if(hwwdg->Instance==WWDG)
  {
  /* USER CODE BEGIN WWDG_MspInit 0 */

  /* USER CODE END WWDG_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_WWDG_CLK_ENABLE();
  /* USER CODE BEGIN WWDG_MspInit 1 */

  /* USER CODE END WWDG_MspInit 1 */
  }

}