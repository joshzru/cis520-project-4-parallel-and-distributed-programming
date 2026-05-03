#ifndef FILE_IO_H
#define FILE_IO_H

#define INIT_LINE_CAPACITY 64
#define MAX_LINE_CAPACITY 1048576
#define LINE_BUFFER_SIZE 4096

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

bool fsa_read_next_chunk(file_string_array_t *fsa, size_t chunk_size);

bool fsa_add_line(file_string_array_t * fsa, char *str);

void close_fsa(file_string_array_t *fsa);

#endif //FILE_IO_H