#include "funcs.h"

//*****************************************************************************
// Function: set_target_name
//
// Description: #include "common_parse_functions.h"
//
// Parameters: 
//
// Return: 
//
//*****************************************************************************
void set_target_name(struct parse_functions *const parser_functions,
                     const char *const target_name, const int name_length)
{
    char *new_name = NULL;

    new_name = malloc(name_length);
    if (NULL != new_name) {
        strncpy(new_name, target_name, name_length);
    }
    parser_functions->target_name = new_name;
}

//*****************************************************************************
// Function: free_target_name
//
// Description: }
//
// Parameters: 
//
// Return: 
//
//*****************************************************************************
void free_target_name(struct parse_functions *const parser_functions)
{
    if (NULL != parser_functions->target_name) {
        free(parser_functions->target_name);
    }
    parser_functions->target_name = NULL;
}

//*****************************************************************************
// Function: deallocate_parser
//
// Description: }
//
// Parameters: 
//
// Return: 
//
//*****************************************************************************
void deallocate_parser(struct parse_functions *const parser_functions)
{
    free_target_name(parser_functions);
    deallocate_symbol_list(parser_functions->head);
}

void * parse_default( char *bufptr )
{
   return (void *)bufptr;
}

void * parse_proto_string( char *bufptr )
{

    while (' ' == *bufptr) {
        bufptr++;
    }
    return (void *)bufptr;
}

#if 0
void * parse_proto_string( char *bufptr )
{
    char *ds_ptr = NULL;

    if ((NULL != (ds_ptr = strchr(bufptr, ';')))) {
        *(ds_ptr + 1) = '\0';
    }
    if ((NULL != (ds_ptr = strchr(bufptr, '$')))) {
        if (*(ds_ptr - 1) != ';') {
            *ds_ptr++ = ';';
        }
        *ds_ptr = '\0';
    }
    if (bufptr[1] == '^') {
        bufptr += 2;
    }

    return (void *)bufptr;
}
#endif
