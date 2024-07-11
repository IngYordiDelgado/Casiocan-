/**
* @file app_clock.h
* @brief this header file will contain all required
* definitions for clock functionality implementation.
*
* @author Yordi Edgardo Delgado Ortiz
*
* @date 01/04/2023
*/

#ifndef APP_CLOCK_H__
#define APP_CLOCK_H__

  #include "app_bsp.h"
  #include "app_serial.h"
  #include "scheduler.h"
  
  #define ONESECOND   60u
  #define BUZZER      GPIO_PIN_14

  typedef struct _APP_Display_MsgTypeDef
  {
    uint8_t           msg;          /*!< Store the message type to send */
    APP_TmTypeDef     tm;     /*!< time and date in stdlib tm format */
    RTC_AlarmTypeDef  sAlarm;
    uint8_t           Alarm_set; 
  }APP_Display_MsgTypeDef;
  

  extern RTC_HandleTypeDef        hrtc;
  extern APP_Display_MsgTypeDef           Display_Buffer[230];
  extern QUEUE_HandleTypeDef      Display_Queue;  
  extern Scheduler_HandleTypeDef  Sche;
  extern uint32_t TimerId;

  extern void Clock_Init( void );
  extern void Clock_Task( void );
  extern void HAL_GPIO_EXTI_Falling_Callback( uint16_t GPIO_Pin );
  extern void HAL_GPIO_EXTI_Rising_Callback( uint16_t GPIO_Pin );
  extern void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc);
  extern void One_Secon_TimerCallback(void);
  extern uint8_t  leap(uint32_t year);
  typedef enum DisplayEvents{
    DISPLAY_NO_MSG  = 0u,
    DISPLAY_ALARM_ACTIVE,
    DISPLAY_ALARM_SET,
    DISPLAY_ALARM_NOSET,
    DISPLAY_TIMESTR_PROCESSING,
    DISPLAY_DATESTR_PROCESSING,
    DISPLAY_LCD_WRITE,
    DISPLAY_LAST  
  }App_Display_Events;
  
 
  typedef enum Clock_Events{
      CLOCK_MSG_NONE = 0u,
      CLOCK_TIME,
      CLOCK_DATE,
      CLOCK_ALARM,
      CLOCK_DISPLAY,
      CLOCK_ALARM_ACTIVATE,
      CLOCK_ALARM_DEACTIVATE,
      CLOCK_ALARM_DISPLAY,
      CLOCK_LAST
    }Clock_Events_TypeDef;
  
  typedef struct Clock_EventMachine
  {
    Clock_Events_TypeDef event;
    void(*const func)(APP_Display_MsgTypeDef *msg_struct);
  }Clock_EventMachine_Typedef;
#endif
