/*  --------------------------------------------------------------------------
 *  i2c_common.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>

#include "stm32f7xx_hal.h"
#include "i2c_common.h"

static void I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler);
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr);

I2C_HandleTypeDef hI2cCommonHandler = {0};

static void I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler) {
    GPIO_InitTypeDef  gpio_init_structure;

    if (i2c_handler == (I2C_HandleTypeDef*)(&hI2cCommonHandler)) {
        /* AUDIO and LCD I2C MSP init */

        /*** Configure the GPIOs ***/
        /* Enable GPIO clock */
        DISCO_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

        /* Configure I2C Tx as alternate function */
        gpio_init_structure.Pin = DISCO_I2Cx_SCL_PIN;
        gpio_init_structure.Mode = GPIO_MODE_AF_OD;
        gpio_init_structure.Pull = GPIO_NOPULL;
        gpio_init_structure.Speed = GPIO_SPEED_FAST;
        gpio_init_structure.Alternate = DISCO_I2Cx_SCL_SDA_AF;
        HAL_GPIO_Init(  DISCO_I2Cx_SCL_SDA_GPIO_PORT, 
                        &gpio_init_structure);

        /* Configure I2C Rx as alternate function */
        gpio_init_structure.Pin = DISCO_I2Cx_SDA_PIN;
        HAL_GPIO_Init(  DISCO_I2Cx_SCL_SDA_GPIO_PORT, 
                        &gpio_init_structure);

        /*** Configure the I2C peripheral ***/
        /* Enable I2C clock */
        DISCO_I2Cx_CLK_ENABLE();

        /* Force the I2C peripheral clock reset */
        DISCO_I2Cx_FORCE_RESET();

        /* Release the I2C peripheral clock reset */
        DISCO_I2Cx_RELEASE_RESET();

        /* Enable and set I2Cx Interrupt to a lower priority */
        HAL_NVIC_SetPriority(DISCO_I2Cx_EV_IRQn, 0x05, 0);
        HAL_NVIC_EnableIRQ(DISCO_I2Cx_EV_IRQn);

        /* Enable and set I2Cx Interrupt to a lower priority */
        HAL_NVIC_SetPriority(DISCO_I2Cx_ER_IRQn, 0x05, 0);
        HAL_NVIC_EnableIRQ(DISCO_I2Cx_ER_IRQn);
    } else {
        /* External, camera and Arduino connector I2C MSP init */

        /*** Configure the GPIOs ***/
        /* Enable GPIO clock */
        DISCO_EXT_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

        /* Configure I2C Tx as alternate function */
        gpio_init_structure.Pin = DISCO_EXT_I2Cx_SCL_PIN;
        gpio_init_structure.Mode = GPIO_MODE_AF_OD;
        gpio_init_structure.Pull = GPIO_NOPULL;
        gpio_init_structure.Speed = GPIO_SPEED_FAST;
        gpio_init_structure.Alternate = DISCO_EXT_I2Cx_SCL_SDA_AF;
        HAL_GPIO_Init(  DISCO_EXT_I2Cx_SCL_SDA_GPIO_PORT, 
                        &gpio_init_structure);

        /* Configure I2C Rx as alternate function */
        gpio_init_structure.Pin = DISCO_EXT_I2Cx_SDA_PIN;
        HAL_GPIO_Init(  DISCO_EXT_I2Cx_SCL_SDA_GPIO_PORT, 
                        &gpio_init_structure);

        /*** Configure the I2C peripheral ***/
        /* Enable I2C clock */
        DISCO_EXT_I2Cx_CLK_ENABLE();

        /* Force the I2C peripheral clock reset */
        DISCO_EXT_I2Cx_FORCE_RESET();

        /* Release the I2C peripheral clock reset */
        DISCO_EXT_I2Cx_RELEASE_RESET();

        /* Enable and set I2Cx Interrupt to a lower priority */
        HAL_NVIC_SetPriority(DISCO_EXT_I2Cx_EV_IRQn, 0x05, 0);
        HAL_NVIC_EnableIRQ(DISCO_EXT_I2Cx_EV_IRQn);

        /* Enable and set I2Cx Interrupt to a lower priority */
        HAL_NVIC_SetPriority(DISCO_EXT_I2Cx_ER_IRQn, 0x05, 0);
        HAL_NVIC_EnableIRQ(DISCO_EXT_I2Cx_ER_IRQn);
    }
}

/**
  * @brief  Initializes I2C HAL.
  * @param  i2c_handler : I2C handler
  * @retval None
  */
void I2Cx_Init(I2C_HandleTypeDef *i2c_handler) 
{
    if(HAL_I2C_GetState(i2c_handler) == HAL_I2C_STATE_RESET)
    {
        if (i2c_handler == (I2C_HandleTypeDef*)(&hI2cCommonHandler))
        {
            /* Audio and LCD I2C configuration */
            i2c_handler->Instance = DISCO_I2Cx;
        }
        else
        {
            /* External, camera and Arduino connector  I2C configuration */
            i2c_handler->Instance = DISCO_EXT_I2Cx;
        }
        i2c_handler->Init.Timing           = DISCO_I2Cx_TIMING;
        i2c_handler->Init.OwnAddress1      = 0;
        i2c_handler->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
        i2c_handler->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
        i2c_handler->Init.OwnAddress2      = 0;
        i2c_handler->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
        i2c_handler->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

        /* Init the I2C */
        I2Cx_MspInit(i2c_handler);
        HAL_I2C_Init(i2c_handler);
    }
}

/**
  * @brief  Reads multiple data.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C address
  * @param  Reg: Reg address 
  * @param  MemAddress: Memory address 
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler,
                                    uint8_t Addr,
                                    uint16_t Reg,
                                    uint16_t MemAddress,
                                    uint8_t *Buffer,
                                    uint16_t Length)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Read(i2c_handler, Addr, (uint16_t)Reg, MemAddress, 
                              Buffer, Length, 1000);

    /* Check the communication status */
    if(status != HAL_OK)
    {
        /* I2C error occurred */
        I2Cx_Error(i2c_handler, Addr);
    }
    return status;
}

/**
  * @brief  Writes a value in a register of the device through BUS in using DMA mode.
  * @param  i2c_handler : I2C handler
  * @param  Addr: Device address on BUS Bus.  
  * @param  Reg: The target register address to write
  * @param  MemAddress: Memory address 
  * @param  Buffer: The target register value to be written 
  * @param  Length: buffer size to be written
  * @retval HAL status
  */
HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler,
                                    uint8_t Addr,
                                    uint16_t Reg,
                                    uint16_t MemAddress,
                                    uint8_t *Buffer,
                                    uint16_t Length)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Write(i2c_handler, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, 1000);

    /* Check the communication status */
    if(status != HAL_OK)
    {
        /* Re-Initiaize the I2C Bus */
        I2Cx_Error(i2c_handler, Addr);
    }
    return status;
}

/**
  * @brief  Checks if target device is ready for communication. 
  * @note   This function is used with Memory devices
  * @param  i2c_handler : I2C handler
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, 
                                     uint16_t DevAddress, uint32_t Trials)
{ 
    return (HAL_I2C_IsDeviceReady(i2c_handler, DevAddress, Trials, 1000));
}

/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  i2c_handler : I2C handler
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr)
{
    /* De-initialize the I2C communication bus */
    HAL_I2C_DeInit(i2c_handler);

    /* Re-Initialize the I2C communication bus */
    I2Cx_Init(i2c_handler);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
