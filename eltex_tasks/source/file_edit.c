#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include <dirent.h>
#include <string.h>

#include "file_edit.h"


void file_task_1() {
    int fd = open("/home/valmit/Documents/QtProjects/eltex_tasks/"
                  "eltex_tasks/resources/test_file_1.txt", O_RDONLY);
    if (fd < 0) {
        perror("failed to open fie");
        exit(1);
    }

    char buf[256];

    ssize_t count = read(fd, (char*)buf, 256);
    if (count < 0) {
        perror("failed to read fie");
        exit(1);
    }
    if (count < 256)
        buf[count] = '\0';

    printf("%.256s", buf);
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("failed to lseek");
        exit(1);
    }
    off_t cur_pos = 0;
    size_t i = 0;
    for (; i < (size_t)(file_size); ++i) {
        cur_pos = lseek(fd, -(ssize_t)i - 1, SEEK_END);
        count = read(fd, (char*)buf + i, 1);
        if (i + 1 == 256)
            break;
    }
    buf[i] = '\0';
    printf("%.256s\n", buf);
}
void sig_winch(int signo){
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

void print_text(file_manager_info info, struct dirent** namelist) {
    werase(info.wnd[info.active_window]);



    for (int i = info.line_first; i < info.selection; ++i) {
        wmove(info.wnd[info.active_window],
              i - info.line_first + info.top_padding, 1);
        wprintw(info.wnd[info.active_window],
                "%.20s", namelist[i]->d_name);
    }

    wattron(info.wnd[info.active_window], A_BOLD);
    wmove(info.wnd[info.active_window],
            info.selection - info.line_first + info.top_padding, 1);

    wprintw(info.wnd[info.active_window],
            "%.20s", namelist[info.selection]->d_name);

    wattroff(info.wnd[info.active_window], A_BOLD);

    for (int i = info.selection + 1; i <= info.line_last; ++i) {
        wmove(info.wnd[info.active_window],
              i - info.line_first + info.top_padding, 1);

        wprintw(info.wnd[info.active_window],
                "%.20s", namelist[i]->d_name);
    }

    box(info.wnd[info.active_window], '|', '-');
    wrefresh(info.wnd[info.active_window]);

}

struct dirent*** get_namelist(file_manager_info* p_info,
                              struct dirent*** p_namelist) {



//    struct dirent** namelist = *p_namelist;

    for (size_t i = 0; i < (size_t)p_info->n; ++i) {
        free((*p_namelist)[i]);
    }
    free(*p_namelist);

    p_info->n = scandir(".", p_namelist, NULL, alphasort);
    if (p_info->n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    p_info->selection = 1;

    if (p_info->n > p_info->screen_height) {
        p_info->line_last = p_info->screen_height -
                p_info->bottom_padding - p_info->top_padding - 1;
    }
    else {
        p_info->line_last = p_info->n - 1;
    }

    return p_namelist;
}

struct dirent*** switch_directory(file_manager_info* p_info,
                                  struct dirent *** p_namelist) {

    char* path = (char*)malloc(strlen((*p_namelist)[p_info->selection]->d_name) + 3);

    strcpy(path, "./");
    strcat(path, (*p_namelist)[p_info->selection]->d_name);
    if (chdir(path) < 0) {
        return p_namelist;
    }



    get_namelist(p_info, p_namelist);

    print_text(*p_info, *p_namelist);

    return p_namelist;
}


void scroll_if_necessary(file_manager_info* p_info,
                         struct dirent*** p_namelist) {
    int dif = 0;
    if (p_info->selection < p_info->line_first) {
        dif = p_info->line_last - p_info->line_first;
        p_info->line_first = p_info->selection;
        p_info->line_last = p_info->line_first + dif;
        print_text(*p_info, *p_namelist);
    }
    else if (p_info->selection > p_info->line_last) {
        dif = p_info->line_last - p_info->line_first;
        p_info->line_last = p_info->selection;
        p_info->line_first = p_info->line_last - dif;
        print_text(*p_info, *p_namelist);
    }
}

void file_manager_loop(file_manager_info info) {
    struct dirent **namelist = NULL;
    info.n = 0;
    get_namelist(&info, &namelist);
    print_text(info, namelist);

    while(1) {
        int ch = getch();
        if (ch == 'q') {
            werase(info.wnd[info.active_window]);
            box(info.wnd[info.active_window], '|', '-');
            wrefresh(info.wnd[info.active_window]);
            break;
        }
        else if (ch == KEY_DOWN | ch == 'j') {
            if (info.selection < info.n - 1) {
                ++info.selection;
                scroll_if_necessary(&info, &namelist);
                print_text(info, namelist);
            }
        }
        else if (ch == KEY_UP | ch == 'k') {
            if (info.selection > 0) {
                --info.selection;
                scroll_if_necessary(&info, &namelist);
                print_text(info, namelist);
            }
        }
        else if (ch == KEY_ENTER | ch == 'e') {
            switch_directory(&info, &namelist);
        }
    }

    for (size_t i = 0; i < (size_t)info.n; ++i) {
        free(namelist[i]);
    }
    free(namelist);

}

void init_ncurses() {
    initscr();
    signal(SIGWINCH, sig_winch);
    cbreak();
    noecho();
    curs_set(0);
    refresh();
}

void file_manager() {
    init_ncurses();
    WINDOW* wnd[2];
    int stdscr_size_x = 0, stdscr_size_y = 0;
    getmaxyx(stdscr, stdscr_size_x, stdscr_size_y);
    wnd[0] = subwin(stdscr, stdscr_size_x, stdscr_size_y / 2, 0, 0);
    wnd[1] = subwin(stdscr, stdscr_size_x, stdscr_size_y / 2, 0, stdscr_size_y / 2);

    box(wnd[0], '|', '-');
    box(wnd[1], '|', '-');

    file_manager_info info = {1, 1, 0, 0, 0, 0,
                              stdscr_size_x, stdscr_size_y, 0,
                              {wnd[0], wnd[1]}};

    file_manager_loop(info);

    wattron(wnd[0], A_BOLD);
    wmove(wnd[0], 5, 5);
    wprintw(wnd[0], "Hello, brave new curses world!\n");
    wattroff(wnd[0], A_BOLD);
    wattron(wnd[0], A_BLINK);
    wmove(wnd[0], 7, 6);
    wprintw(wnd[0], "Press any key to continue...");
    wrefresh(wnd[0]);
    wrefresh(wnd[1]);
//    refresh();
    getch();
    delwin(wnd[0]);
    delwin(wnd[1]);
    endwin();
}
