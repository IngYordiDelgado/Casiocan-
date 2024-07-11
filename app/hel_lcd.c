/**
* @file hel_lcd.c
* @brief    This is a driver implementation for the ST7032 controller.
*           this implementation use the SPI interface. 
* 
* @details  This driver implements the following interfaces
*
*           void HEL_LCD_MspInit(LCD_HandleTypeDef *hlcd) weekly linked function, The programer should implement it in the msps file.
*           This fuction should provide the correct initialization for the GPIO used by the LCD.This fuction is called by the HEL_LCD_Init().
*
*           void HEL_LCD_Init( LCD_HandleTypeDef *hlcd ) Implements the basic configuration and initialization of the lcd. 
*
*           void HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd ) This function is used to send commands to the lcd. 
*
*           void HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )   Sends a character to be write in the lcd. 
*
*           void HEL_LCD_String( LCD_HandleTypeDef *hlcd, const char *str ) Prints a string in the lcd.
*
*           void HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col ) Sets the cursor on the desire position.
*
*           void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state )  Fuction to turn on/off the display backlight.
*
*           void HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast ) Function to set the desired contrast in the lcd.
*
*           void HEL_LCD_Clear(LCD_HandleTypeDef *hlcd) Function to clear the lcd.
*
*  
* @author Yordi Edgardo Delgado Ortiz
*
* @date 01/15/2023
*/

#include "app_bsp.h"
#include "hel_lcd.h"
#include <string.h>

/**
* @brief    Weekly linked Fuction.
*           This fuction must be defined by the programmer in the app_msps.c file and must configure the GPIO used by the lcd.
*
* @param    hlcd This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains all the lcd GPIO information for initialization.
* 
* @return   void
*/
__weak void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd ) /* cppcheck-suppress [misra-c2012-8.6,misra-c2012-8.7]; Bug with weekly linked fuctions this is a false/positive */ 
{
  (void)hlcd; 
}

/**
* @brief    This Function is used to initialize the lcd with a basic configuration to start working with it.
*
* @details  Initializes the LCD so that it is ready to receive data to be displayed on its screen, using the LCD_HandleTypeDef structure, the SPI handler (already configured) 
*           to be used must be selected, as well as the pins that will be used as RST, CS, and RS. At the moment the LCD only handles the simplest configuration and cannot be changed (2x16 lines and display from left to right).
*           This fuction apply the required initialization sequence to work with the lcd.   
*
* @param    hlcd This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains all the lcd GPIO information for initialization.
* 
* @return   void
*/
void HEL_LCD_Init( LCD_HandleTypeDef *hlcd )
{
  HEL_LCD_MspInit(hlcd);  
  HAL_GPIO_WritePin(hlcd->BklPort, hlcd->BklPin, SET); /* Turn on the backlight */
  HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET);   
  HAL_GPIO_WritePin(hlcd->RstPort, hlcd->RstPin,RESET);
  HAL_Delay( 2 );
  HAL_GPIO_WritePin(hlcd->RstPort, hlcd->RstPin, SET);
  HAL_Delay( 20 );
  HEL_LCD_Command(hlcd, LCD_FUNCTIONSET_ | LCD_8BITMODE_);                                           /* LCD Wake Up*/
  HAL_Delay(2);
  HEL_LCD_Command(hlcd,LCD_FUNCTIONSET_ | LCD_8BITMODE_);                                            /* LCD Wake Up*/
  HEL_LCD_Command(hlcd,LCD_FUNCTIONSET_ | LCD_8BITMODE_);                                            /* LCD Wake Up*/
  HEL_LCD_Command(hlcd,LCD_FUNCTIONSET_ | LCD_8BITMODE_ | LCD_2LINE_ | LCD_INSTRUCTIONTABLE_1_);     /* Lcd in 8 bit mode and 2*16 lines */
  HEL_LCD_Command(hlcd,LCD_BIASINTOSCSET_ | LCD_BIAS_1_5_ | LCD_OSC_347HZ_);                         /* Set the lcd to work with the internal oscillator at 347hz*/ 
  HEL_LCD_Command(hlcd,LCD_POWICONCONTRASTHSET_ | LCD_BOOST_ON_ | LCD_CONTRAST_C5_ON_);              
  HEL_LCD_Command(hlcd,LCD_FOLLOWERCONTROLSET_ | LCD_FOLLOWER_ON_ | LCD_RAB_2_50_);                   
  HAL_Delay(200);
  HEL_LCD_Command(hlcd,LCD_CONTRASTSET_);                                                            /* Set lcd contrast */
  HEL_LCD_Command(hlcd,LCD_DISPLAYCONTROLSET_ | LCD_DISPLAYON_ | LCD_CURSORON_| LCD_BLINKOFF_);      /* Lcd display on, Cursor on, Blink off*/
  HEL_LCD_Command(hlcd,LCD_ENTRYMODESET_ | LCD_ENTRYLEFT_ | LCD_ENTRYSHIFTDECREMENT_);               /* Lcd shit from left to right */
  HEL_LCD_Command(hlcd,LCD_CLEARDISPLAY_);                                                           /* Clear the display */
  HAL_Delay( 1 );
}

