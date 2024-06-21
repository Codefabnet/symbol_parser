#if !defined(__FUNCS_H__)
#define __FUNCS_H__
#include "symbol_target_structs.h"
#include "funcs_parse_functions.h"
#include "vars_parse_functions.h"
#include "common_parse_functions.h"

void * parse_proto_string( char *bufptr );
void deallocate_symbol( struct symbol_def **symbol_list_head );

#if 0
static inline
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



#endif // !defined(__FUNCS_H__)

