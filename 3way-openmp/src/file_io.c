#include <file_io.h>

bool read_text_file(char* filename, file_string_array_t * fsa, long offset, size_t chunk_size)
{
    int err;
    FILE *fd;

    if (chunk_size > MAX_CAPACITY) {
        fprintf(stderr, "Chunk size %zu exceeds maximum capacity %d\n", chunk_size, MAX_CAPACITY);
        return false;
    }

     if (offset < 0) {
        fprintf(stderr, "Offset cannot be negative: %ld\n", offset);
        return false;
    }

    char *line_buffer = (char*) malloc( BUFFER_SIZE ); // no lines larger than 2000 chars
    if (line_buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for line buffer when reading text file\n");
        return false;
    }

    if ( !create_fsa( fsa ) )
    {
        fprintf(stderr, "Failed to create file string array when reading text file\n");
        free(line_buffer);
        return false;
    }

    // Open the file and seek to the offset
    fd = fopen( filename, "r" );
    if ( fd == NULL )
    {
        fprintf(stderr, "Failed to open file %s\n", filename);
        free(line_buffer);
        return false;
    }

    if (fseek(fd, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to seek to offset %ld in file %s\n", offset, filename);
        fclose(fd);
        free(line_buffer);
        return false;
    }



    for ( size_t i = 0; i < chunk_size; i++ )
    {
        err = fscanf( fd, "%65535[^\n]\n", line_buffer);
        if ( err == EOF ) {
            fsa->end_of_file = true;
            break;
        }
        if ( err < 0 || err >= BUFFER_SIZE )
        {
            fprintf(stderr, "Failed to read line from file %s\n", filename);
            return false;
        }
        
        fsa_add_line( fsa, line_buffer);
    }

    fclose( fd );
    free( line_buffer );

    return true;
}

bool create_fsa(file_string_array_t * fsa)
{
    fsa->capacity = (size_t) INIT_CAPACITY;
    fsa->count = 0;
    fsa->end_of_file = false;

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
    // Free all char array pointers that have been filled in the line array
    for ( size_t i = 0; i < fsa->count; i++ )
    {
        free( fsa->line_array[i] );
    }

    free(fsa->line_array);
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