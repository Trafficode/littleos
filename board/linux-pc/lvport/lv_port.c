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

static pthread_t worker_handle;
static void *lv_worker(void *param);

static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

static pthread_mutex_t gui_lock;

const char *level_trans[4] = {"TRACE", "INFO", "WARN", "ERROR"};

static void lv_log( lv_log_level_t lvl, const char *file, uint32_t line, 
                    const char *desc)
{
    unix_board_debug("%s - %s[%d] - %s\n", level_trans[lvl], file, line, desc);
}

void lv_port_init( void ) {
    int rc = 0;
    char *mem_type = (0 == LV_VDB_ADR) ? "Builtin":"Provided";
    
    rc = pthread_mutex_init( &gui_lock, NULL );
    if( 0 != rc ) {
        unix_board_error("[lv_port_init] Failed init gui lock RC:%d\n", rc );
    }

    if( 0 != (rc = xlib_input_init(LV_HOR_RES, LV_VER_RES)) ) {
        unix_board_error("[lv_port_init] xlib_input_init: %d\n", rc);
        goto exit;
    }

    if( 0 != (rc = xlib_graphic_init(LV_HOR_RES, LV_VER_RES))) {
        unix_board_error("[lv_port_init] xlib_graphic_init: %d\n", rc);
        goto exit;
    }
    
    lv_init();
    lv_log_register_print(lv_log);

    unix_board_critical("[lv_port_init] VDB_SIZE %u Bytes in %s memory\n", 
                         LV_VDB_SIZE*2, mem_type);

    lv_disp_drv_init(&disp_drv);
    disp_drv.disp_flush = disp_flush;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);                  
    indev_drv.type = LV_INDEV_TYPE_POINTER;         
    indev_drv.read = ts_input_read;
    lv_indev_drv_register(&indev_drv);

    rc = pthread_create(&worker_handle, NULL, lv_worker, NULL);
    if ( 0 != rc ) {
        unix_board_error("[lv_port_init] Create lv worker failed\n");
    }

exit:
    return;
}

void lv_port_gui_lock( void ) {
    pthread_mutex_lock( &gui_lock );
}

void lv_port_gui_unlock( void ) {
    pthread_mutex_unlock( &gui_lock );
}

static os_tsk_ret_t lv_worker(void *param) {

	for( ;; ) {
        sys_sleep_ms(5);
        pthread_mutex_lock( &gui_lock );
        lv_task_handler();
        pthread_mutex_unlock( &gui_lock );
    }
}

static bool ts_input_read(lv_indev_data_t *data) {
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    /* Read your touchpad */
    if( xlib_input_read( &last_x, &last_y ) ) {
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
    xlib_graphic_flush( x1, y1, x2, y2, (uint16_t *)color_p );
    lv_flush_ready();
    return;
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
