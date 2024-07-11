#include "app_display.h"
#include "hel_lcd.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/*Definitions for Lcd */
LCD_HandleTypeDef LCDHandler;
TIM_HandleTypeDef TIM6_Handler;

/*Definitions for spi*/
SPI_HandleTypeDef SpiHandle; /*estructura para manejar el spi*/

APP_Display_MsgTypeDef Display_Buffer[230];
QUEUE_HandleTypeDef   Display_Queue;  

static char       TimeStr[10];
static char       DateStr[16];



static void AlarmFormat(char *str,RTC_AlarmTypeDef  *Alarm)
{
  char minutes  [3]   = {0};
  char hours    [3]   = {0};
  __itoa(Alarm->AlarmTime.Minutes,minutes,10); 
  if( Alarm->AlarmTime.Minutes < 10u ) 
  {
    char swap;
    swap = minutes[0];
    minutes[0] = '0';
    minutes[1] = swap;
    minutes[2] = '\0';
  }
  __itoa(Alarm->AlarmTime.Hours,hours,10); 
  if( Alarm->AlarmTime.Hours < 10u ) 
  {
    char swap;
    swap = hours[0];
    hours[0] = '0';
    hours[1] = swap;
    hours[2] = '\0';
  }
  (void)strcat(str,hours);
  (void)strcat(str,":");
  (void)strcat(str,minutes);
}


static void TimeFormat(char *str,APP_Display_MsgTypeDef *msg_struct)
{
  char seconds  [3]   = {0};
  char minutes  [3]   = {0};
  char hours    [3]   = {0};
  __itoa(msg_struct->tm.tm_sec,seconds,10); 
  if( msg_struct->tm.tm_sec < 10u ) 
  {
    char swap;
    swap = seconds[0];
    seconds[0] = '0';
    seconds[1] = swap;
    seconds[2] = '\0';
  }
  __itoa(msg_struct->tm.tm_min,minutes,10); 
  if( msg_struct->tm.tm_min < 10u ) 
  {
    char swap;
    swap = minutes[0];
    minutes[0] = '0';
    minutes[1] = swap;
    minutes[2] = '\0';
  }
  __itoa(msg_struct->tm.tm_hour,hours,10); 
  if( msg_struct->tm.tm_hour < 10u ) 
  {
    char swap;
    swap = hours[0];
    hours[0] = '0';
    hours[1] = swap;
    hours[2] = '\0';
  }
  (void)strcat(str,hours);
  (void)strcat(str,":");
  (void)strcat(str,minutes);
  (void)strcat(str,":");
  (void)strcat(str,seconds);
}

static void DateFormat(char *str,APP_Display_MsgTypeDef *msg_struct)
{

  char *months  [12]  = {
      "ENE,",
      "FEB,",
      "MAR,",
      "ABR,",
      "MAY,",
      "JUN,",
      "JUL,",
      "AGO,",
      "SEP,",
      "OCT,",
      "NOV,",
      "DIC," };

  char *Wdays  [7]   =  {
      "Lu",
      "MA",
      "Mi",
      "Ju",
      "Vi",
      "Sa",
      "Do"
  };    
  char day        [3];
  char year_least [3];
  char year_most  [3];
  __itoa(msg_struct->tm.tm_mday,day,10); 
  if( msg_struct->tm.tm_mday < 10u ) 
  {
    char swap;
    swap = day[0];
    day[0] = '0';
    day[1] = swap;
    day[2] = '\0';
  }

   __itoa(msg_struct->tm.tm_year,year_least,10); 
  if( msg_struct->tm.tm_year < 10u ) 
  {
    char swap;
    swap = year_least[0];
    year_least[0] = '0';
    year_least[1] = swap;
    year_least[2] = '\0';
  }
  __itoa(date_most_significative_backup,year_most,10);
  if( date_most_significative_backup < 10u ) 
  {
    char swap;
    swap = year_most[0];
    year_most[0] = '0';
    year_most[1] = swap;
    year_most[2] = '\0';
  }
  (void)strcat(str,months[(msg_struct->tm.tm_mon) -1u ] );
  (void)strcat(str,day);
  (void)strcat(str,"\v");
  (void)strcat(str,year_most);
  (void)strcat(str,year_least);
  (void)strcat(str,"\v");
  (void)strcat(str,Wdays[(msg_struct->tm.tm_wday) - 1u ]);
} 

void display_Init(void)
{
   
  /*Configuramos al spi en modo maestro, comunicacion full-duplex, polaridad
  del reloj en alto y fase en flanco de bajada */
  SpiHandle.Instance                  = SPI2;
  SpiHandle.Init.Mode                 = SPI_MODE_MASTER;
  SpiHandle.Init.BaudRatePrescaler    = SPI_BAUDRATEPRESCALER_16;
  SpiHandle.Init.Direction            = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase             = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity          = SPI_POLARITY_HIGH;
  SpiHandle.Init.DataSize             = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit             = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS                  = SPI_NSS_SOFT;
  SpiHandle.Init.CRCCalculation       = SPI_CRCCALCULATION_DISABLED;
  SpiHandle.Init.TIMode               = SPI_TIMODE_DISABLED;
  HAL_SPI_Init( &SpiHandle );
  LCDHandler.BklPort                  = GPIOC;
  LCDHandler.BklPin                   = GPIO_PIN_12;
  LCDHandler.CsPort                   = GPIOB;
  LCDHandler.CsPin                    = GPIO_PIN_15;
  LCDHandler.RsPort                   = GPIOC;
  LCDHandler.RsPin                    = GPIO_PIN_9;
  LCDHandler.RstPort                  = GPIOC;
  LCDHandler.RstPin                   = GPIO_PIN_8;
  LCDHandler.SpiHandler               = &SpiHandle;
  Delay_us_Init();
  HEL_LCD_Init(&LCDHandler);
}

