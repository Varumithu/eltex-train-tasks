#include <stdio.h>

#include "print_matrix.h"

int main()
{
    int test[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    print_matrix_int(&test[0][0], 3, 3);
    return 0;
}
