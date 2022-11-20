#include "Tetris.h"
#include <argp.h>

const char *argp_program_version = "Tetris by zaterval, Version == 1.0.0";
const char *argp_program_bug_address = "<8203200527@csu.edu.cn>";
static char doc[] = "LinuxTetris\nA Tetris game using c in linux terminal.\n";
static char args_doc[] = "[Username]...";
static struct argp_option options[] = {
    // long name, short name, value flag, optional flag, doc
    // OPTION_ARG_OPTIONAL
    { "width",      'w', "value", OPTION_ARG_OPTIONAL, "Window width, should be int, default 30."},
    { "height",     'e', "value", OPTION_ARG_OPTIONAL, "Window height, should be int, default 50."},
    { "boldness",   'b', "value", OPTION_ARG_OPTIONAL, "Border boldness."},
    { "gamemode",   'g', 0, 0, "Select gamemode, NORMAL:0, HARD:1, HELL:2."},
    { "color",      'c', 0, 0, "Enable color mode."},
    { "history",    'i', 0, 0, "Nothing."},
    { 0 } 
};

struct arguments program_args = {20, 20, 1, NORMAL, false, false};
static error_t parse_opt(int key, char *arg, struct argp_state *state);
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

struct winsize win_size_;
struct Tetris config_;
struct Tetris_info info_;
struct Tetris_status status_;

int main(int argc, char *argv[]){
    argp_parse(&argp, argc, argv, 0, 0, &program_args);
    printf("Start process....\n");

    init_game_configs(&program_args);

    // for (int i = 0; i < 3; i++)
    // {
    //     gui_update_next_block(i);
    //     sleep(1);
    // }

    status_listen_for_start();

    gui_exit();
    printf("End process....\n");
    return 0;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    int temp;
    switch (key) {
        case 'w': 
            temp = atoi(arg);
            if(temp >= 20 && temp <= 60){
                arguments->width = temp;
            }else{
                printf("Invalid width!\n");
            }
            break;
        case 'e':     
            temp = atoi(arg);
            if(temp >= arguments->width && temp <= 60){
                arguments->height = temp;
            }else{
                printf("Invalid height!\n");
            }
            break;
        case 'b':     
            temp = atoi(arg);
            if(temp > 0 && temp <= 3){
                arguments->boldness = temp;
            }else{
                printf("Invalid boldness!\n");
            }
            break;
        case 'g':
            temp = atoi(arg);
            if(temp >= 0 && temp <= 2){
                arguments->mode = temp;
            }else{
                printf("Invalid mode!\n");
            }
            break;
        case 'i': arguments->load_history = true; break;
        case 'c': arguments->using_color = true; break;
        case ARGP_KEY_ARG: return 0;
        default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}