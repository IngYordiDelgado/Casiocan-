/**
* @file app_serial.c
* @brief this file contains the functions implementation of the CAN Communication
* and also implements the state machine for CAN message validation and processing 
*
* @author Yordi Edgardo Delgado Ortiz
*
* @date 01/03/2023
*/

#include "app_serial.h"
#include "app_bsp.h"
#include "hil_queue.h"


/**
 * @brief  Structure to store clock configuration 
 */

QUEUE_HandleTypeDef Serial_Queue;    // Estructura de control dela cola 


/* type declarations for can initialization */

/**
 * @brief  structure for can initialization
 */
FDCAN_HandleTypeDef CANHandler;

/**
 * @brief  structure for the can tx header
 */
static FDCAN_TxHeaderTypeDef CANTxHeader;

//static FDCAN_RxHeaderTypeDef CANRxHeader;

uint8_t received_flag;



/**
 * @brief  Variable that stores the number of bytes received
 */
uint8_t bytes_received ;



/**
 * @brief Variable to store the most significative part of the year
 */
uint8_t date_most_significative_backup;






/**
* @brief    Function for can initialization
* @param  
*           None.
* @details  This Function configure the CAN Module to a baudrate of 250 kbps and a sample rate of 75%.
*/
void Serial_Init( void )
{
  /**
 * @brief  structure for filter configuration 
 */
  FDCAN_FilterTypeDef   CANFilter;
  static CAN_MsgTypeDef Serial_Buffer[23];
  /*Set configuration for can module*/
  CANHandler.Instance                 = FDCAN1;
  CANHandler.Init.Mode                = FDCAN_MODE_NORMAL;
  CANHandler.Init.FrameFormat         = FDCAN_FRAME_CLASSIC;
  CANHandler.Init.ClockDivider        = FDCAN_CLOCK_DIV2;
  CANHandler.Init.TxFifoQueueMode     = FDCAN_TX_FIFO_OPERATION;
  CANHandler.Init.AutoRetransmission  = DISABLE;
  CANHandler.Init.TransmitPause       = DISABLE;
  CANHandler.Init.ProtocolException   = DISABLE;
  CANHandler.Init.ExtFiltersNbr       = 0;
  CANHandler.Init.StdFiltersNbr       = 1;  
  CANHandler.Init.NominalPrescaler    = 4;
  CANHandler.Init.NominalSyncJumpWidth = 1;
  CANHandler.Init.NominalTimeSeg1     = 11;
  CANHandler.Init.NominalTimeSeg2     = 4;
  /*Apply configuration to CAN Module */
  HAL_FDCAN_Init( &CANHandler );
  /* Configure reception filter to Rx FIFO 0*/
  CANFilter.IdType = FDCAN_STANDARD_ID;
  CANFilter.FilterIndex = 0;
  CANFilter.FilterType = FDCAN_FILTER_MASK;
  CANFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  CANFilter.FilterID1 = 0x111;
  CANFilter.FilterID2 = 0x7FF;
  HAL_FDCAN_ConfigFilter( &CANHandler, &CANFilter );
  HAL_FDCAN_ConfigGlobalFilter(&CANHandler, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

  /* CAN transmission parameters */
  CANTxHeader.IdType      = FDCAN_STANDARD_ID;
  CANTxHeader.FDFormat    = FDCAN_CLASSIC_CAN;
  CANTxHeader.TxFrameType = FDCAN_DATA_FRAME;
  CANTxHeader.Identifier  = 0x122;
  CANTxHeader.DataLength  = FDCAN_DLC_BYTES_8;
  
  /* Change FDCAN instance from initialization mode to normal mode */
  HAL_FDCAN_Start( &CANHandler);
  HAL_FDCAN_ActivateNotification( &CANHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0 );

  Serial_Queue.Buffer   = (void*)Serial_Buffer; // indicamos el buffer que usara la cola como espacio de memoria
  Serial_Queue.Elements = 23u;         // indicamos el numero de elementos maximo que podra alamcenar
  Serial_Queue.Size     = sizeof(CAN_MsgTypeDef); // indicamos el tama;o en bytes del tipo de elemntos a manejar
  HIL_QUEUE_Init(&Serial_Queue);

  Clock_Queue.Buffer = (void*)Clock_Buffer; // indicamos el buffer que usara la cola como espacio de memoria
  Clock_Queue.Elements = 115u;         // indicamos el numero de elementos maximo que podra alamcenar
  Clock_Queue.Size = sizeof( APP_MsgTypeDef ) + 1u; // indicamos el tama;o en bytes del tipo de elemntos a manejar
  HIL_QUEUE_Init(&Clock_Queue);
  
}

/**
* @brief Private function to transmit Can message of a desire length defined by size and send the payload pointed by data
* the fuction accepts values between 0 and 8, if a different value is introduce, it will apply the default size of 8 bytes 
* @param data Pointer to the payload to send 
* @param size length of the CAN message
*/
static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size )
{
  data[0] = size;
  HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );

}
/**
* @brief  Private function to read messages from CAN, where data will be stored in *data and the number of bytes received in *size.
*         If the function returns 1 then a certain number of bytes were received, otherwise, no message was received yet. 
* @param  data Pointer to store the message 
* @param  size Pointer to store the number of bytes received.
* @return 1 if receive something 0 otherwise 
*/
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size )
{
  uint8_t exit_flag = 0;
  if( (data[0] >= 0u) && (data[0] <= 7u) )
  {  
    *size = data[0];
    exit_flag = 1;
  }
  else
  {
    data[1] = SERIAL_MSG_NONE;
  }
  return exit_flag;
}

