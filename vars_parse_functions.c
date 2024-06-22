#include "funcs.h"
#include "vars_parse_functions.h"

// struct symbol_def list head pointer for vars_parse_functions struct.
struct symbol_def *vars_symbol_list_head = NULL;

// Command string to grep for all instances of a given symbol.
const char *const vars_command_string = "grep --include=*.h --include=*.c -IRnw %s *"; 

// Helper functions, data structs and schema for vars parser.
struct parse_functions vars_parse_functions = { 
    .command_string = vars_command_string,
    .head = &vars_symbol_list_head,
    .alloc_function = allocate_vars_symbol,
    .dealloc_function = deallocate_vars_symbol,
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
// Description: Determines if the given struct symbol_def struct should be appended
//              to the symbols list for the parser.
//
// Parameters: symbol - Pointer the symbol definition to be skipped or not.
//
// Return: True if the append should be skipped, otherwise False.
//
//*****************************************************************************
bool skip_vars_symbol(struct symbol_def *symbol)
{
    // TODO: Run "gcc -E -fpreprocessed" on the file and look for prototype
    // in the output to skip in comment and conditional compile blocks.

    // Skip if the symbol is part of a single line comment.
    const char *comment_str = "//";
    char *comment_ptr = NULL;

    comment_ptr = strstr(symbol->prototype, comment_str);
    if (comment_ptr && comment_ptr < strstr(symbol->prototype, symbol->name)) {
        return true;
    }
    return false;
}

//*****************************************************************************
// Function: print_vars_file_symbols_line
//
// Description: Print information about the given struct symbol_def.
//
// Parameters: symbol - struct symbol_def to print.
//
// Return: void
//
//*****************************************************************************

void print_vars_file_symbols_line(struct symbol_def *symbol)
{
    printf("%ld\t%s\t%s\n",
            symbol->linenum,
            symbol->filename,
            symbol->prototype);
//    printf("%d\t%s\n", symbol->linenum, symbol->prototype);

}


//*****************************************************************************
// Function: deallocate_vars_symbol
//
// Description: Calls the common deallocate_symbol() function to free
//              the struct symbol_def list for the vars parser.
//
// Parameters: void
//
// Return: void
//
//*****************************************************************************

void deallocate_vars_symbol(void)
{
    deallocate_symbol(&vars_symbol_list_head);
}


//*****************************************************************************
// Function: allocate_vars_symbol
//
// Description: Malloc and initialize new struct symbol_def struct.
//
// Parameters: void
//
// Return: Pointer to new struct symbol_def.
//
//*****************************************************************************
struct symbol_def *allocate_vars_symbol(void)
{

   struct symbol_def *symbol_ptr;

   symbol_ptr = malloc(sizeof(struct symbol_def));

   if (NULL == symbol_ptr) {
       return NULL;
   }

   symbol_ptr->name = vars_parse_functions.target_name;
   symbol_ptr->symbol[name_idx]        = (void**)&symbol_ptr->name;
   symbol_ptr->symbol[filename_idx]    = (void**)&symbol_ptr->filename;
   symbol_ptr->symbol[prototype_idx]   = (void**)&symbol_ptr->prototype;
   symbol_ptr->symbol[symbol_enum_idx] = (void**)&symbol_ptr->sym_type;
   symbol_ptr->symbol[linenum_idx]     = (void**)&symbol_ptr->linenum;
   symbol_ptr->symbol[null_term_idx]   = NULL;

#if 0
   symbol_ptr->line_schema[filename_v_idx]      = (struct line_schema) {.symbol = (void**)&symbol_ptr->filename,
                                                          .delimiter =  ":",
                                                          .parse_function = parse_default};
   symbol_ptr->line_schema[linenum_v_idx]   = (struct line_schema) {.symbol = (void**)&symbol_ptr->linenum,
                                                          .delimiter = ":",
                                                          .parse_function = parse_vars_line_number};
   symbol_ptr->line_schema[prototype_v_idx] = (struct line_schema) {.symbol = (void**)&symbol_ptr->prototype,
                                                          .delimiter = "\n",
                                                          .parse_function = parse_proto_string};
   symbol_ptr->line_schema[null_term_v_idx] = (struct line_schema) {.symbol = NULL,
                                                          .delimiter = NULL,
                                                          .parse_function = NULL};
#endif
#if 0
   symbol_ptr->print_function = print_vars_file_symbols_line;
   symbol_ptr->reference_print_function = print_vars_file_reference_line;
   symbol_ptr->skip_function = skip_vars_symbol;
   symbol_ptr->dealloc_function = deallocate_vars_symbol;
   symbol_ptr->head = &vars_symbol_head;
#endif
   return symbol_ptr;

}

