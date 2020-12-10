#include "phonebook.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

phonebook* phonebook_create() {
    phonebook* pb = (phonebook*)malloc(sizeof(phonebook));
    pb->first_entry = NULL;
    pb->size = 0;
    return pb;
}

void phonebook_delete(phonebook* pb) {
    phonebook_entry* pentry = pb->first_entry, *next = NULL;
    while (pentry != NULL) {
        next = pentry->next_entry;
        phonebook_delete_entry(pb, pentry);
        pentry = next;
    }

    free(pb);
}

phonebook_entry* phonebook_add_entry(phonebook* pb, char* first_name, char* last_name,
                                     char* phone_number) {
    phonebook_entry* pentry = (phonebook_entry*)malloc(sizeof(phonebook_entry));
    pentry->number_allocated = ARR_SIZE;
    pentry->first_name_allocated = ARR_SIZE;
    pentry->last_name_allocated = ARR_SIZE;

    pentry->next_entry = NULL;

    size_t fn_size = strlen(first_name);
    size_t ln_size = strlen(last_name);
    size_t num_size = strlen(phone_number);

    if (fn_size < pentry->first_name_allocated) {
        strcpy(pentry->first_name, first_name);
        pentry->first_name_count = fn_size;
    }
    else {
        pentry->first_name_count = 0;
        pentry->first_name[0] = '\0';
    }

    if (ln_size < pentry->last_name_allocated) {
        strcpy(pentry->last_name, last_name);
        pentry->last_name_count = ln_size;
    }
    else {
        pentry->last_name_count = 0;
        pentry->last_name[0] = '\0';
    }

    if (num_size < pentry->number_allocated) {
        strcpy(pentry->phone_number, phone_number);
        pentry->number_count = num_size;
    }
    else {
        pentry->number_count = 0;
        pentry->phone_number[0] = '\0';
    }

    phonebook_entry* last_entry = phonebook_last_entry(pb);
    if (last_entry == NULL) {
        pb->first_entry = pentry;
        pentry->prev_entry = NULL;

    }
    else {
        last_entry->next_entry = pentry;
        pentry->prev_entry = last_entry;
    }
    ++pb->size;
    return pentry;
}

int phonebook_delete_entry(phonebook* pb, phonebook_entry* pentry) {
    phonebook_entry* prev = pentry->prev_entry;
    phonebook_entry* next = pentry->next_entry;
    if (prev == NULL || next == NULL) {
        pb->first_entry = NULL;
        --pb->size;
        free(pentry);
        return 0;
    }
    else if (prev == NULL) {
        pb->first_entry = next;
        next->prev_entry = NULL;
        --pb->size;
        free(pentry);
        return 0;
    }
    else if (next == NULL) {
        prev->next_entry = next;
        --pb->size;
        free(pentry);
        return 0;
    }
    else {
        next->prev_entry = prev;
        prev->next_entry = next;
        --pb->size;
        free(pentry);
        return 0;
    }
}

phonebook_entry* phonebook_search(phonebook* pb, char* first_name, size_t fn_size, char* last_name, size_t ln_size,
                                  char* phone_number, size_t num_size) {

}

phonebook_entry* phonebook_last_entry(phonebook* pb) {
   phonebook_entry* next = pb->first_entry, *last_entry = NULL;
   while (next != NULL) {
       last_entry = next;
       next = next->next_entry;
   }
   return last_entry;
}


void phonebook_print_entry(phonebook_entry* pentry) {
    printf("%s %s\n%s\n", pentry->first_name, pentry->last_name, pentry->phone_number);
}

typedef enum {
    MENU,
    ADD,
    PRINT,
    EXIT,
    INPUT_ERR

} pb_state;

pb_state phonebook_menu_input() {
    int input = 0;
    scanf("%d", &input);
    getchar();
//    while (scanf("%*s") > 0); //eating \n's and stuff
    switch (input) {
    case 1:
        return PRINT;
    case 2:
        return ADD;
    case 3:
        return EXIT;
    default:
        return INPUT_ERR;
    }
}

void phonebook_entry_input(phonebook* pb) {
    char first_name[ARR_SIZE];
    char last_name[ARR_SIZE];
    char phone_number[ARR_SIZE];
    // TODO do this properly with checking for errors etc
    printf("enter first name\n");
    fgets(first_name, ARR_SIZE, stdin);
    printf("enter last name\n");
    fgets(last_name, ARR_SIZE, stdin);
    printf("enter phone_number\n");
    fgets(phone_number, ARR_SIZE, stdin);
    phonebook_add_entry(pb, first_name, last_name, phone_number);
}

void print_all_entries(phonebook* pb) {
    phonebook_entry* entry = pb->first_entry;
    while (entry != NULL) {
        phonebook_print_entry(entry);
        entry = entry->next_entry;
    }
}

void phonebook_interactive(phonebook* pb) {
    char must_free = 0;
    if (pb == NULL) {
        pb = phonebook_create();
    }


    pb_state state = MENU;
    while(state != EXIT) {
        switch(state) {
        case MENU:
            printf("Phonebook with %lu entries\n", pb->size);
            printf("enter 1 to print all entries, 2 to add an entry, 3 to exit. Searching is not implemented yet\n");
            state = phonebook_menu_input();
            break;
        case ADD:
//            printf("todo (?)\n\n");
            phonebook_entry_input(pb);
            state = MENU;
            break;
        case PRINT:
//            printf("todo (??)\n\n");
            print_all_entries(pb);
            state = MENU;
            break;
        case INPUT_ERR:
            printf("invalid input\n\n");
            state = MENU;
            break;
        default:
            break;
        }
    }

    if (must_free) {
        phonebook_delete(pb);
    }
}





















