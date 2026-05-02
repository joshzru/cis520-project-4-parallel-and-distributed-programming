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
    int num_threads, num_nodes, my_node;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <num_threads> <num_nodes> <my_node>\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_threads = atoi(argv[1]);
    num_nodes = atoi(argv[2]);
    my_node = atoi(argv[3]);

    if (num_threads < 1 || num_nodes < 1 || my_node < 0 || my_node >= num_nodes) {
        fprintf(stderr, "Invalid node parameters: num_threads=%d, num_nodes=%d, my_node=%d\n", 
                num_threads, num_nodes, my_node);
        return EXIT_FAILURE;
    }

    omp_set_num_threads(num_threads);

    file_string_array_t fsa;

    if (!read_text_file(WIKI_DUMP_FILENAME, &fsa)) {
        fprintf(stderr, "Failed to read file\n");
        return EXIT_FAILURE;
    }

    size_t lines_per_node = fsa.count / num_nodes;
    size_t remainder = fsa.count % num_nodes;

    size_t node_start_line = 0;
    for (int n = 0; n < my_node; n++) {
        node_start_line += lines_per_node + (n < (int)remainder ? 1 : 0);
    }

    size_t node_end_line = node_start_line + lines_per_node + (my_node < (int)remainder ? 1 : 0);

    size_t my_line_count = node_end_line - node_start_line;
    int *max_ascii = malloc(my_line_count * sizeof(int));
    if (!max_ascii) {
        destroy_fsa(&fsa);
        fprintf(stderr, "Failed to allocate memory for max_ascii\n");
        return EXIT_FAILURE;
    }

    #pragma omp parallel for
    for (size_t i = node_start_line; i < node_end_line; i++) {
        char c;
        int charOrd, maxOrd = -1;

        for (int j = 0; (c = fsa.line_array[i][j]) != '\0'; j++) {
            charOrd = (int)c;
            if (charOrd < 0 || charOrd > 127) {
                continue;
            }

            if (charOrd > maxOrd) {
                maxOrd = charOrd;
            }
        }

        max_ascii[i - node_start_line] = maxOrd;
    }

    // Print output for this node
    for (size_t i = 0; i < my_line_count; i++) {
        printf("Node %d, Line %zu: Max ASCII = %d\n", my_node, node_start_line + i, max_ascii[i]);
    }
    
    destroy_fsa(&fsa);
    free(max_ascii);
}