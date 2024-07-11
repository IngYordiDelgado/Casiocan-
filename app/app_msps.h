
#ifndef APP_MSPS_H_
#define APP_MSPS_H_
    #include "app_bsp.h"
    #include "hel_lcd.h"
    extern void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan);
    extern void HAL_MspInit( void );
    extern void HAL_TIM_Base_MspInit( TIM_HandleTypeDef *htim );
    extern void HAL_RTC_MspInit( RTC_HandleTypeDef* hrtc ) ;
    extern void HAL_SPI_MspInit( SPI_HandleTypeDef *hspi );
    extern void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd );
    extern void HAL_WWDG_MspInit(WWDG_HandleTypeDef* hwwdg);
#endif