#include "symbol_target_structs.h"

void set_target_name(struct parse_functions *const parser_functions,
        const char *const target_name, const int name_length);
void free_target_name(struct parse_functions *const parser_functions);
void deallocate_parser(struct parse_functions *const parser_functions);
void * parse_default( char *bufptr );
void * parse_proto_string( char *bufptr );
void read_data(const struct parse_functions *const parse_functions,
                FILE * stream, const bool print_entries);
bool run_parse(const struct parse_functions *const parse_functions,
               const bool print_list);
void run_vim(struct symbol_def *symbol_in_target);
struct symbol_def *get_symbol_selection(struct parse_functions *parse_functions);
void find_symbols();
