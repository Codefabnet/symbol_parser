#include "funcs.h"
#include "vars_parse_functions.h"

symbol_def_t *vars_symbol_table_head = NULL;
const char *vars_command_string = "grep --include=*.c -IRn %s *"; 

parse_functions_t vars_parser_functions = { 
    .command_string = "grep --include=*.c -IRn %s *",
    .alloc_function = allocate_vars_symbol_table,
    .dealloc_function = deallocate_vars_symbol_table
};


void *parse_vars_line_number( char *bufptr )
{
   return (void *)strtol(bufptr, NULL, 10);
}

bool skip_vars_symbol(symbol_def_t *s_table)
{
    return false;
}

void print_vars_file_symbols_line(symbol_def_t *s_table)
{
    printf("%ld\t%s\t%s\n", s_table->linenum, s_table->filename, s_table->prototype);
//    printf("%d\t%s\n", s_table->linenum, s_table->prototype);

}


void deallocate_vars_symbol_table(void)
{
    deallocate_symbol_table(&vars_symbol_table_head);
}


// The schema array and the operation functions determine the parse type, in this case a "vars" parse.
symbol_def_t *allocate_vars_symbol_table(void)
{

   symbol_def_t *s_table_ptr;

   s_table_ptr = malloc(sizeof(symbol_def_t));
//   s_table_ptr = allocate_symbol_table(&vars_symbol_table_head);

   s_table_ptr->line_schema[filename_v_idx]      = (line_schema_t) {.symbol = (void**)&s_table_ptr->filename,
                                                          .delimiter =  ":",
                                                          .parse_function = parse_default};
   s_table_ptr->line_schema[linenum_v_idx]   = (line_schema_t) {.symbol = (void**)&s_table_ptr->linenum,
                                                          .delimiter = ":",
                                                          .parse_function = parse_vars_line_number};
   s_table_ptr->line_schema[prototype_v_idx] = (line_schema_t) {.symbol = (void**)&s_table_ptr->prototype,
                                                          .delimiter = "\n",
                                                          .parse_function = parse_proto_string};
   s_table_ptr->line_schema[null_term_v_idx] = (line_schema_t) {.symbol = NULL,
                                                          .delimiter = NULL,
                                                          .parse_function = NULL};
   s_table_ptr->print_function = print_vars_file_symbols_line;
   s_table_ptr->reference_print_function = print_vars_file_reference_line;
   s_table_ptr->skip_function = skip_vars_symbol;
   s_table_ptr->dealloc_function = deallocate_vars_symbol_table;
   s_table_ptr->head = &vars_symbol_table_head;
   return s_table_ptr;

}

