// 3way-pthread/max_ascii_mpi.c
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "file_io.h"

#define BATCH_SIZE 10000

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // this process ID
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    file_string_array_t fsa;
    size_t total_lines = 0;

    if ( rank == 0 )
    {
        const char *filename = "/homes/eyv/cis520/wiki_dump.txt";
        
        if (!read_text_file((char*)filename, &fsa)) {
            fprintf(stderr, "Rank 0: Failed to read file %s\n", filename);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        total_lines = fsa.count;
        printf("Rank 0: Successfully read %zu lines from wiki_dump.txt\n", total_lines);
    } else {
        if (!create_fsa(&fsa)) {
            fprintf(stderr, "Rank %d: Failed to create fsa\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&total_lines, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    if (total_lines == 0) {
        if (rank == 0) fprintf(stderr, "No lines in file!\n");
        destroy_fsa(&fsa);
        MPI_Finalize();
        return 1;
    }

    size_t lines_per_rank = total_lines / size;
    size_t remainder = total_lines % size;

    size_t my_start = 0;
    for (int r = 0; r < rank; r++) {
        my_start += lines_per_rank + (r < (int)remainder ? 1 : 0);
    }
    size_t my_count = lines_per_rank + (rank < (int)remainder ? 1 : 0);

    // Distribute lines
    if (rank == 0) {
        for (int r = 1; r < size; r++) {
            size_t start_r = 0;
            for (int i = 0; i < r; i++) {
                start_r += lines_per_rank + (i < (int)remainder ? 1 : 0);
            }
            size_t count_r = lines_per_rank + (r < (int)remainder ? 1 : 0);

            for (size_t i = 0; i < count_r; i += BATCH_SIZE) {
                size_t batch = (i + BATCH_SIZE < count_r) ? BATCH_SIZE : (count_r - i);
                for (size_t j = 0; j < batch; j++) {
                    char *line = fsa.line_array[start_r + i + j];
                    size_t len = strlen(line) + 1;
                    MPI_Send(&len, 1, MPI_UNSIGNED_LONG, r, 0, MPI_COMM_WORLD);
                    MPI_Send(line, len, MPI_CHAR, r, 1, MPI_COMM_WORLD);
                }
            }
        }
    } else {
        char *recv_buffer = malloc(BUFFER_SIZE);
        if (recv_buffer == NULL) {
            fprintf(stderr, "Rank %d: malloc failed\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (size_t i = 0; i < my_count; i++) {
            size_t len;
            MPI_Recv(&len, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(recv_buffer, len, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            fsa_add_line(&fsa, recv_buffer);
        }
        free(recv_buffer);
    }

    // Now every rank has its local lines in fsa

    // Process lines
    int *local_max = calloc(my_count, sizeof(int));
    if (local_max == NULL) {
        fprintf(stderr, "Rank %d: calloc failed for local_max\n", rank);
        destroy_fsa(&fsa);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (size_t i = 0; i < my_count; i++) {
        int maxOrd = -1;
        char *line = fsa.line_array[i];

        for (int j = 0; line[j] != '\0'; j++) {
            int charOrd = (int)line[j];
            if (charOrd >= 0 && charOrd <= 127 && charOrd > maxOrd) {
                maxOrd = charOrd;
            }
        }
        local_max[i] = maxOrd;
    }

    // Gather results to rank 0
    if (rank == 0) {
        int *global_max = calloc(total_lines, sizeof(int));
        if (global_max == NULL) {
            fprintf(stderr, "Rank 0: calloc failed for global_max\n");
            destroy_fsa(&fsa);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (size_t i = 0; i < my_count; i++) {
            global_max[my_start + i] = local_max[i];
        }

        for (int r = 1; r < size; r++) {
            size_t start_r = 0;
            for (int i = 0; i < r; i++) {
                start_r += lines_per_rank + (i < (int)remainder ? 1 : 0);
            }
            size_t count_r = lines_per_rank + (r < (int)remainder ? 1 : 0);

            MPI_Recv(global_max + start_r, count_r, MPI_INT, r, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("First 100 lines of output:\n");
        for (size_t i = 0; i < 100 && i < total_lines; i++) {
            printf("%zu: %d\n", i, global_max[i]);
        }

        free(global_max);
    } 
    else {
        MPI_Send(local_max, my_count, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    free(local_max);
    destroy_fsa(&fsa);

    MPI_Finalize();
    return 0;
}