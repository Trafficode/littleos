/*  --------------------------------------------------------------------------
 *  i2c_common.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef I2C_COMMON_H_
#define I2C_COMMON_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

/* Exported constant IO ---------------------------------------------------- */

#define LCD_I2C_ADDRESS                  ((uint16_t)0x70)
#define CAMERA_I2C_ADDRESS               ((uint16_t)0x60)
#define AUDIO_I2C_ADDRESS                ((uint16_t)0x34)
#define EEPROM_I2C_ADDRESS_A01           ((uint16_t)0xA0)
#define EEPROM_I2C_ADDRESS_A02           ((uint16_t)0xA6)
#define TS_I2C_ADDRESS                   ((uint16_t)0x70)

/* I2C clock speed configuration (in Hz) 
   WARNING: 
   Make sure that this define is not already declared in other files (ie. 
   stm32746g_discovery.h file). It can be used in parallel by other modules. */
#ifndef I2C_SPEED
 #define I2C_SPEED                       ((uint32_t)100000)
#endif /* I2C_SPEED */

/* User can use this section to tailor I2Cx/I2Cx instance used and associated 
   resources */
/* Definition for AUDIO and LCD I2Cx resources */
#define DISCO_I2Cx                              I2C3
#define DISCO_I2Cx_CLK_ENABLE()                __HAL_RCC_I2C3_CLK_ENABLE()
#define DISCO_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()

#define DISCO_I2Cx_FORCE_RESET()               __HAL_RCC_I2C3_FORCE_RESET()
#define DISCO_I2Cx_RELEASE_RESET()             __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCO_I2Cx_SCL_PIN                     GPIO_PIN_7
#define DISCO_I2Cx_SCL_SDA_GPIO_PORT           GPIOH
#define DISCO_I2Cx_SCL_SDA_AF                  GPIO_AF4_I2C3
#define DISCO_I2Cx_SDA_PIN                     GPIO_PIN_8

/* I2C interrupt requests */
#define DISCO_I2Cx_EV_IRQn                     I2C3_EV_IRQn
#define DISCO_I2Cx_ER_IRQn                     I2C3_ER_IRQn

/* Definition for external, camera and Arduino connector I2Cx resources */
#define DISCO_EXT_I2Cx                               I2C1
#define DISCO_EXT_I2Cx_CLK_ENABLE()                 __HAL_RCC_I2C1_CLK_ENABLE()
#define DISCO_EXT_DMAx_CLK_ENABLE()                 __HAL_RCC_DMA1_CLK_ENABLE()
#define DISCO_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISCO_EXT_I2Cx_FORCE_RESET()            __HAL_RCC_I2C1_FORCE_RESET()
#define DISCO_EXT_I2Cx_RELEASE_RESET()          __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define DISCO_EXT_I2Cx_SCL_PIN                  GPIO_PIN_8
#define DISCO_EXT_I2Cx_SCL_SDA_GPIO_PORT        GPIOB
#define DISCO_EXT_I2Cx_SCL_SDA_AF               GPIO_AF4_I2C1
#define DISCO_EXT_I2Cx_SDA_PIN                  GPIO_PIN_9

/* I2C interrupt requests */
#define DISCO_EXT_I2Cx_EV_IRQn                  I2C1_EV_IRQn
#define DISCO_EXT_I2Cx_ER_IRQn                  I2C1_ER_IRQn

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated from APB1 source clock = 50 MHz */
/* Due to the big MOFSET capacity for adapting the camera level the rising time is very large (>1us) */
/* 0x40912732 takes in account the big rising and aims a clock of 100khz */
/* this value might be adapted when next Rev Birdie board is available */
#define DISCO_I2Cx_TIMING                      ((uint32_t)0x40912732)  

HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler,
                                    uint8_t Addr,
                                    uint16_t Reg,
                                    uint16_t MemAddress,
                                    uint8_t *Buffer,
                                    uint16_t Length);

HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler,
                                     uint8_t Addr,
                                     uint16_t Reg,
                                     uint16_t MemAddress,
                                     uint8_t *Buffer,
                                     uint16_t Length);

HAL_StatusTypeDef I2Cx_IsDeviceReady( I2C_HandleTypeDef *i2c_handler,
                                      uint16_t DevAddress, 
                                      uint32_t Trials );

void I2Cx_Init( I2C_HandleTypeDef *i2c_handler );

extern I2C_HandleTypeDef hI2cCommonHandler;

#define I2C_COMMON_HANDLER      (&hI2cCommonHandler)

#endif /* I2C_COMMON_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
