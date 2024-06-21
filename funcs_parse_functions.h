#include "symbol_target_structs.h"
extern struct symbol_def *funcs_symbol_list_head;
extern const char *const funcs_command_string;

extern struct parse_functions funcs_parse_functions;

void *parse_funcs_line_number( char *bufptr );
void *parse_funcs_symbol_type( char *bufptr );
struct symbol_def *allocate_funcs_symbol(void);
void deallocate_funcs_symbol(void);
bool skip_funcs_symbol(struct symbol_def *s_table);
void print_vars_file_reference_line(struct symbol_def *s_table);
void print_funcs_file_symbols_line(struct symbol_def *s_table);
void print_funcs_file_reference_line(struct symbol_def *s_table);
void print_funcs_file_symbols_table(struct symbol_def *s_table);

