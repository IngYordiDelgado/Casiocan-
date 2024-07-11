/**
* @file app_serial.h
* @brief this header file  contain all required
* variable, structures and  CAN functions definitions for the serial State Machine.
* @author Yordi Edgardo Delgado Ortiz
*
* @date 01/03/2023
*/

#ifndef APP_SERIAL_H__
#define APP_SERIAL_H__
#include <stdint.h>
#include "app_bsp.h"
#include "hil_queue.h"

#define   SERIAL_PERIOD            10U
#define   Heart_BEAT_PERIOD       300U
#define   CLOCK_PERIOD             50U
#define   DISPLAY_PERIOD          100U


typedef  uint8_t CAN_MsgTypeDef[8];

/* Global variable definitions */
extern FDCAN_HandleTypeDef CANHandler;
//extern uint8_t  RxData[8];
extern uint8_t  bytes_received;
extern uint8_t  received_flag;
extern uint8_t  date_most_significative_backup; 
extern uint32_t serial_tick;
extern void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs );
extern uint8_t  leap(uint32_t year);

/**
 * @brief  This structure is used for the storage of the rtc time,date,alarm parameters
 */
  typedef struct _APP_TmTypeDef 
{
    uint32_t tm_sec;         /* seconds,  range 0 to 59          */
    uint32_t tm_min;         /* minutes, range 0 to 59           */
    uint32_t tm_hour;        /* hours, range 0 to 23             */
    uint32_t tm_mday;        /* day of the month, range 1 to 31  */
    uint32_t tm_mon;         /* month, range 0 to 11             */
    uint32_t tm_year;        /* years in rage 1900 2100          */
    uint32_t tm_wday;        /* day of the week, range 0 to 6    */
    uint32_t tm_yday;        /* day in the year, range 0 to 365  */
    uint32_t tm_isdst;       /* daylight saving time             */
}APP_TmTypeDef;

/**
 * @brief  Structure used for message passing 
 */

typedef struct _APP_MsgTypeDef
{
    uint8_t msg;          /*!< Store the message type to send */
    APP_TmTypeDef tm;     /*!< time and date in stdlib tm format */
}APP_MsgTypeDef;



/**
 * @brief  instantiation of the message struct
 */
//extern APP_MsgTypeDef       Serial_Buffer[23];
extern QUEUE_HandleTypeDef  Serial_Queue;
extern APP_MsgTypeDef       Clock_Buffer[115];
extern QUEUE_HandleTypeDef  Clock_Queue; 

/* Function prototypes */
extern void Serial_Init( void );
extern void Serial_Task( void );

/**
 * @brief  Enumeration used for message type definition
 */
typedef enum
{
    SERIAL_MSG_NONE = 0u, /* No message received */
    SERIAL_MSG_TIME_CONFIG,      /* Time configuration received */
    SERIAL_MSG_DATE_CONFIG,      /* Date configuration received */  
    SERIAL_MSG_ALARM_CONFIG,     /* Alarm configuration received */
    SERIAL_OK_MSG,
    SERIAL_ERROR_MSG,
    SERIAL_LAST
}APP_Serial_Events;

typedef struct EventsMachine
{
  APP_Serial_Events event;
  void(*const func)(const uint8_t *RxData);
}Serial_EventMachine_Typedef;

 


#endif