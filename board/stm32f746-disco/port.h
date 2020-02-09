/*  --------------------------------------------------------------------------
 *  port.h    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef PORT_H_
#define PORT_H_

#include "led.h"
#include "uart_debug.h"
#include "osapi.h"
#include "ts.h"
#include "lcd.h"
#include "stm_general.h"
#include "lv_port.h"
#include "cpu_utils.h"

/*  --------------------------------------------------------------------------
 *  Application specyfic definitions */

/*  Linux console has auto echo */
#define STD_TERMINAL_USE_ECHO       ( true )

/*  --------------------------------------------------------------------------
 *  System specyfic utilities initialize */

/* void system_init(void) */
#define system_init                 stm_general_init

/*  --------------------------------------------------------------------------
 *  Board support package */

/* void status_led_init(void) */
#define status_led_init             led_init

/* void status_led_toggle(void) */
#define status_led_toggle           led_toggle

/* void status_led_on(void) */
#define status_led_on               led_on

/* void status_led_off(void) */
#define status_led_off              led_off

/* int std_print_init(void)
 * return: 0 - success, !0 fail */
#define std_print_init              uart_debug_init

/* int std_print_putd(uint8_t *data, uint16_t size)
 * return: 0 - success, !0 fail */
#define std_print_putd              uart_debug_send

/* int std_getc_init(void)
 * return: 0 - success, !0 fail */
#define std_getc_init               uart_debug_getc_init

/* int std_getc(uint8_t *byte, uint32_t timeout)
 * return: 0 - success, -1 timeout */
#define std_getc                    uart_debug_getc

/* void *std_malloc( size_t size )
 * return: !NULL - success, NULL - failed */
#define std_malloc                  rtos_malloc

/* uint32_t sys_get_mstick( void )
 * return: milliseconds from startup */
#define sys_get_mstick              rtos_get_systick

/* void sys_sleep_ms( uint32_t ms ) */
#define sys_sleep_ms                rtos_sleep_ms

/* uint16_t sys_cpu_usage( void ) */
#define sys_cpu_usage               os_cpu_usage

/*  --------------------------------------------------------------------------
 *  Operating system port */
#define OS_WAIT_FOREVER             UINT32_MAX

/*
typedef enum {
    osIdlePriority,
    osLowPriority,
    osBelowNormalPriority,
    osNormalPriority,
    osAboveNormalPriority,
    osHighPriority
} pthread_prio_t; 
*/
#define os_prio_t                   rtos_prio_t

/*
int os_tsk_cr(  os_tsk_t *self, 
                os_tsk_ret_t (*f)(void *),
                void *param,
                const char * const name,
                os_prio_t priority,
                uint32_t stksize )
  return: 0 - success, -1 - failed 
*/
#define os_tsk_t                    rtos_tsk_t
#define os_tsk_ret_t                rtos_tsk_ret_t
#define os_tsk_cr                   rtos_tsk_cr

/* int os_lock_cr( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_lock_t                   rtos_lock_t
#define os_lock_cr                  rtos_lock_cr

/* int os_lock_take( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_lock_take                rtos_lock_take

/* int os_lock_give( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_lock_give                rtos_lock_give


/* int os_sem_cr( os_lock_t *self, uint32_t max_count )
 * return: 0 - success, !0 - failed */
#define os_sem_t                   rtos_sem_t
#define os_sem_cr                  rtos_sem_cr

/* int os_sem_wait( os_lock_t *self, uint32_t timeout )
 * return: 0 - success, !0 - failed */
#define os_sem_wait                rtos_sem_wait

/* int os_sem_give( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_sem_post                rtos_sem_post

/* void os_kernel_run( void ) */
#define os_kernel_run               rtos_kernel_run

/*  --------------------------------------------------------------------------
 *  Graphic and input devices port */

/* void lv_board_init( void ); */
#define lv_board_init               lv_port_init

/* void lv_port_gui_lock(void); */
#define lv_gui_lock                 lv_port_gui_lock

/* void lv_port_gui_unlock(void); */
#define lv_gui_unlock               lv_port_gui_unlock

#endif /* PORT_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */