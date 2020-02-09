/*  --------------------------------------------------------------------------
 *  app_settings.c
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>

#include "lvgl.h"
#include "port.h"

#include "app_settings_common.h"
#include "app_settings.h"

static gui_rc_t settings_open(lv_obj_t *area);
static gui_rc_t settings_close(void);
static gui_rc_t settings_worker(void);

extern lv_img_dsc_t icon_settings;

gui_app_t app_settings = {
    .name = "Settings",
    .app_open = settings_open,
    .app_close = settings_close,
    .app_worker = settings_worker,
    .icon = &icon_settings
};

static settings_local_t *settd;
extern os_sem_t apps_common_sem;

static gui_rc_t settings_open( lv_obj_t *area ) {
    settd = (settings_local_t *)gui_apps_common_mem(sizeof(settings_local_t));
    if( NULL == settd ) {
        return( GuiFailed );
    }
    /* Init local aplication data here, default all are 0x00 */
    settd->sm = SettingsRun;
    return( GuiOk );
}

static gui_rc_t settings_close( void ) {
    settd->sm = SettingsForceExit;
    os_sem_post(&apps_common_sem);
    return( GuiOk );
}

static gui_rc_t settings_worker( void ) {

    for( ;; ) {
        os_sem_wait( &apps_common_sem, OS_WAIT_FOREVER );
        switch( settd->sm ) {
            case SettingsRun: {
                break;
            }
            case SettingsForceExit: {
                break;
            }
            default: {
                break;
            }
        }
        
        if( SettingsForceExit == settd->sm ) {
            break;
        }
    }
    
    return( GuiOk );
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */