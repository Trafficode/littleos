/*  --------------------------------------------------------------------------
 *  led.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */

#ifndef LED_H_
#define LED_H_

#define LED1_GPIO_PORT                   GPIOI
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOI_CLK_DISABLE()
#define LED1_PIN                         GPIO_PIN_1

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

#endif /* LED_H_ */

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */

