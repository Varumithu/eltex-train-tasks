#include "modifying_bits.h"

#include <stdio.h>

unsigned int edit_byte_shift(unsigned int value, uint8_t new_byte_value, size_t byte_number) {

    unsigned int byte_eraser = (unsigned int)((int)0xFFFFFF00 << (byte_number * 8)) ;
    value &= byte_eraser;
    value |= (unsigned int)new_byte_value << (byte_number * 8);
    return value;
}

unsigned int edit_byte_pointer(unsigned int value, uint8_t new_byte_value, size_t byte_number) {

    return value;
}

uint8_t get_byte_shift(unsigned int value, size_t byte_number) {
    return (uint8_t)(value >> (byte_number * 8) & 0x000000FF);
}

uint8_t get_byte_pointer(unsigned int value, size_t byte_number) {
    return *((uint8_t*)(&value) + byte_number);
}
