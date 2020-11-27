#include "diagonal_matrix.h"

void fill_diagonal_matrix(int* parray, size_t size)
{

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = size - 1 - i; j < size; ++j) {
            parray[i * size + j] = 1;
        }
    }

}
