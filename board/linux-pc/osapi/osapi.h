/*  --------------------------------------------------------------------------
 *  osapi.h    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef OSAPI_H_
#define OSAPI_H_

#include <pthread.h>
#include <semaphore.h>

typedef enum {
    osIdlePriority = 0,
    osLowPriority,
    osBelowNormalPriority,
    osNormalPriority,
    osAboveNormalPriority,
    osHighPriority
} pthread_prio_t;

typedef void * pthread_tsk_ret_t;

typedef struct {
    pthread_t handle;
} pthread_tsk_t;

int pthread_tsk_cr( pthread_tsk_t *self, 
            		pthread_tsk_ret_t (*f)(void *),
					void *param,
					const char * const name,
                	pthread_prio_t priority,
                	uint32_t stksize );

typedef struct {
	pthread_mutex_t handle;
} pthread_lock_t;

typedef struct {
    sem_t handle;
} unix_sem_t;

typedef struct {
	void **buffer;
    int capacity;
	int size;
	int in;
	int out;
	pthread_mutex_t mutex;
	pthread_cond_t cond_full;
	pthread_cond_t cond_empty;
} unix_queue_t;

int unix_queue_cr( unix_queue_t *self, uint32_t size );
int unix_queue_send( unix_queue_t *self, void *msg );
void * unix_queue_recv( unix_queue_t *self );

int unix_sem_cr( unix_sem_t *self, uint32_t max_count );
int unix_sem_wait( unix_sem_t *self , uint32_t timeout );
int unix_sem_post( unix_sem_t *self );

int pthread_lock_cr( pthread_lock_t *self );
int pthread_lock_take( pthread_lock_t *self );
int pthread_lock_give( pthread_lock_t *self );

void pthread_kernel_run( void );

#endif /* OSAPI_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */