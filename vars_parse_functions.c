#include "funcs.h"
#include "vars_parse_functions.h"

// symbol_def_t list head pointer for vars_parse_functions struct.
symbol_def_t *vars_symbol_table_head = NULL;

// Command string to grep for all instances of a given symbol.
const char *const vars_command_string = "grep --include=*.h --include=*.c -IRnw %s *"; 

// Helper functions, data structs and schema for vars parser.
parse_functions_t vars_parse_functions = { 
    .command_string = vars_command_string,
    .head = &vars_symbol_table_head,
    .alloc_function = allocate_vars_symbol_table,
    .dealloc_function = deallocate_vars_symbol_table,
    .line_schema = {{.symbol_idx = filename_idx,
                       .delimiter = ":",
                       .parse_function = parse_default},
                      {.symbol_idx = linenum_idx,
                       .delimiter = ":",
                       .parse_function = parse_vars_line_number},
                      {.symbol_idx = prototype_idx,
                       .delimiter = "\n",
                       .parse_function = parse_proto_string},
                      {.symbol_idx = null_term_idx,
                       .delimiter = NULL,
                       .parse_function = NULL}},
    .print_function = print_vars_file_symbols_line,
    .reference_print_function = print_vars_file_reference_line,
    .skip_function = skip_vars_symbol
};


//*****************************************************************************
// Function: parse_vars_line_number
//
// Description: Convert the string at bufptr to an integer line number.
//
// Parameters: bufptr - Pointer to the line number of the symbol in the 
//                      grep output.
//
// Return: line number
//
//*****************************************************************************
void *parse_vars_line_number( char *bufptr )
{
   return (void *)strtol(bufptr, NULL, 10);
}

//*****************************************************************************
// Function: skip_vars_symbol
//
// Description: Determines if the given symbol_def_t struct should be appended
//              to the symbols list for the parser.
//
// Parameters: s_table - Pointer the symbol definition to be skipped or not.
//
// Return: True if the append should be skipped, otherwise False.
//
//*****************************************************************************
bool skip_vars_symbol(symbol_def_t *s_table)
{
    return false;
}

//*****************************************************************************
// Function: print_vars_file_symbols_line
//
// Description: Print information about the given symbol_def_t.
//
// Parameters: s_table - symbol_def_t to print.
//
// Return: void
//
//*****************************************************************************

void print_vars_file_symbols_line(symbol_def_t *s_table)
{
    printf("%ld\t%s\t%s\n",
            s_table->linenum,
            s_table->filename,
            s_table->prototype);
//    printf("%d\t%s\n", s_table->linenum, s_table->prototype);

}


//*****************************************************************************
// Function: deallocate_vars_symbol_table
//
// Description: Calls the common deallocate_symbol_table() function to free
//              the symbol_def_t list for the vars parser.
//
// Parameters: void
//
// Return: void
//
//*****************************************************************************

void deallocate_vars_symbol_table(void)
{
    deallocate_symbol_table(&vars_symbol_table_head);
}


//*****************************************************************************
// Function: allocate_vars_symbol_table
//
// Description: Malloc and initialize new symbol_def_t struct.
//
// Parameters: void
//
// Return: Pointer to new symbol_def_t.
//
//*****************************************************************************
symbol_def_t *allocate_vars_symbol_table(void)
{

   symbol_def_t *s_table_ptr;

   s_table_ptr = malloc(sizeof(symbol_def_t));

   if (NULL != s_table_ptr) {
       return NULL;
   }

   s_table_ptr->name = vars_parse_functions.target_name;
   s_table_ptr->symbol[name_idx]        = (void**)&s_table_ptr->name;
   s_table_ptr->symbol[filename_idx]    = (void**)&s_table_ptr->filename;
   s_table_ptr->symbol[prototype_idx]   = (void**)&s_table_ptr->prototype;
   s_table_ptr->symbol[symbol_type_idx] = (void**)&s_table_ptr->sym_type;
   s_table_ptr->symbol[linenum_idx]     = (void**)&s_table_ptr->linenum;
   s_table_ptr->symbol[null_term_idx]   = NULL;

#if 0
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
#endif
#if 0
   s_table_ptr->print_function = print_vars_file_symbols_line;
   s_table_ptr->reference_print_function = print_vars_file_reference_line;
   s_table_ptr->skip_function = skip_vars_symbol;
   s_table_ptr->dealloc_function = deallocate_vars_symbol_table;
   s_table_ptr->head = &vars_symbol_table_head;
#endif
   return s_table_ptr;

}

