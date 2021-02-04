#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

typedef int (*binary_func)(int, int);

typedef struct {
    binary_func p_func;
    char func_name[100];
} plugin_data;

// pointer to pointer is for the future, to
// possibly realloc the array if too small
size_t load_funcs(plugin_data** p_func_arr, size_t arr_size) {
    DIR* plugins_folder = opendir("./plugins");
    if (!plugins_folder) {
        perror("failed to open plugins folder\n\n");
        exit(1);
    }
    
    struct dirent* entry = readdir(plugins_folder);
    size_t count = 0;
    char filename[513];// 256 * 2 + 1
    while (entry != NULL) {
        strcpy(filename, "./plugins/");
        strncat(filename, entry->d_name, 513 - 11);
        void* handle = dlopen(filename, RTLD_LAZY);
        if (!handle) {
            printf("failed to open plugin %s\n%s\n\n", filename, 
                    dlerror());
            entry = readdir(plugins_folder);
            continue;
        }
        
        dlerror();
        const char** func_name = (const char**)
                             (dlsym(handle, "func_name"));
        char* dlerr_return = dlerror();
        if (dlerr_return != NULL) {
            printf("failed to load func_name from plugin %s\n%s\n\n",
                    filename, dlerr_return);
            entry = readdir(plugins_folder);
            continue;
        }
    
        binary_func func = (binary_func)dlsym(handle, *func_name); 
        if (dlerr_return != NULL) {
            printf("failed to load function %s"
                   " from plugin %s\n%s\n",
                    *func_name, filename, dlerr_return);
            entry = readdir(plugins_folder);
            continue;
        }
        (*p_func_arr)[count].p_func = func;
        strncpy((*p_func_arr)[count].func_name, *func_name, 100 - 1);
        ++count;
        if (count >= arr_size) break;
         
        entry = readdir(plugins_folder);
    } 
    
    closedir(plugins_folder);
    return count;
}


void calc_menu(plugin_data* func_arr, size_t arr_size) {
    int input_1 = 0, a = 0, b = 0;
    printf("interactive menu\n==============\n");
    //printf("debug: %d + %d = %d", 1, 1, func_arr[0].p_func(1, 1));
    while(1) {
        size_t i = 0;
        for (i = 0; i < arr_size; ++i) {
            printf("%ld - %s\n", i, func_arr[i].func_name);
        }
        printf("%ld - exit\n", i);
        scanf("%d", &input_1);
        if ((input_1 >= arr_size) | (input_1 < 0))
            break;

        printf("enter_operands\n");
        scanf("%d%d", &a, &b);
        printf("%s(%d, %d) = %d\n\n", func_arr[input_1].func_name,
               a, b, func_arr[input_1].p_func(a, b));
    }
}

int main() {
   /* void* handle = dlopen("./plugins/libadd.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("error loading so:%s\n", dlerror());
        exit(1);
    }
  
    const char** func_name = (const char**)
                             (dlsym(handle, "func_name"));
   
    char* dlerr_return = dlerror();
    dlerr_return = dlerror();

    if (dlerr_return != NULL) {
        printf("error loading symbol: %s\n", dlerr_return);
        exit(1);
    }  
    dlerror();
    */
    plugin_data func_array[20];
    plugin_data *p_func = &func_array[0];

    size_t func_amount = load_funcs(&p_func, 20); 
    /*func_array[0] = (int (*) (int, int))(dlsym(handle, *func_name));

    if (dlerr_return != NULL) {
        printf("error loading symbol: %s\n", dlerr_return);
        exit(1);
    }*/
 
   // printf("loaded func_name = %s\n", *func_name);


    //printf("1 + 1 = %d\n", func_array[0].p_func(1, 1));
    calc_menu((plugin_data*)(func_array), func_amount);

}