/**
* @brief    This Function is used to send a command to the lcd.
*
* @param    hlcd This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @param    cmd  This is the command to send to the lcd 
* @return   void
*/
void HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{
  HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, RESET);  /*HEL_LCD_Command mode*/
  HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, RESET);  /*chip select on*/
  HAL_SPI_Transmit(hlcd->SpiHandler, &cmd, 1, 1000 );   /*Send the command thought the SPI interface */
  HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET);    /*chip select off*/
  Delay_us(30);
}

/**
* @brief    This Function is used to send a character to the lcd.
*
* @param    hlcd This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @param    data  This is the character to send to the lcd 
* @return   void
*/
void HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data ) /* cppcheck-suppress misra-c2012-8.7; Bug with weekly linked fuctions this is a false/positive */
{
    HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, SET);    /* data mode*/
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, RESET);  /* chip select on*/
    HAL_SPI_Transmit(hlcd->SpiHandler, &data, 1, 1000 );  /* send the character trough the spi interface */
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET);    /*chip select off*/
    Delay_us( 30 );
}

/**
* @brief    This Function is used to print a string in the lcd.
*
* @param    hlcd  This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @param    str   This is a pointer to a string to send to the lcd. 
* @return   void
*/ 
void HEL_LCD_String( LCD_HandleTypeDef *hlcd, const char *str )
{
  for (int i=0; i < (int)strlen(str); i++ )       /* We send character by character until the counter reach the length of the string */
  {
    HEL_LCD_Data(hlcd,str[i]);                    /* Send the character to the lcd */
  }
}

/**
* @brief    This Function is used to set the position of the cursor.
*
* @param    hlcd  This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @param    row   This is position of the row to set the cursor.
* @param    col   This is the position of the col to set the cursor. 
* @return   void
*/ 
 void HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{
  uint8_t mrow; /* Define this variables to avoid misra rule parameter should not be modified */
  uint8_t mcol; /* Define this variables to avoid misra rule parameter should not be modified */
  if( row != 0u )
  {
    mrow = 0x40; /* Set DDRAM address to the second line */
  }
  else
  {
    mrow = 0u;
  }
  if( ( col < 0u ) || ( col > 15u ) ) /* Validate the column is in a valid range if not set to position 0*/
  {
    mcol = 0u;
  }
  else
  {
    mcol = col;
  }
    HEL_LCD_Command(hlcd, (uint8_t)LCD_SETDDRAM_ADDR_ | (mrow + mcol)); /* Send the command to set the cursor */
}

/**
* @brief    This Function is used to Turn on/off the backlight.
*
* @param    hlcd  This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @param    state This parameter defines the state of the backlight (1 on, 0 off).
* @return   void
*/ 
void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state )
{
  HAL_GPIO_WritePin(hlcd->BklPort, hlcd->BklPin, state); /* Write the state to the corresponding GPIO */
}

/**
* @brief    This Function is used to Turn on/off the backlight.
*
* @param    hlcd      This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @param    contrast  Level of contrast to be set 16 levels allowed.
* @return   void
*/ 
void HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast )
{
  HEL_LCD_Command(hlcd, (uint8_t)LCD_CONTRASTSET_ | (contrast & 0x0Fu) );
}

/**
* @brief    This Function is used to clear the lcd .
*
* @param    hlcd      This is a pointer to a structure of the type LCD_HandleTypeDef, this structure contains the spi instance to use and the gpio configuration of the lcd.
* @return   void
*/ 
void HEL_LCD_Clear(LCD_HandleTypeDef *hlcd)
{
  HEL_LCD_Command(hlcd, LCD_CLEARDISPLAY_);
  HAL_Delay(2);
}
