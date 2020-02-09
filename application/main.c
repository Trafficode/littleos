/*  --------------------------------------------------------------------------
 *  main.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"
#include "logger.h"
#include "terminal.h"
#include "lvgl.h"
#include "gui_worker.h"

os_tsk_t init_tsk;

uint16_t led_toggle_templ_idx=0;
uint16_t led_toggle_templ=0x0F0F;

logger_t stdlog;

// static lv_res_t slider_action(lv_obj_t * slider) {
//     logger_critical(&stdlog, "New slider value: %d\n", 
// 							 lv_slider_get_value(slider));
//     return LV_RES_OK;
// }

// lv_obj_t *slider, *screen;

static os_tsk_ret_t init ( void *arg ) {
	std_print_init();
	std_getc_init();

	stdlog.init.name = "main";
	stdlog.init.level = loggerLevelDebug;
	stdlog.init.putd = std_print_putd;
	stdlog.init.buff_size = 128;
	logger_init( &stdlog, NULL );

	logger_critical(&stdlog, "status_led_init\n");
	status_led_init();

	logger_critical(&stdlog, "terminal_init\n");
	terminal_init();

	logger_critical(&stdlog, "lv_port_init\n");
	lv_board_init();

	logger_critical(&stdlog, "gui_worker_init\n");
	gui_worker_init();

	logger_critical(&stdlog, "all inited\n");

	// screen = lv_scr_act();
	// /* Create a default slider */
	// slider = lv_slider_create(screen, NULL);
	// lv_obj_set_size(slider, 360, 80);
	// lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
	// lv_slider_set_action(slider, slider_action);
	// lv_bar_set_value(slider, 0);

	for( ;; ) {
		if( led_toggle_templ & (1 << led_toggle_templ_idx) ) {
			status_led_on();
		} else {
			status_led_off();
		}
		led_toggle_templ_idx = (led_toggle_templ_idx+1) % 16;
		sys_sleep_ms(100);
	}
}

int main(void) {
	int rc=0;
	
	system_init();

	rc = os_tsk_cr(	&init_tsk, init, "init", NULL, osNormalPriority, 2048);
	if( 0 != rc ) {
		for( ;; );
	}
	
	os_kernel_run();
	
	return(0);
}

void sys_assert_fail( err_t err ) {
	led_toggle_templ = 0x0000;
	err = (err_t)err;
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
