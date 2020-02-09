/*  --------------------------------------------------------------------------
 *  logger.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "sysdefs.h"
#include "port.h"

static uint32_t 
logger_place_padding(logger_t *self, const char *lvl, uint32_t lvl_size);

const char _debug[] = 		"   debug - ";
const char _info[] = 		"    info - ";
const char _warn[] = 		" warning - ";
const char _critical[] = 	"critical - ";
const char _error[] = 		"   error - ";

void logger_init(logger_t *self, logger_t *parent) {
	if( NULL == parent ) {
		if( 0 != os_lock_cr( &self->_buffer_lock ) ) {
			sys_assert_fail(errCreate);
		}
		self->_buffer = (uint8_t *)std_malloc(self->init.buff_size);
		if( NULL == self->_buffer ) {
			sys_assert_fail(errMalloc);
		}
	} else {
		self->_buffer = parent->_buffer;
		self->_buffer_lock = parent->_buffer_lock;
	}
}

static uint32_t 
logger_place_padding(logger_t *self, const char *lvl, uint32_t lvl_size) {
	uint32_t curr_len=0;
	unsigned long ms_tick = sys_get_mstick();
	/* [000.000]: */
	ms_tick %= 1000000;
	curr_len = sprintf(	(char *)self->_buffer, 
						"[%03lu.%03lu]: %12s - ",
						ms_tick/1000, ms_tick%1000, self->init.name);
	
	memcpy((char *)self->_buffer+curr_len, lvl, lvl_size);
	curr_len += lvl_size;
	return(curr_len);
}

void logger_error(logger_t *self, const char *fmt, ...) {
	uint32_t curr_len=0, n=0;
	va_list args;
	va_start(args, fmt);

	if( loggerLevelError >= self->init.level ) {
		os_lock_take( &self->_buffer_lock );

		/* Place timestamp, name and debug level */
		curr_len = logger_place_padding(self, _error, sizeof(_error));
		
		n = vsnprintf(	(char *)(self->_buffer+curr_len), 
								self->init.buff_size-curr_len,
								fmt, args);

		if( self->init.buff_size < n+curr_len ) {
			self->_buffer[self->init.buff_size-2] = '$';
			self->_buffer[self->init.buff_size-1] = '\n';
			n = self->init.buff_size;
		} else {
			n += curr_len;
		}

		self->init.putd(self->_buffer, n);

		os_lock_give( &self->_buffer_lock );
	}

	va_end(args);
}

void logger_critical(logger_t *self, const char *fmt, ...) {
	uint32_t curr_len=0, n=0;
	va_list args;
	va_start(args, fmt);

	if( loggerLevelCritical >= self->init.level ) {
		os_lock_take( &self->_buffer_lock );

		/* Place timestamp, name and debug level */
		curr_len = logger_place_padding(self, _critical, sizeof(_critical)-1);

		n = vsnprintf(	(char *)(self->_buffer+curr_len), 
								self->init.buff_size-curr_len,
								fmt, args);

		if( self->init.buff_size < n+curr_len ) {
			self->_buffer[self->init.buff_size-2] = '$';
			self->_buffer[self->init.buff_size-1] = '\n';
			n = self->init.buff_size;
		} else {
			n += curr_len;
		}

		self->init.putd(self->_buffer, n);

		os_lock_give( &self->_buffer_lock );
	}

	va_end(args);
}

void logger_warn(logger_t *self, const char *fmt, ...) {
	uint32_t curr_len=0, n=0;
	va_list args;
	va_start(args, fmt);

	if( loggerLevelWarn >= self->init.level ) {
		os_lock_take( &self->_buffer_lock );

		/* Place timestamp, name and debug level */
		curr_len = logger_place_padding(self, _critical, sizeof(_warn)-1);

		n = vsnprintf(	(char *)(self->_buffer+curr_len), 
								self->init.buff_size-curr_len,
								fmt, args);
		
		if( self->init.buff_size < n+curr_len ) {
			self->_buffer[self->init.buff_size-2] = '$';
			self->_buffer[self->init.buff_size-1] = '\n';
			n = self->init.buff_size;
		} else {
			n += curr_len;
		}

		self->init.putd(self->_buffer, n);

		os_lock_give( &self->_buffer_lock );
	}

	va_end(args);
}

void logger_info(logger_t *self, const char *fmt, ...) {
	uint32_t curr_len=0, n=0;
	va_list args;
	va_start(args, fmt);

	if( loggerLevelInfo >= self->init.level ) {
		os_lock_take( &self->_buffer_lock );

		/* Place timestamp, name and debug level */
		curr_len = logger_place_padding(self, _critical, sizeof(_info)-1);

		n = vsnprintf(	(char *)(self->_buffer+curr_len), 
								self->init.buff_size-curr_len,
								fmt, args);
		
		if( self->init.buff_size < n+curr_len ) {
			self->_buffer[self->init.buff_size-2] = '$';
			self->_buffer[self->init.buff_size-1] = '\n';
			n = self->init.buff_size;
		} else {
			n += curr_len;
		}

		self->init.putd(self->_buffer, n);

		os_lock_give( &self->_buffer_lock );
	}

	va_end(args);
}

void logger_debug(logger_t *self, const char *fmt, ...) {
	uint32_t curr_len=0, n=0;
	va_list args;
	va_start(args, fmt);

	if( loggerLevelDebug >= self->init.level ) {
		os_lock_take( &self->_buffer_lock );

		/* Place timestamp, name and debug level */
		curr_len = logger_place_padding(self, _critical, sizeof(_debug)-1);

		n = vsnprintf(	(char *)(self->_buffer+curr_len), 
								self->init.buff_size-curr_len,
								fmt, args);
		
		if( self->init.buff_size < n+curr_len ) {
			self->_buffer[self->init.buff_size-2] = '$';
			self->_buffer[self->init.buff_size-1] = '\n';
			n = self->init.buff_size;
		} else {
			n += curr_len;
		}
		
		self->init.putd(self->_buffer, n);

		os_lock_give( &self->_buffer_lock );
	}

	va_end(args);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

