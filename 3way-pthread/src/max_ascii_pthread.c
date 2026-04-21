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
    for (size_t i = t_data->start; i < t_data->end; i++) {
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

    return NULL;
}

int main(int argc, char *argv[])
{
    int num_threads = 4;
    int total_nodes = 1;
    int my_node     = 0;

    if (argc > 1) num_threads = atoi(argv[1]);
    if (argc > 2) total_nodes = atoi(argv[2]);
    if (argc > 3) my_node = atoi(argv[3]);

    if (total_nodes < 1 || my_node < 0 || my_node >= total_nodes) {
        fprintf(stderr, "Invalid node parameters: total_nodes=%d, my_node=%d\n", 
                total_nodes, my_node);
        return 1;
    }

    const char *filename = "/homes/eyv/cis520/wiki_dump.txt";

    file_string_array_t fsa;
    
    if (!read_text_file((char*)filename, &fsa)) {
        fprintf(stderr, "Failed to read file\n");
        return 1;
    }

    // Calculate which lines this node is responsible for
    size_t lines_per_node = fsa.count / total_nodes;
    size_t remainder = fsa.count % total_nodes;

    size_t node_start = 0;
    for (int n = 0; n < my_node; n++) {
        node_start += lines_per_node + (n < (int)remainder ? 1 : 0);
    }
    size_t node_end = node_start + lines_per_node + (my_node < (int)remainder ? 1 : 0);

    size_t my_line_count = node_end - node_start;
    int *max_values = calloc(my_line_count, sizeof(int));
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

    size_t lines_per_thread = my_line_count / num_threads;
    size_t thread_remainder = my_line_count % num_threads;
    size_t start = 0;

    for (int i = 0; i < num_threads; i++) {
        size_t extra = (i < (int)thread_remainder) ? 1 : 0;
        tdata[i].thread_id = i;
        tdata[i].start = node_start + start;
        tdata[i].end = node_start + start + lines_per_thread + extra;
        tdata[i].max_values = max_values;
        tdata[i].fsa = &fsa;
        
        pthread_create(&threads[i], NULL, process_chunk, &tdata[i]);
        start += lines_per_thread + extra;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print final output
    for (size_t i = 0; i < my_line_count; i++) {
        size_t global_line = node_start + i;
        printf("%zu: %d\n", global_line, max_values[i]);
    }

    destroy_fsa(&fsa);
    free(max_values);
    free(threads);
    free(tdata);

    return 0;
}