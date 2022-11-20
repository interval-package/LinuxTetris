#ifndef TETRIS_H
#define TETRIS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "errno.h"

#include <sys/ioctl.h>
#include <termios.h>
#include <curses.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <signal.h>
// #include <linux/input.h>

// ### data truct

#define MAXIMUM_WINDOW_WIDTH 256
#define MAXIMUM_WINDOW_HEIGHT 256

#define DRAW_COLOR_PAIR_BORDER 6
#define DRAW_COLOR_PAIR_DROP_BOX 1
#define DRAW_COLOR_PAIR_CAPTION 3
#define DRAW_COLOR_PAIR_DOC 4
#define DRAW_COLOR_PAIR_BUTTON 5
#define DRAW_COLOR_PAIR 0

#define WIDTH_NEXT 20
#define HEIGHT_NEXT 6
#define WIDTH_DOC 30

#define BLOCK_TYPES 28
#define BUFFER_SIZE 2048

#define UPDATE_NEXT_BLOCK status_.next_block = rand()%BLOCK_TYPES;

enum MODE{ 
    NORMAL, HARD, HELL 
};


struct Tetris
{
    int width;
    int height;
    enum MODE mode;
    bool load_history;
    bool using_color;
};


struct Tetris_info
{
    int pos_x, pos_y;
    int width, height;
    int border_boldness;
    int screen[MAXIMUM_WINDOW_HEIGHT][MAXIMUM_WINDOW_WIDTH];
};


struct Tetris_status
{
    int score;
    enum {
        IN_GAME, 
        STOP_GAME, 
        UNSTART_GAME,
        GAME_OVER
    } game_status;
    int dropping;

    int current_block, next_block;
    int cur_pos_x, cur_pos_y;
};


struct arguments {
    int width;
    int height;
    int boldness;
    enum MODE mode;
    bool load_history;
    bool using_color;
};

// ### Init ###

extern int init_game_configs(struct arguments*);

extern int exit_game();

// ### GUI utils ###

/*
    Init gui, and setup color pairs.
*/
extern int gui_init();
/*
    Exit gui, calls the endwin.
*/
extern int gui_exit();
/*
    Clean the game place.
*/
extern int gui_clean_window();
/*
    Draw every game detail.
*/
extern int gui_update_draw_window();
/*
    Draw the game window frame.
*/
extern int gui_update_draw_frame();
/*
    Draw the docstring for the game.
*/
extern int gui_update_draw_doc();
/*
    Draw the caption for the game.
*/
extern int gui_update_draw_caption();
extern int gui_update_show_log(const char*);

extern int gui_update_next_block(int next);

extern int gui_update_change_cur_block(bool remove, bool tomem);

// dynamically update windows size
extern int gui_update_window_info();

// ### EVENTS ###

extern int init_event_listener();
extern int status_gaming();
extern int status_restart();

#define ACTION_DOWN     0
#define ACTION_LEFT     1
#define ACTION_RIGHT    2
#define ACTION_R        3
#define ACTION_L        4
#define ACTION_END      5

extern int detect_action_validation(int);
extern int detect_collision(int type, int mode, int x, int y);
extern int tranpose(int, int);

extern int detect_line_filled_one(int y);
extern int detect_line_filled_all();

extern void* key_listen_thread(void *);



#endif  // TETRIS_H