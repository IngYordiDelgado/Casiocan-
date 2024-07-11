#include <stdint.h>
#include <memory.h>
#include "hil_queue.h"
#include "app_bsp.h"
void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue ){
    
    hqueue->Head     = 0u;
    hqueue->Tail     = 0u;
    hqueue->Counter  = 0u;
    hqueue->Full     = 0u;
    hqueue->Empty    = 1u;
}
uint8_t HIL_QUEUE_IsEmpty(QUEUE_HandleTypeDef *hqueue)
{
    return hqueue->Empty;
}


uint8_t HIL_QUEUE_IsFull(QUEUE_HandleTypeDef *hqueue)
{
    return hqueue->Full;	
}

uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
  uint8_t exit_flag = 0;
   if ( !HIL_QUEUE_IsFull(hqueue) )
   {
      (void) memcpy(&((uint8_t *)hqueue->Buffer)[hqueue->Tail], data, hqueue->Size); /* cppcheck-suppress misra-c2012-11.5; void pointer conversion need */
      hqueue->Tail += hqueue->Size;
      hqueue->Tail %= (hqueue->Elements * hqueue->Size);
      hqueue->Counter++;
      hqueue->Empty = 0u; 
      if(hqueue->Counter == hqueue->Elements)
      {
          hqueue->Full = 1u;    
      }
      exit_flag = 1;
   }
  
  return exit_flag;
}

uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t exit_flag = 0;
    if ( !HIL_QUEUE_IsEmpty(hqueue) )
    {
       (void) memcpy(data,&((uint8_t *)hqueue->Buffer)[hqueue->Head], hqueue->Size);  /* cppcheck-suppress misra-c2012-11.5; void pointer conversion need */
        hqueue->Head += hqueue->Size;
        hqueue->Counter--;
        hqueue->Head %= (hqueue->Elements * hqueue->Size);
        hqueue->Full = 0u;
        if(hqueue->Counter == 0u)
            {
                hqueue->Empty = 1u;    
                hqueue->Tail  = 0u;
                hqueue->Head  = 0u;
            }  
      exit_flag = 1u;
    }
     
    return exit_flag;    
}

void HIL_QUEUE_Flush( QUEUE_HandleTypeDef *hqueue ) /* cppcheck-suppress misra-c2012-8.7; cpp check  bug  this is a false/positive */
{
  (void) memset(hqueue->Buffer,0,( hqueue->Elements));
  HIL_QUEUE_Init(hqueue);
}

uint8_t HIL_QUEUE_WriteISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr )
{
  uint8_t exit_flag;
  HAL_NVIC_DisableIRQ(isr);
  exit_flag = HIL_QUEUE_Write(hqueue,data);
  HAL_NVIC_EnableIRQ(isr);
  return exit_flag;
}

uint8_t HIL_QUEUE_ReadISR( QUEUE_HandleTypeDef *hqueue, void *data, uint8_t isr )
{
  uint8_t exit_flag;
  HAL_NVIC_DisableIRQ(isr);
  exit_flag = HIL_QUEUE_Read(hqueue,data);
  HAL_NVIC_EnableIRQ(isr);
  return exit_flag;
}

uint8_t HIL_QUEUE_IsEmptyISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr )
{
  uint8_t exit_flag;
  HAL_NVIC_DisableIRQ(isr);
  exit_flag = HIL_QUEUE_IsEmpty(hqueue);
  HAL_NVIC_EnableIRQ(isr);
  return exit_flag;
}

void HIL_QUEUE_FlushISR( QUEUE_HandleTypeDef *hqueue, uint8_t isr )
{
  HAL_NVIC_DisableIRQ(isr);
  HIL_QUEUE_Flush(hqueue);
  HAL_NVIC_EnableIRQ(isr); 

}