void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
{
  (void)hfdcan;
  FDCAN_RxHeaderTypeDef CANRxHeader;
  CAN_MsgTypeDef RxData;
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
  {
    HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, RxData );
    if(!HIL_QUEUE_IsFull(&Serial_Queue))
    {
      if(CanTp_SingleFrameRx( RxData,&bytes_received) == 1u )
      {
         (void)HIL_QUEUE_WriteISR(&Serial_Queue,RxData,0xff);
      } 
    }
    
  }
}

/**
* @brief Private function to convert bcd format number to decimal
* @param x number to convert from bcd to decimal 
* @return return the result of the conversion 
*/
static uint8_t bcd_to_decimal(uint8_t x)
{
    return (x - (6u * (x >> 4u)));
}

static uint8_t date_validation(int d,int m, int y)
{
   uint8_t daysinmonth[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   uint8_t exit_flag = 0;
   if( (y >= 1900 ) && ( y <= 2100 ))
   {
    // leap year checking, if ok add 29 days to february
    if(leap(y) == 1u)
    {
     daysinmonth[1]=29;
    }
    // days in month checking
    if (m<13)
    {
       if( (uint8_t)d <= daysinmonth[m-1] )
       {
         exit_flag=1;
       }
    }
   }
  return exit_flag;
}

static void Do_Nothing(const uint8_t *RxData)
{
  (void)RxData;
}
static uint8_t Time_Config_IsValid(const uint8_t *RxData)
{
  uint8_t exit_flag = 0u;
  if( ( (bcd_to_decimal(RxData[2]) >= 0u) && (bcd_to_decimal(RxData[2]) <= 24u) ) && 
      ( (bcd_to_decimal(RxData[3]) >= 0u) && (bcd_to_decimal(RxData[3]) <= 59u) ) && 
      ( (bcd_to_decimal(RxData[4]) >= 0u) && (bcd_to_decimal(RxData[4]) <= 59u) ) ) 
  {
    exit_flag = 1u;
  }
  return exit_flag;
}
static void Time_Config(const uint8_t *RxData)
{
  APP_Serial_Events serial_msg;
  APP_MsgTypeDef msg_struct;
  CAN_MsgTypeDef Internal_Event;
  if(Time_Config_IsValid(RxData) == 1u)
  {
    
    serial_msg     = SERIAL_MSG_TIME_CONFIG;
    msg_struct.msg = serial_msg;
    msg_struct.tm.tm_hour = bcd_to_decimal(RxData[2]);
    msg_struct.tm.tm_min  = bcd_to_decimal(RxData[3]);
    msg_struct.tm.tm_sec  = bcd_to_decimal(RxData[4]);
    (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);
    serial_msg = SERIAL_OK_MSG;
    Internal_Event[1] = serial_msg;
    (void)HIL_QUEUE_Write(&Serial_Queue,Internal_Event);
            
  }
  else 
  {
    serial_msg = SERIAL_ERROR_MSG;
    Internal_Event[1] = serial_msg;
    (void)HIL_QUEUE_Write(&Serial_Queue,Internal_Event);
  }
}

static void Date_Config(const uint8_t *RxData)
{
  APP_Serial_Events serial_msg;
  APP_MsgTypeDef msg_struct;
  CAN_MsgTypeDef Internal_Event;
  if ( date_validation( bcd_to_decimal(RxData[2]), bcd_to_decimal(RxData[3]),
     ( (bcd_to_decimal(RxData[4]) * 100u ) + bcd_to_decimal(RxData[5]) ) ) == 1u )
  {
    serial_msg     = SERIAL_MSG_DATE_CONFIG;
    msg_struct.msg = serial_msg;
    msg_struct.tm.tm_mday = bcd_to_decimal(RxData[2]);
    msg_struct.tm.tm_mon  = bcd_to_decimal(RxData[3]);
    msg_struct.tm.tm_year = bcd_to_decimal(RxData[5]);
    date_most_significative_backup = bcd_to_decimal(RxData[4]);
    (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);
    serial_msg = SERIAL_OK_MSG;
    Internal_Event[1] = serial_msg;
    (void)HIL_QUEUE_Write(&Serial_Queue,Internal_Event);
  }
  else 
  {
    serial_msg = SERIAL_ERROR_MSG;
    Internal_Event[1] = serial_msg;
    (void)HIL_QUEUE_Write(&Serial_Queue,Internal_Event);
  }
}

static uint8_t Alarm_Config_IsValid (const uint8_t *RxData)
{
  uint8_t exit_flag = 0;
    if( ( (bcd_to_decimal(RxData[2]) >= 0u) && (bcd_to_decimal(RxData[2]) <= 24u) ) && 
      ( (bcd_to_decimal(RxData[3]) >= 0u) && (bcd_to_decimal(RxData[3]) <= 59u) ) )
    {
      exit_flag = 1; 
    } 
    return exit_flag;
}
static void Alarm_Config(const uint8_t *RxData)
{
  APP_Serial_Events serial_msg;
  APP_MsgTypeDef msg_struct;
  CAN_MsgTypeDef Internal_Event;
  if( Alarm_Config_IsValid(RxData) == 1u ) 
  {
    serial_msg     = SERIAL_MSG_ALARM_CONFIG;
    msg_struct.msg = serial_msg;
    msg_struct.tm.tm_hour = bcd_to_decimal(RxData[2]);
    msg_struct.tm.tm_min  = bcd_to_decimal(RxData[3]);
    (void)HIL_QUEUE_Write(&Clock_Queue,&msg_struct);
    serial_msg = SERIAL_OK_MSG;
    Internal_Event[1] = serial_msg;
    (void)HIL_QUEUE_Write(&Serial_Queue,Internal_Event);
  }
  else 
  {
    serial_msg = SERIAL_ERROR_MSG;
    Internal_Event[1] = serial_msg;
    (void)HIL_QUEUE_Write(&Serial_Queue,Internal_Event);
  }
}

static void Ok_Msg_Acknowledge(const uint8_t *RxData)
{
  (void)RxData;
  uint8_t ok_msg[2]={bytes_received,0x55};
  
  CanTp_SingleFrameTx(ok_msg,1);
}


static void Error_Msg_Acknowledge(const uint8_t *RxData)
{
  (void)RxData;
  uint8_t err_msg[2]={bytes_received,0xAA};
  CanTp_SingleFrameTx(err_msg,1);
}

/**
* @brief This function implements the CAN state machine that proccess and store the information
* for the clock configuration.It manage all the CAN reception,transmission and message validation
*/
void Serial_Task( void )
{ 
  uint8_t RxData[8];
  uint8_t event = SERIAL_MSG_NONE;

  static const Serial_EventMachine_Typedef Events_Machine[] =
  { 
    {SERIAL_MSG_NONE,Do_Nothing},
    {SERIAL_MSG_TIME_CONFIG,Time_Config},
    {SERIAL_MSG_DATE_CONFIG,Date_Config},
    {SERIAL_MSG_ALARM_CONFIG,Alarm_Config},
    {SERIAL_OK_MSG,Ok_Msg_Acknowledge},
    {SERIAL_ERROR_MSG,Error_Msg_Acknowledge}
  };

  while(!HIL_QUEUE_IsEmptyISR(&Serial_Queue, TIM16_FDCAN_IT0_IRQn))
  {
   
    if(HIL_QUEUE_Read(&Serial_Queue,RxData) == 1u )
    {
      event = RxData[1];        
    }
    if( event < (uint8_t)SERIAL_LAST)
    {
      (*Events_Machine[event].func)(RxData);
    }
    
  }   
     
}