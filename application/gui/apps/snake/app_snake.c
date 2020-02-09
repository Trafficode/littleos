/*  --------------------------------------------------------------------------
 *  app_snake.h
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <stdint.h>
#include <stdlib.h>


#include "lvgl.h"
#include "port.h"

#include "app_snake_common.h"
#include "app_snake.h"

#define SNAKE_BTN_UP_ID         (0)
#define SNAKE_BTN_DOWN_ID       (1)
#define SNAKE_BTN_LEFT_ID       (2)
#define SNAKE_BTN_RIGHT_ID      (3)
#define SNAKE_BTN_CTRL_ID       (4)

#define SNAKE_FLAG_MOVE_UP      (SNAKE_BTN_UP_ID)
#define SNAKE_FLAG_MOVE_DOWN    (SNAKE_BTN_DOWN_ID)
#define SNAKE_FLAG_MOVE_LEFT    (SNAKE_BTN_LEFT_ID)
#define SNAKE_FLAG_MOVE_RIGHT   (SNAKE_BTN_RIGHT_ID)

static gui_rc_t snake_open(lv_obj_t *area);
static gui_rc_t snake_close(void);
static gui_rc_t snake_worker(void);

static void snake_draw_game_area(void);
static void snake_draw_control_area(void);

static void snake_born( void );
static void snake_move( void );
static bool snake_set_flag( uint32_t flag );
static bool snake_check_point( uint32_t x, uint32_t y, uint32_t offset );
static void snake_game_over( void );
static void snake_gen_part( void );
static void snake_check_to_relong( void );

extern lv_img_dsc_t icon_snake;

gui_app_t app_snake = {
    .name = "Snake",
    .app_open = snake_open,
    .app_close = snake_close,
    .app_worker = snake_worker,
    .icon = &icon_snake
};

static snake_local_t *snaked;
extern os_sem_t apps_common_sem;

extern logger_t stdlog;

static gui_rc_t snake_open(lv_obj_t *area) {
    snaked = (snake_local_t *)gui_apps_common_mem(sizeof(snake_local_t));
    if( NULL == snaked ) {
        return( GuiFailed );
    }
    /* Init local aplication data here, default all are 0x00 */
    snaked->area = area;

    snaked->log = stdlog;
    snaked->log.init.name = "snake";
    snaked->log.init.level = loggerLevelInfo;
    
    snaked->score = 0;
    snaked->game_speed = 1000;

    snaked->area_w = lv_obj_get_width(area);
    snaked->area_h = lv_obj_get_height(area);

    snaked->game_over_label = NULL;
    snaked->new_part = NULL;

    snaked->sm = SnakeDrawOpen;
    logger_info(&snaked->log, "%s\n", __FUNCTION__);

    lv_style_copy( &snaked->snake_body_style, &lv_style_plain_color);
    snaked->snake_body_style.body.radius = SNAKE_POINT_SIZE/4;

    lv_style_copy( &snaked->new_part_style, &lv_style_plain_color );
    snaked->new_part_style.body.main_color = LV_COLOR_MAGENTA;
    snaked->new_part_style.body.grad_color = LV_COLOR_MAGENTA;
    snaked->new_part_style.body.radius = SNAKE_POINT_SIZE/4;

    lv_style_copy( &snaked->game_over_label_style, &lv_style_plain );
    snaked->game_over_label_style.text.color = LV_COLOR_ORANGE;
    
    srand(sys_get_mstick());

    snake_draw_game_area();
    snake_draw_control_area();

    os_sem_post(&apps_common_sem);
    return( GuiOk );
}

static gui_rc_t snake_close(void) {
    snaked->sm = SnakeForceExit;
    os_sem_post(&apps_common_sem);
    return( GuiOk );
}

static gui_rc_t snake_worker(void) {
    logger_info(&snaked->log, "Snake worker starting\n");
    for( ;; ) {
        os_sem_wait( &apps_common_sem, snaked->game_speed );
        switch( snaked->sm ) {
            case SnakeIdle: {
                break;
            }
            case SnakeDrawOpen: {
                logger_info(&snaked->log, "SnakeDrawOpen\n");
                snaked->sm = SnakeIdle;
                break;
            }
            case SnakeStartAction: {
                logger_info(&snaked->log, "SnakeStartAction\n");
                lv_gui_lock();
                snaked->score = 0;
                sprintf(snaked->label_score_buffer,"Score: %d",snaked->score);
                lv_label_set_static_text(snaked->label_score, 
                                            snaked->label_score_buffer);
                if( NULL != snaked->game_over_label ) {
                    lv_obj_del( snaked->game_over_label );
                }
                lv_btn_set_state(snaked->btn_up, LV_BTN_STATE_REL);
                lv_btn_set_state(snaked->btn_down, LV_BTN_STATE_REL);
                lv_btn_set_state(snaked->btn_left, LV_BTN_STATE_REL);
                lv_btn_set_state(snaked->btn_right, LV_BTN_STATE_REL);
                lv_btn_set_state(snaked->btn_control, LV_BTN_STATE_INA);
                snaked->sm = SnakeGaming;
                snake_born();
                snake_gen_part();
                lv_gui_unlock();
                break;
            }         
            case SnakeGaming: {
                logger_debug(&snaked->log, "SnakeGaming\n");
                lv_gui_lock();
                snake_move();
                bool rc = snake_check_point(
                                    lv_obj_get_x(snaked->snake[0].body),
                                    lv_obj_get_y(snaked->snake[0].body),
                                    1);
                if( true == rc ) {
                    logger_critical(&snaked->log, "Game Over\n");
                    snaked->sm = SnakeGameOver;
                    os_sem_post(&apps_common_sem);
                } else {
                    rc = snake_check_point(
                                    lv_obj_get_x(snaked->new_part),
                                    lv_obj_get_y(snaked->new_part),
                                    0);
                    if( true == rc ) {
                        logger_critical(&snaked->log, "Relong snake\n");
                        snake_check_to_relong();
                        snake_gen_part();
                        snaked->game_speed -= snaked->game_speed/15;
                        snaked->game_speed = (snaked->game_speed > 100) ?
                                              snaked->game_speed:100;

                        logger_critical(&snaked->log, "New period: %d[ms]\n", 
                                    snaked->game_speed);
                    }
                }

                lv_gui_unlock();
                break;
            }
            case SnakeGameOver: {
                logger_info(&snaked->log, "SnakeGameOver\n");
                lv_gui_lock();
                snake_game_over();
                snaked->sm = SnakeIdle;
                snaked->game_speed = 1000;
                lv_gui_unlock();
                break;
            }
            case SnakeForceExit: {
                logger_info(&snaked->log, "SnakeForceExit\n");
                break;
            }
            default: {
                break;
            }
        }

        if( SnakeForceExit == snaked->sm ) {
            break;
        }
    }
    
    return( GuiOk );
}

static void snake_check_to_relong( void ) {
    snake_part_t snake_last_part = snaked->snake[snaked->snake_len-1];
    uint32_t snake_len = snaked->snake_len;
    lv_align_t align_type;

    if( SNAKE_FLAG_MOVE_DOWN == snake_last_part.dir ) {
        align_type = LV_ALIGN_OUT_TOP_MID;
    } else if ( SNAKE_FLAG_MOVE_UP == snake_last_part.dir ) {
        align_type = LV_ALIGN_OUT_BOTTOM_MID;
    } else if ( SNAKE_FLAG_MOVE_LEFT == snake_last_part.dir ) {
        align_type = LV_ALIGN_OUT_RIGHT_MID;
    } else if ( SNAKE_FLAG_MOVE_RIGHT == snake_last_part.dir ) {
        align_type = LV_ALIGN_OUT_LEFT_MID;
    } else {
        logger_error(&snaked->log, "Can not append new part, bad flag\n");
        return;
    }

    snaked->snake[snake_len].body = snaked->new_part;
    snaked->snake[snake_len].dir = snaked->snake[snake_len-1].dir;
    lv_obj_set_style(snaked->snake[snake_len].body, 
                        &snaked->snake_body_style);
    lv_obj_align(snaked->snake[snake_len].body,
                    snaked->snake[snake_len-1].body, 
                    align_type, 0, 0);

    snaked->snake_len++;
    snaked->score++;
    sprintf(snaked->label_score_buffer, "Score: %d", snaked->score);
    lv_label_set_static_text(snaked->label_score, 
                                snaked->label_score_buffer);
}

static void snake_gen_part( void ) {
    uint32_t new_part_x=0, new_part_y=0;
    while( true ) {
        new_part_x = (rand() % snaked->game_area_w);
        new_part_x -= new_part_x % SNAKE_POINT_SIZE;
        new_part_y = (rand() % snaked->game_area_h);
        new_part_y -= new_part_y % SNAKE_POINT_SIZE;

        if( false == snake_check_point( new_part_x, new_part_y, 0 ) ) {
            /* New part is outside the snake body - OK */
            snaked->new_part = lv_obj_create(snaked->game_area, NULL);
            lv_obj_set_size(snaked->new_part, SNAKE_POINT_SIZE, 
                            SNAKE_POINT_SIZE);
            lv_obj_set_style(snaked->new_part, &snaked->new_part_style);
            lv_obj_align(snaked->new_part, NULL, LV_ALIGN_IN_TOP_LEFT, 
                         new_part_x, new_part_y);
            logger_info(&snaked->log, "Created new part at (%d,%d)\n", 
                        new_part_x, new_part_y );
            break;
        }
    }
    
}

static void snake_game_over( void ) {
    if( NULL != snaked->new_part ) {
        lv_obj_del( snaked->new_part );
    }

    for( uint32_t idx=0; idx<snaked->snake_len; idx++ ) {
        lv_obj_del( snaked->snake[idx].body );
    }

    snaked->game_over_label = lv_label_create(snaked->game_area, NULL);
    lv_obj_set_style(snaked->game_over_label, &snaked->game_over_label_style);
    lv_label_set_static_text(snaked->game_over_label, "Game Over!");
    lv_obj_align(snaked->game_over_label, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_btn_set_state(snaked->btn_up, LV_BTN_STATE_INA);
    lv_btn_set_state(snaked->btn_down, LV_BTN_STATE_INA);
    lv_btn_set_state(snaked->btn_left, LV_BTN_STATE_INA);
    lv_btn_set_state(snaked->btn_right, LV_BTN_STATE_INA);
    lv_btn_set_state(snaked->btn_control, LV_BTN_STATE_REL);
    lv_label_set_static_text(snaked->btn_ctrl_label, "RETRY");
}

static bool snake_check_point( uint32_t x, uint32_t y, uint32_t offset ) {
    bool rc = false;
    uint32_t part_x=0, part_y=0, idx=0;

    for( idx=offset; idx<snaked->snake_len; idx++ ) {
        part_x = lv_obj_get_x(snaked->snake[idx].body);
        part_y = lv_obj_get_y(snaked->snake[idx].body);
        if( (part_x == x) && (part_y == y) ) {
            rc = true;
            break;
        }
    }

    return( rc );
}


static lv_res_t snake_btn_action(lv_obj_t * btn) {
    uint32_t id = lv_obj_get_free_num( btn );

    if( SNAKE_BTN_CTRL_ID == id ) {
        if( SnakeIdle == snaked->sm ) {
            snaked->sm = SnakeStartAction;
            os_sem_post(&apps_common_sem);
        } else {
            /* Stop moving snake for a while */
        }
    } else {
        /* Button id is equal to flag */
        if( true == snake_set_flag(id) ) {
            os_sem_post(&apps_common_sem);
        }
    }

    return LV_RES_OK; /* Return OK if the button is not deleted */
}

static void snake_draw_game_area(void) {
    snaked->game_area = lv_obj_create(snaked->area, NULL);
    snaked->game_area_h = snaked->area_h - 2*10;
    snaked->game_area_h -= snaked->game_area_h % SNAKE_POINT_SIZE;
    snaked->game_area_w = (snaked->area_w*3)/5; 
    snaked->game_area_w -= snaked->game_area_w % SNAKE_POINT_SIZE;

    logger_info(&snaked->log, "Game area size [%d,%d]\n", snaked->game_area_w,
                        snaked->game_area_h);

    lv_style_copy(&snaked->game_area_style, &lv_style_plain_color);
    snaked->game_area_style.body.radius = 2;
    snaked->game_area_style.body.border.width = 2;
    snaked->game_area_style.body.main_color = LV_COLOR_MAKE(222,222,222);
    snaked->game_area_style.body.grad_color = LV_COLOR_MAKE(222,222,222);
    lv_obj_set_style(snaked->game_area, &snaked->game_area_style);
    lv_obj_set_size(snaked->game_area, snaked->game_area_w, 
                    snaked->game_area_h);
    lv_obj_align(snaked->game_area, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
}

static void snake_draw_control_area(void) {
    lv_obj_t *img;
    uint32_t btn_dir_w = (snaked->area_w - 5*10 - snaked->game_area_w)/3;
    uint32_t btn_dir_h = btn_dir_w;
    uint32_t btn_ctrl_w = snaked->area_w - snaked->game_area_w - 3*10;
    uint32_t btn_ctrl_h = btn_dir_h;

    sprintf(snaked->sym_left, "%s", SYMBOL_LEFT);
    sprintf(snaked->sym_right, "%s", SYMBOL_RIGHT);
    sprintf(snaked->sym_down, "%s", SYMBOL_DOWN);
    sprintf(snaked->sym_up, "%s", SYMBOL_UP);

    snaked->btn_control = lv_btn_create(snaked->area, NULL);
    lv_btn_set_action(snaked->btn_control, LV_BTN_ACTION_CLICK, 
                      snake_btn_action);
    lv_obj_set_size(snaked->btn_control, btn_ctrl_w, btn_ctrl_h);
    lv_obj_set_free_num(snaked->btn_control, SNAKE_BTN_CTRL_ID);
    lv_obj_align(snaked->btn_control, snaked->game_area, 
                 LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
    snaked->btn_ctrl_label = lv_label_create(snaked->btn_control, NULL);
    lv_label_set_static_text(snaked->btn_ctrl_label, "START");
    lv_obj_align(snaked->btn_ctrl_label, NULL, LV_ALIGN_CENTER, 0, 0);

    logger_info(&snaked->log, "Control btn size [%d,%d]\n", btn_ctrl_w,
                              btn_ctrl_h);

    snaked->btn_down = lv_btn_create(snaked->area, NULL);
    lv_btn_set_action(snaked->btn_down, LV_BTN_ACTION_CLICK, snake_btn_action);
    lv_obj_set_size(snaked->btn_down, btn_dir_w, btn_dir_h);
    lv_obj_set_free_num(snaked->btn_down, SNAKE_BTN_DOWN_ID);
    lv_btn_set_state(snaked->btn_down, LV_BTN_STATE_INA);
    lv_obj_align(snaked->btn_down, snaked->btn_control, 
                 LV_ALIGN_OUT_TOP_MID, 0, -20);
    img = lv_img_create(snaked->btn_down, NULL);
    lv_img_set_src(img, snaked->sym_down);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    snaked->btn_left = lv_btn_create(snaked->area, NULL);
    lv_btn_set_action(snaked->btn_left, LV_BTN_ACTION_CLICK, snake_btn_action);
    lv_obj_set_size(snaked->btn_left, btn_dir_w, btn_dir_h);
    lv_obj_set_free_num(snaked->btn_left, SNAKE_BTN_LEFT_ID);
    lv_btn_set_state(snaked->btn_left, LV_BTN_STATE_INA);
    lv_obj_align(snaked->btn_left, snaked->btn_down, 
                 LV_ALIGN_OUT_TOP_MID, -(btn_dir_w+10), 0);
    img = lv_img_create(snaked->btn_left, NULL);
    lv_img_set_src(img, snaked->sym_left);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    snaked->btn_right = lv_btn_create(snaked->area, NULL);
    lv_btn_set_action(snaked->btn_right, LV_BTN_ACTION_CLICK, snake_btn_action);
    lv_obj_set_size(snaked->btn_right, btn_dir_w, btn_dir_h);
    lv_obj_set_free_num(snaked->btn_right, SNAKE_BTN_RIGHT_ID);
    lv_btn_set_state(snaked->btn_right, LV_BTN_STATE_INA);
    lv_obj_align(snaked->btn_right, snaked->btn_down, 
                 LV_ALIGN_OUT_TOP_MID, btn_dir_w+10, 0);
    img = lv_img_create(snaked->btn_right, NULL);
    lv_img_set_src(img, snaked->sym_right);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    snaked->btn_up = lv_btn_create(snaked->area, NULL);
    lv_btn_set_action(snaked->btn_up, LV_BTN_ACTION_CLICK, snake_btn_action);
    lv_obj_set_size(snaked->btn_up, btn_dir_w, btn_dir_h);
    lv_obj_set_free_num(snaked->btn_up, SNAKE_BTN_UP_ID);
    lv_btn_set_state(snaked->btn_up, LV_BTN_STATE_INA);
    lv_obj_align(snaked->btn_up, snaked->btn_down, 
                 LV_ALIGN_OUT_TOP_MID, 0, -btn_dir_h);
    img = lv_img_create(snaked->btn_up, NULL);
    lv_img_set_src(img, snaked->sym_up);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    snaked->label_score = lv_label_create(snaked->area, NULL);
    sprintf(snaked->label_score_buffer, "Score: %d", snaked->score);
    lv_label_set_static_text(snaked->label_score, snaked->label_score_buffer);
    lv_obj_align(snaked->label_score, snaked->game_area,
    			 LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
}

static bool snake_set_flag( uint32_t flag ) {
    bool rc = true;
    uint32_t curr_dir = snaked->snake[0].dir;
    if( SNAKE_FLAG_MOVE_LEFT == flag ) {
        if(SNAKE_FLAG_MOVE_RIGHT != curr_dir ) {
            snaked->snake[0].dir = SNAKE_FLAG_MOVE_LEFT;
        }
    } else if( SNAKE_FLAG_MOVE_RIGHT == flag ) {
        if( SNAKE_FLAG_MOVE_LEFT != curr_dir ) {
            snaked->snake[0].dir = SNAKE_FLAG_MOVE_RIGHT;
        }
    } else if( SNAKE_FLAG_MOVE_UP == flag ) {
        if( SNAKE_FLAG_MOVE_DOWN != curr_dir ) {
            snaked->snake[0].dir = SNAKE_FLAG_MOVE_UP;
        }
    } else if( SNAKE_FLAG_MOVE_DOWN == flag ) {
        if( SNAKE_FLAG_MOVE_UP != curr_dir ) {
            snaked->snake[0].dir = SNAKE_FLAG_MOVE_DOWN;
        }
    } else {
        logger_error(&snaked->log, "Unknown flag, idx:%d\n", flag);
        rc = false;
    }
    return(rc);
}

static void snake_born( void ) {
    uint32_t start_x=snaked->game_area_w/2, start_y=snaked->game_area_h/2;

    start_x -= start_x % SNAKE_POINT_SIZE;
    start_y -= start_y % SNAKE_POINT_SIZE;
    logger_info(&snaked->log, "Snake start at (%d,%d)\n", start_x, start_y);

    snaked->snake[0].dir = SNAKE_FLAG_MOVE_LEFT;
    snaked->snake[0].body = lv_obj_create(snaked->game_area, NULL);
    lv_obj_set_size(snaked->snake[0].body, SNAKE_POINT_SIZE, SNAKE_POINT_SIZE);
    lv_obj_set_style(snaked->snake[0].body, &snaked->snake_body_style);
    lv_obj_align(snaked->snake[0].body, NULL, LV_ALIGN_IN_TOP_LEFT, 
                 start_x, start_y);

    if( SNAKE_START_LEN < 2 ) {
        snaked->snake_len = 1;
        return;
    } else if( SNAKE_START_LEN > 16 ) {
        snaked->snake_len = 16;
    } else {
        snaked->snake_len = SNAKE_START_LEN;
    }

    for( uint32_t idx=1; idx<snaked->snake_len; idx++ ) {
        snaked->snake[idx].dir = SNAKE_FLAG_MOVE_LEFT;
        snaked->snake[idx].body = lv_obj_create(snaked->game_area, NULL);
        lv_obj_set_size(snaked->snake[idx].body, SNAKE_POINT_SIZE, 
                        SNAKE_POINT_SIZE);
        lv_obj_set_style(snaked->snake[idx].body, &snaked->snake_body_style);
        lv_obj_align(snaked->snake[idx].body, snaked->snake[idx-1].body,
                    LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }
}

static void snake_move( void ) {
    uint32_t idx=0, xpos=0, ypos=0;

    for( idx=0; idx<snaked->snake_len; idx++ ) {
        ypos = lv_obj_get_y( snaked->snake[idx].body );
        xpos = lv_obj_get_x( snaked->snake[idx].body );
        if( SNAKE_FLAG_MOVE_LEFT == snaked->snake[idx].dir ) {
            if( 0 == xpos ) {    /* Make mirror */
                xpos = snaked->game_area_w-SNAKE_POINT_SIZE;
            } else {
                xpos -= SNAKE_POINT_SIZE;
            } 
        } else if( SNAKE_FLAG_MOVE_UP == snaked->snake[idx].dir ) {
            if( 0 == ypos ) {
                /* Make mirror */
                ypos = snaked->game_area_h-SNAKE_POINT_SIZE;
            } else {
                ypos -= SNAKE_POINT_SIZE;
            }
        } else if( SNAKE_FLAG_MOVE_RIGHT == snaked->snake[idx].dir ) {
            if( snaked->game_area_w-SNAKE_POINT_SIZE == xpos ) {
                xpos = 0;
            } else {
                xpos += SNAKE_POINT_SIZE;
            }
        } else if( SNAKE_FLAG_MOVE_DOWN == snaked->snake[idx].dir ) {
            if( snaked->game_area_h-SNAKE_POINT_SIZE == ypos ) {
                ypos = 0;
            } else {
                ypos += SNAKE_POINT_SIZE;
            }
        } else {
            logger_error(&snaked->log, "Unknown direction, idx:%d\n", idx);
            continue;
        }

        lv_obj_align(snaked->snake[idx].body, snaked->game_area, 
                        LV_ALIGN_IN_TOP_LEFT, xpos, ypos);
    }

    for( idx=snaked->snake_len-1; idx>0; idx-- ) {
        snaked->snake[idx].dir = (snaked->snake[idx-1].dir & 0x0F);
    }
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
