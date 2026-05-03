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
    int max_ascii_i = 0; // The next free index
    int *max_ascii = malloc(max_ascii_size * sizeof(int));
    if (!max_ascii) {
        fprintf(stderr, "Failed to allocate memory for max_ascii array\n");
        return EXIT_FAILURE;
    }

    file_string_array_t fsa;
    if (!create_fsa(&fsa, WIKI_DUMP_FILENAME)) {
        fprintf(stderr, "Failed to create fsa");
        return EXIT_FAILURE;
    }

    while (!fsa.end_of_file) {
        if (!read_next_chunk(&fsa, CHUNK_SIZE)) {
            fpritnf(stderr, "Failed to read chunk");
            return EXIT_FAILURE;
        }

        // Process the chunk of lines read in from the file
        #pragma omp parallel for
        for (size_t i = 0; i < fsa.count; i++) {
            char c;
            int charOrd, maxOrd = -1;

            // For each character in the line, check if it's a valid ASCII
            // character and update the max ASCII value for that line, if
            // applicable
            for (int j = 0; (c = fsa.line_array[i][j]) != '\0'; j++) {
                charOrd = (int)c;
                if (charOrd < ASCII_MIN || charOrd > ASCII_MAX) {
                    continue;
                }

                if (charOrd > maxOrd) {
                    maxOrd = charOrd;
                }
            }

            max_ascii[max_ascii_i++] = maxOrd;
        }

        // Resize the max_ascii array to accomodate the new chunk
        if (max_ascii_size < max_ascii_i) {
            while (max_ascii_size < max_ascii_i) {
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
    for (int i = 0; i < max_ascii_i; i++) {
        printf("Line %ld: Max ASCII = %d\n", i, max_ascii[i]);
    }
    
    // Clean up
    // fsa is destroyed at the end of each loop
    free(max_ascii);

    return EXIT_SUCCESS;
}