#include "game.h"
#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


void move_cursor(struct game* st, int dx, int dy) {
		st->cursor_x += dx;
    	st->cursor_y += dy;
    	if(st->cursor_x < 0)        
			st->cursor_x = 0;
    	if(st->cursor_x >= BOARD_W) 
			st->cursor_x = BOARD_W - 1;
    	if(st->cursor_y < 0)        
			st->cursor_y = 0;
    	if(st->cursor_y >= BOARD_H) 
			st->cursor_y = BOARD_H - 1;
}

void place_mines(struct game* st, int safe_x, int safe_y) {
    	const int SAFE_RADIUS = 1;
    	int safe_cells = (2 * SAFE_RADIUS + 1) * (2 * SAFE_RADIUS + 1);
    	int available  = BOARD_W * BOARD_H - safe_cells;

    	int radius = (NUM_MINES <= available) ? SAFE_RADIUS : 0;

    	int placed = 0;
    	while(placed < NUM_MINES) {
        	int rx = rand() % BOARD_W;
        	int ry = rand() % BOARD_H;
        	if(abs(rx - safe_x) <= radius && abs(ry - safe_y) <= radius) 
			continue;
        	if(st->board[ry][rx].is_mine) 
			continue;
        	st->board[ry][rx].is_mine = true;
        	placed++;
    	}
    	for(int y = 0; y < BOARD_H; y++) {
        	for(int x = 0; x < BOARD_W; x++) {
            		if(st->board[y][x].is_mine) 
				continue;
            		int count = 0;
            		for(int dy = -1; dy <= 1; dy++) {
                		for(int dx = -1; dx <= 1; dx++) {
                    			int nx = x + dx, ny = y + dy;
                    			if(nx >= 0 && nx < BOARD_W && ny >= 0 && ny < BOARD_H) {
                        			if(st->board[ny][nx].is_mine) 
							count++;
					}
                		}
			}
            		st->board[y][x].neighbor_mines = count;
        	}
    	}
}

void reveal_cell(struct game* st, int x, int y) {
    	if(x < 0 || x >= BOARD_W || y < 0 || y >= BOARD_H) 
		return;
    	if(st->board[y][x].is_revealed || st->board[y][x].is_flagged) 
		return;

    	st->board[y][x].is_revealed = true;

    	if(st->board[y][x].is_mine) {
        	st->game_state = -1;
        	return;
    	}
    	if(st->board[y][x].neighbor_mines == 0) {
        	for(int dy = -1; dy <= 1; dy++) {
            		for(int dx = -1; dx <= 1; dx++) {
                		reveal_cell(st, x + dx, y + dy);
			}
		}
    	}

    	int unrevealed = 0;
    	for(int a = 0; a < BOARD_H; a++) {
        	for(int b = 0; b < BOARD_W; b++) {
            		if(!st->board[a][b].is_revealed) 
				unrevealed++;
		}
	}
    	if(unrevealed == NUM_MINES)
        	st->game_state = 1;
}

static void reset_game(struct game* st) {
    	memset(st, 0, sizeof(struct game));
    	st->cursor_x = BOARD_W / 2;
    	st->cursor_y = BOARD_H / 2;
}


void* init_game() {
    	struct game* st = calloc(1, sizeof(struct game));
    	st->cursor_x = BOARD_W / 2;
    	st->cursor_y = BOARD_H / 2;
    	return st;
}

void show_bar(int x, int y) {
	for(int a = 22; a > 2; a--) {
		set_color_cell('+', x - a, y - 16, COLOR_CYAN, COLOR_BLACK);
		set_color_cell('+', x - a, y - 2, COLOR_CYAN, COLOR_BLACK);
	}
	for(int a = 15; a > 2; a--) {
                set_color_cell('|', x - 22, y - a, COLOR_CYAN, COLOR_BLACK);
        	set_color_cell('|', x - 3, y - a, COLOR_CYAN, COLOR_BLACK);
        }
        set_message("USE:", x - 14, y - 14);
        set_message("Move:    Arrows", x - 20, y - 12);
        set_message("Open:    Space", x - 20, y - 10);
       	set_message("Flag:    Press F", x - 20, y - 8);
        set_message("Quit:    ESC", x - 20, y - 6);
	set_message("Restart: Press R", x - 20, y - 4);
}

void show_title(int x, int y) {
    	const char* title[] = {
        	"#   #  ###  #   #  #####  #####  #   #  #####  #####  #####  #####  #####",
    		"## ##   #   ##  #  #      #      #   #  #      #      #   #  #      #   #",
        	"# # #   #   # # #  ###    #####  # # #  ###    ###    #####  ###    #####",
        	"#   #   #   #  ##  #          #  ## ##  #      #      #      #      #  # ",
        	"#   #  ###  #   #  #####  #####  #   #  #####  #####  #      #####  #   #"
    	};

    	for(int a = 0; a < 5; a++) {
        	int len = strlen(title[a]);
        	for(int b = 0; b < len; b++) {
            		if(title[a][b] == '#') {
                		set_color_cell('#', x + 4 + b, y - 11 + a, COLOR_YELLOW, COLOR_BLACK);
            		}	
        	}
    	}
}

void show_lose(int x, int y) {
	const char* title[] = {
		"#   #   #####   #     #      #       #####    #####  #####",
                " # #   #     #  #     #      #      #     #   #        #  ",
                "  #    #     #  #     #      #      #     #   #####    #  ",
                "  #    #     #  #     #      #      #     #       #    #  ",
                "  #     #####    #####       #####   #####    #####    #  "
        };

	for(int a = 0; a < 5; a++) {
                int len = strlen(title[a]);
                for(int b = 0; b < len; b++) {
                        if(title[a][b] == '#') {
                                set_color_cell('#', x + 4 + b, y - 11 + a, COLOR_RED, COLOR_BLACK);
                        }
                }
        }
}

void show_win(int x, int y) {
	const char* title[] = {
                "#   #  #####  #   #    #     #  ###  #   #",
                " # #   #   #  #   #    #     #   #   ##  #",
                "  #    #   #  #   #    #  #  #   #   # # #",
                "  #    #   #  #   #     # # #    #   #  ##",
                "  #    #####  #####      # #    ###  #   #"
        };
        
	for(int a = 0; a < 5; a++) {
                int len = strlen(title[a]);
                for(int b = 0; b < len; b++) {
                        if(title[a][b] == '#') {
                                set_color_cell('#', x + 4 + b, y - 11 + a, COLOR_GREEN, COLOR_BLACK);
                        }
                }
        }
}

static short num_color(int n) {
    	switch(n) {
        	case 1: return COLOR_BLUE;
        	case 2: return COLOR_GREEN;
        	case 3: return COLOR_RED;
        	case 4: return COLOR_MAGENTA;
        	case 5: return COLOR_CYAN;
        	case 6: return COLOR_BLACK;
        	case 7: return COLOR_WHITE;
        	default: return COLOR_WHITE;
    	}
}

static void draw_game(struct game* st, struct event* event) {
    	clear_screen();

    	int box_w = BOARD_W * 2 + 2;
    	int start_x = (event->width - box_w) / 2;
    	int start_y = (event->height - (BOARD_H + 6)) / 2;
    	if(start_x < 0) 
		start_x = 0;
    	if(start_y < 0) 
		start_y = 0;
	
	int end_x = start_x + 1;
    	for(int a = 1; a < box_w - 1; a++) {
        	set_cell('-', start_x + a, start_y);
		end_x++;
	}
	set_cell('+', start_x, start_y);
	set_cell('+', end_x, start_y);

    	char info[80];
    	int info_len = sprintf(info, " Mines: %02d      Flags: %02d     Time: %03ld ", NUM_MINES, st->flags_placed, st->elapsed_time);

    	set_cell('|', start_x, start_y + 1);
    	
	for(int a = 0; a < box_w - 2; a++) {
        	char c = (a < info_len) ? info[a] : ' ';
        	set_color_cell(c, start_x + 1 + a, start_y + 1, COLOR_YELLOW, COLOR_BLACK);
    	}
    	set_cell('|', start_x + box_w - 1, start_y + 1);
    	set_cell('|', start_x, start_y + 2);

    	for(int a = 1; a < box_w - 1; a++) {
        	set_cell('-', start_x + a, start_y + 2);
	}
    	set_cell('|', start_x + box_w - 1, start_y + 2);

    	for(int y = 0; y < BOARD_H; y++) {
        	int draw_y = start_y + 3 + y;
        	set_cell('|', start_x, draw_y);

        	for(int x = 0; x < BOARD_W; x++) {
            		int draw_x = start_x + 1 + x * 2;
            		char c  = ' ';
            		short fg = COLOR_WHITE;
            		short bg = COLOR_BLACK;

            		bool show_mine = st->game_state == -1 && st->board[y][x].is_mine && !st->board[y][x].is_flagged;

            		if(st->board[y][x].is_revealed || show_mine) {
                		if(st->board[y][x].is_mine) {
                    			c = '*';
                    			fg = COLOR_WHITE;
                    			bg = COLOR_RED;
                		} 
				else if(st->board[y][x].neighbor_mines > 0) {
                    			c = '0' + st->board[y][x].neighbor_mines;
                    			fg = num_color(st->board[y][x].neighbor_mines);
                    			bg = COLOR_WHITE;
                		} 
				else
                    			bg = COLOR_WHITE;
            		} 
			else if(st->board[y][x].is_flagged) {
                		c = 'F';
                		fg = COLOR_RED;
            		} 
			else {
                		c = '#';
                		fg = COLOR_CYAN;
            		}

            		if(x == st->cursor_x && y == st->cursor_y) {
                		bg = COLOR_YELLOW;
                		fg = COLOR_BLACK;
            		}
            		set_color_cell(c, draw_x, draw_y, fg, bg);
            		set_color_cell(' ', draw_x + 1, draw_y, fg, bg);
        	}
        	set_cell('|', start_x + box_w - 1, draw_y);
    	}
    	
	int bot_y = start_y + 3 + BOARD_H;
	set_cell('+', start_x, bot_y);
    	
	for(int a = 1; a < box_w - 1; a++) {
		set_cell('-', start_x + a, bot_y);
	}
	set_cell('+', end_x, bot_y);

    	int msg_y = bot_y + 1;
    	if(msg_y < event->height) {
        	if(st->game_state == 1) {
			show_win(start_x - 4, msg_y + 15);
			set_message("Press 'R' to restart or ESC to escape", start_x + 2, msg_y + 10);
			show_bar(start_x, msg_y);
			show_title(start_x - 17, msg_y - 15);
		}
        	else if(st->game_state == -1) {
            		show_lose(start_x - 12, msg_y + 15);
			set_message("Press 'R' to restart or ESC to escape", start_x + 2, msg_y + 10);
			show_bar(start_x, msg_y);
			show_title(start_x - 17, msg_y - 15);
		}
        	else {
			show_bar(start_x, msg_y);
			show_title(start_x - 17, msg_y - 15);
		}
	}
}


int game_event(struct event* event, void* game) {
    	struct game* st = (struct game*)game;

    	if(event->type == EVENT_START) {
        	game_speed(500);
        	draw_game(st, event);
        	return 0;
    	}

    	if(event->type == EVENT_ESC)
        	return 1;

    	if(st->game_state == 0 && st->first_click_done)
        	st->elapsed_time = (long)(time(NULL) - st->start_time);

    	if(event->type == EVENT_KEY) {
        	if(st->game_state == 0) {
            		if(event->key == KEY_UP)    
				move_cursor(st, 0, -1);
            		if(event->key == KEY_DOWN)  
				move_cursor(st, 0, 1);
            		if(event->key == KEY_LEFT)  
				move_cursor(st, -1, 0);
            		if(event->key == KEY_RIGHT) 
				move_cursor(st, 1, 0);

            		if(event->key == 'f' || event->key == 'F') {
                		Cell* cell = &st->board[st->cursor_y][st->cursor_x];
                		if(!cell->is_revealed) {
                    			cell->is_flagged = !cell->is_flagged;
                    			st->flags_placed += cell->is_flagged ? 1 : -1;
                		}
            		}

            		if(event->key == '\n' || event->key == ' ' || event->key == KEY_ENTER) {
                		if(!st->first_click_done) {
                    			place_mines(st, st->cursor_x, st->cursor_y);
                    			st->first_click_done = true;
                    			st->start_time = time(NULL);
                		}
                		reveal_cell(st, st->cursor_x, st->cursor_y);
            		}
        	}

        	if(event->key == 'r' || event->key == 'R')
            		reset_game(st);
    	}
    	draw_game(st, event);
    	return 0;
}
