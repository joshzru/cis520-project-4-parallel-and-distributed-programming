// 3way-openmp/max_ascii_openmp.c
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

    int max_ascii_size = CHUNK_SIZE;
    int *max_ascii = malloc(max_ascii_size * sizeof(int));
    if (!max_ascii) {
        fprintf(stderr, "Failed to allocate memory for max_ascii array\n");
        return EXIT_FAILURE;
    }

    // Start at offset 0 and read in chunks of the file at a time.
    long offset = 0;
    file_string_array_t fsa;
    if (!read_text_file(WIKI_DUMP_FILENAME, &fsa, offset, CHUNK_SIZE)) {
        fprintf(stderr, "Failed to read file\n");
        return EXIT_FAILURE;
    }

    while (!fsa.end_of_file && fsa.count > 0) {
        // Process the chunk of lines read in from the file
        #pragma omp parallel for
        for (size_t i = 0; i < fsa.count; i++) {
            char c;
            int charOrd, maxOrd = -1;

            // For each character in the line, check if it's a valid ASCII character
            // and update the max ASCII value for that line, if applicable
            for (int j = 0; (c = fsa.line_array[i][j]) != '\0'; j++) {
                charOrd = (int)c;
                if (charOrd < ASCII_MIN || charOrd > ASCII_MAX) {
                    continue;
                }

                if (charOrd > maxOrd) {
                    maxOrd = charOrd;
                }
            }

            // Store the max ASCII value for this line in the array
            // Don't need a lock because each thread has a mutually exclusive range
            max_ascii[i + offset] = maxOrd;
        }

        offset += fsa.count;
        // Destroy the fsa and read in the next chunk of lines from the file.
        destroy_fsa(&fsa);
        if (!read_text_file(WIKI_DUMP_FILENAME, &fsa, offset, CHUNK_SIZE)) {
            fprintf(stderr, "Failed to read file\n");
            return EXIT_FAILURE;
        }

        // Resize the max_ascii array to accomodate the new chunk
        if (max_ascii_size < offset + fsa.count) {
            while (max_ascii_size < offset + fsa.count) {
                max_ascii_size *= 2;
            }
            int *temp = realloc(max_ascii, max_ascii_size * sizeof(int));
            if (!temp) {
                fprintf(stderr, "Failed to reallocate memory for max_ascii array\n");
                destroy_fsa(&fsa);
                free(max_ascii);
                return EXIT_FAILURE;
            }
            max_ascii = temp;
        }
    }

    // Print output for this node
    for (long i = 0; i < offset; i++) {
        printf("Line %ld: Max ASCII = %d\n", i, max_ascii[i]);
    }
    
    // Clean up
    // fsa is destroyed at the end of each loop
    free(max_ascii);

    return EXIT_SUCCESS;
}