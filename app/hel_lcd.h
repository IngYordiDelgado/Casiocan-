#ifndef HEL_LCD_H__
#define HEL_LCD_H__

  #include "app_bsp.h"

  // COMMANDS

  // Write "20H" to DDRAM. and set DDRAM address to "00H" from AC

  #define LCD_CLEARDISPLAY_          0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */

  // SET DDRAM address to "00H" from AC and return cursor to its original position if shifted. The contents of DDRAM are not changed.
  #define LCD_RETURNHOME_            0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SETS cursor move direction and specifies display shift. These operations are performed during data write and read.
  #define LCD_ENTRYMODESET_          0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // flags for display entry mode
  #define LCD_ENTRYRIGHT_            0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_ENTRYLEFT_             0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_ENTRYSHIFTINCREMENT_   0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_ENTRYSHIFTDECREMENT_   0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SETS: entire display on/off, cursor on/off, cursor position on/off
  #define LCD_DISPLAYCONTROLSET_     0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // flagsfor display control set
  #define LCD_DISPLAYON_             0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_DISPLAYOFF_            0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CURSORON_              0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CURSOROFF_             0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_BLINKON_               0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_BLINKOFF_              0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SETS: interface data is 8/4 bits, number of line is 2/1, double height font, instruction table select.
  #define LCD_FUNCTIONSET_           0x20 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      

  // flags for function set
  #define LCD_8BITMODE_              0x10 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_4BITMODE_              0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_2LINE_                 0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_1LINE_                 0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_5x10DOTS_              0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_5x8DOTS_               0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_INSTRUCTIONTABLE_1_    0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_INSTRUCTIONTABLE_0_    0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SET DDRAM address in address counter
  #define LCD_SETDDRAM_ADDR_         0x80 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */     

  //=== INTRUCTION TABLE 0 (IS=0)
  // SET cursor moving and display shift control bit, and the direction, without changing DDRAM data.
  #define LCD_CURSORSHIFTSET_        0x10 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */       

  // flags for display/cursor shift
  #define LCD_DISPLAYMOVE_           0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CURSORMOVE_            0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_MOVERIGHT_             0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_MOVELEFT_              0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // SET CGRAM address in address counter
  #define LCD_SETCGRAMADDR_          0x40 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */       

  //=== INSTRUCTION TABLE 1 (IS=1)

  // SET Bias selection / Internal OSC frequency adjust
  #define LCD_BIASINTOSCSET_         0x10 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      

  // flags for Bias selection

  // bias will be 1/4
  #define LCD_BIAS_1_4_              0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */  
  // bias will be 1/5    
  #define LCD_BIAS_1_5_              0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      
  // flagsfor Adjust Internal OSC
  // 122Hz@3.0V
  #define LCD_OSC_122HZ_             0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // 131Hz@3.0V     
  #define LCD_OSC_131HZ_             0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // 144Hz@3.0V      
  #define LCD_OSC_144HZ_             0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */  
  // 161Hz@3.0V    
  #define LCD_OSC_161HZ_             0x03 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // 183Hz@3.0V      
  #define LCD_OSC_183HZ_             0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */   
  // 221Hz@3.0V   
  #define LCD_OSC_221HZ_             0x05 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */  
  // 274Hz@3.0V    
  #define LCD_OSC_274HZ_             0x06 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // 347Hz@3.0V     
  #define LCD_OSC_347HZ_             0x07 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SET ICON address in address counter.      
  #define LCD_SETICONADDR_           0x40 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */       
  #define LCD_POWER_CONTROL_         0x56 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SET Power / ICON control / Contrast set(high byte)
  #define LCD_POWICONCONTRASTHSET_   0x50 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      

  // flagsPower / ICON control / Contrast set(high byte)

   // ICON display on
  #define LCD_ICON_ON_               0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // ICON display off    
  #define LCD_ICON_OFF_              0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // booster circuit is turn on     
  #define LCD_BOOST_ON_              0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // booster circuit is turn off     
  #define LCD_BOOST_OFF_             0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      
  #define LCD_CONTRAST_C5_ON_        0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CONTRAST_C4_ON_        0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 

  // SET Follower control
  #define LCD_FOLLOWERCONTROLSET_    0x60 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      

  // flagsFollower control

  // internal follower circuit is turn on
  #define LCD_FOLLOWER_ON_           0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // internal follower circuit is turn off      
  #define LCD_FOLLOWER_OFF_          0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */  
   // 1+(Rb/Ra)=1.00    
  #define LCD_RAB_1_00_              0x00 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */     
  // 1+(Rb/Ra)=1.25
  #define LCD_RAB_1_25_              0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // 1+(Rb/Ra)=1.50     
  #define LCD_RAB_1_50_              0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */      
  // 1+(Rb/Ra)=1.80
  #define LCD_RAB_1_80_              0x03 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */   
  // 1+(Rb/Ra)=2.00   
  #define LCD_RAB_2_00_              0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */     
  // 1+(Rb/Ra)=2.50  
  #define LCD_RAB_2_50_              0x05 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  // 1+(Rb/Ra)=3.00     
  #define LCD_RAB_3_00_              0x06 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */   
  // 1+(Rb/Ra)=3.75   
  #define LCD_RAB_3_75_              0x07 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */   
  // Contrast set(low byte)    
  #define LCD_CONTRASTSET_           0x70 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */     

  // flagscontrast set (low byte)
  #define LCD_CONTRAST_C3_ON_        0x08 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CONTRAST_C2_ON_        0x04 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CONTRAST_C1_ON_        0x02 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 
  #define LCD_CONTRAST_C0_ON_        0x01 /* cppcheck-suppress misra-c2012-2.5 ; we allow unused definitions */ 


  typedef struct
  {
      SPI_HandleTypeDef   *SpiHandler;  /* SPI handler address of the spi to use with the LCD*/
      GPIO_TypeDef        *RstPort;     /*port where the pin to control the LCD reset pin is*/
      uint32_t            RstPin;       /*pin to control the LCD reset pin */
      GPIO_TypeDef        *RsPort;      /*port where the pin to control the LCD RS pin*/
      uint32_t            RsPin;        /*pin to control the LCD RS pin */
      GPIO_TypeDef        *CsPort;      /*port where the pin to control the LCD chip select is*/
      uint32_t            CsPin;        /*pin to control the LCD chip select pin */
      GPIO_TypeDef        *BklPort;     /*port where the pin to control the LCD backlight is*/
      uint32_t            BklPin;       /*pin to control the LCD backlight pin */
  } LCD_HandleTypeDef;

  extern LCD_HandleTypeDef LCDHandler;

  extern void HEL_LCD_Init( LCD_HandleTypeDef *hlcd );
  extern void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd );
  extern void HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );
  extern void HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );
  extern void HEL_LCD_String( LCD_HandleTypeDef *hlcd, const char *str );
  extern void HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );
  extern void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state );
  extern void HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast );
  extern void HEL_LCD_Clear(LCD_HandleTypeDef *hlcd);
  

#endif