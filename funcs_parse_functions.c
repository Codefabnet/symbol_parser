#include "funcs_parse_functions.h"

symbol_def_t *funcs_symbol_table_head = NULL;


void *parse_funcs_line_number( char *bufptr )
{
   return (void *)strtol(strchr(bufptr, ':')+1, NULL, 10);
}


bool skip_funcs_symbol(symbol_def_t *s_table)
{
    return ((s_table->sym_type != func) && (s_table->sym_type != var));
}


void print_vars_file_reference_line(symbol_def_t *s_table)
{
    printf("%ld: %s\n", s_table->linenum, s_table->prototype);
//    printf("%d\t%s\n", s_table->linenum, s_table->prototype);

}


void print_funcs_file_symbols_line(symbol_def_t *s_table)
{
//    if (s_table->sym_type == func) {
//        printf("%d\t%s\n", s_table->linenum, s_table->name);
        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", s_table->index, s_table->name, s_table->prototype, s_table->filename, s_table->linenum);
//        printf("\nFile: %s\n%d: \nFunction \n%ld: %s", s_table->filename, s_table->index, s_table->linenum, s_table->prototype);
//    }
}


void print_funcs_file_reference_line(symbol_def_t *s_table)
{
//    if (s_table->sym_type == func) {
//        printf("%d\t%s\n", s_table->linenum, s_table->name);
//        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", s_table->index, s_table->name, s_table->prototype, s_table->filename, s_table->linenum);
        printf("%s\n%ld: %s\n", s_table->filename, s_table->linenum, s_table->prototype);
//    }
}

