/*  --------------------------------------------------------------------------
 *  terminal.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdbool.h>
#include <stdint.h>

#include "port.h"
#include "cmdline.h"

extern cmd_t cmds[];
extern uint32_t cmds_no;

cmdline_t stdterm;

void terminal_init( void ) {
    stdterm.init.cmds = cmds;
    stdterm.init.cmds_no = cmds_no;
    stdterm.init.name = "std";
    stdterm.init.use_prompt = true;
    stdterm.init.use_echo = STD_TERMINAL_USE_ECHO;
    stdterm.init.getch = std_getc;
    stdterm.init.putd = std_print_putd;
    stdterm.init.rcv_buff_size = 64;
    stdterm.init.ans_buff_size = 256;
    stdterm.init.tsk_stack_size = 1024;
    cmdline_init(&stdterm);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
