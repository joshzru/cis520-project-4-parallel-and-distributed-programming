// 3way-pthread/max_ascii_pthread.c
#include "max_ascii_pthread.h"

void *process_chunk(void *args) {
    thread_data_t *tdata = (thread_data_t *)args;
    char c;
    int charOrd, maxOrd;

    // Loop through each line
    for (size_t i = tdata->start; i < tdata->end; i++) {
        maxOrd = -1;
        // Loop through the string until we hit the null terminator
        for (int j = 0; (c = tdata->fsa->line_array[i][j]) != '\0'; j++) {
            // Get the ordinal for this character
            charOrd = (int)c;
    
            // Skip non-ASCII characters
            if (charOrd < ASCII_MIN || charOrd > ASCII_MAX) {
                continue;
            }

            // Check if this character is the largest we've seen
            if (charOrd > maxOrd) {
                maxOrd = charOrd;
            }
        }

        // Shouldn't need a mutex since each thread has an exclusive chunk
        tdata->max_values[tdata->offset + i] = maxOrd;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    size_t num_threads;

    if (argc != 2) {
        fprintf(stderr, "Usage %s: <num_threads>\n", argv[0]);
    }

    num_threads = atoi(argv[1]);

    if (num_threads < 1) {
        fprintf(stderr, "Invalid node parameters: num_threads=%zu\n", 
                num_threads);
        return EXIT_FAILURE;
    }

    file_string_array_t fsa;
    if (!create_fsa(&fsa, WIKI_DUMP_FILENAME)) {
        fprintf(stderr, "Failed to create fsa");
        return EXIT_FAILURE;
    }

    int max_values[NUM_LINES];
    size_t max_values_i = 0;
    pthread_t threads[num_threads];
    thread_data_t tdata[num_threads];

    while (!fsa.end_of_file) {
        if (!fsa_read_next_chunk(&fsa, CHUNK_SIZE)) {
            fprintf(stderr, "Failed to read next chunk from file");
            destroy_fsa(&fsa);
            return EXIT_FAILURE;
        }

        size_t lines_per_thread = fsa.count / num_threads;
        size_t thread_remainder = fsa.count % num_threads;
        size_t start = 0;

        for (size_t i = 0; i < num_threads; i++) {
            size_t extra = i < thread_remainder ? 1 : 0;

            tdata[i].start = start;
            tdata[i].end = start + lines_per_thread + extra;
            tdata[i].max_values = max_values;
            tdata[i].fsa = &fsa;
            tdata[i].offset = max_values_i;

            pthread_create(&threads[i], NULL, process_chunk, &tdata[i]);
            start += lines_per_thread + extra;
        }

        for (size_t i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        max_values_i += fsa.count;
    }

    // Print final output
    for (size_t i = 0; i < max_values_i; i++) {
        printf("Line: %zu; Max ASCII: %d\n", i, max_values[i]);
    }

    destroy_fsa(&fsa);

    return EXIT_SUCCESS;
}