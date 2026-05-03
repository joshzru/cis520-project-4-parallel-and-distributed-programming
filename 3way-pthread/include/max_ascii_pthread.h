#ifndef MAX_ASCII_PTHREAD_H
#define MAX_ASCII_PTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "file_io.h"

#define WIKI_DUMP_FILENAME "/homes/eyv/cis520/wiki_dump.txt"
#define CHUNK_SIZE 20000
#define ASCII_MAX 127
#define ASCII_MIN 0
#define NUM_LINES 1000000

typedef struct {
    size_t start;
    size_t end;
    size_t offset;
    int *max_values;
    file_string_array_t *fsa;
} thread_data_t;

#endif