/*  --------------------------------------------------------------------------
 *  cmdline.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "cmdline.h"
#include "port.h"
#include "sysdefs.h"

typedef enum {
	execAction=0,
	newlAction,
	tlngAction,
	backAction,
	idleAction
} ll_t;

static void cmdline_putc( cmdline_t *self, uint8_t c );
static void cmdline_prompt( cmdline_t *self );
static void cmdline_parse( cmdline_t *self, uint8_t **cmd, uint8_t **arg );
static bool cmdline_execute( cmdline_t *self );
static ll_t cmdline_getline( cmdline_t *self );

static os_tsk_ret_t cmdline_task(void *params);

void cmdline_init(cmdline_t *self) {
	int rc=0;
	self->_rcv_buffer = (uint8_t *)std_malloc(self->init.rcv_buff_size);
	if( NULL == self->_rcv_buffer ) {
		sys_assert_fail(errMalloc);
	}

	self->_ans_buffer = (uint8_t *)std_malloc(self->init.ans_buff_size);
	if( NULL == self->_ans_buffer ) {
		sys_assert_fail(errMalloc);
	}

	rc = os_tsk_cr(	&self->_tsk, cmdline_task, self, self->init.name, 
					osNormalPriority, self->init.tsk_stack_size );
	if( 0 != rc ) {
		sys_assert_fail(errCreate);
	}
}

void cmdline_answer(cmdline_t *self, const char *fmt, ...) {
	uint16_t n=0;
	va_list args;
	va_start(args, fmt);

	n = vsnprintf(	(char *)(self->_ans_buffer), self->init.ans_buff_size, 
					fmt, args);

	if( self->init.ans_buff_size < n ) {
		self->_ans_buffer[self->init.ans_buff_size-2] = '$';
		self->_ans_buffer[self->init.ans_buff_size-1] = '\n';
		n = self->init.ans_buff_size;
	}

	self->init.putd(self->_ans_buffer, n);

	va_end(args);
}

void cmdline_commit(cmdline_t *self) {	
	ll_t rc = cmdline_getline( self );
	switch( rc ) {
		case execAction: {
			if( true == self->init.use_echo ) {
				cmdline_putc(self, '\n');
			}
			if( false == cmdline_execute( self ) ) {
				cmdline_answer(	self, "Command <%s> not found\n",
								self->_rcv_buffer);
			}
			cmdline_putc(self, '\n');
			cmdline_prompt(self);
			break;
		}
		case newlAction: {
			if( true == self->init.use_echo ) {
				cmdline_putc(self, '\n');
			}
			cmdline_prompt(self);
			break;
		}
		case tlngAction: {
			cmdline_answer( self, "\nLine too long, max %u chars!\n", 
						 	self->init.rcv_buff_size);
			break;
		}
		case backAction: {
			/* Backspace pressed */
			break;
		}
		case idleAction: {
			break;
		}
		default: {
			break;
		}
	}
}

static os_tsk_ret_t cmdline_task( void *params ) {
	cmdline_t *self = (cmdline_t *)params;
	for( ;; ) {
		cmdline_commit(self);
	}
}

static ll_t cmdline_getline(cmdline_t *self) {
	uint8_t ch = 0;
	ll_t action = idleAction;
	
	while( !self->init.getch( &ch, OS_WAIT_FOREVER ) ) {
		if( self->init.rcv_buff_size <= self->_rcv_idx+1 ) {
			action = tlngAction;
			self->_rcv_idx = 0;
			break;
		} else if( '\b' == ch ) {
			if( 0 < self->_rcv_idx ) {
				self->_rcv_buffer[self->_rcv_idx] = '\0';
				self->_rcv_idx--;
				cmdline_putc(self, '\b');
			}
			action = backAction;
		} else if( '\t' == ch ) {
			self->_rcv_buffer[self->_rcv_idx++] = ' ';
			self->_rcv_buffer[self->_rcv_idx] = '\0';
		} else if( ('\n' == ch) || ('\r' == ch) ) {
			if( 0 == self->_rcv_idx ) {
				action = newlAction;
				break;
			} else {
				self->_rcv_buffer[self->_rcv_idx] = '\0';
				self->_rcv_idx = 0;
				action = execAction;
				break;
			}
		} else {
			self->_rcv_buffer[self->_rcv_idx++] = ch;
			self->_rcv_buffer[self->_rcv_idx] = '\0';
			if( true == self->init.use_echo ) {
				cmdline_putc(self, ch);
			}
		}
	}
	return(action);
}

static bool cmdline_execute(cmdline_t *self) {
	bool stat = true;
	uint16_t cmd_idx=0, c_idx=0, cmd_len=0;
	uint8_t *cmd = NULL, *arg = NULL;

	cmdline_parse( self, &cmd, &arg );
	
	cmd_len = strlen((char *)cmd);
	
	for( c_idx=0; c_idx<cmd_len; c_idx++ ) {
		cmd[c_idx] = (uint8_t)toupper((int)cmd[c_idx]);
	}
	
	for( cmd_idx=0; cmd_idx<self->init.cmds_no; cmd_idx++ ) {
		if( 0 == strcmp((char *)self->init.cmds[cmd_idx].name, (char *)cmd)) {
			self->init.cmds[cmd_idx].f((void *)self, arg);
			break;
		}
	}

	if(cmd_idx == self->init.cmds_no) {
		stat = false;
	}

	return stat;
}

static void cmdline_parse(cmdline_t *self, uint8_t **cmd, uint8_t **arg ) {
	uint8_t *cmd_start = self->_rcv_buffer, *cmd_end = NULL;

	while(*cmd_start == ' ' || *cmd_start == '\0') {
		cmd_start++;
	}

	cmd_end = cmd_start;

	while(	*cmd_end != ' ' && *cmd_end != '\n' && *cmd_end != '\r'	\
			 && *cmd_end != '\0')
	{
		cmd_end++;
	}

	*cmd_end = '\0';
	*cmd = cmd_start;
	*arg = cmd_end+1;
}

static void cmdline_putc( cmdline_t *self, uint8_t c ) {
	self->init.putd( &c, 1 );
}

static void cmdline_prompt(cmdline_t *self) {
	uint16_t n=0;
	if( true == self->init.use_prompt ) {
		n = sprintf((char *)self->_ans_buffer, "%s> ", self->init.name);
		self->init.putd(self->_ans_buffer, n);
	}
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

