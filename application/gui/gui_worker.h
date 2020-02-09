/*  --------------------------------------------------------------------------
 *  gui_worker.h
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef GUI_WORKER_H_
#define GUI_WORKER_H_

#define GUI_PANEL_SIZE_PX       (60)
#define GUI_PANEL_INNER_PADD    (4)
#define GUI_PANEL_WIDGET_WIDTH  (GUI_PANEL_SIZE_PX-6)
#define GUI_PANEL_WIDGET_HEIGHT (GUI_PANEL_SIZE_PX-12)

#define GUI_ICON_2_ICON_PADD    (32)
#define GUI_ICON_EDGE_PADD      (32)
#define GUI_ICON_OBJ_WIDTH      (80)
#define GUI_ICON_OBJ_HEIGHT     (80)

typedef enum _gui_rc {
    GuiOk = 0,
    GuiFailed,
} gui_rc_t;

/* That memory will be used by all application at runtime,
 * Always filled with zero before putting to app */
#define GUI_APP_COMMON_MEM_SIZE (8U*1024U)

typedef struct _gui_app {
    gui_rc_t (*app_open)(lv_obj_t *area);
    gui_rc_t (*app_close)(void);
    gui_rc_t (*app_worker)(void);
    const char *name;
    const lv_img_dsc_t *icon;
} gui_app_t;

void gui_worker_init( void );
void * gui_apps_common_mem(uint32_t req_size);

#endif /* GUI_WORKER_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */