/*  --------------------------------------------------------------------------
 *  port.h: linux-pc
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef PORT_H_
#define PORT_H_

#include "system.h"
#include "osapi.h"
#include "led.h"
#include "xlib_graphic.h"
#include "lv_port.h"

/*  --------------------------------------------------------------------------
 *  Application specyfic definitions */

/*  Linux console has auto echo */
#define STD_TERMINAL_USE_ECHO       ( false )

/*  --------------------------------------------------------------------------
 *  System specyfic utilities initialize */

/* void system_init(void) */
#define system_init                 unix_system_init

/*  --------------------------------------------------------------------------
 *  Board support package */

/* void status_led_init(void) */
#define status_led_init             pc_status_led_init

/* void status_led_toggle(void) */
#define status_led_toggle           pc_status_led_toggle

/* void status_led_on(void) */
#define status_led_on               pc_status_led_on

/* void status_led_off(void) */
#define status_led_off              pc_status_led_off

/* int std_print_init(void)
 * return: 0 - success, !0 fail */
#define std_print_init              unix_system_print_init

/* int std_print_putd(uint8_t *data, uint16_t size)
 * return: 0 - success, !0 fail */
#define std_print_putd              unix_system_print_putd

/* int std_getc_init(void)
 * return: 0 - success, !0 fail */
#define std_getc_init               unix_system_getc_init

/* int std_getc(uint8_t *byte, uint32_t timeout)
 * return: 0 - success, -1 timeout */
#define std_getc                    unix_system_std_getc

/* void *std_malloc( size_t size )
 * return: !NULL - success, NULL - failed */
#define std_malloc                  unix_system_malloc

/* uint32_t sys_get_mstick( void )
 * return: milliseconds from startup */
#define sys_get_mstick              unix_system_get_mstick

/* void sys_sleep_ms( uint32_t ms ) */
#define sys_sleep_ms                unix_system_sleep_ms

/* uint16_t sys_cpu_usage( void ) */
#define sys_cpu_usage               unix_cpu_usage

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
#define os_prio_t                   pthread_prio_t

/*
int os_tsk_cr(  os_tsk_t *self, 
          	    os_tst_ret_t (*f)(void *),
                void *param,
			    const char * const name,
                os_prio_t priority,
                uint32_t stksize )
  return: 0 - success, -1 - failed 
*/
#define os_tsk_t                    pthread_tsk_t
#define os_tsk_ret_t                pthread_tsk_ret_t
#define os_tsk_cr                   pthread_tsk_cr

#define os_queue_t                  unix_queue_t

/* int os_queue_cr( os_queue_t *self, uint32_t size )
 * return: 0 - success, !0 - failed */
#define os_queue_cr                 unix_queue_cr

/* void * os_queue_recv( os_queue_cr *self )
 * return: pointer to message */
#define os_queue_recv               unix_queue_recv

/* int os_queue_send( os_queue_cr *self, void *msg )
 * return: 0 - success, !0 - failed */
#define os_queue_send               unix_queue_send

/* int os_lock_cr( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_lock_t                   pthread_lock_t
#define os_lock_cr                  pthread_lock_cr

/* int os_lock_take( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_lock_take                pthread_lock_take

/* int os_lock_give( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_lock_give                pthread_lock_give


/* int os_sem_cr( os_lock_t *self, uint32_t max_count )
 * return: 0 - success, !0 - failed */
#define os_sem_t                   unix_sem_t
#define os_sem_cr                  unix_sem_cr

/* int os_sem_wait( os_lock_t *self, uint32_t timeout )
 * return: 0 - success, !0 - failed */
#define os_sem_wait                unix_sem_wait

/* int os_sem_give( os_lock_t *self )
 * return: 0 - success, !0 - failed */
#define os_sem_post                unix_sem_post


/* void os_kernel_run( void ) */
#define os_kernel_run               pthread_kernel_run

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
