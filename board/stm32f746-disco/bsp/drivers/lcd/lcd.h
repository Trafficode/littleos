/*  --------------------------------------------------------------------------
 *  lcd.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef LCD_H_
#define LCD_H_

#include "stm32f7xx_hal.h"

/* Display enable pin */
#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()

/* Backlight control pin */
#define LCD_BL_CTRL_PIN                  GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT            GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOK_CLK_DISABLE()

#define CPY_BUF_DMA_STREAM               DMA2_Stream0
#define CPY_BUF_DMA_CHANNEL              DMA_CHANNEL_0
#define CPY_BUF_DMA_STREAM_IRQ           DMA2_Stream0_IRQn
#define CPY_BUF_DMA_STREAM_IRQHANDLER    DMA2_Stream0_IRQHandler

int lcd_init( int16_t xsize, int16_t ysize );

void lcd_flush( int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t *col);

#endif /* LCD_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */