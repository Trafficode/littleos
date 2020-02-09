/*  --------------------------------------------------------------------------
 *  gui_worker.c
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>
#include <string.h>

#include "port.h"
#include "logger.h"
#include "lvgl.h"

#include "app_settings.h"
#include "app_snake.h"

#include "gui_worker.h"

static gui_app_t *apps[] = {
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_snake,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
    &app_settings,
};

#define APPS_NUM    (sizeof(apps)/sizeof(apps[0]))

#define GUI_CPU_USAGE_POINTS    (GUI_PANEL_SIZE_PX/2)

static os_tsk_t worker_tsk;
static os_tsk_t app_worker_tsk;
static os_tsk_ret_t gui_worker ( void *arg );
static os_tsk_ret_t app_worker ( void *arg );
static void gui_desktop_cr( void );
static void gui_set_cpu_usage( void );
static void gui_place_apps(void);

static lv_obj_t *screen, *panel, *cpu_chart, *icons_page, *app_area;
static lv_obj_t *close_btn;

static lv_chart_series_t *cpu_chart_serie;
static lv_style_t chart_style, icon_style, scrl_style, app_name, panel_style;

static uint32_t running_app = 0;

os_sem_t apps_common_sem;
uint8_t  apps_common_memory[GUI_APP_COMMON_MEM_SIZE];

static os_sem_t worker_sync_sem;

#define WORK_AREA_WIDTH     (LV_HOR_RES-GUI_PANEL_SIZE_PX)
extern logger_t stdlog;
logger_t guilog;

void gui_worker_init( void ) {
    int rc=0;
    guilog = stdlog;
    guilog.init.name = "gui";
    guilog.init.level = loggerLevelDebug;

    rc = os_sem_cr(&worker_sync_sem, 1);
    if( 0 != rc ) {
		logger_critical(&guilog, "Create worker sem failed\n");
	}

    rc = os_sem_cr(&apps_common_sem, 1);
    if( 0 != rc ) {
		logger_critical(&guilog, "Create apps common sem failed\n");
	}

	rc = os_tsk_cr(&worker_tsk,gui_worker,"gui",NULL,osNormalPriority,2048);
	if( 0 != rc ) {
		logger_critical(&guilog, "Create gui worker task failed\n");
	}

    rc = os_tsk_cr(&app_worker_tsk,app_worker,"app",NULL,osNormalPriority,2048);
	if( 0 != rc ) {
		logger_critical(&guilog, "Create gui apps worker task failed\n");
	}
}

void * gui_apps_common_mem(uint32_t req_size) {
    if(GUI_APP_COMMON_MEM_SIZE < req_size) {
        logger_error(&guilog, "Failed alloc %u bytef for application\n",
                                req_size);
        return NULL;
    }
    memset(apps_common_memory, 0x00, req_size);
    logger_info(&guilog, "Success alloc %u bytes for application\n", req_size);
    return (void *)apps_common_memory;
}

static os_tsk_ret_t gui_worker ( void *arg ) {
    logger_critical(&guilog, "Gui worker is running\n");

    /* get main screen */
    screen = lv_scr_act();

    lv_gui_lock();

    gui_desktop_cr();

    lv_gui_unlock();

    for( ;; ) {
        sys_sleep_ms(1000);
        lv_gui_lock();
        gui_set_cpu_usage();
        lv_gui_unlock();
    }
}

static lv_res_t app_close_btn_action(lv_obj_t * btn) {

    logger_debug(&guilog, "Close Application: %s\n", apps[running_app]->name);
    apps[running_app]->app_close();
    return LV_RES_OK; /* Return OK if the button is not deleted */
}

static os_tsk_ret_t app_worker ( void *arg ) {
    gui_rc_t rc = 0;
    char symbol_close_buf[8];
    sprintf(symbol_close_buf, "%s", SYMBOL_CLOSE);

    logger_critical(&guilog, "Gui app worker is running\n");

    for( ;; ) {
        os_sem_wait(&worker_sync_sem, OS_WAIT_FOREVER);
        logger_critical(&guilog, "Launched app: %s\n", apps[running_app]->name);

        lv_gui_lock();

        /* Open aplication */
        gui_app_t * app = apps[running_app];
        lv_obj_del(icons_page);

        app_area = lv_obj_create(screen, NULL);
        lv_obj_set_size(app_area, WORK_AREA_WIDTH, LV_VER_RES);
        lv_obj_align(app_area, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
        lv_obj_set_style(app_area, &lv_style_transp);

        lv_gui_unlock();

        if( GuiOk == (rc = app->app_open(app_area)) ) {
            logger_info(&guilog, "Open [%s] success\n", app->name);

            lv_gui_lock();
            /* Create close btn */
            close_btn = lv_btn_create(panel, NULL);
            lv_btn_set_action(close_btn, LV_BTN_ACTION_CLICK, 
                                app_close_btn_action);
            lv_obj_set_size(close_btn, GUI_PANEL_WIDGET_WIDTH, 
                                       GUI_PANEL_WIDGET_HEIGHT);
            lv_obj_align(close_btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 
                            GUI_PANEL_INNER_PADD);

            lv_obj_t * img_sym = lv_img_create(close_btn, NULL);
            lv_img_set_src(img_sym, symbol_close_buf);
            lv_obj_align(img_sym, NULL, LV_ALIGN_CENTER, 0, 0);
            lv_gui_unlock();
        } else {
            logger_info(&guilog, "Open [%s] failed\n", app->name);
        }

        if( GuiOk == rc ) {
            app->app_worker();
            logger_info(&guilog, "Application [%s] done\n", app->name);
            lv_gui_lock();

            lv_obj_del(close_btn);
            lv_obj_del(app_area);
            gui_place_apps();
            running_app = 0;
            
            lv_gui_unlock();
        }
    }
}

static void gui_desktop_cr( void ) {

    lv_style_copy(&panel_style, &lv_style_pretty);

    panel = lv_obj_create(screen, NULL);
    lv_obj_set_size(panel, GUI_PANEL_SIZE_PX, LV_VER_RES);
    lv_obj_set_style(panel, &panel_style);
    lv_obj_align(panel, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);

    lv_style_copy(&chart_style, &lv_style_pretty);
    chart_style.body.shadow.width = 0;
    chart_style.body.border.width = 1;
    chart_style.body.radius = 1;
    chart_style.line.width = 0;

    cpu_chart = lv_chart_create(panel, NULL);
    lv_obj_set_size(cpu_chart, GUI_PANEL_WIDGET_WIDTH, 
                        GUI_PANEL_WIDGET_HEIGHT);
    lv_obj_set_style(cpu_chart, &chart_style);
    lv_chart_set_type(cpu_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_series_width(cpu_chart, 1);
    lv_chart_set_point_count(cpu_chart, GUI_CPU_USAGE_POINTS);
    lv_chart_set_range(cpu_chart, 0, 100);
    lv_obj_align(cpu_chart, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 
                    -GUI_PANEL_INNER_PADD);

    cpu_chart_serie = lv_chart_add_series(cpu_chart, LV_COLOR_RED);
    memset(cpu_chart_serie->points, 0, 2*GUI_CPU_USAGE_POINTS);

    lv_chart_refresh(cpu_chart);

    /* Place application on desktop */
    gui_place_apps();
}

static lv_res_t icon_click_action(lv_obj_t * icon) {
    
    running_app = lv_obj_get_free_num( icon );
    logger_debug(&guilog, "Run Application: %s\n", apps[running_app]->name);
    
    os_sem_post( &worker_sync_sem );

    return LV_RES_OK; /* Return OK if the button is not deleted */
}

static void gui_place_apps(void) {
    lv_obj_t *img, *icon, *label;

    logger_debug(&guilog, "gui_place_apps()\n");

    /* Create apps icon page */
    icons_page = lv_page_create(screen, NULL);
    
    lv_page_set_scrl_height(icons_page, LV_VER_RES-20);
    
    lv_page_set_sb_mode(icons_page, LV_SB_MODE_DRAG);
    lv_page_set_scrl_layout(icons_page, LV_LAYOUT_GRID);
    lv_obj_set_style(icons_page, &lv_style_transp);
    lv_obj_set_size(icons_page, WORK_AREA_WIDTH, LV_VER_RES);

    lv_obj_align(icons_page, screen, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    lv_style_copy(&scrl_style, &lv_style_transp);
    scrl_style.body.padding.ver = GUI_ICON_EDGE_PADD;
    scrl_style.body.padding.hor = GUI_ICON_EDGE_PADD;
    scrl_style.body.padding.inner = GUI_ICON_2_ICON_PADD;
    lv_obj_set_style( lv_page_get_scrl(icons_page), &scrl_style );

    lv_style_copy(&icon_style, &lv_style_transp);

    lv_style_copy(&app_name, &lv_style_plain);
    app_name.text.font = &lv_font_dejavu_20;
    app_name.text.letter_space = 1;
    app_name.text.line_space = 1;

    for (int app_cnt=0; app_cnt<APPS_NUM; app_cnt++) {
        icon = lv_btn_create(icons_page, NULL);

        /* Set a unique number for the button means app no */
        lv_obj_set_free_num(icon, app_cnt);

        lv_obj_set_size(icon, GUI_ICON_OBJ_WIDTH, GUI_ICON_OBJ_HEIGHT);

        lv_btn_set_action(icon, LV_BTN_ACTION_CLICK, icon_click_action);

        img = lv_img_create(icon, NULL);
        lv_img_set_src(img, apps[app_cnt]->icon);

        /* Set modified btn icon style */
        lv_obj_set_style(icon, &icon_style);
        lv_btn_set_style(icon, LV_BTNM_STYLE_BG, &icon_style);
        lv_btn_set_style(icon, LV_BTNM_STYLE_BTN_REL, &icon_style);
        lv_btn_set_style(icon, LV_BTNM_STYLE_BTN_PR, &icon_style);

        label = lv_label_create(icon, NULL);
        lv_label_set_static_text(label, apps[app_cnt]->name);
        lv_label_set_style(label, &app_name);
        lv_obj_align(label, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    }
}

static void gui_set_cpu_usage(void) {
    uint16_t cpu_usage=0;
    cpu_usage = sys_cpu_usage();
    lv_chart_set_next(cpu_chart, cpu_chart_serie, cpu_usage);
    lv_chart_refresh(cpu_chart);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */