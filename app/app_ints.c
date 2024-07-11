/**------------------------------------------------------------------------------------------------
 * Archivo con la funciones de interrupcion del micrcontroladores, revisar archivo startup_stm32g0b1.S
-------------------------------------------------------------------------------------------------*/
#include "app_ints.h"
#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void NMI_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SVC_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void PendSV_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SysTick_Handler( void )
{
    HAL_IncTick( );
}
/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void EXTI4_15_IRQHandler( void )
{
    /*funcion que realiza las operaciones de interrupcion del gpio*/
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );
}


    
void TIM16_FDCAN_IT0_IRQHandler( void )
{
    
    HAL_FDCAN_IRQHandler( &CANHandler );
}



void RTC_TAMP_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_TAMP_IRQn 0 */

  /* USER CODE END RTC_TAMP_IRQn 0 */
  HAL_RTC_AlarmIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_TAMP_IRQn 1 */

  /* USER CODE END RTC_TAMP_IRQn 1 */
}