#include "symbol_target_structs.h"
extern symbol_def_t *vars_symbol_table_head;
extern const char *const vars_command_string;

extern parse_functions_t vars_parse_functions;


void *parse_vars_line_number( char *bufptr );
bool skip_vars_symbol(symbol_def_t *s_table);
void print_vars_file_symbols_line(symbol_def_t *s_table);
void deallocate_vars_symbol_table(void);
symbol_def_t *allocate_vars_symbol_table(void);
