/*
 * stm32f7xx_it.c
 *
 *  Created on: 14.09.2017
 *      Author: kamil
 */
#include "stm32f7xx_it.h"
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "uart_debug.h"

extern void xPortSysTickHandler(void);

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void) {
  ;
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
  DEBUG_PUTS("\nErrHardFault\n");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1) {
    ;
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void) {
  DEBUG_PUTS("\nErrMemManage\n");
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1) {
    ;
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void) {
  DEBUG_PUTS("\nErrBusFault\n");
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1) {
    ;
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
  DEBUG_PUTS("\nUsageFault\n");
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1) {
    ;
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void) {
  ;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
	HAL_IncTick();
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    xPortSysTickHandler();
  }
}

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA
  *         used for USART data transmission
  */
void USARTx_DMA_RX_IRQHandler(void) {
  ;
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA
  *         used for USART data reception
  */
void USARTx_DMA_TX_IRQHandler(void) {
  ;
}


/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA
  *         used for USART data transmission
  */
void USARTx_IRQHandler(void) {
  ;
}

/*****************************************************************************
 * 								END OF FILE
 *****************************************************************************/
