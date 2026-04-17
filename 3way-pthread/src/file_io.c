#include <file_io.h>

bool read_text_file(char* filename, file_string_array_t * file_string_struct)
{
    int i, err;
    FILE *fd;
    char *line_buffer = (char*) malloc( BUFFER_SIZE ); // no lines larger than 2000 chars
    if (line_buffer == NULL) {
        return false;
    }

    file_string_array_t fsa;
    if ( !create_fsa( &fsa ) )
    {
        free(line_buffer);
        return false;
    }

    // Read in the lines from the data file
    fd = fopen( filename, "r" );

    if ( fd == NULL )
    {
        fprintf(stderr, "Failed to open file\n");
        return false;
    }

    for ( int i = 0; i < MAX_CAPACITY; i++ )
    {
        err = fscanf( fd, "%[^\n]\n", line_buffer);
        if ( err == EOF ) break;
        if ( err < 0 || err >= BUFFER_SIZE )
        {
            return false;
        }
        
        fsa_add_line( &fsa, line_buffer);
    }

    fclose( fd );
    free( line_buffer );

    return true;
}

bool create_fsa(file_string_array_t * fsa)
{
    fsa->capacity = (size_t) INIT_CAPACITY;
    fsa->count = (size_t) 0;

    char ** line_array_buffer = (char **) calloc( sizeof(char *), fsa->capacity );

    if ( line_array_buffer == NULL )
    {
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
            return false;
        }

        char ** line_array_buffer = (char **) realloc( fsa->line_array, new_capacity * sizeof(char *) );
        if ( line_array_buffer == NULL )
        {
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
        return false;
    }

    // Copy the string from the stack into the allocated string buffer
    snprintf( str_buffer, sizeof(char*) * (line_len + 1), "%s", str );

    // Set the char* in the line array to point to the new string buffer
    fsa->line_array[fsa->count] = str_buffer;
    fsa->count++;

    return true;
}