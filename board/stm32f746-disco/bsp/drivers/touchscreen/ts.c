/*  --------------------------------------------------------------------------
 *  ts.c   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>

#include "ft5336.h"
#include "i2c_common.h"
#include "cmsis_os.h"
#include "uart_debug.h"

#include "ts.h"

#define TS_SWAP_NONE                    ((uint8_t) 0x01)
#define TS_SWAP_X                       ((uint8_t) 0x02)
#define TS_SWAP_Y                       ((uint8_t) 0x04)
#define TS_SWAP_XY                      ((uint8_t) 0x08)

#define TS_MAX_NB_TOUCH                 ((uint32_t) FT5336_MAX_DETECTABLE_TOUCH)

typedef enum {
    TS_OK                = 0x00, /*!< Touch Ok */
    TS_ERROR             = 0x01, /*!< Touch Error */
    TS_TIMEOUT           = 0x02, /*!< Touch Timeout */
    TS_DEVICE_NOT_FOUND  = 0x03  /*!< Touchscreen device not found */
}TS_StatusTypeDef;

typedef struct {
    /*!< Total number of active touches detected at last scan */
    uint8_t  touchDetected;
    /*!< Touch X[0], X[1] coordinates on 12 bits */
    uint16_t touchX[TS_MAX_NB_TOUCH];
     /*!< Touch Y[0], Y[1] coordinates on 12 bits */
    uint16_t touchY[TS_MAX_NB_TOUCH];     
} TS_StateTypeDef;

static uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State);

static TS_DrvTypeDef *tsDriver;
static uint16_t tsXBoundary, tsYBoundary; 
static uint8_t  tsOrientation;
static uint8_t  I2cAddress;

static TS_StateTypeDef TS_State;

int ts_init( int16_t xsize, int16_t ysize ) {
    uint8_t status = TS_OK;
    tsXBoundary = xsize;
    tsYBoundary = ysize;

    /* Read ID and verify if the touch screen driver is ready */
    ft5336_ts_drv.Init(TS_I2C_ADDRESS);
    if(ft5336_ts_drv.ReadID(TS_I2C_ADDRESS) == FT5336_ID_VALUE) { 
        /* Initialize the TS driver structure */
        tsDriver = &ft5336_ts_drv;
        I2cAddress = TS_I2C_ADDRESS;
        tsOrientation = TS_SWAP_XY;

        /* Initialize the TS driver */
        tsDriver->Start(I2cAddress);
    } else {
        uart_board_error("[ts_init] Touchscreen not faund\n");
        status = TS_DEVICE_NOT_FOUND;
    }

    return (status == TS_OK) ? 0:1;
}

int ts_read( int16_t *x, int16_t *y ) {
    int rc = 0;

    BSP_TS_GetState( &TS_State );
    if( TS_State.touchDetected ) {
        *x = TS_State.touchX[0];
        *y = TS_State.touchY[0];
        rc = 1;
    }
    return( rc );
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
static uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State) 
{
    static uint32_t _x[TS_MAX_NB_TOUCH] = {0, 0};
    static uint32_t _y[TS_MAX_NB_TOUCH] = {0, 0};
    uint8_t ts_status = TS_OK;
    uint16_t x[TS_MAX_NB_TOUCH];
    uint16_t y[TS_MAX_NB_TOUCH];
    uint16_t brute_x[TS_MAX_NB_TOUCH];
    uint16_t brute_y[TS_MAX_NB_TOUCH];
    uint16_t x_diff;
    uint16_t y_diff;
    uint32_t index;

    /* Check and update the number of touches active detected */
    TS_State->touchDetected = tsDriver->DetectTouch(I2cAddress);
  
    if(TS_State->touchDetected)
    {
        for(index=0; index < TS_State->touchDetected; index++)
        {
            /* Get each touch coordinates */
            tsDriver->GetXY(I2cAddress, &(brute_x[index]), &(brute_y[index]));

            if(tsOrientation == TS_SWAP_NONE)
            {
                x[index] = brute_x[index];
                y[index] = brute_y[index];
            }

            if(tsOrientation & TS_SWAP_X)
            {
                x[index] = 4096 - brute_x[index];   
            }

            if(tsOrientation & TS_SWAP_Y)
            {
                y[index] = 4096 - brute_y[index];
            }

            if(tsOrientation & TS_SWAP_XY)
            {
                y[index] = brute_x[index];
                x[index] = brute_y[index];
            }

            x_diff = x[index]>_x[index]? (x[index]-_x[index]):(_x[index]-x[index]);
            y_diff = y[index]>_y[index]? (y[index]-_y[index]):(_y[index]-y[index]);

            if ((x_diff + y_diff) > 5)
            {
                _x[index] = x[index];
                _y[index] = y[index];
            }

            if(I2cAddress == FT5336_I2C_SLAVE_ADDRESS)
            {
                TS_State->touchX[index] = x[index];
                TS_State->touchY[index] = y[index];
            }
            else
            {
                /* 2^12 = 4096 : indexes are expressed on a dynamic of 4096 */
                TS_State->touchX[index] = (tsXBoundary * _x[index]) >> 12;
                TS_State->touchY[index] = (tsYBoundary * _y[index]) >> 12;
            }

        } /* of for(index=0; index < TS_State->touchDetected; index++) */

    } /* end of if(TS_State->touchDetected != 0) */

    return (ts_status);
}

/**
  * @brief  Initializes Touchscreen low level.
  * @retval None
  */
void TS_IO_Init(void)
{
    I2Cx_Init(I2C_COMMON_HANDLER);
}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
    I2Cx_WriteMultiple( I2C_COMMON_HANDLER, Addr, (uint16_t)Reg, 
                        I2C_MEMADD_SIZE_8BIT, (uint8_t*)&Value, 1);
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg)
{
    uint8_t read_value = 0;

    I2Cx_ReadMultiple(I2C_COMMON_HANDLER, Addr, Reg, I2C_MEMADD_SIZE_8BIT, 
                     (uint8_t*)&read_value, 1);

    return read_value;
}

/**
  * @brief  TS delay
  * @param  Delay: Delay in ms
  * @retval None
  */
void TS_IO_Delay(uint32_t Delay)
{
    osDelay(Delay);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */