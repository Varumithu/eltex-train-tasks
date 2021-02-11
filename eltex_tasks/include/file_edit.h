#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curses.h>

typedef void (*command_cb)(void* p_params);

typedef struct {
    int top_padding;
    int bottom_padding;
    int selection;
    int line_first;
    int line_last;
    int active_window;
    int screen_height;
    int screen_width;
    WINDOW* wnd[2];
} file_manager_info;

void file_manager();

void file_task_1();

