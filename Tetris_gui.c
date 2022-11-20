#include "Tetris.h"

#define DOC_NUM 8


extern struct winsize win_size_;
extern struct Tetris config_;
extern struct Tetris_info info_;
extern struct Tetris_status status_;

static const char* doc_string[] = {
    "space: start or stop.",
    "->:    mv right.",  
    "<-:    mv left.",          
    "s:     speed up.",         
    "q:     exit.",        
    "a:     left span.",           
    "b:     right span.",
    "r:     restart."
};


// ### gui setup ###

int gui_init(){
    initscr();
    if(config_.using_color){
        if(!has_colors()){
            endwin();
            fprintf(stderr,"Error - no color support on this terminal \n");
            exit(1);
        }
        if(start_color() != OK){
            endwin();
            fprintf(stderr,"Error -could not initialize colors\n");
            exit(2);
        }
        init_pair(DRAW_COLOR_PAIR_BORDER,COLOR_BLUE,COLOR_WHITE);
        init_pair(DRAW_COLOR_PAIR_DROP_BOX,COLOR_RED,COLOR_BLACK);
        init_pair(DRAW_COLOR_PAIR_DOC,COLOR_RED,COLOR_BLACK);


        attrset(COLOR_PAIR(DRAW_COLOR_PAIR_DROP_BOX));
    }
    cbreak();       //关闭缓冲
    nonl();
    noecho();       //关闭回显
    intrflush(stdscr,FALSE);
    keypad(stdscr,TRUE);
    curs_set(0);    //光标不可见

    gui_update_draw_window();
    return 0;
}


int gui_exit(){
    endwin();
    return 0;
}


int gui_update_draw_doc(){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_DOC));
    }else{
        attroff(A_REVERSE);
    }
    int base_pos_x = info_.pos_x + info_.border_boldness*3 + info_.width + WIDTH_NEXT;
    int base_pos_y = info_.pos_y + info_.border_boldness;
    for (int i=0; i<DOC_NUM; i++){
        mvprintw(base_pos_y+i, base_pos_x, doc_string[i]);
    }
    refresh();
    return 0;
}


int gui_update_draw_caption(){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_CAPTION));
    }else{
        attroff(A_REVERSE);
    }
    int base_pos_x = info_.pos_x + info_.border_boldness*2 + info_.width;
    int base_pos_y = info_.pos_y + info_.border_boldness*2 + HEIGHT_NEXT;
    mvprintw(base_pos_y, base_pos_x, "your score is: %d", status_.score);
    refresh();
    return 0;
}


int gui_update_draw_frame(){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_BORDER));
    }else{
        attron(A_REVERSE);
    }
    // draw frame

    // vertical border
    for (int i = 0; i < info_.border_boldness*2 + info_.height; i++){
        for (int j = 0; j < info_.border_boldness; j++){
            // right
            mvaddch( info_.pos_y+i, info_.pos_x+j, ' ');
            info_.screen[info_.pos_x+j][info_.pos_y+i] = DRAW_COLOR_PAIR_BORDER;
            // info
            mvaddch( info_.pos_y+i, info_.pos_x+info_.border_boldness+info_.width+j, ' ');
            info_.screen[info_.pos_x+info_.border_boldness+info_.width+j]
            [info_.pos_y+i] = DRAW_COLOR_PAIR_BORDER;
            // doc
            mvaddch( info_.pos_y+i, info_.pos_x+info_.border_boldness*2+info_.width+WIDTH_NEXT+j, ' ');
            info_.screen[info_.pos_x+info_.border_boldness*2+info_.width+WIDTH_NEXT+j]
            [info_.pos_y+i] = DRAW_COLOR_PAIR_BORDER;
            // left
            mvaddch( info_.pos_y+i, info_.pos_x+info_.border_boldness*3+info_.width+WIDTH_NEXT+WIDTH_DOC+j, ' ');
            info_.screen[info_.pos_x+info_.border_boldness*3+info_.width+WIDTH_NEXT+WIDTH_DOC+j]
            [info_.pos_y+i] = DRAW_COLOR_PAIR_BORDER;
        }
    }

    // horizotal border
    for (int i = 0; i < info_.border_boldness*4 + info_.width + WIDTH_NEXT + WIDTH_DOC; i++)
    {
        for (int j = 0; j < info_.border_boldness; j++){
            // top
            mvaddch( info_.pos_y+j, info_.pos_x+i, ' ');
            info_.screen[info_.pos_x+i][info_.pos_y+j]=DRAW_COLOR_PAIR_BORDER;
            // buttom
            mvaddch( info_.pos_y+info_.border_boldness+info_.height+j, info_.pos_x+i, ' ');
            info_.screen[info_.pos_x+i]
            [info_.pos_y+info_.border_boldness+info_.height+j]=DRAW_COLOR_PAIR_BORDER;
        }
    }

    // caption split
    for (int i = 0; i < WIDTH_NEXT; i++)
    {
        for(int j = 0; j < info_.border_boldness; j++)
        {
            mvaddch( info_.pos_y+info_.border_boldness+HEIGHT_NEXT+j, 
            info_.pos_x+info_.border_boldness*2+info_.width+i, ' ');
            info_.screen[info_.pos_x+info_.border_boldness*2+info_.width+i]
            [info_.pos_y+info_.border_boldness+HEIGHT_NEXT+j]=DRAW_COLOR_PAIR_BORDER;
        }
    }
    refresh();
    return 0;
}


