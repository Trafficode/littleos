/*  --------------------------------------------------------------------------
 *  osapi.h    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef OSAPI_H_
#define OSAPI_H_

#include <stdint.h>

#include "cmsis_os.h"

typedef enum {
    osIdlePriority = tskIDLE_PRIORITY,
    osLowPriority,
    osBelowNormalPriority,
    osNormalPriority,
    osAboveNormalPriority,
    osHighPriority
} rtos_prio_t;

typedef void rtos_tsk_ret_t;

typedef struct {
    TaskHandle_t handle;
} rtos_tsk_t;

typedef struct {
    SemaphoreHandle_t xSemaphore;
} rtos_lock_t;

typedef struct {
    SemaphoreHandle_t xSemaphore;
} rtos_sem_t;

void *rtos_malloc( size_t size );

int rtos_sem_cr( rtos_sem_t *self, uint32_t max_count );
int rtos_sem_wait( rtos_sem_t *self, uint32_t timeout );
int rtos_sem_post( rtos_sem_t *self );

int rtos_lock_cr( rtos_lock_t *self );
int rtos_lock_take( rtos_lock_t *self );
int rtos_lock_give( rtos_lock_t *self );

int rtos_tsk_del( rtos_tsk_t *self );
int rtos_tsk_cr(rtos_tsk_t *self, 
                rtos_tsk_ret_t (*f)(void *),
                void *param,
			    const char * const name,
                rtos_prio_t priority,
                uint32_t stksize );         /* stack size in bytes */

void rtos_sleep_ms( uint32_t ms );

void rtos_kernel_run( void );

uint32_t rtos_get_systick( void );

#endif /* OSAPI_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
