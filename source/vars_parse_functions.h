#include "symbol_target_structs.h"
extern struct symbol_def *vars_symbol_list_head;
extern const char *const vars_command_string;

extern struct parse_functions vars_parse_functions;


void *parse_vars_line_number( char *bufptr );
bool skip_vars_symbol(struct symbol_def *s_table);
void print_vars_file_symbols_line(struct symbol_def *s_table);
void deallocate_vars_symbols(void);
struct symbol_def *allocate_vars_symbol(void);
