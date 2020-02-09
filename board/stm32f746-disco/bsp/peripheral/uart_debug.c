/*  --------------------------------------------------------------------------
 *  uart_debug.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sysdefs.h"
#include "uart_debug.h"

UART_HandleTypeDef debugUart_Handle;
DMA_HandleTypeDef debugUart_txDmaHandle;

static SemaphoreHandle_t txCompleteSem = NULL;
static SemaphoreHandle_t txLock = NULL;

static QueueHandle_t rxQueue = NULL;
static uint8_t rxChar = 0;

#define DEBUG_BUFFER_SIZE   256

static char debug_buffer[DEBUG_BUFFER_SIZE];
static SemaphoreHandle_t debugLock = NULL;

static int debug_level = UART_BOARD_DEBUG_LVL;

void UART_DEBUG_DMA_TX_IRQHandler(void) {
	HAL_DMA_IRQHandler(debugUart_Handle.hdmatx);
}

void UART_DEBUG_IRQHandler(void) {
	HAL_UART_IRQHandler(&debugUart_Handle);
}

int uart_debug_init(void) {

    debugUart_Handle.Instance = UART_DEBUG;
	debugUart_Handle.Init.BaudRate = UART_DEBUG_BAUDRATE;
	debugUart_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	debugUart_Handle.Init.StopBits = UART_STOPBITS_1;
	debugUart_Handle.Init.Parity = UART_PARITY_NONE;
	debugUart_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	debugUart_Handle.Init.Mode = UART_MODE_TX_RX;
	debugUart_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&debugUart_Handle);
	
	DEBUG_PUTC('\n');

	txLock = xSemaphoreCreateMutex();
	if( NULL == txLock ) { for( ;; ); }

	debugLock = xSemaphoreCreateMutex();
	if( NULL == debugLock ) { for( ;; ); }

    txCompleteSem = xSemaphoreCreateBinary(); 
	if( NULL == txCompleteSem ) { for( ;; ); }

    xSemaphoreTake( txCompleteSem, 0 );
	return(0);
}

void uart_board_info( const char *fmt, ... ) {
	uint32_t n = 0;
	va_list args;

	if( debug_level < UART_BOARD_DEBUG_LVL_INFO ) {
		return;
	}

	va_start(args, fmt);

	/* Lock board debug */
	xSemaphoreTake( debugLock, osWaitForever );
	n = sprintf( debug_buffer, "stm32f746-disco > %8s - ", "info");
    n += vsnprintf( debug_buffer+n, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    uart_debug_send( (uint8_t *)debug_buffer, n );

	/* Unlock board debug */
	xSemaphoreGive( debugLock );
    va_end(args);
}

void uart_board_error( const char *fmt, ... ) {
	uint32_t n = 0;
	va_list args;

	va_start(args, fmt);

	/* Lock board debug */
	xSemaphoreTake( debugLock, osWaitForever );

	n = sprintf( debug_buffer, "stm32f746-disco > %8s - ", "error");
    n += vsnprintf( debug_buffer+n, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    uart_debug_send( (uint8_t *)debug_buffer, n );

	/* Unlock board debug */
	xSemaphoreGive( debugLock );
	va_end(args);
}

void uart_board_critical( const char *fmt, ... ) {
    uint32_t n = 0;
	va_list args;

	if( debug_level < UART_BOARD_DEBUG_LVL_CRITILAC ) {
		return;
	}

	va_start(args, fmt);

	/* Lock board debug */
	xSemaphoreTake( debugLock, osWaitForever );

	n = sprintf( debug_buffer, "stm32f746-disco > %8s - ", "critical");
    n += vsnprintf( debug_buffer+n, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    uart_debug_send( (uint8_t *)debug_buffer, n );

	/* Unlock board debug */
	xSemaphoreGive( debugLock );

    va_end(args);
}

void uart_board_debug( const char *fmt, ... ) {
    uint32_t n = 0;
	va_list args;
	va_start(args, fmt);

	/* Lock board debug */
	xSemaphoreTake( debugLock, osWaitForever );

    n = vsnprintf( debug_buffer, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    uart_debug_send( (uint8_t *)debug_buffer, n );

	/* Unlock board debug */
	xSemaphoreGive( debugLock );

    va_end(args);
}

int uart_debug_getc_init(void) {
    rxQueue = xQueueCreate( 128, sizeof(uint8_t) );
    if( rxQueue == NULL ) {
        /* Queue was not created and must not be used. */
		sys_assert_fail(errCreate);
        for( ;; );
    }

    HAL_UART_Receive_IT(&debugUart_Handle, &rxChar, 1);
    return(0);
}

int uart_debug_getc(uint8_t *byte, uint32_t timeout) {
    /* Wait given timeout to new data in Queue */
    if( pdTRUE != xQueueReceive(rxQueue, byte, timeout) ) {
        /* Exit with error code when no input data */
        return(-1);
    }
    return(0);
}

int uart_debug_send(uint8_t *data, uint16_t size) {
	/* Lock stdout */
	xSemaphoreTake( txLock, osWaitForever );

    /* Send next data using DMA */
    HAL_UART_Transmit_DMA(&debugUart_Handle, data, size);

	/* Wait till data send complete */
    xSemaphoreTake(txCompleteSem, osWaitForever);

	/* Unlock stdout */
	xSemaphoreGive( txLock );

    return(0);
}

void __uart_debug_msp_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable GPIO TX/RX clock */
	UART_DEBUG_TX_GPIO_CLK_ENABLE();
	UART_DEBUG_RX_GPIO_CLK_ENABLE();

	/* Enable USARTx clock */
	UART_DEBUG_CLK_ENABLE();

	/* Enable DMA1 clock */
	UART_DEBUG_DMA_CLK_ENABLE();

	GPIO_InitStruct.Pin = UART_DEBUG_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = UART_DEBUG_TX_AF;

	HAL_GPIO_Init(UART_DEBUG_TX_GPIO_PORT, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = UART_DEBUG_RX_PIN;
	GPIO_InitStruct.Alternate = UART_DEBUG_RX_AF;

	HAL_GPIO_Init(UART_DEBUG_RX_GPIO_PORT, &GPIO_InitStruct);

	/* Configure the DMA handler for Transmission process */
	debugUart_txDmaHandle.Instance = UART_DEBUG_DMA_TX_STREAM;

	debugUart_txDmaHandle.Init.Channel = UART_DEBUG_DMA_TX_CHANNEL;
	debugUart_txDmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
	debugUart_txDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	debugUart_txDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
	debugUart_txDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	debugUart_txDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	debugUart_txDmaHandle.Init.Mode = DMA_NORMAL;
	debugUart_txDmaHandle.Init.Priority = DMA_PRIORITY_LOW;
	debugUart_txDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	debugUart_txDmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	debugUart_txDmaHandle.Init.MemBurst = DMA_MBURST_INC4;
	debugUart_txDmaHandle.Init.PeriphBurst = DMA_PBURST_INC4;
	HAL_DMA_Init(&debugUart_txDmaHandle);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(&debugUart_Handle, hdmatx, debugUart_txDmaHandle);

	/* NVIC configuration for DMA transfer complete interrupt (USARTx_TX) */
	HAL_NVIC_SetPriority(UART_DEBUG_DMA_TX_IRQn, 10, 15);
	HAL_NVIC_EnableIRQ(UART_DEBUG_DMA_TX_IRQn);

	/* NVIC configuration for USART TC interrupt */
	HAL_NVIC_SetPriority(UART_DEBUG_IRQn, 10, 15);
	HAL_NVIC_EnableIRQ(UART_DEBUG_IRQn);
}

void __uart_debug_tx_complete(void) {
    /* Send signal to waiting tasks that DMA is ready to next data */
    xSemaphoreGiveFromISR(txCompleteSem, pdFALSE);
}

void __uart_debug_rx_complete(void) {
    /* Put received byte to Queue */
    xQueueSendFromISR(rxQueue, &rxChar, pdFALSE);

    /* Set pointer to buffer on next char */
    HAL_UART_Receive_IT(&debugUart_Handle, &rxChar, 1);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */