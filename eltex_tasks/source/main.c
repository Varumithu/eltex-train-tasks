#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "spiral_matrix.h"
#include "print_matrix.h"
#include "diagonal_matrix.h"
#include "modifying_bits.h"
#include "phonebook.h"
#include "struct_task.h"
#include "file_edit.h"
#include "shell.h"
#include "threads.h"
#include "chat.h"

void test_print_with_negatievs() {
    int array[] = {10, 100, 90, -101, -505, 780, 1, 2, 3};
    print_matrix_int((int*)array, 3, 3);
}

void test_1() {
    size_t size = 5;
    int* parray = malloc(size * size * sizeof(int));
    if (!parray) {
        perror("memory allocation");
        exit(1);
    }
    fill_spiral_matrix(parray, 5);
    print_matrix_int(parray, 5, 5);
    memset(parray, 0, size * size * sizeof(int));

    fill_diagonal_matrix(parray, 5);
    print_matrix_int(parray, size, size);
    free(parray);
}

void test_bytes_getter_s() {
    printf("%hhX\n", get_byte_shift(0x000000FF, 0));
    printf("%hhX\n", get_byte_shift(0x0000FF00, 1));
    printf("%hhX\n", get_byte_shift(0x00FF00FF, 2));
    printf("%hhX\n", get_byte_shift(0xFF0000FF, 3));
    printf("%hhX\n", get_byte_shift(0x000000FF, 3));
    printf("%hhX\n", get_byte_shift(0x000000FF, 2));
    printf("%hhX\n\n", get_byte_shift(0x000000FF, 1));

    printf("%hhX\n", get_byte_shift(0xABCDEF19, 0));
    printf("%hhX\n", get_byte_shift(0xABCDEF19, 1));
    printf("%hhX\n", get_byte_shift(0xABCDEF19, 2));
    printf("%hhX\n", get_byte_shift(0xABCDEF19, 3));

}

void test_byte_getter_p() {
    printf("%hhX\n", get_byte_pointer(0x000000FF, 0));
    printf("%hhX\n", get_byte_pointer(0x0000FF00, 1));
    printf("%hhX\n", get_byte_pointer(0x00FF00FF, 2));
    printf("%hhX\n", get_byte_pointer(0xFF0000FF, 3));
    printf("%hhX\n", get_byte_pointer(0x000000FF, 3));
    printf("%hhX\n", get_byte_pointer(0x000000FF, 2));
    printf("%hhX\n\n", get_byte_pointer(0x000000FF, 1));

    printf("%hhX\n", get_byte_pointer(0xABCDEF19, 0));
    printf("%hhX\n", get_byte_pointer(0xABCDEF19, 1));
    printf("%hhX\n", get_byte_pointer(0xABCDEF19, 2));
    printf("%hhX\n", get_byte_pointer(0xABCDEF19, 3));

}

void test_byte_edit() {
    unsigned int value = 0xABCDEF12;
    for (size_t i = 0; i < 4; ++i) {
        printf("%hhX ", get_byte_shift(value, i));
    }
    printf("\n");
    printf("%hhX %hhX %hhX %hhX\n",
           get_byte_shift(edit_byte_shift(value, 0x34, 0), 0),
           get_byte_shift(edit_byte_shift(value, 0x56, 1), 1),
           get_byte_shift(edit_byte_shift(value, 0x78, 2), 2),
           get_byte_shift(edit_byte_shift(value, 0x9A, 3), 3));
}

void test_pb() {
    phonebook* pb = phonebook_create();
    phonebook_entry* pentry = phonebook_add_entry(pb, "Vladimir", "Bachurin", "+7 992 999 99 99");
    phonebook_print_entry(pentry);
    phonebook_delete(pb);

    phonebook_interactive(NULL);
}

void struct_task() {
    char structure[] = {52, 0, 0, 0, 222, 0, 0, 0}; // first byte of int is the least significant one

    struct test_struct_t* test_struct = (struct test_struct_t*)structure;
    printf("%d, %d\n", test_struct->a, test_struct->i);
}

void chat_launcher() {
    int choice = 0;
    printf("1 to launch server, 2 to launch client\n");
    scanf("%d", &choice);
    scanf("%*c");
    if (choice == 1) {
        chat_server();
    }
    else if (choice == 2) {
        chat_client();
    }
}

int main()
{
//    test_print_with_negatievs();
//    test_1();
//    test_bytes_getter_s();
//    test_byte_getter_p();
//    test_byte_edit();
//    test_pb();
//    struct_task();
//    file_task_1();
//    file_manager();
//    fork_task();
//    shell_start();
//    threads_task_1();
//    shop_task();
//    err_str_test1();
    chat_launcher();
    return 0;
}
