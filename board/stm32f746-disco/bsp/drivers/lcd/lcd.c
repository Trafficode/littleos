/*  --------------------------------------------------------------------------
 *  lcd.c   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "osapi.h"
#include "rk043fn48h.h"
#include "sdram.h"
#include "uart_debug.h"
#include "lcd.h"

static void lcd_io_init(void);
static void lcd_io_msp_init(void);
static void lcd_io_display_on(void);
static void lcd_io_clock_config(void);
static void lcd_layer_rgb565_init(uint32_t fb_addr);

static LTDC_HandleTypeDef hLtdcHandler;

int16_t horResolution=0, verResolution=0;

static __IO uint16_t * frame_buffer = (__IO uint16_t*) (SDRAM_DEVICE_ADDR);

int lcd_init( int16_t xsize, int16_t ysize ) {
    int rc = 0;
    horResolution = xsize;
    verResolution = ysize;

    lcd_io_init();
    lcd_layer_rgb565_init((uint32_t)frame_buffer);
    lcd_io_display_on();

    return(rc);
}

void lcd_flush( int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t *col )
{
    uint32_t row_len=0, cnt=0, y_flush=0;
    uint16_t *from=NULL, *to=NULL;

    /* Return if the area is out the screen */
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > horResolution - 1) return;
    if(y1 > verResolution - 1) return;

    /* Truncate the area to the screen */
    x1 = x1 < 0 ? 0 : x1;
    y1 = y1 < 0 ? 0 : y1;
    x2 = x2 > horResolution - 1 ? horResolution - 1 : x2;
    y2 = y2 > verResolution - 1 ? verResolution - 1 : y2;

    row_len = x2 - x1 + 1;

    for( y_flush=y1; y_flush<=y2; y_flush++ ) {

        to = (uint16_t *)frame_buffer + (y_flush*horResolution + x1);
        from = col + ((y_flush - y1)*row_len);

        for( cnt=0; cnt<row_len; cnt++ ) {
            to[cnt] = from[cnt];
        }
    }
}

static void lcd_layer_rgb565_init(uint32_t fb_addr) {
    LTDC_LayerCfgTypeDef  layer_cfg;

    /* Layer Init */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = horResolution;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = verResolution;
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    layer_cfg.FBStartAdress = fb_addr;
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = horResolution;
    layer_cfg.ImageHeight = verResolution;

    HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 0);
}

static void lcd_io_clock_config(void) {
    static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

    /* RK043FN48H LCD clock configuration */
    /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
    /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
    /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/5 = 38.4 Mhz */
    /* LTDC clock frequency = PLLLCDCLK/LTDC_PLLSAI_DIVR_4 = 38.4/4 = 9.6Mhz */
    periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    periph_clk_init_struct.PLLSAI.PLLSAIN = 192;
    periph_clk_init_struct.PLLSAI.PLLSAIR = RK043FN48H_FREQUENCY_DIVIDER;
    periph_clk_init_struct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);
}

static void lcd_io_display_on(void) {
    /* Display On */
    __HAL_LTDC_ENABLE(&hLtdcHandler);
    HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);
}

static void lcd_io_msp_init(void) {
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable the LTDC and DMA2D clocks */
    __HAL_RCC_LTDC_CLK_ENABLE();
    
    /* Enable GPIOs clock */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();
    LCD_DISP_GPIO_CLK_ENABLE();
    LCD_BL_CTRL_GPIO_CLK_ENABLE();

    /*** LTDC Pins configuration ***/
    /* GPIOE configuration */
    gpio_init_structure.Pin = GPIO_PIN_4;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* GPIOG configuration */
    gpio_init_structure.Pin = GPIO_PIN_12;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    /* GPIOI LTDC alternate configuration */
    gpio_init_structure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | \
                                GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &gpio_init_structure);

    /* GPIOJ configuration */
    gpio_init_structure.Pin = \
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |     \
            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |     \
            GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |   \
            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;

    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

    /* GPIOK configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | \
            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOK, &gpio_init_structure);

    /* LCD_DISP GPIO configuration */
    /* LCD_DISP pin has to be manually controlled */
    gpio_init_structure.Pin = LCD_DISP_PIN;     
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_DISP_GPIO_PORT, &gpio_init_structure);

    /* LCD_BL_CTRL GPIO configuration */
    /* LCD_BL_CTRL pin has to be manually controlled */
    gpio_init_structure.Pin = LCD_BL_CTRL_PIN;  
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
}

static void lcd_io_init( void ) {
    /* Select the used LCD */

    /* The RK043FN48H LCD 480x272 is selected */
    /* Timing Configuration */
    hLtdcHandler.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
    hLtdcHandler.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
    hLtdcHandler.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hLtdcHandler.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hLtdcHandler.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + \
                                    RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hLtdcHandler.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + \
                                    RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hLtdcHandler.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + \
                                    RK043FN48H_VBP + RK043FN48H_VFP - 1);
    hLtdcHandler.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + \
                                    RK043FN48H_HBP + RK043FN48H_HFP - 1);

    /* LCD clock configuration */
    lcd_io_clock_config();

    /* Initialize the LCD pixel width and pixel height */
    hLtdcHandler.LayerCfg->ImageWidth  = RK043FN48H_WIDTH;
    hLtdcHandler.LayerCfg->ImageHeight = RK043FN48H_HEIGHT;

    /* Background value */
    hLtdcHandler.Init.Backcolor.Blue = 0;
    hLtdcHandler.Init.Backcolor.Green = 0;
    hLtdcHandler.Init.Backcolor.Red = 0;

    /* Polarity */
    hLtdcHandler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hLtdcHandler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hLtdcHandler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hLtdcHandler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hLtdcHandler.Instance = LTDC;

    if(HAL_LTDC_GetState(&hLtdcHandler) == HAL_LTDC_STATE_RESET) {
        /* This __weak function can be rewritten by the application */
        lcd_io_msp_init();
    }

    HAL_LTDC_Init(&hLtdcHandler);

    /* Assert display enable LCD_DISP pin */
    HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);

    /* Assert backlight LCD_BL_CTRL pin */
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

    BSP_SDRAM_Init();
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */