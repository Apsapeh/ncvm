

#include <stdlib.h>
#include <stdio.h>

#include <ncvm.h>

#include <extc_vec.h>
#include <extc_string.h>
#include <extc_rint.h>

int main(int argc, char* argv[]) {
    FILE *file = fopen("examples/asm/1.asm", "r");

    fseek(file, 0, SEEK_END); 
    usize size = ftell(file)+1;
    fseek(file, 0, SEEK_SET); 

    u8* data = (u8*)malloc(size);
    fread(data, size-1, 1, file);
    data[size] = '\0';
    

    string data_str = string_from_n(data, size);
    free(data);
    vec_string lines = string_split(&data_str, "\n", true);

    u8 is_stack_def = 0;
    usize i;
    for (i=0; i<lines.size; ++i) {
        vec_string words = string_split(&lines.data[i], " ", true);

        string* f_str = &words.data[0];
        
        if (
            f_str->size > 2 
         && f_str->str[0] == '#' 
         && (
                f_str->str[f_str->size-1] == '(' 
                || (words.size > 1 && words.data[1].str[0] == '(')
            )
        ) {
            printf("Static memory definition found: %s\n", f_str->str);
        }

        free_vec_string(&words);
    }

    free_vec_string(&lines);
}