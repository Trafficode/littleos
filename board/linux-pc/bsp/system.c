/*  --------------------------------------------------------------------------
 *  system.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include <sys/time.h>
#include "system.h"

static uint32_t ms_start=0;
static pthread_mutex_t putd_lock;

#define DEBUG_BUFFER_SIZE   256
static char debug_buffer[DEBUG_BUFFER_SIZE];
static int std_putd_inited = 0;

static int debug_level = UNIX_BOARD_DEBUG_LVL;

void unix_board_debug( const char *fmt, ... ) {
    uint32_t n = 0;
	va_list args;
	va_start(args, fmt);
    
    if(!std_putd_inited) {
        goto exit;
    }

    n = vsnprintf( debug_buffer, DEBUG_BUFFER_SIZE, fmt, args);

    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    unix_system_print_putd( (uint8_t *)debug_buffer, n );

exit:
    va_end(args);
}

void unix_board_info( const char *fmt, ... ) {
	uint32_t n = 0;
	va_list args;

	if( debug_level < UNIX_BOARD_DEBUG_LVL_INFO ) {
		return;
	}

	va_start(args, fmt);

	/* Lock board debug */
	n = sprintf( debug_buffer, "unix > %8s - ", "info");
    n += vsnprintf( debug_buffer+n, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    unix_system_print_putd( (uint8_t *)debug_buffer, n );
    
    va_end(args);
}

void unix_board_error( const char *fmt, ... ) {
	uint32_t n = 0;
	va_list args;

	va_start(args, fmt);

	n = sprintf( debug_buffer, "unix > %8s - ", "error");
    n += vsnprintf( debug_buffer+n, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    unix_system_print_putd( (uint8_t *)debug_buffer, n );

	va_end(args);
}

void unix_board_critical( const char *fmt, ... ) {
    uint32_t n = 0;
	va_list args;

	if( debug_level < UNIX_BOARD_DEBUG_LVL_CRITILAC ) {
		return;
	}

	va_start(args, fmt);

	n = sprintf( debug_buffer, "unix > %8s - ", "critical");
    n += vsnprintf( debug_buffer+n, DEBUG_BUFFER_SIZE, fmt, args);
	
    if( DEBUG_BUFFER_SIZE < n ) {
        debug_buffer[DEBUG_BUFFER_SIZE-2] = '$';
        debug_buffer[DEBUG_BUFFER_SIZE-1] = '\n';
        n = DEBUG_BUFFER_SIZE;
    }
    unix_system_print_putd( (uint8_t *)debug_buffer, n );

    va_end(args);
}

uint16_t unix_cpu_usage( void ) {
	// cat /proc/loadavg
	// open file and scanf
    return(5);
}

void *unix_system_malloc( size_t size ) {
    return malloc( size );
}

void unix_system_init( void ) {
    struct timeval te; 

    pthread_mutex_init( &putd_lock, NULL );
    pthread_mutex_unlock( &putd_lock );
    /* We can't debug of debug if failed */

    std_putd_inited = 1;

    gettimeofday(&te, NULL);
    ms_start = (uint32_t)(te.tv_sec*1000LL + te.tv_usec/1000);
}

int unix_system_print_init( void ) {
    /* nothing to init */;
    return(0);
}

int unix_system_print_putd( uint8_t *data, uint16_t size ) {
    uint16_t i=0;

    pthread_mutex_lock( &putd_lock );
    for( i=0; i<size; i++ ) {
        putc( data[i], stdout );
    }
    pthread_mutex_unlock( &putd_lock );
    return(0);
}

int unix_system_getc_init( void ) {
    /* nothing to init */
    return(0);
}

int unix_system_std_getc( uint8_t *byte, uint32_t timeout ) {
    /* That will return only when enter pressed, but it isn't a problem */
    *byte = (uint8_t )getc(stdin);
    return(0);
}

uint32_t unix_system_get_mstick( void ) {
    struct timeval te; 
    gettimeofday(&te, NULL);
    return (uint32_t)(te.tv_sec*1000LL + te.tv_usec/1000) - ms_start;
}

void unix_system_sleep_ms( uint32_t ms ) {
    struct timespec tsleep;
    tsleep.tv_sec = ms/1000;
    tsleep.tv_nsec = (ms%1000)*1000000;
    nanosleep(&tsleep, NULL);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
