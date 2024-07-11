/**
* @file app_clock.c
* @brief this file contains the functions implementation of the RTC
* and also implements the state machine for RTC configuration and visualization 
*
* @author Yordi Edgardo Delgado Ortiz
*
* @date 01/04/2023
*/

#include "app_bsp.h"
#include "app_clock.h"
#include "app_serial.h"
#include "hel_lcd.h"
#include <memory.h>
#include <stdlib.h>

static uint8_t AlarmSet          = FALSE;
static uint8_t AlarmActive       = FALSE;
static uint8_t BtnPush           = FALSE;
static uint8_t DisplayAlarm_Run  = FALSE;


void HAL_GPIO_EXTI_Falling_Callback( uint16_t GPIO_Pin )
{
  APP_MsgTypeDef  msg_struct  = {0};
  Clock_Events_TypeDef   Internal_Event =  CLOCK_MSG_NONE;
  (void)GPIO_Pin;
  BtnPush = TRUE;
  DisplayAlarm_Run = TRUE;
  if( AlarmActive == TRUE )
  {
    Internal_Event = CLOCK_ALARM_DEACTIVATE;
    msg_struct.msg = Internal_Event;
    (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);
  }
  else
  {
    Internal_Event = CLOCK_ALARM_DISPLAY;
    msg_struct.msg = Internal_Event;
    (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);  
  }
}
void HAL_GPIO_EXTI_Rising_Callback( uint16_t GPIO_Pin )
{
  (void) GPIO_Pin;
  BtnPush = FALSE;
  DisplayAlarm_Run = FALSE;
  (void)HIL_SCHEDULER_StartTask(&Sche,3);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc)
{
  (void)rtc;
  AlarmActive = TRUE;
}


static void Clock_DisplayAlarm(APP_Display_MsgTypeDef *msg_struct)
{
  App_Display_Events      Display_Event = DISPLAY_NO_MSG; 
  RTC_AlarmTypeDef        sAlarm        = {0};
  HAL_RTC_GetAlarm(&hrtc,&sAlarm,RTC_ALARM_A,FORMAT_BIN);
  msg_struct->sAlarm = sAlarm;
  msg_struct->Alarm_set = AlarmSet;
  if(!AlarmSet)
   {
      Display_Event = DISPLAY_ALARM_NOSET;
      msg_struct->msg = Display_Event;
      (void)HIL_QUEUE_Write(&Display_Queue,msg_struct);
   }
   else
   {
      Display_Event = DISPLAY_ALARM_SET;
      msg_struct->msg = Display_Event;
      (void)HIL_QUEUE_Write(&Display_Queue,msg_struct);
   }
}
static void RunAlarm(void)
{
  static uint32_t         i               = 0;
  APP_MsgTypeDef          msg_struct      = {0};
  Clock_Events_TypeDef    Internal_Event  = SERIAL_MSG_NONE;
  if ( i < ONESECOND ) 
  {
    HAL_GPIO_TogglePin(GPIOB,BUZZER);
    i++;
  }
  else
  {
    Internal_Event = CLOCK_ALARM_DEACTIVATE;
    msg_struct.msg = Internal_Event;
    (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);
  }
}

void One_Secon_TimerCallback(void)
{
  APP_MsgTypeDef  msg_struct  = {0};
  Clock_Events_TypeDef    Internal_Event =  CLOCK_MSG_NONE;
  
    if(!AlarmActive && !BtnPush )
    {
      Internal_Event = CLOCK_DISPLAY;
      msg_struct.msg = Internal_Event;
      (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);
    }
    if( AlarmActive == TRUE )
    {
      Internal_Event = CLOCK_ALARM_ACTIVATE;
      msg_struct.msg = Internal_Event;
      (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);      
    }
    (void)HIL_SCHEDULER_StartTimer(&Sche,TimerId); 
}

/**
 * @brief This is the structure to store all rtc clock configuration parameters.
*/
RTC_HandleTypeDef hrtc;
APP_MsgTypeDef Clock_Buffer[115];
QUEUE_HandleTypeDef   Clock_Queue; 
uint8_t  leap(uint32_t year)
{
    return ( ( ( (year % 4u ) == 0u ) && ( (year % 100u ) != 0u ) ) || ( (year % 400u) == 0u ) );
}

/* Returns a number representing the number of days since March 1 in the
* hypothetical year 0, not counting the change from the Julian calendar
* to the Gregorian calendar that occured in the 16th century. This
* algorithm is loosely based on a function known as "Zeller's Congruence".
* This number MOD 7 gives the day of week, where 0 = Monday and 6 = Sunday.
*/
static int zeller (int year, int month, int day)
{
  int yearMod   = year;
  int monthMod = month;
  yearMod += ((month + 9 ) / 12 ) - 1;
  monthMod = (month + 9 ) % 12;
  return (int)leap (yearMod) + (monthMod * 30 ) + ( ( ( 6 * monthMod) + 5 ) / 10 ) + day;
}
  
static int Day_Of_Week (int year, int month, int day)
{
  return (zeller (year, month, day) % 7 ) + 1;
}

static void Clock_Do_Nothing(APP_Display_MsgTypeDef *msg_struct)
{
  (void)msg_struct;
}
static void Clock_Display_Func(APP_Display_MsgTypeDef *msg_struct)
{
  RTC_TimeTypeDef       sTime           = {0};
  RTC_DateTypeDef       sDate = {0};
  //HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0);
  msg_struct->Alarm_set = AlarmSet;
  HAL_RTC_GetTime( &hrtc, &sTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate( &hrtc, &sDate, RTC_FORMAT_BIN );
  HAL_RTC_GetAlarm(&hrtc, &(msg_struct->sAlarm),RTC_ALARM_A, RTC_FORMAT_BIN);
  msg_struct->tm.tm_hour = sTime.Hours;
  msg_struct->tm.tm_min  = sTime.Minutes;
  msg_struct->tm.tm_sec  = sTime.Seconds;
  msg_struct->tm.tm_mday = sDate.Date;
  msg_struct->tm.tm_mon  = sDate.Month;
  msg_struct->tm.tm_year = sDate.Year;
  msg_struct->tm.tm_wday = sDate.WeekDay;
  msg_struct->msg        = DISPLAY_TIMESTR_PROCESSING;
  (void)HIL_QUEUE_Write(&Display_Queue,msg_struct);
}

static void Clock_Time_Config(APP_Display_MsgTypeDef *msg_struct)
{
  Clock_Events_TypeDef  Internal_Event;
  RTC_TimeTypeDef       sTime           = {0};
  sTime.Hours   = msg_struct->tm.tm_hour;
  sTime.Minutes = msg_struct->tm.tm_min;
  sTime.Seconds = msg_struct->tm.tm_sec;
  sTime.SubSeconds = 0x00;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime( &hrtc, &sTime, RTC_FORMAT_BIN);
  if(AlarmActive == TRUE)
  {
    (void)memset(msg_struct,0,sizeof(msg_struct));
    Internal_Event  = CLOCK_ALARM_DEACTIVATE;
    msg_struct->msg = Internal_Event;
    (void)HIL_QUEUE_Write(&Clock_Queue,msg_struct);
  }
  (void)memset(msg_struct,0,sizeof (msg_struct));
  Internal_Event  = CLOCK_DISPLAY;
  msg_struct->msg = Internal_Event;
  (void)HIL_QUEUE_Write(&Clock_Queue,msg_struct);
}

static void Clock_Date_Config(APP_Display_MsgTypeDef *msg_struct)
{
  Clock_Events_TypeDef  Internal_Event;
  RTC_DateTypeDef sDate = {0};
  sDate.Month   = msg_struct->tm.tm_mon;
  sDate.Date    = msg_struct->tm.tm_mday;
  sDate.Year    = msg_struct->tm.tm_year ; 
  sDate.WeekDay = Day_Of_Week(msg_struct->tm.tm_year,msg_struct->tm.tm_mon,msg_struct->tm.tm_mday);
  HAL_RTC_SetDate( &hrtc, &sDate, RTC_FORMAT_BIN );
   if(AlarmActive == TRUE)
  {
    (void)memset(msg_struct,0,sizeof(msg_struct));
    Internal_Event =  CLOCK_ALARM_DEACTIVATE;
    msg_struct->msg = Internal_Event;
    (void)HIL_QUEUE_Write(&Clock_Queue,msg_struct);
  }
  (void)memset(msg_struct,0,sizeof(msg_struct));
  Internal_Event  = CLOCK_DISPLAY;
  msg_struct->msg = Internal_Event;
  (void)HIL_QUEUE_Write(&Clock_Queue,msg_struct);
}
static void Clock_Alarm_Config(APP_Display_MsgTypeDef *msg_struct)
{
  Clock_Events_TypeDef  Internal_Event;
  AlarmSet = (~AlarmSet) & 0x01;
  msg_struct->Alarm_set = AlarmSet;
  msg_struct->sAlarm.AlarmTime.Hours    = msg_struct->tm.tm_hour;
  msg_struct->sAlarm.AlarmTime.Minutes  = msg_struct->tm.tm_min;
  msg_struct->sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  msg_struct->sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  msg_struct->sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  msg_struct->sAlarm.AlarmMask = RTC_ALARMMASK_SECONDS | RTC_ALARMMASK_DATEWEEKDAY;
  msg_struct->sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  msg_struct->sAlarm.AlarmDateWeekDay = 1;
  msg_struct->sAlarm.Alarm = RTC_ALARM_A;
  if( AlarmSet == TRUE )
  {
    HAL_RTC_SetAlarm_IT(&hrtc,&(msg_struct->sAlarm), FORMAT_BIN);
  }
  else
  {
    (void)memset(msg_struct,0,sizeof(msg_struct));
    Internal_Event = CLOCK_ALARM_DEACTIVATE;
    msg_struct->msg = Internal_Event;
    (void)HIL_QUEUE_Write(&Clock_Queue,msg_struct);
  }
}
static void Clock_Alarm_Deactive (APP_Display_MsgTypeDef *msg_struct )
{
  (void)msg_struct;
  AlarmActive = 0;
  AlarmSet    = 0;
  HAL_GPIO_WritePin(GPIOB,BUZZER,RESET);
  HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A);
}

static void Clock_Alarm_Active (APP_Display_MsgTypeDef *msg_struct)
{
  App_Display_Events event;
  RunAlarm();
  (void)memset(msg_struct,0,sizeof(msg_struct));
  event = DISPLAY_ALARM_ACTIVE;
  msg_struct->msg = event;
  (void)HIL_QUEUE_Write(&Display_Queue,msg_struct);
}

/**
* @brief  Function for clock initialization 
*         we set the RTC in 24 hours time format and set the correct prescaler parameters to get 1 hz in the RTC form the LSE clock. This parameters can be obtain from the table in the Data Sheet.
* @param
*         None. 
*/
void Clock_Init( void )
{
  /*configuration*/
  hrtc.Instance             = RTC;
  hrtc.Init.HourFormat      = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv    = 127;
  hrtc.Init.SynchPrediv     = 255;
  hrtc.Init.OutPut          = RTC_OUTPUT_DISABLE;
  /* initialize the RTC with 24 hour format and no output signal enable */
  HAL_RTC_Init( &hrtc );
  Display_Queue.Buffer = (void*)Display_Buffer; // indicamos el buffer que usara la cola como espacio de memoria
  Display_Queue.Elements = 230u;         // indicamos el numero de elementos maximo que podra alamcenar
  Display_Queue.Size = sizeof( APP_Display_MsgTypeDef ) + 1u; // indicamos el tama;o en bytes del tipo de elemntos a manejar
  HIL_QUEUE_Init(&Display_Queue);
  (void)HIL_SCHEDULER_StartTimer(&Sche,TimerId);
}

/**
* @brief Function that implements the clock state machine 
*        the state machine is in charge to validate and apply the configuration to the RTC
*        send by the serial task and display the date and time.
*        
* @details  This State machine implements 7 States as defined in the states_clock enum, in the app_clock.h file 
*           
*           CLOCK_IDLE      Wait for a message from the serial state machine to arrive.
*           CLOCK_MESSAGE   Evaluate which message have arrived.
*           CLOCK_TIME      Apply the time  configuration received from the serial state machine into the RTC
*           CLOCK_DATE      Apply the date  configuration received from the serial state machine into the RTC
*           CLOCK_ALARM     Apply the alarm configuration received from the serial state machine into the RTC
*           CLOCK_DISPLAY   Fill  the message passing structure with the data get from the RTC to be displayed for the Display state machine
* @param 
*           None.
*/
void Clock_Task( void )
{
  static const Clock_EventMachine_Typedef Clock_EventsMachine[] = 
  {
    {CLOCK_MSG_NONE,Clock_Do_Nothing},
    {CLOCK_TIME,Clock_Time_Config},
    {CLOCK_DATE,Clock_Date_Config},
    {CLOCK_ALARM,Clock_Alarm_Config},
    {CLOCK_DISPLAY,Clock_Display_Func},
    {CLOCK_ALARM_ACTIVATE,Clock_Alarm_Active},
    {CLOCK_ALARM_DEACTIVATE,Clock_Alarm_Deactive},
    {CLOCK_ALARM_DISPLAY,Clock_DisplayAlarm}
  };
  APP_Display_MsgTypeDef  msg_struct  = {0};
  Clock_Events_TypeDef    Event;    
  /* get systick to display the date and time every second */
 
  while(!HIL_QUEUE_IsEmptyISR(&Clock_Queue,0xff))
  {
    (void)HIL_QUEUE_Read(&Clock_Queue,&msg_struct);
    Event = msg_struct.msg;
    if(Event < CLOCK_LAST)
    {
      (*Clock_EventsMachine[Event].func)(&msg_struct);
    }
  } 

}
