#include "scheduler.h"
#include "app_bsp.h"


uint8_t HIL_SCHEDULER_RegisterTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timeout, void (*CallbackPtr)(void) )
{
  uint8_t exit_flag = 0u;
  if( ( (Timeout % hscheduler->tick) == 0u )  && (Timeout > 0u ) )
  {
    hscheduler->timerPtr[hscheduler->TimerCount].callbackPtr = CallbackPtr;
    hscheduler->timerPtr[hscheduler->TimerCount].Timeout = Timeout;
    hscheduler->TimerCount++;
    exit_flag = hscheduler->TimerCount;
  }
  return exit_flag;
}


uint32_t HIL_SCHEDULER_GetTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer )
{
  uint32_t exit_flag = 0u;
  if( ( Timer > 0u ) && ( Timer <= hscheduler->TimerCount) )
  {
    exit_flag = hscheduler->timerPtr[Timer - 1u].Count;
  }
  return exit_flag;
}


uint8_t HIL_SCHEDULER_ReloadTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer, uint32_t Timeout )
{
  uint8_t exit_flag = 0u;
  if( ( Timer > 0u ) && ( Timer <= hscheduler->TimerCount) )
  {
    hscheduler->timerPtr[Timer - 1u].Count = Timeout;
    exit_flag = TRUE;
  }
  return exit_flag;
}

uint8_t HIL_SCHEDULER_StartTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer )
{
  uint8_t exit_flag = 0u;
  if( ( Timer > 0u ) && ( Timer <= hscheduler->TimerCount) )
  {
    
    hscheduler->timerPtr[Timer - 1u].StartFlag = 1u;
    hscheduler->timerPtr[Timer - 1u].Count =  hscheduler->timerPtr[Timer - 1u].Timeout ;
    exit_flag = TRUE;
  }
  return exit_flag;
}
uint8_t HIL_SCHEDULER_StopTimer( Scheduler_HandleTypeDef *hscheduler, uint32_t Timer )
{
  uint8_t exit_flag = 0u;
  if( ( Timer > 0u ) && ( Timer <= hscheduler->TimerCount) )
  {
    hscheduler->timerPtr[Timer - 1u].StartFlag = 0u;
    exit_flag = TRUE;
  }
  return exit_flag;
}

void HIL_SCHEDULER_Init( Scheduler_HandleTypeDef *hscheduler )
{
  hscheduler->tasksCount      = 0u;
  hscheduler->TimerCount      = 0u;
  /* Timers initialization */
  for(uint32_t timer_index = 0u; timer_index < hscheduler->timers; timer_index++ )
  {
    hscheduler->timerPtr[timer_index].callbackPtr = NULL;
    hscheduler->timerPtr[timer_index].Count       = 0u;
    hscheduler->timerPtr[timer_index].StartFlag   = 0u;
    hscheduler->timerPtr[timer_index].Timeout     = 0u; 

  }

  for( uint32_t i = 0u; i < hscheduler->tasks; i++ )
  {
    hscheduler->taskPtr[i].initFunc = NULL;
    hscheduler->taskPtr[i].taskFunc = NULL;
    hscheduler->taskPtr[i].elapsed  = 0u;
    hscheduler->taskPtr[i].period   = 0u;
    hscheduler->taskPtr[i].Run_Flag = 1u;
    
  }
}

uint8_t HIL_SCHEDULER_RegisterTask( Scheduler_HandleTypeDef *hscheduler, void (*InitPtr)(void), void (*TaskPtr)(void), uint32_t Period )
{
  uint8_t exit_flag =  0u;
  if( ((Period % hscheduler->tick) == 0u)  && (Period > 0u) && (TaskPtr != NULL) ) 
  { 
    hscheduler->taskPtr[hscheduler->tasksCount].initFunc = InitPtr;
    hscheduler->taskPtr[hscheduler->tasksCount].taskFunc = TaskPtr;
    hscheduler->taskPtr[hscheduler->tasksCount].period   = Period;
    hscheduler->tasksCount++;
    exit_flag = hscheduler->tasksCount;
  }
  
  return exit_flag;
}

uint8_t HIL_SCHEDULER_StopTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{
  uint8_t exit_flag = 0u;
  if(task > 0u )
  {
    hscheduler->taskPtr[task - 1u].Run_Flag = 0u;  
    exit_flag = 1u;  
  }
  
  return exit_flag;
}
uint8_t HIL_SCHEDULER_StartTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task )
{
  uint8_t exit_flag = 0u;
  if(task > 0u )
  {
    hscheduler->taskPtr[task - 1u].Run_Flag = 1u;  
    exit_flag = 1u;  
  }
  
  return exit_flag;
}

uint8_t HIL_SCHEDULER_PeriodTask( Scheduler_HandleTypeDef *hscheduler, uint32_t task, uint32_t period )
{
  uint8_t exit_flag = 0u;
  if(((period % hscheduler->tick) == 0u)  && (period > 0u) && ( task > 0u ) )
  {
    hscheduler->taskPtr[task - 1u].period = period;
    exit_flag = 1u;
  }

  return exit_flag;

}

void HIL_SCHEDULER_Start( Scheduler_HandleTypeDef *hscheduler )
{
  static  uint32_t scheduler_tick;
  
  /* This runs the init fuctions */
  for(uint32_t i = 0u; i < hscheduler->tasks; i++)
  {
    if(hscheduler->taskPtr[i].initFunc != NULL)
    {
      hscheduler->taskPtr[i].initFunc();
    }
  }

  /* We get the current system tick to get our time base and compare later */
  scheduler_tick = HAL_GetTick();

  for(;;)
  {
    /* check if tick time has passed */
    if( ( HAL_GetTick() - scheduler_tick ) >= hscheduler->tick )
    {
      scheduler_tick = HAL_GetTick();

      /* Timers execution */
      for(uint32_t timer_index = 0u; timer_index < hscheduler->timers; timer_index++)
      {
        if( hscheduler->timerPtr != NULL)
        {
          if(hscheduler->timerPtr[timer_index].StartFlag == 1u )
          {
            hscheduler->timerPtr[timer_index].Count--;
          }
          if (hscheduler->timerPtr[timer_index].Count == 0u)
          {
            hscheduler->timerPtr[timer_index].StartFlag = 0u;
            if(hscheduler->timerPtr[timer_index].callbackPtr != NULL)
            {
              hscheduler->timerPtr[timer_index].callbackPtr();
            }
          }
          
        }
      }

      /* Tasks execution */
      for( uint32_t task_index = 0u; ( task_index < hscheduler->tasks ); task_index++ )
      {
        hscheduler->taskPtr[task_index].elapsed += hscheduler->tick;
        if( ( hscheduler->taskPtr[task_index].elapsed >= hscheduler->taskPtr[task_index].period ) )
        {
          if( hscheduler->taskPtr[task_index].Run_Flag == TRUE )
          {
            if(hscheduler->taskPtr[task_index].taskFunc != NULL)
            {
              hscheduler->taskPtr[task_index].taskFunc();
            }
          }
          hscheduler->taskPtr[task_index].elapsed = 0u;
        }
      }
    }
  }

}