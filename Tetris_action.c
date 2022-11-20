#include "Tetris.h"
extern struct winsize win_size_;
extern struct Tetris config_;
extern struct Tetris_info info_;
extern struct Tetris_status status_;

char string_buffer[BUFFER_SIZE];
#define ACTIVE_LOG_KEY(key) sprintf(string_buffer, "current key:%d", key); gui_update_show_log(string_buffer); 
#define ACTIVE_LOG_CONST(s) sprintf(string_buffer, s); gui_update_show_log(string_buffer); 

int init_event_listener(){
    keypad(stdscr,1);
    return 0;
}

int status_listen_for_start(){
    int cur_key;

    status_restart();

    while (true)
    {
        if(cur_key = getch()){
            ACTIVE_LOG_KEY(cur_key)
            switch (cur_key)
            {
                case 'q':
                    return 1;
                    break;
                
                case 'r':
                    status_restart();
                    break;

                case ' ':
                    status_.game_status = status_gaming();
                    break;

                default:
                    break;
            }

            if (status_.game_status == GAME_OVER){
                status_restart();
                ACTIVE_LOG_CONST("SELECT NEW GAME!")
            }
            
        }
    }
    return 0;
}


static int gaming_key;
static int exit_flag;
static int speed_up_flag;
pthread_mutex_t drawer_mutex;
#define DRAWER_MOVE_LOCK(phrase)    if(pthread_mutex_lock(&drawer_mutex) != 0)perror("Mutext fail!");else{ \
                                    gui_update_change_cur_block(true, false); \
                                    phrase; \
                                    gui_update_change_cur_block(false, false); \
                                    pthread_mutex_unlock(&drawer_mutex);}

void* key_listen_thread(void *arg){
    while (true){
        gaming_key = getch();
        if(exit_flag)break;
        ACTIVE_LOG_KEY(gaming_key)
        switch (gaming_key){
            case ' ':
                gaming_key = -1;
                ACTIVE_LOG_CONST("  #####STOP#####  ")
                pthread_mutex_destroy(&drawer_mutex);
                return STOP_GAME;
                break;
            case KEY_LEFT:
                if(!detect_action_validation(ACTION_LEFT)){
                    DRAWER_MOVE_LOCK(tranpose(0,-1))
                }
                break;
            case KEY_RIGHT:
                if(!detect_action_validation(ACTION_RIGHT)){
                    DRAWER_MOVE_LOCK(tranpose(0,1))
                }
                break;
            case 'a':
                if(!detect_action_validation(ACTION_L)){
                    DRAWER_MOVE_LOCK(tranpose(-1,0))
                }
                break;
            case 'd':
                if(!detect_action_validation(ACTION_R)){
                    DRAWER_MOVE_LOCK(tranpose(1,0))
                }
                break;
            default:
                break;
        }
    }
    exit_flag = false;
    return -2;
}

int status_gaming(){
    gaming_key = 1;
    status_.game_status = IN_GAME;
    int cur_key = 1; 
    exit_flag = false;
    int err;
    pthread_t thread;
    err = pthread_create(&thread, NULL, key_listen_thread, (void *)&gaming_key);
    if(err){
        ACTIVE_LOG_CONST("Thread fail!")
        sleep(3);
        perror("Thread fail!");
        gui_exit();
        exit(-1);
    }
    if(pthread_mutex_init(&drawer_mutex, NULL) != 0){
        ACTIVE_LOG_CONST("Mutex fail!");
        sleep(3);
        perror("Mutex fail!");
        gui_exit;
        exit(-1);
    }

    while (true){
        cur_key = gaming_key;
        if(cur_key == -1){
            return STOP_GAME;
        }

        if(status_.dropping){
            if (detect_action_validation(ACTION_DOWN)){
                gui_update_change_cur_block(false, true);
                status_.dropping = false;
                detect_line_filled_all();
                sleep(1);
                continue;
            }else{
                DRAWER_MOVE_LOCK(status_.cur_pos_y += 1)
            }
        }else{
            // new block
            status_.current_block = status_.next_block;
            status_.cur_pos_x = info_.width/2 - 2*info_.border_boldness;
            status_.cur_pos_y = 0;
            status_.dropping = true;

            UPDATE_NEXT_BLOCK
            gui_update_next_block(status_.next_block);

            if (detect_action_validation(ACTION_END)){

                pthread_mutex_destroy(&drawer_mutex);
                ACTIVE_LOG_CONST("Press any key.");
                exit_flag = true;
                while (exit_flag){
                    sleep(1);
                }
                return GAME_OVER;
            }

            gui_update_change_cur_block(false, false);
            // cancel the speed up
            // cur_key = cur_key=='s'?0:cur_key;
            continue;
        }

        if(cur_key == 's'){
            speed_up_flag = !speed_up_flag;
            cur_key = 't';
        }

        if(speed_up_flag){
            // ACTIVE_LOG_CONST("Speed up");
            usleep(10000);
        }else{
            usleep(150000);
        }
    }
}

int status_restart(){
    status_.current_block = rand()%BLOCK_TYPES;
    UPDATE_NEXT_BLOCK

    gui_update_next_block(status_.current_block);
    speed_up_flag=false;
    status_.game_status = UNSTART_GAME;
    status_.dropping = false;
    gui_update_draw_window();
    // gui_clean_window();
}

int detect_action_validation(int action){
    int type = status_.current_block/4, mode = status_.current_block%4;

    int x = status_.cur_pos_x + info_.pos_x + info_.border_boldness, 
        y = status_.cur_pos_y+info_.pos_y + info_.border_boldness;
    switch (action)
    {
        case ACTION_DOWN:
            y++;
            return -1*detect_collision(type, mode, x, y);
        case ACTION_LEFT:
            x--;
            return detect_collision(type, mode, x, y);
        case ACTION_RIGHT:
            x++;
            return detect_collision(type, mode, x, y);
        case ACTION_R:
            mode = (5+mode)%4;
            return detect_collision(type, mode, x, y);
        case ACTION_L:
            mode = (3+mode)%4;
            return detect_collision(type, mode, x, y);
        case ACTION_END:
            return detect_collision(type, mode, x, y);
        default:
            break;
    }
    return 0;
}

extern int detect_collision(int type, int mode, int x, int y){
    extern int blocks[7][4][4][4];
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if (blocks[type][mode][j][i] && info_.screen[x+i][y+j])
            {
                return 1;
            }
        }
    }
    return 0;
}

int tranpose(int rotate, int move){ 
    int type = status_.current_block/4, mode = status_.current_block%4;
    status_.cur_pos_x += move;
    mode = (4+rotate+mode)%4;
    status_.current_block = type*4 + mode;
    return 0;
}

int detect_line_filled_one(int y){
    if(y >= info_.height)return 0;
    y = info_.border_boldness + y + info_.pos_y;
    int base_x = info_.border_boldness + info_.pos_x;
    for(int i=base_x; i<base_x + info_.width*info_.border_boldness; i++){
        if(!info_.screen[i][y])return 0;
    }
    return 1;
}

int detect_line_filled_all(){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_DROP_BOX));
    }

    int base_x = info_.border_boldness;
    for(int i=0; i<4; i++){
        if(detect_line_filled_one(status_.cur_pos_y+i)){
            int last_y = status_.cur_pos_y+ i + info_.border_boldness + info_.pos_y;
            for(int j=last_y; j > info_.border_boldness; j--){
                for(int k=base_x; k < base_x + info_.width*info_.border_boldness; k++){
                    info_.screen[k][j] = info_.screen[k][j-1];
                    if(info_.screen[k][j]){
                        attron(A_REVERSE);
                        // mvaddch(j,k,'#');
                    }else{
                        attroff(A_REVERSE);
                        // mvaddch(j,k,'@');
                    }
                    mvaddch(j,k,' ');
                }
            }
            status_.score += 1;
            gui_update_draw_caption();
        }
    }
    refresh();
    return 0;
}