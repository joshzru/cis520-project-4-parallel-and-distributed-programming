#ifndef FILE_IO_H
#define FILE_IO_H

#define INIT_CAPACITY 64
#define MAX_CAPACITY 2097152
#define BUFFER_SIZE 65535

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct file_string_array {
	size_t count;
    size_t capacity;
	char * line_array;
} file_string_array_t;


bool read_text_file(char* filename, file_string_array_t * file_string_struct);

bool create_fsa(file_string_array_t * fsa);

void destroy_fsa(file_string_array_t * fsa);

bool fsa_add_line(file_string_array_t * fsa, char *str);


#endif //FILE_IO_H