static void Display_TimeStr_Processing(APP_Display_MsgTypeDef *msg_struct)
{
  TimeFormat(TimeStr,msg_struct);
  msg_struct->msg = DISPLAY_DATESTR_PROCESSING;
  (void)HIL_QUEUE_Write(&Display_Queue,msg_struct);
}

static void Display_DateStr_Processing(APP_Display_MsgTypeDef *msg_struct)
{
  DateFormat(DateStr,msg_struct);
  msg_struct->msg = DISPLAY_LCD_WRITE;
  (void)HIL_QUEUE_Write(&Display_Queue,msg_struct);
}

static void Display_Lcd_Write(APP_Display_MsgTypeDef *msg_struct)
{
  
  HEL_LCD_Backlight(&LCDHandler,SET);
  HEL_LCD_Clear(&LCDHandler);
 
  HEL_LCD_SetCursor(&LCDHandler, 0, 1);
  HEL_LCD_String(&LCDHandler,DateStr);
  (void)strcpy(DateStr,"");
  if(msg_struct->Alarm_set == TRUE)
  {
    HEL_LCD_SetCursor(&LCDHandler, 1, 0);
    HEL_LCD_Data(&LCDHandler,'A');
  }
  HEL_LCD_SetCursor(&LCDHandler, 1, 3);
  
  HEL_LCD_String(&LCDHandler,TimeStr);
  (void)strcpy(TimeStr,"");
  (void)memset(&msg_struct,0,sizeof msg_struct);
  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_2);
}

static void Display_Alamr_Active (APP_Display_MsgTypeDef *msg_struct )
{
  static uint8_t    lcdBacklight_state = 0;
  (void)msg_struct;
  HEL_LCD_SetCursor(&LCDHandler,1,0);
  HEL_LCD_Data(&LCDHandler,0);
  HEL_LCD_SetCursor(&LCDHandler,1,3);
  HEL_LCD_Data(&LCDHandler,0);
  HEL_LCD_String(&LCDHandler,"ALARM!!!");
  lcdBacklight_state = (~lcdBacklight_state) & 0x01;
  HEL_LCD_Backlight(&LCDHandler,lcdBacklight_state);
}

static void Display_Alarm_Set (APP_Display_MsgTypeDef *msg_struct)
{
  
  char              AlarmStr[12]  = {0};
  char              Str[6]        = {0}; 
  HEL_LCD_SetCursor(&LCDHandler,1,0);
  HEL_LCD_Data(&LCDHandler,0);
  HEL_LCD_SetCursor(&LCDHandler,1,2);
  AlarmFormat(Str,&(msg_struct->sAlarm));
  (void)strcat(AlarmStr,"ALARM=");
  (void)strcat(AlarmStr,Str);
  HEL_LCD_String(&LCDHandler,AlarmStr);
  (void)memset(msg_struct,0,sizeof(msg_struct));
  (void)HIL_SCHEDULER_StopTask(&Sche,3);
}
static void Display_Alarm_No_Set(APP_Display_MsgTypeDef *msg_struct)
{
  
  HEL_LCD_SetCursor(&LCDHandler,1,0);
  HEL_LCD_String(&LCDHandler,"ALARM");
  HEL_LCD_Data(&LCDHandler,0);
  HEL_LCD_String(&LCDHandler,"NO");
  HEL_LCD_Data(&LCDHandler,0);
  HEL_LCD_String(&LCDHandler,"CONFIG");
  (void)memset(msg_struct,0,sizeof(msg_struct));
  (void)HIL_SCHEDULER_StopTask(&Sche,3);
}
static void Display_Do_Nothing (APP_Display_MsgTypeDef *msg_struct)
{
  (void)msg_struct;
}


void display_Task(void)
{
  
  APP_Display_MsgTypeDef msg_struct;
  App_Display_Events event = DISPLAY_NO_MSG;
  msg_struct.msg = event;
  static const Display_EventMachine_Typedef Display_EventsMachine[] = 
  {
    {DISPLAY_NO_MSG,Display_Do_Nothing},
    {DISPLAY_ALARM_ACTIVE,Display_Alamr_Active},
    {DISPLAY_ALARM_SET,Display_Alarm_Set},
    {DISPLAY_ALARM_NOSET,Display_Alarm_No_Set},
    {DISPLAY_TIMESTR_PROCESSING,Display_TimeStr_Processing},
    {DISPLAY_DATESTR_PROCESSING,Display_DateStr_Processing},
    {DISPLAY_LCD_WRITE,Display_Lcd_Write}
  };
  while(!HIL_QUEUE_IsEmpty(&Display_Queue))
  {
    (void)HIL_QUEUE_Read(&Display_Queue,&msg_struct);
    event = msg_struct.msg;
    if( event < DISPLAY_LAST )
    {
      (*Display_EventsMachine[event].func)(&msg_struct);
    }
   
  }
}

