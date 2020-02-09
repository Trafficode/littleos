/*  --------------------------------------------------------------------------
 *  logger.h    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "port.h"
#include "sysdefs.h"

typedef enum logger_level {
	loggerLevelDebug,
	loggerLevelInfo,
	loggerLevelWarn,
	loggerLevelCritical,
	loggerLevelError
} logger_level_t;

typedef struct {
	/* Initialize object in this structure */
	struct {
		const char *name;
		int (*putd)(uint8_t *, uint16_t);
		logger_level_t level;
		uint16_t buff_size;
	} init;
	
	/* Private variables */
	os_lock_t _buffer_lock;
	uint8_t *_buffer;
} logger_t;

void logger_init(logger_t *self, logger_t *parent);

void logger_error(logger_t *self, const char *fmt, ...);
void logger_critical(logger_t *self, const char *fmt, ...);
void logger_warn(logger_t *self, const char *fmt, ...);
void logger_info(logger_t *self, const char *fmt, ...);
void logger_debug(logger_t *self, const char *fmt, ...);

#endif /* LOGGER_H_ */

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
