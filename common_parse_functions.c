#include "funcs.h"
#include "common_parse_functions.h"



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
