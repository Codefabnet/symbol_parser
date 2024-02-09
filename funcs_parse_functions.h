#include "symbol_target_structs.h"
extern symbol_def_t *funcs_symbol_table_head;
extern const char *const funcs_command_string;

extern parse_functions_t funcs_parse_functions;

void *parse_funcs_line_number( char *bufptr );
void *parse_funcs_symbol_type( char *bufptr );
symbol_def_t *allocate_funcs_symbol_table(void);
void deallocate_funcs_symbol_table(void);
bool skip_funcs_symbol(symbol_def_t *s_table);
void print_vars_file_reference_line(symbol_def_t *s_table);
void print_funcs_file_symbols_line(symbol_def_t *s_table);
void print_funcs_file_reference_line(symbol_def_t *s_table);
void print_funcs_file_symbols_table(symbol_def_t *s_table);

