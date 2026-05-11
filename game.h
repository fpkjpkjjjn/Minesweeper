#ifndef _GAME_H_INCLUDE_
#define _GAME_H_INCLUDE_

#include "world.h"
#include <stdbool.h>
#include <time.h>   /* FIX: нужен для time_t */

/* Размеры поля и количество мин */
#define BOARD_W   20
#define BOARD_H   15
#define NUM_MINES 45

/* Минимальный размер терминала для корректной отрисовки */
#define MIN_TERM_W (BOARD_W * 2 + 2)
#define MIN_TERM_H (BOARD_H + 6)

/* Структура для одной клетки поля */
typedef struct {
    bool is_mine;
    bool is_revealed;
    bool is_flagged;
    int  neighbor_mines;
} Cell;

/* Структура общего состояния игры */
struct game {
    Cell   board[BOARD_H][BOARD_W];
    int    cursor_x;
    int    cursor_y;
    int    flags_placed;
    bool   first_click_done;
    int    game_state;      /* 0 = играем, 1 = победа, -1 = поражение */
    time_t start_time;      /* FIX: time_t вместо long */
    long   elapsed_time;
};

void* init_game();
int   game_event(struct event* event, void* game);

#endif
