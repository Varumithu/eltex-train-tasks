#ifndef MODIFYING_BITS_H_
#define MODIFYING_BITS_H_

#include <stddef.h>
#include <stdint.h>

unsigned int edit_byte_shift(unsigned int value, uint8_t new_byte_value, size_t byte_number);

unsigned int edit_byte_pointer(unsigned int value, uint8_t new_byte_value, size_t byte_number);

uint8_t get_byte_shift(unsigned int value, size_t byte_number);

uint8_t get_byte_pointer(unsigned int value, size_t byte_number);

#endif
