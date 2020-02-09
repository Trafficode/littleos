/*  --------------------------------------------------------------------------
 *  cmds.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>

#include "cmdline.h"

static void cmd_help(void *self, uint8_t *param);

cmd_t cmds[] = {
	{"HELP", cmd_help, "Available commands"}
};

uint32_t cmds_no = sizeof(cmds)/sizeof(cmds[0]);

static void cmd_help(void *self, uint8_t *param) {
    uint32_t idx=0;
	cmdline_answer(self, "Commands number: %d\n", cmds_no);
	for( idx=0; idx<cmds_no; idx++ ) {
		cmdline_answer(self, "%10s - %s\n", cmds[idx].name, cmds[idx].help);
	}
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/