/*  --------------------------------------------------------------------------
 *  system.h    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int unix_system_print_init( void );

int unix_system_print_putd( uint8_t *data, uint16_t size );

int unix_system_getc_init( void );

int unix_system_std_getc( uint8_t *byte, uint32_t timeout );

uint32_t unix_system_get_mstick( void );

void unix_system_sleep_ms( uint32_t ms );

void *unix_system_malloc( size_t size );

void unix_system_init( void );

uint16_t unix_cpu_usage( void );

/*  --------------------------------------------------------------------------
 *  Board debug option */

#define UNIX_BOARD_DEBUG_LVL_INFO       2
#define UNIX_BOARD_DEBUG_LVL_CRITILAC   1
#define UNIX_BOARD_DEBUG_LVL_ERROR      0

#define UNIX_BOARD_DEBUG_LVL            UNIX_BOARD_DEBUG_LVL_INFO

void unix_board_critical( const char *fmt, ... );
void unix_board_error( const char *fmt, ... );
void unix_board_info( const char *fmt, ... );

void unix_board_debug( const char *fmt, ... );

#endif /* SYSTEM_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
