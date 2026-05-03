#include <file_io.h>

bool read_next_chunk(file_string_array_t *fsa, size_t chunk_size) {
    if (!fsa) {
        return false;
    }

    // Free all lines so we don't leak memory overwriting them.
    destroy_fsa_lines(fsa);

    // Retain current capacity, and overwrite previous lines.
    char buff[BUFFER_SIZE];
    for (size_t i = 0; i < chunk_size; i++) {
        if (fgets(buff, BUFFER_SIZE, fsa->fp) == NULL) {
            // Check for end of file.
            if (feof(fsa->fp) != 0) {
                fsa->end_of_file = true;
                break;
            }
            fprintf(stderr, "Failed to read line from file");
            destroy_fsa_lines(fsa);
            free(buff);
            return false;
        }

        if (!fsa_add_line(fsa, buff)) {
            fprintf(stderr, "Failed to add line to fsa");
            return false;
        }
    }

    return true;
}

bool create_fsa(file_string_array_t * fsa, char *filename)
{
    fsa->capacity = (size_t) INIT_CAPACITY;
    fsa->count = 0;
    fsa->end_of_file = false;
    
    FILE * fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open file");
        return false;
    }

    fsa->fp = fp;

    char ** line_array_buffer = (char **) calloc( fsa->capacity, sizeof(char *) );
    if ( line_array_buffer == NULL )
    {
        fprintf(stderr, "Failed to allocate memory for line array buffer when creating file string array\n");
        return false;
    }

    fsa->line_array = line_array_buffer;

    return true;
}

void destroy_fsa(file_string_array_t * fsa)
{
    destroy_fsa_lines(fsa);

    free(fsa->line_array);
    fclose(fsa->fp);
    fsa->line_array = NULL;
    fsa->fp = NULL;
    fsa->capacity = 0;
}

void destroy_fsa_lines(file_string_array_t *fsa) {
    // Free all char array pointers that have been filled in the line array
    for (size_t i = 0; i < fsa->count; i++) {
        free(fsa->line_array[i]);
        fsa->line_array[i] = NULL;
    }
    fsa->count = 0;
}

bool fsa_add_line(file_string_array_t * fsa, char *str)
{
    // reallocate array to larger size if necessary
    if ( fsa->count >= fsa->capacity )
    {
        size_t new_capacity = fsa->capacity * 2;
        if ( new_capacity >= (size_t) MAX_CAPACITY )
        {
            fprintf(stderr, "Exceeded maximum capacity for file string array\n");
            return false;
        }

        char ** line_array_buffer = (char **) realloc( fsa->line_array, new_capacity * sizeof(char *) );
        if ( line_array_buffer == NULL )
        {
            fprintf(stderr, "Failed to reallocate memory for line array\n");
            return false;
        }
        fsa->line_array = line_array_buffer;
        fsa->capacity = new_capacity;
    }

    // Add the string to the next pointer in the allocated array
    size_t line_len = strnlen( str, (size_t) BUFFER_SIZE );

    // allocate a buffer for the line (+1 since strnlen doesnt include null terminator)
    char * str_buffer = malloc( line_len + 1 );
    if ( str_buffer == NULL )
    {
        fprintf(stderr, "Failed to allocate memory for a new line buffer");
        return false;
    }

    // Copy the string from the stack into the allocated string buffer
    memcpy(str_buffer, str, line_len + 1);

    // Set the char* in the line array to point to the new string buffer
    fsa->line_array[fsa->count++] = str_buffer;

    return true;
}