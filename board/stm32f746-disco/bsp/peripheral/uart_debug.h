/*  --------------------------------------------------------------------------
 *  uart_debug.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */

#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include <stdbool.h>
#include "cmsis_os.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#define UART_DEBUG                       	USART1
#define UART_DEBUG_IRQn                  	USART1_IRQn
#define UART_DEBUG_IRQHandler             	USART1_IRQHandler
#define UART_DEBUG_CLK_ENABLE()          	__HAL_RCC_USART1_CLK_ENABLE();

#define UART_DEBUG_DMA_TX_CHANNEL        	DMA_CHANNEL_4
#define UART_DEBUG_DMA_TX_STREAM         	DMA2_Stream7
#define UART_DEBUG_DMA_TX_IRQn          	DMA2_Stream7_IRQn
#define UART_DEBUG_DMA_TX_IRQHandler    	DMA2_Stream7_IRQHandler
#define UART_DEBUG_DMA_CLK_ENABLE()        	__HAL_RCC_DMA2_CLK_ENABLE()

#define UART_DEBUG_TX_PIN                 	GPIO_PIN_9
#define UART_DEBUG_TX_GPIO_PORT          	GPIOA
#define UART_DEBUG_TX_AF                 	GPIO_AF7_USART1
#define UART_DEBUG_TX_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOA_CLK_ENABLE()

#define UART_DEBUG_RX_PIN                	GPIO_PIN_7
#define UART_DEBUG_RX_GPIO_PORT           	GPIOB
#define UART_DEBUG_RX_AF               		GPIO_AF7_USART1
#define UART_DEBUG_RX_GPIO_CLK_ENABLE()  	__HAL_RCC_GPIOB_CLK_ENABLE()

#define UART_DEBUG_BAUDRATE                 (115200)

int uart_debug_init(void);
int uart_debug_send(uint8_t *data, uint16_t size);

int uart_debug_getc(uint8_t *byte, uint32_t timeout);
int uart_debug_getc_init(void);

/*  --------------------------------------------------------------------------
 *  Board debug option */
#define UART_BOARD_DEBUG_LVL_INFO       2
#define UART_BOARD_DEBUG_LVL_CRITILAC   1
#define UART_BOARD_DEBUG_LVL_ERROR      0

#define UART_BOARD_DEBUG_LVL            UART_BOARD_DEBUG_LVL_CRITILAC

void uart_board_critical( const char *fmt, ... );
void uart_board_error( const char *fmt, ... );
void uart_board_info( const char *fmt, ... );

/* Used by lvgl to debug */
void uart_board_debug( const char *fmt, ... );

/* Can be use from interrupt or other critical like stack overflow hook */
#define DEBUG_PUTS(s)      {char *str = s;    \
                            while( *str ) {   \
                                UART_DEBUG->TDR=*str; \
                                while(!(UART_DEBUG->ISR & USART_ISR_TXE));  \
                                str++;    \
                            }}
                            
#define DEBUG_PUTC(c)       UART_DEBUG->TDR=c; \
                            while(!(UART_DEBUG->ISR & USART_ISR_TXE));
#endif /* UART_DEBUG_H_ */

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */