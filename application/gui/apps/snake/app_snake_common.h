/*  --------------------------------------------------------------------------
 *  app_snake_common.h
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef APP_SNAKE_COMMON_H_
#define APP_SNAKE_COMMON_H_

#include "logger.h"

#define SNAKE_POINT_SIZE    (8)

/* if less than 2 equal 1, more than 16 equal 16 */
#define SNAKE_START_LEN     (4)     

typedef struct _snake_part {
    uint32_t dir;
    uint32_t flags;
    lv_obj_t *body;
} snake_part_t;

typedef enum _snake_sm {
    SnakeIdle=0,
    SnakeForceExit=1,
    SnakeDrawOpen,
    SnakeStartAction,
    SnakeGaming,
    SnakeGameOver,
} snake_sm_t;

typedef struct _snake_local {
    snake_sm_t sm;
    logger_t log;
    lv_obj_t *area;
    uint32_t area_w;
    uint32_t area_h;
    
    uint32_t game_state; 
    uint32_t game_speed;

    char sym_left[8];
    char sym_right[8];
    char sym_down[8];
    char sym_up[8];
    
    lv_obj_t *btn_control;
    lv_obj_t *btn_up;
    lv_obj_t *btn_down;
    lv_obj_t *btn_left;
    lv_obj_t *btn_right;
    lv_obj_t *btn_ctrl_label;

    lv_obj_t *game_over_label;
    lv_style_t game_over_label_style;

    lv_obj_t *new_part;
    lv_style_t new_part_style;

    int score;
    lv_obj_t *label_score;
    char label_score_buffer[16];

    uint32_t game_area_w;
    uint32_t game_area_h;
    lv_obj_t *game_area;
    lv_style_t game_area_style;

    uint32_t snake_len;
    lv_style_t snake_body_style;
    snake_part_t snake[256];
} snake_local_t;

#endif /* APP_SNAKE_COMMON_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
