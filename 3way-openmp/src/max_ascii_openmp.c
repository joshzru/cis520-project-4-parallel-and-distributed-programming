// 3way-openmp/max_ascii_openmp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include "file_io.h"
#include "max_ascii_openmp.h"

int main(int argc, char *argv[]) {
    int num_threads;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_threads = atoi(argv[1]);

    if (num_threads < 1) {
        fprintf(stderr, "Invalid node parameters: num_threads=%d\n", 
                num_threads);
        return EXIT_FAILURE;
    }

    omp_set_num_threads(num_threads);

    file_string_array_t fsa;
    if (!read_text_file(WIKI_DUMP_FILENAME, &fsa)) {
        fprintf(stderr, "Failed to read file\n");
        return EXIT_FAILURE;
    }

    // Allocate memory for each line's max ASCII value
    int *max_ascii = malloc(fsa.count * sizeof(int));
    if (!max_ascii) {
        destroy_fsa(&fsa);
        fprintf(stderr, "Failed to allocate memory for max_ascii\n");
        return EXIT_FAILURE;
    }

    // Parallelize the for loop
    #pragma omp parallel for
    for (size_t i = 0; i < fsa.count; i++) {
        char c;
        int charOrd, maxOrd = -1;

        // For each character in the line, check if it's a valid ASCII character
        // and update the max ASCII value for that line, if applicable
        for (int j = 0; (c = fsa.line_array[i][j]) != '\0'; j++) {
            charOrd = (int)c;
            if (charOrd < 0 || charOrd > 127) {
                continue;
            }

            if (charOrd > maxOrd) {
                maxOrd = charOrd;
            }
        }

        // Record this line's max ASCII value
        // Don't need a lock since each thread has a mutually exclusive range
        // of lines
        max_ascii[i] = maxOrd;
    }

    // Print output for this node
    for (size_t i = 0; i < fsa.count; i++) {
        printf("Line %zu: Max ASCII = %d\n", i, max_ascii[i]);
    }
    
    // Clean up
    destroy_fsa(&fsa);
    free(max_ascii);

    return EXIT_SUCCESS;
}