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

    int max_ascii_i = 0; // The next free index
    int max_ascii[NUM_LINES];

    file_string_array_t fsa;
    if (!create_fsa(&fsa, WIKI_DUMP_FILENAME)) {
        fprintf(stderr, "Failed to create fsa");
        return EXIT_FAILURE;
    }

    while (!fsa.end_of_file) {
        if (!fsa_read_next_chunk(&fsa, CHUNK_SIZE)) {
            fprintf(stderr, "Failed to read chunk");
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

            // Index with local i to prevent race conditions.
            max_ascii[max_ascii_i + i] = maxOrd;
        }
        
        max_ascii_i += fsa.count;
    }

    // Print output for this node
    for (int i = 0; i < max_ascii_i; i++) {
        printf("Line %d: Max ASCII = %d\n", i, max_ascii[i]);
    }
    
    // Clean up
    // fsa is destroyed at the end of each loop
    free(max_ascii);
    destroy_fsa(&fsa);

    return EXIT_SUCCESS;
}