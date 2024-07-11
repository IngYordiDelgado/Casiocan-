#ifndef BSP_H_
#define BSP_H_

  #include "stm32g0xx.h"
  #include <stdint.h> 
  extern TIM_HandleTypeDef TIM6_Handler;    
  void Delay_us_Init(void);
  void Delay_us(uint16_t delay);

#endif

