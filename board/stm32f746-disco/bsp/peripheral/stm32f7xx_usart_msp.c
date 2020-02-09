/*  --------------------------------------------------------------------------
 *  uart_debug.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include "stm32f7xx_hal.h"

extern UART_HandleTypeDef debugUart_Handle;
extern void __uart_debug_msp_init(void);
extern void __uart_debug_tx_complete(void);
extern void __uart_debug_rx_complete(void);

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
	if( huart == &debugUart_Handle ) {
		__uart_debug_msp_init();
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if( huart == &debugUart_Handle ) {
		__uart_debug_tx_complete();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if( huart == &debugUart_Handle ) {
		__uart_debug_rx_complete();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if( huart == &debugUart_Handle ) {
  		;
  	}
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
