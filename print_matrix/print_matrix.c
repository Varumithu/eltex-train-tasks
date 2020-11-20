#include "print_matrix.h"

#include <stdio.h>

void print_matrix_int(int* parray, size_t width, size_t height) {
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            printf("%d ", parray[i * width + j]);
        }
        printf("\n");
    }
    printf("\n");
}
