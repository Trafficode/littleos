/*  --------------------------------------------------------------------------
 *  osapi.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <pthread.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include "port.h"
#include "system.h"
#include "osapi.h"

int pthread_tsk_cr( pthread_tsk_t *self, 
          		    pthread_tsk_ret_t (*f)(void *),
                    void *param,
				    const char * const name,
                    pthread_prio_t priority,
                    uint32_t stksize ) 
{
    priority = (pthread_prio_t)priority;
    stksize = (uint32_t)stksize;

    int rc = pthread_create(&self->handle, NULL, f, param);
    if ( rc != 0 ) {
        unix_board_error("[pthread_tsk_cr] pthread_create RC:%d\n", rc);
    }
    return( rc );
}

int unix_queue_cr( unix_queue_t *self, uint32_t len ) {
    self->buffer = malloc(len*sizeof(void *));
    self->capacity = len;
    self->size = 0;
    self->in = 0;
    self->out = 0;
    pthread_mutex_init( &self->mutex, NULL );
    pthread_cond_init( &self->cond_full, NULL );
    pthread_cond_init( &self->cond_empty, NULL );
    return(0);
}

int unix_queue_send( unix_queue_t *self, void *msg ) {
    pthread_mutex_lock(&(self->mutex));
	while (self->size == self->capacity) {
        pthread_cond_wait(&(self->cond_full), &(self->mutex));
    }
	self->buffer[self->in] = msg;
	++ self->size;
	++ self->in;
	self->in %= self->capacity;
	pthread_mutex_unlock(&(self->mutex));
	pthread_cond_broadcast(&(self->cond_empty));
    return(0);
}

void * unix_queue_recv( unix_queue_t *self ) {
    void *value;
    pthread_mutex_lock(&(self->mutex));
	while ( 0 == self->size )  {
        pthread_cond_wait(&(self->cond_empty), &(self->mutex));
    }
	value = self->buffer[self->out];
	-- self->size;
	++ self->out;
	self->out %= self->capacity;
	pthread_mutex_unlock(&(self->mutex));
	pthread_cond_broadcast(&(self->cond_full));
	return value;
}

int unix_sem_cr( unix_sem_t *self, uint32_t max_count ) {
    int rc = 0;
    if(-1 == sem_init(&self->handle, 0, max_count)) {
        rc = -1;
    } else {
        sem_trywait(&self->handle);
    }
    return( rc );
}

int unix_sem_wait( unix_sem_t *self, uint32_t timeout ) {
    int rc = 0;
    if ( OS_WAIT_FOREVER == timeout ) {
        rc = sem_wait(&self->handle);
    } else {
        do {
            if( 0 == (rc = sem_trywait(&self->handle))) {
                break;
            }
            timeout--;
            unix_system_sleep_ms(1);
        } while( timeout );
    }
    return( rc );
}

int unix_sem_post( unix_sem_t *self ) {
    sem_post(&self->handle);
    return( 0 );
}

int pthread_lock_cr( pthread_lock_t *self ) {
    int rc = pthread_mutex_init( &self->handle, NULL );
    if( 0 != rc ) {
        unix_board_error("[pthread_lock_cr] pthread_mutex_init RC:%d\n", rc );
    }
    return(rc);
}

int pthread_lock_take( pthread_lock_t *self ) {
    pthread_mutex_lock( &self->handle );
    return(0);
}

int pthread_lock_give( pthread_lock_t *self ) {
    pthread_mutex_unlock( &self->handle );
    return(0);
}

void pthread_kernel_run( void ) {
    for( ;; ) {
        unix_system_sleep_ms( 1000 );
    }
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */