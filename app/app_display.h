#ifndef APP_DISPLAY_H__
#define APP_DISPLAY_H__
  
  #include "app_serial.h"
  #include "app_clock.h"

  extern uint32_t display_task_tick;
  /* extern char TimeStr[10];
  extern char DateStr[14]; */
  extern SPI_HandleTypeDef SpiHandle; /*estructura para manejar el spi*/
  
  

  void display_Init(void);
  void display_Task(void);

  

  typedef struct Display_EventMachine
  {
    App_Display_Events event;
    void(*const func)(APP_Display_MsgTypeDef *msg_struct);
  }Display_EventMachine_Typedef;


#endif