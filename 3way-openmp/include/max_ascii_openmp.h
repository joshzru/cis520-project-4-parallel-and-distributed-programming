#ifndef MAX_ASCII_OPENMP_H
#define MAX_ASCII_OPENMP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include "file_io.h"

#define WIKI_DUMP_FILENAME "/homes/eyv/cis520/wiki_dump.txt"
#define CHUNK_SIZE 50000
#define ASCII_MAX 127
#define ASCII_MIN 0
#define NUM_LINES 1000000

#endif // MAX_ASCII_OPENMP_H