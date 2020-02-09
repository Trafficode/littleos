/*
 * stm32f7xx_it.h
 *
 *  Created on: 14.09.2017
 *      Author: kamil
 */

#ifndef STM32F7XX_IT_H_
#define STM32F7XX_IT_H_

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void EXTI15_10_IRQHandler(void);
void USARTx_DMA_RX_IRQHandler(void);
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);

#endif /* STM32F7XX_IT_H_ */
