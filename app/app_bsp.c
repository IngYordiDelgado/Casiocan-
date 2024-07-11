#include "app_bsp.h"

void Delay_us_Init(void)
{
  __TIM6_CLK_ENABLE();
  TIM6_Handler.Instance = TIM6;
  TIM6_Handler.Init.Prescaler = 64;
  TIM6_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM6_Handler.Init.Period = 1000;
  HAL_TIM_Base_Init( &TIM6_Handler );
}
void Delay_us(uint16_t delay)
{
  HAL_TIM_Base_Start(&TIM6_Handler);
  while( __HAL_TIM_GetCounter(&TIM6_Handler) < delay )
  {
    
  }
  __HAL_TIM_SET_COUNTER(&TIM6_Handler,0);
  HAL_TIM_Base_Stop(&TIM6_Handler);
}
