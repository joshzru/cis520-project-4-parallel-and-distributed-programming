// 3way-pthread/max_ascii_pthread.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "file_io.h"

typedef struct {
    int thread_id;
    size_t start;
    size_t end;
    int *max_values;
    file_string_array_t *fsa;
} thread_data_t;

void *process_chunk(void *args) {
    thread_data_t *t_data = (thread_data_t *)args;
    int charOrd, maxOrd;

    // Loop through each line
    for (int i = t_data->start; i < t_data->end; i++) {
        maxOrd = -1;
        // Loop through the string until we hit the null terminator
        for (int j = 0; t_data->fsa->line_array[i][j] != '\0'; j++) {
            // Get the ordinal for this character
            charOrd = (int)t_data->fsa->line_array[i][j];
    
            // Skip non-ASCII characters
            if (charOrd < 0 || charOrd > 127) {
                continue;
            }

            // Check if this character is the largest we've seen
            if (charOrd > maxOrd) {
                maxOrd = charOrd;
            }
        }

        // Shouldn't need a mutex since each thread has an exclusive chunk
        t_data->max_values[i] = maxOrd;
    }
}

int main(int argc, char *argv[])
{
    int num_threads = (argc > 1) ? atoi(argv[1]) : 4;
    const char *filename = "/homes/eyv/cis520/wiki_dump.txt";

    file_string_array_t fsa;
    
    if (!read_text_file((char*)filename, &fsa)) {
        fprintf(stderr, "Failed to read file\n");
        return 1;
    }

    int *max_values = calloc(fsa.count, sizeof(int));
    if (!max_values) {
        destroy_fsa(&fsa);
        return 1;
    }

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    if (!threads) {
        destroy_fsa(&fsa);
        free(max_values);
        return 1;
    }
    thread_data_t *tdata = malloc(num_threads * sizeof(thread_data_t));
    if (!tdata) {
        destroy_fsa(&fsa);
        free(max_values);
        free(threads);
        return 1;
    }

    size_t lines_per_thread = fsa.count / num_threads;
    size_t remainder = fsa.count % num_threads;
    size_t start = 0;

    for (int i = 0; i < num_threads; i++) {
        size_t extra = (i < (int)remainder) ? 1 : 0;
        tdata[i].thread_id = i;
        tdata[i].start = start;
        tdata[i].end = start + lines_per_thread + extra;
        tdata[i].max_values = max_values;
        tdata[i].fsa = &fsa;
                                          //not yet implemented
        pthread_create(&threads[i], NULL, process_chunk, &tdata[i]);
        start += lines_per_thread + extra;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print final output
    for (size_t i = 0; i < fsa.count; i++) {
        printf("%zu: %d\n", i, max_values[i]);
    }

    destroy_fsa(&fsa);
    free(max_values);
    free(threads);
    free(tdata);

    return 0;
}