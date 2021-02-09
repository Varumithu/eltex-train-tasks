#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

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
