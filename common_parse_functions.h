#include "symbol_target_structs.h"

void set_target_name(struct parse_functions *const parser_functions,
        const char *const target_name, const int name_length);
void free_target_name(struct parse_functions *const parser_functions);
void deallocate_parser(struct parse_functions *const parser_functions);
void * parse_default( char *bufptr );
void * parse_proto_string( char *bufptr );
