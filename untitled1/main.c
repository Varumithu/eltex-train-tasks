#include <stdio.h>
#include <stdlib.h>

#include "print_matrix.h"

int main()
{
    size_t size = 5;
    int* parray = (int*)malloc(size * size * sizeof(int));
    for (size_t i = 0; i < size * size; ++i) {
        parray[i] = 0;
    }
    print_matrix_int(parray, size, size);
    printf("\n");
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = size - 1 - i; j < size; ++j) {
            parray[i * size + j] = 1;
        }
    }
    print_matrix_int(parray, size, size);
    return 0;
}
