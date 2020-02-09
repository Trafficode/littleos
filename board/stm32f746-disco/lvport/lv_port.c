/*  --------------------------------------------------------------------------
 *  lv_port.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include "port.h"
#include "lvgl.h"
#include "logger.h"

#include "lv_port.h"

static bool ts_input_read( lv_indev_data_t *data );
static void disp_flush( int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                        const lv_color_t * color_p );

static TaskHandle_t worker_handle;
static void lv_worker(void *param);
static SemaphoreHandle_t gui_lock;

static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

const char *level_trans[4] = {"TRACE", "INFO", "WARN", "ERROR"};

static void lv_log( lv_log_level_t lvl, const char *file, uint32_t line, 
                    const char *desc)
{
    uart_board_debug("%s - %s[%d] - %s\n", level_trans[lvl], file, line, desc);
}

void lv_port_init( void ) {
    int rc = 0;
    char *mem_type = (0 == LV_VDB_ADR) ? "Builtin":"Provided";

    lv_init();
    lv_log_register_print(lv_log);

    uart_board_critical("[lv_port_init] VDB_SIZE %u Bytes in %s memory\n", 
                         LV_VDB_SIZE*2, mem_type);
    
    gui_lock = xSemaphoreCreateMutex();
	if( NULL == gui_lock ) {
		uart_board_error("[lv_port_init] Create gui lock\n");
        goto exit;
	}

    if( 0 != (rc = ts_init(LV_HOR_RES, LV_VER_RES)) ) {
        uart_board_error("[lv_port_init] ts_init: %d\n", rc);
        goto exit;
    }

    if( 0 != (rc = lcd_init(LV_HOR_RES, LV_VER_RES))) {
        uart_board_error("[lv_port_init] lcd_init: %d\n", rc);
        goto exit;
    }
    
    lv_disp_drv_init(&disp_drv);
    disp_drv.disp_flush = disp_flush;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);                  
    indev_drv.type = LV_INDEV_TYPE_POINTER;         
    indev_drv.read = ts_input_read;
    lv_indev_drv_register(&indev_drv);

    rc = xTaskCreate(lv_worker, "lv", 6*1024/sizeof(uint32_t), NULL, 
                     osNormalPriority, worker_handle);
	if( pdPASS != rc ) {
        uart_board_error("[lv_port_init] Create lv worker failed\n");
    }

exit:
    return;
}

void lv_port_gui_lock(void) {
    xSemaphoreTake( gui_lock, osWaitForever );
}

void lv_port_gui_unlock(void) {
    xSemaphoreGive( gui_lock );
}

static os_tsk_ret_t lv_worker(void *param) {
    uart_board_critical("[lv_worker] Started ...\n");
	for( ;; ) {
        sys_sleep_ms(5);
        xSemaphoreTake( gui_lock, osWaitForever );
        lv_task_handler();
        xSemaphoreGive( gui_lock );
    }
}

static bool ts_input_read(lv_indev_data_t *data) {
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    /* Read your touchpad */
    if( ts_read( &last_x, &last_y ) ) {
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = last_x;
    data->point.y = last_y;

    return false;
}

static void disp_flush( int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                        const lv_color_t * color_p )
{   
    uint32_t t_start = sys_get_mstick(), t_takes = 0;
    uart_board_info("[disp_flush] Flush ...\n");
    lcd_flush( x1, y1, x2, y2, (uint16_t *)color_p );
    lv_flush_ready();
    t_takes = sys_get_mstick()-t_start;
    uart_board_info("[disp_flush] (%d,%d):(%d,%d) takes %u ms\n",
                        x1, y1, x2, y2, t_takes );
    return;
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
