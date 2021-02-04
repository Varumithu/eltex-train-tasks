#ifndef _INCLUDE_PHONEBOOK_H_
#define _INCLUDE_PHONEBOOK_H_

#include <stddef.h>

#define ARR_SIZE 20

typedef struct phonebook_entry_t { //easy/dumb implementation for now
    size_t first_name_allocated; //not used for now
    size_t last_name_allocated; //not used
    size_t number_allocated; //not_used

    size_t first_name_count;
    size_t last_name_count;
    size_t number_count;

    size_t id;

    char first_name[ARR_SIZE];
    char last_name[ARR_SIZE];
    char phone_number[ARR_SIZE];

    char info[ARR_SIZE * 3]; // All three strings combined for ease of searching, will probably use this

    struct phonebook_entry_t* prev_entry;
    struct phonebook_entry_t* next_entry;
} phonebook_entry;

typedef struct {
    phonebook_entry* first_entry;
    size_t size;
    size_t latest_id;
} phonebook;

phonebook* phonebook_create();

void phonebook_delete(phonebook* pb);

phonebook_entry* phonebook_add_entry(phonebook* pb, char* first_name, char* last_name,
                                     char* phone_number);

int phonebook_delete_entry(phonebook* pb, phonebook_entry* pentry);// return value is for errors

void phonebook_search(phonebook* pb, char* first_name, char* last_name,
                                  size_t* return_array, size_t return_array_size);

phonebook_entry* phonebook_last_entry(phonebook* pb);

void phonebook_print_entry(phonebook_entry* pentry);

void phonebook_interactive(phonebook* pb);

phonebook_entry* get_entry_by_id(phonebook* pb, size_t id);

#endif
