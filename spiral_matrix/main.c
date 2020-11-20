#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "print_matrix.h"

int main()
{
    size_t size = 5;
    int* parray = (int*)malloc(size * size * sizeof(int));
    if (parray == NULL) {
        perror("failed to allocate memory");
        exit(1);
    }
    print_matrix_int(parray, size, size);
    size_t count = 0, size_sq = size * size;
    int64_t right_limit = size, left_limit = -1, down_limit = size, up_limit = 0, x = 0, y = 0;
    uint8_t state = 0; //0 - go right, 1 - go down, 2 - go left, 3 - go up

    while(count < size_sq) {
        switch(state){
        case 0: //going right
            for (; x < right_limit; ++x) {
                parray[y * size + x] = count + 1;
                ++count;
            }
            --right_limit; // new right limit for next turn of the spiral is one lower
            --x; // need to decrement x because for loop increments it one time too many
            ++y; // to avoid assigning same element in the corner twice
            state = 1; //after going right we turn down
            break;
        case 1: // going down
            for (; y < down_limit; ++y) {
                parray[y * size + x] = count + 1;
                ++count;
            }
            --down_limit;
            --y;
            --x;
            state = 2; //after going down we turn left
            break;
        case 2: // going left
            for (; x > left_limit; --x) {
                parray[y * size + x] = count + 1;
                ++count;
            }
            ++left_limit;
            ++x;
            --y;
            state = 3; //after going down turn up
            break;
        case 3: // going up
            for (; y > up_limit; --y) {
                parray[y * size + x] = count + 1;
                ++count;
            }
            ++up_limit;
            ++y;
            ++x;
            state = 0; //after going up turn right
            break;
        }

    }
    print_matrix_int(parray, size, size);
    free(parray);    return 0;
}