int gui_update_draw_window(){
    gui_update_draw_frame();
    gui_update_draw_doc();
    gui_update_draw_caption();
    gui_clean_window();
    return 0;
}


int gui_clean_window(){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_DROP_BOX));
    }else{
        attroff(A_REVERSE);
    }
    for (int i = info_.border_boldness; i < info_.border_boldness + info_.width; i++){
        for (int j = info_.border_boldness; j < info_.border_boldness + info_.height; j++){
            info_.screen[i][j] = 0;
            mvaddch(info_.pos_y+j,info_.pos_x+i,' ');
        }
    }
    refresh();
    return 0;
}


int gui_update_show_log(const char* buffer){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_CAPTION));
    }else{
        attroff(A_REVERSE);
    }
    int base_pos_x = info_.pos_x + info_.border_boldness*2 + info_.width;
    int base_pos_y = info_.pos_y + info_.border_boldness*2 + HEIGHT_NEXT*2;
    mvprintw(base_pos_y, base_pos_x, buffer);
    refresh();
    return 0;
}


int gui_update_window_info(){
    ioctl(STDIN_FILENO,TIOCGWINSZ,&win_size_);
    return 0;
}

int gui_update_next_block(int next){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_DROP_BOX));
    }else{
        attroff(A_REVERSE);
    }
    extern int blocks[7][4][4][4];
    int base_x = info_.width+info_.border_boldness*2+(WIDTH_NEXT/2-2);
    int base_y = info_.border_boldness+(HEIGHT_NEXT/2-2*info_.border_boldness);
    int type=next/4, mode=next%4;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(blocks[type][mode][j][i]){
                attron(A_REVERSE);
            }else{
                attroff(A_REVERSE);
            }
            for (int k = 0; k < info_.border_boldness; k++)
            {
                mvaddch(base_y+j, base_x+i+k, '#');
            }

        }
    }
    refresh();
    return 0;
} 

int gui_update_change_cur_block(bool remove, bool tomem){
    if(config_.using_color){
        attron(COLOR_PAIR(DRAW_COLOR_PAIR_DROP_BOX));
    }

    if(remove){
        attroff(A_REVERSE);
    }else{
        attron(A_REVERSE);
    }
    

    extern int blocks[7][4][4][4];
    
    int x = (status_.cur_pos_x + info_.pos_x + 1) * info_.border_boldness, 
        y = status_.cur_pos_y + info_.pos_y + info_.border_boldness;
    
    int type = status_.current_block/4, 
        mode = status_.current_block%4;
    
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(blocks[type][mode][j][i]){
                for (int k = 0; k < info_.border_boldness; k++){
                    mvaddch(y+j, x+i+k, ' ');
                    if(tomem)info_.screen[x+i+k][y+j] = !remove;
                }
            }
        }
    }
    refresh();
    return 0;
}