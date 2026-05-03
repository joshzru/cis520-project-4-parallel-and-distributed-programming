#ifndef FILE_IO_H
#define FILE_IO_H

#define INIT_CAPACITY 64
#define MAX_CAPACITY 2097152
#define BUFFER_SIZE 8192

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct file_string_array {
	size_t count;
    size_t capacity;
	char ** line_array;
	bool end_of_file;
	FILE *fp;
} file_string_array_t;

bool create_fsa(file_string_array_t * fsa, char *filename);

void destroy_fsa(file_string_array_t * fsa);

void destroy_fsa_lines(file_string_array_t *fsa);

bool read_next_chunk(file_string_array_t *fsa, size_t chunk_size);

bool fsa_add_line(file_string_array_t * fsa, char *str);

void close_fsa(file_string_array_t *fsa);

#endif //FILE_IO_H