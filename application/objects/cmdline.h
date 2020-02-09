/*  --------------------------------------------------------------------------
 *  osapi.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef CMDLINE_H__
#define CMDLINE_H__

#include <stdbool.h>
#include "logger.h"

typedef struct {
	const char *name;
	void (*f)(void *self, uint8_t *param);
	const char *help;
} cmd_t;

typedef struct {
	struct {
		cmd_t 	*cmds;
		uint32_t cmds_no;

		const char *name;
		bool use_prompt;
		bool use_echo;
		
		uint16_t rcv_buff_size;
		uint16_t ans_buff_size;

		uint32_t tsk_stack_size;
		
		int (*getch)(uint8_t *ch, uint32_t tmo);
		int (*putd)(uint8_t *data, uint16_t len);
	} init;

	/* Private variables */
	uint8_t *_rcv_buffer;
	uint16_t _rcv_idx;
	uint8_t *_ans_buffer;
	os_tsk_t _tsk;
} cmdline_t;

void cmdline_init(cmdline_t *self);

void cmdline_answer(cmdline_t *self, const char *fmt, ...);

#endif /* CMDLINE_H__ */

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

