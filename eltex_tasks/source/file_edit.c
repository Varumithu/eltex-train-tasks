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

void print_text(file_manager_info info, struct dirent** namelist, size_t n) {
    werase(info.wnd[info.active_window]);



    for (int i = info.line_first; i < info.selection; ++i) {
        wmove(info.wnd[info.active_window],
              i - info.line_first + info.top_padding, 1);
        wprintw(info.wnd[info.active_window],
                "%s", namelist[i]->d_name);
    }

    wattron(info.wnd[info.active_window], A_BOLD);
    wmove(info.wnd[info.active_window],
            info.selection - info.line_first + info.top_padding, 1);

    wprintw(info.wnd[info.active_window],
            "%s", namelist[info.selection]->d_name);

    wattroff(info.wnd[info.active_window], A_BOLD);

    for (int i = info.selection + 1; i <= info.line_last; ++i) {
        wmove(info.wnd[info.active_window],
              i - info.line_first + info.top_padding, 1);

        wprintw(info.wnd[info.active_window],
                "%s", namelist[i]->d_name);
    }

    box(info.wnd[info.active_window], '|', '-');
    wrefresh(info.wnd[info.active_window]);

}

void scroll_if_necessary(file_manager_info info) {
    //TODO - scroll screen if selection goes off screen
}

void file_manager_loop(file_manager_info info) {
    struct dirent **namelist;
    int n;

    n = scandir(".", &namelist, NULL, alphasort);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    info.selection = 3;

    if (n > info.screen_height) {
        info.line_last = info.screen_height - info.bottom_padding - info.top_padding - 1;
    }
    else {
        info.line_last = n - 1;
    }

    print_text(info, namelist, (size_t)n);

    while(1) {
        int ch = getch();
        if (ch == 'q') {
            werase(info.wnd[info.active_window]);
            box(info.wnd[info.active_window], '|', '-');
            wrefresh(info.wnd[info.active_window]);
            break;
        }
        else if (ch == KEY_DOWN | ch == 'j') {
            if (info.selection < info.line_last) {
                ++info.selection;
                scroll_if_necessary(info);
                print_text(info, namelist, (size_t)n);
            }
        }
        else if (ch == KEY_UP | ch == 'k') {
            if (info.selection > info.line_first) {
                --info.selection;
                scroll_if_necessary(info);
                print_text(info, namelist, (size_t)n);
            }
        }
    }

    for (size_t i = 0; i < (size_t)n; ++i) {
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
                              stdscr_size_x, stdscr_size_y,
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
