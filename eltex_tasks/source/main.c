#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "spiral_matrix.h"
#include "print_matrix.h"
#include "diagonal_matrix.h"

int main()
{
    size_t size = 5;
    int* parray = malloc(size * size * sizeof(int));
    if (!parray) {
        perror("memory allocation");
        exit(1);
    }
    print_matrix_int(parray, 5, 5);
    fill_spiral_matrix(parray, 5);
    print_matrix_int(parray, 5, 5);
    memset(parray, 0, size * size * sizeof(int));

    fill_diagonal_matrix(parray, 5);
    print_matrix_int(parray, size, size);
    free(parray);
    return 0;
}
