#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "spiral_matrix.h"
#include "print_matrix.h"
#include "diagonal_matrix.h"

void test_print_with_negatievs() {
    int array[] = {10, 100, 90, -101, -505, 780, 1, 2, 3};
    print_matrix_int(&array[0], 3, 3);
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

int main()
{
    test_print_with_negatievs();
    test_1();
    return 0;
}
