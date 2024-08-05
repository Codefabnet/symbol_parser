#include "funcs.h"
#include "funcs_parse_functions.h"

// struct symbol_def list head pointer for funcs_parse_functions struct.
struct symbol_def *funcs_symbol_list_head = NULL;

// Command string to list all symbols (see man ctags - kinds) for a given file.
const char *const funcs_command_string = "echo %s | ctags --sort=no --c-kinds=+p --filter=yes --fields=nk -x";

// helper functions, data structs and schema for funcs parser.
struct parse_functions funcs_parse_functions = {
    .command_string = funcs_command_string,
    .head = &funcs_symbol_list_head,
    .alloc_function = allocate_funcs_symbol,
    .dealloc_function = deallocate_parser,
    .line_schema = {{.symbol_idx = name_idx,
                       .delimiter = " ",
                       .parse_function = parse_default},
                      {.symbol_idx = symboltype_idx,
                       .delimiter = " ",
                       .parse_function = parse_default},
                      {.symbol_idx = linenum_idx,
                       .delimiter = " ",
                       .parse_function = parse_funcs_line_number},
                      {.symbol_idx = filename_idx,
                       .delimiter = " ",
                       .parse_function = parse_default},
                      {.symbol_idx = prototype_idx,
                       .delimiter = "\n",
                       .parse_function = parse_proto_string},
                      {.symbol_idx = null_term_idx,
                       .delimiter = NULL,
                       .parse_function = NULL}},
    .print_function = print_funcs_file_symbols_line,
    .reference_print_function = print_funcs_file_reference_line,
    .skip_function = skip_funcs_symbol
};

void *parse_funcs_line_number( char *bufptr )
{
//   return (void *)strtol(strchr(bufptr, ':')+1, NULL, 10);
   return (void *)strtol(bufptr, NULL, 10);
}

void *parse_funcs_symbol_type( char *bufptr )
{
   uint64_t rval = 0;

    switch ((char)*bufptr) {
        case 'd':
            rval = macro;
            break;

        case 'v':
            rval = var;
            break;

        case 'f':
            rval = func;
            break;

        case 'p':
            rval = proto;
            break;

        case 's':
            rval = strct;
            break;

        case 'm':
            rval = member;
            break;

        case 'g':
            rval = enm;
            break;

        case 'e':
            rval = enmm;
            break;

        case 't':
            rval = tdef;
            break;

        default:
            printf("Unhandled ctag kind: %c\n", (char)*bufptr);

   };

   return (void *)rval;
}


bool skip_funcs_symbol(struct symbol_def *symbol)
{
    symbol->sym_type = (enum symboltype)parse_funcs_symbol_type(symbol->symboltype);
//    return ((symbol->sym_type != func) && (symbol->sym_type != var));
    return ((symbol->sym_type != func) &&
            (symbol->sym_type != var)  &&
            (symbol->sym_type != proto)  &&
            (symbol->sym_type != strct)); //  &&
//            (symbol->sym_type != tdef)  &&
//            (symbol->sym_type != macro));
}


//*****************************************************************************
// Function: print_funcs_file_symbols_line
//
// Description: Print information about the given struct symbol_def.
//
// Parameters: symbol - struct symbol_def to print.
//
// Return: void
//
//*****************************************************************************
void print_funcs_file_symbols_line(struct symbol_def *symbol)
{
        printf("%d: %s - %s(%ld)\n\t%s\t%s\n\n",
                symbol->header.index,
                symbol->name,
                symbol->filename,
                symbol->linenum,
                symbol->symboltype,
                symbol->prototype);
}


//*****************************************************************************
// Function: print_funcs_file_reference_line
//
// Description: Print information about the given struct symbol_def.
//
// Parameters: symbol - struct symbol_def to print.
//
// Return: void
//
//*****************************************************************************
void print_funcs_file_reference_line(struct symbol_def *symbol)
{
        printf("%s\n%ld: %s\n",
                symbol->filename,
                symbol->linenum,
                symbol->prototype);
}


// The schema array and the operation functions determine the parse type, in this case a "funcs" parse.

//*****************************************************************************
// Function: allocate_funcs_symbol
//
// Description:  Malloc and initialize new symbol_def struct.
//
// Parameters: none
//
// Return: Pointer to new struct symbol_def.
//
//*****************************************************************************
struct symbol_def *allocate_funcs_symbol() {

   struct symbol_def *symbol_ptr;

   symbol_ptr = malloc(sizeof(struct symbol_def));

   if (NULL == symbol_ptr) {
       return NULL;
   }
   symbol_ptr->symbol[name_idx]        = (void**)&symbol_ptr->name;
   symbol_ptr->symbol[filename_idx]    = (void**)&symbol_ptr->filename;
   symbol_ptr->symbol[prototype_idx]   = (void**)&symbol_ptr->prototype;
   symbol_ptr->symbol[symboltype_idx]  = (void**)&symbol_ptr->symboltype;
   symbol_ptr->symbol[symbol_enum_idx] = (void**)&symbol_ptr->sym_type;
   symbol_ptr->symbol[linenum_idx]     = (void**)&symbol_ptr->linenum;
   symbol_ptr->symbol[null_term_idx]   = NULL;
   return symbol_ptr;

}

