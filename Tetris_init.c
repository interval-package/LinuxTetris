#include "Tetris.h"

extern struct winsize win_size_;
extern struct Tetris config_;
extern struct Tetris_info info_;
extern struct Tetris_status status_;

int init_game_configs(struct arguments* tar){
    info_.width = config_.width = tar->width;
    info_.height = config_.height = tar->height;
    info_.border_boldness = tar->boldness;
    config_.mode = tar->mode;
    config_.using_color = tar->using_color;
    
    init_event_listener();
    init_blocks_info();
    gui_init();

    gui_update_show_log("Gui start!");
    return 0;
}

int exit_game(){
    gui_exit();
    exit(0);
    return 0;
}

int blocks[7][4][4][4] = {
    {{
        {0,0,0,0},
        {1,1,1,0},
        {0,1,0,0},
        {0,0,0,0},
        }},
    {{
        {0,0,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,1,1,0},
        }},
    {{
        {0,0,0,0},
        {0,0,1,0},
        {0,0,1,0},
        {0,1,1,0},
        }},
    {{
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        }},
    {{
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        }},
    {{
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        }},
    {{
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0},
        }},
};

int init_blocks_info(){
    for (int i = 0; i < 7; i++){
        for (int j = 1; j < 4; j++){
            for(int m=0; m<4; m++){
                for(int n=0; n<4; n++){
                    blocks[i][j][m][n] = blocks[i][j-1][n][3-m];
                }
            }
        }
    }
    return 0;
}