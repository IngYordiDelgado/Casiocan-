#include "app_serial.h"
#include "app_bsp.h"
#include "app_clock.h"
#include "hel_lcd.h"
#include <string.h>
#include "app_display.h"
#include "scheduler.h"

#define TASKS_N     5
#define TICK_VAL    1


/*Function definitions */
static void       hearth_init(void);
static void       dog_init(void);
static void       hearth_beat(void);
static void       peth_the_dog(void);



Scheduler_HandleTypeDef Sche;
uint32_t TimerId;

/**
 * @brief  Variable used for the hearth beat timing 
 */
static uint32_t   hearth_tick;
/**
 * @brief  structure declaration for watchdog initialization 
 */

//static uint32_t   dog_tick;

static WWDG_HandleTypeDef Wwdg_Handler;

int main( void )
{
  HAL_Init(); /* initialization of HAL Library */
  static  Task_TypeDef tasks[ TASKS_N ];
 
  static  Timer_TypeDef timers[ 1 ];
  __GPIOC_CLK_ENABLE();
  
  Sche.tick     = TICK_VAL;
  Sche.tasks    = TASKS_N;
  Sche.taskPtr  = tasks;
  Sche.timers   = 1;
  Sche.timerPtr = timers;
  HIL_SCHEDULER_Init( &Sche );
  
  /*register two task with thier corresponding init fucntions and their periodicyt, 100ms and 500ms*/
  (void)HIL_SCHEDULER_RegisterTask( &Sche,hearth_init , hearth_beat , Heart_BEAT_PERIOD );
  (void)HIL_SCHEDULER_RegisterTask( &Sche,Serial_Init , Serial_Task , SERIAL_PERIOD    );
  (void)HIL_SCHEDULER_RegisterTask( &Sche,Clock_Init  , Clock_Task  , CLOCK_PERIOD     );
  (void)HIL_SCHEDULER_RegisterTask( &Sche,display_Init, display_Task, DISPLAY_PERIOD   );
  (void)HIL_SCHEDULER_RegisterTask( &Sche,dog_init    , peth_the_dog, 8 );
  TimerId = HIL_SCHEDULER_RegisterTimer( &Sche, 1000u,&One_Secon_TimerCallback);
 
  

  /*run the scheduler in a infinite loop*/
  (void)HIL_SCHEDULER_Start( &Sche );

}
/**
 * @brief  Fuction to initialize the resources necessaries for the use of the hearth beat
 */
static void hearth_init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct; /* Structure for GPIO initialization*/
  /* Port C configuration */
  __HAL_RCC_GPIOC_CLK_ENABLE();        
  GPIO_InitStruct.Pin =  0x7F;             
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   
  GPIO_InitStruct.Pull = GPIO_NOPULL;           
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  
  HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
  hearth_tick = HAL_GetTick(); /*Get the current tick of the system */
}
/**
 * @brief  This fuction toggles a led every 300 ms to indicate that the program is working
 */
static void hearth_beat(void)
{
    hearth_tick = HAL_GetTick();
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
}
/**
 * @brief  Fuction to initialize the watchdog timer 
 */
static void dog_init(void)
{
     /*  Default WWDG Configuration:
      1] Set WWDG counter to 0x7F  and window to 0x50
      2] Set Prescaler to WWDG_PRESCALER_1
      Timing calculation:
      a) WWDG clock counter period (in ms) = (4096 * WWDG_PRESCALER_1) / (PCLK1 / 1000)
                                           = 0.128 ms
      b) WWDG timeout (in ms) = (0x7F + 1) * 0.128
                              ~= 16.384 ms
      => After refresh, WWDG will expires after 16.384 ms and generate reset if
      counter is not reloaded.
      c) Time to enter inside window
      Window timeout (in ms) = (127 - 126 + 1) * 0.128
                             = 256 us */
  
  Wwdg_Handler.Instance        = WWDG;
  Wwdg_Handler.Init.Prescaler  = WWDG_PRESCALER_32;
  Wwdg_Handler.Init.Window     = 126;
  Wwdg_Handler.Init.Counter    = 127;
  Wwdg_Handler.Init.EWIMode    = WWDG_EWI_DISABLE;
  HAL_WWDG_Init(&Wwdg_Handler);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,SET);
}

/**
 * @brief  Function to reset the watchdog 
 */
static void peth_the_dog(void)
{
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
    HAL_WWDG_Refresh(&Wwdg_Handler); 
}

