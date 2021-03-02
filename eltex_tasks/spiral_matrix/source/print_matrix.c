#include "print_matrix.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int max_abs_element(int* parray, size_t width, size_t height) {
    unsigned int max = (unsigned int)abs(parray[0]);
    size_t size = width * height, max_i = 0;
    for (size_t i = 0; i < size; ++i) {
        if ((unsigned int)abs(parray[i]) > max) {
            max = (unsigned int)abs(parray[i]);
            max_i = i;
        }
    }
    return parray[max_i];
}

size_t number_length(int a) {
    if (0 == a) {
        return 1;
    }
    else if (a < 0) {
        return (size_t)floor(log10(abs(a))) + 2;
    }
    else {
        return (size_t)floor(log10(abs(a))) + 1;
    }
}

size_t max_element_length(int* parray, size_t size) {
    size_t res = 1;
    for (size_t i = 0; i < size; ++i) {
        size_t length = number_length(parray[i]);
        if (length > res) {
            res = length;
        }
    }
    return res;
}

void print_matrix_int(int* parray, size_t width, size_t height) {

    size_t max_number_length = max_element_length(parray, width * height), num_length = 0;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {

            num_length = (size_t)printf("%d", parray[i * width + j]);
            for (size_t k = 0; k < max_number_length - num_length + 1; ++k) {
                printf(" ");
            }

        }
        printf("\n");
    }
    printf("\n");
}
