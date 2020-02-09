/*  --------------------------------------------------------------------------
 *  osapi.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>
#include <stdlib.h>

#include "osapi.h"

void *rtos_malloc( size_t size ) {
	return pvPortMalloc(size);
}

uint32_t rtos_get_systick(void) {
	return (uint32_t)xTaskGetTickCount();
}

int rtos_sem_cr( rtos_sem_t *self, uint32_t max_count ) {
	self->xSemaphore = xSemaphoreCreateCounting(max_count, 0);
	if( NULL == self->xSemaphore ) {
		return(-1);
	}
	return(0);
}


int rtos_sem_wait( rtos_sem_t *self, uint32_t timeout ) {
	int rc = xSemaphoreTake( self->xSemaphore, ( TickType_t )timeout );
	return (rc == pdTRUE ? 0:-1);
}

int rtos_sem_post( rtos_sem_t *self ) {
	xSemaphoreGive( self->xSemaphore );
	return(0);
}

int rtos_lock_cr( rtos_lock_t *self ) {
	self->xSemaphore = xSemaphoreCreateMutex();
	if( NULL == self->xSemaphore ) {
		return(-1);
	}
	return(0);
}

int rtos_lock_take( rtos_lock_t *self ) {
	xSemaphoreTake( self->xSemaphore, osWaitForever );
	return(0);
}

int rtos_lock_give( rtos_lock_t *self ) {
	xSemaphoreGive( self->xSemaphore );
	return(0);
}

int rtos_tsk_cr(rtos_tsk_t *self, 
          		rtos_tsk_ret_t (*f)(void *),
				void *param,
				const char * const name,
                rtos_prio_t priority,
                uint32_t stksize )
{
	int rc = xTaskCreate(f, name, stksize/sizeof(uint32_t), param, priority, 
						 self->handle);
	return ( pdPASS == rc ) ? 0:1;
}

void rtos_sleep_ms(uint32_t ms) {
	osDelay(ms);
}

void rtos_kernel_run(void) {
	/* Start scheduler */
	osKernelStart ();

	/* We should never get here as control is now taken by the scheduler */
	for( ;; );
}
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */