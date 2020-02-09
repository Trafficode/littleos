/*  --------------------------------------------------------------------------
 *  led.c   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */

#include <stdint.h>
#include <stdlib.h>

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "Legacy/stm32_hal_legacy.h"

#include "led.h"

/**
  * @brief  Configures LED on GPIO.
  * @param  Led: LED to be configured.
  *      	This parameter can be one of the following values:
  * @arg  	LED1
  * @retval None
  */
void led_init(void) {
	GPIO_InitTypeDef  gpio_init_structure;
	GPIO_TypeDef*     gpio_led;

	gpio_led = LED1_GPIO_PORT;
	/* Enable the GPIO_LED clock */
	LED1_GPIO_CLK_ENABLE();

	/* Configure the GPIO_LED pin */
	gpio_init_structure.Pin = LED1_PIN;
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(gpio_led, &gpio_init_structure);

	/* By default, turn off LED */
	HAL_GPIO_WritePin(gpio_led, LED1_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on
  *    		This parameter can be one of the following values:
  * @arg  	LED1
  * @retval None
  */
void led_on(void) {
 	GPIO_TypeDef* gpio_led;
	gpio_led = LED1_GPIO_PORT;
	HAL_GPIO_WritePin(gpio_led, LED1_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: LED to be set off
  *     	This parameter can be one of the following values:
  * @arg  	LED1
  * @retval None
  */
void led_off(void) {
	GPIO_TypeDef* gpio_led;
	gpio_led = LED1_GPIO_PORT;
	HAL_GPIO_WritePin(gpio_led, LED1_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: LED to be toggled
  *     	This parameter can be one of the following values:
  * @arg  	LED1
  * @retval None
  */
void led_toggle(void) {
	GPIO_TypeDef* gpio_led;
	gpio_led = LED1_GPIO_PORT;
	HAL_GPIO_TogglePin(gpio_led, LED1_PIN);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
