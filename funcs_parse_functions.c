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
    .dealloc_function = deallocate_funcs_symbol,
    .line_schema = {{.symbol_idx = name_idx,
                       .delimiter = " ",
                       .parse_function = parse_default},
                      {.symbol_idx = symbol_type_idx,
                       .delimiter = " ",
                       .parse_function = parse_funcs_symbol_type},
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
//    return ((symbol->sym_type != func) && (symbol->sym_type != var));
    return ((symbol->sym_type != func)); // &&
//            (symbol->sym_type != var)  &&
//            (symbol->sym_type != strct)  &&
//            (symbol->sym_type != tdef)  &&
//            (symbol->sym_type != macro));
}

void deallocate_funcs_symbol(void)
{
    deallocate_symbol(&funcs_symbol_list_head);
}

void print_vars_file_reference_line(struct symbol_def *symbol)
{
    printf("%ld: %s\n", symbol->linenum, symbol->prototype);
//    printf("%d\t%s\n", symbol->linenum, symbol->prototype);

}


void print_funcs_file_symbols_line(struct symbol_def *symbol)
{
//    if (symbol->sym_type == func) {
//        printf("%d\t%s\n", symbol->linenum, symbol->name);
//        printf("%d: %s\n\t%s\n\tfilename: %s \tlinenum: %ld\n\n",
//                symbol->header.index,
//                symbol->name,
//                symbol->prototype,
//                symbol->filename,
//                symbol->linenum);
        printf("%d: %s\t%s:%ld\n\t%s\n\n",
                symbol->header.index,
                symbol->name,
                symbol->filename,
                symbol->linenum,
                symbol->prototype);
//        printf("\nFile: %s\n%d: \nFunction \n%ld: %s", symbol->filename, symbol->index, symbol->linenum, symbol->prototype);
//    }
}


void print_funcs_file_reference_line(struct symbol_def *symbol)
{
//    if (symbol->sym_type == func) {
//        printf("%d\t%s\n", symbol->linenum, symbol->name);
//        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", symbol->index, symbol->name, symbol->prototype, symbol->filename, symbol->linenum);
        printf("%s\n%ld: %s\n",
                symbol->filename,
                symbol->linenum,
                symbol->prototype);
//    }
}


// The schema array and the operation functions determine the parse type, in this case a "funcs" parse.
struct symbol_def *allocate_funcs_symbol() {

   struct symbol_def *symbol_ptr;

   symbol_ptr = malloc(sizeof(struct symbol_def));

   if (NULL == symbol_ptr) {
       return NULL;
   }
//   symbol_ptr = allocate_symbol(&funcs_symbol_list_head);
   symbol_ptr->symbol[name_idx]        = (void**)&symbol_ptr->name;
   symbol_ptr->symbol[filename_idx]    = (void**)&symbol_ptr->filename;
   symbol_ptr->symbol[prototype_idx]   = (void**)&symbol_ptr->prototype;
   symbol_ptr->symbol[symbol_type_idx] = (void**)&symbol_ptr->sym_type;
   symbol_ptr->symbol[linenum_idx]     = (void**)&symbol_ptr->linenum;
   symbol_ptr->symbol[null_term_idx]   = NULL;
#if 0
   symbol_ptr->line_schema[name_f_idx]      = (struct line_schema) {.symbol = (void**)&symbol_ptr->name,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_default};
   symbol_ptr->line_schema[filename_f_idx]  = (struct line_schema) {.symbol = (void**)&symbol_ptr->filename,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_default};
   symbol_ptr->line_schema[prototype_f_idx] = (struct line_schema) {.symbol = (void**)&symbol_ptr->prototype,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_proto_string};
   symbol_ptr->line_schema[symboltype_f_idx]  = (struct line_schema) {.symbol = (void**)&symbol_ptr->sym_type,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_funcs_symbol_type};
   symbol_ptr->line_schema[linenum_f_idx]   = (struct line_schema) {.symbol = (void**)&symbol_ptr->linenum,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_funcs_line_number};
   symbol_ptr->line_schema[null_term_f_idx] = (struct line_schema) {.symbol = NULL,
                                                          .delimiter = NULL,
                                                          .parse_function = NULL};
#endif

#if 0
   symbol_ptr->print_function = print_funcs_file_symbols_line;
   symbol_ptr->reference_print_function = print_funcs_file_reference_line;
   symbol_ptr->skip_function = skip_funcs_symbol;
   symbol_ptr->dealloc_function = deallocate_funcs_symbol;
   symbol_ptr->head = &funcs_symbol_list_head;
   return symbol_ptr;
#endif
   return symbol_ptr;

}

void print_funcs_file_symbolsymbol(struct symbol_def *symbol) {

    printf("name = %s\n", symbol->name);
    printf("file = %s\n", symbol->filename);
    printf("prototype = %s\n", symbol->prototype);
    switch (symbol->sym_type) {
        case macro:
            printf("symbol type = macro\n");
            break;

        case var:
            printf("symbol type = var\n");
            break;

        case func:
            printf("symbol type = func\n");
            break;

        case proto:
            printf("symbol type = proto\n");
            break;

        case strct:
            printf("symbol type = struct\n");
            break;

        case member:
            printf("symbol type = struct member\n");
            break;

        case enm:
            printf("symbol type = enum\n");
            break;

        case enmm:
            printf("symbol type = enum member\n");
            break;

        case tdef:
            printf("symbol type = typedef\n");
            break;

        default:
            printf("Unknown symbol type\n");

    };
    printf("line number = %ld\n", symbol->linenum);

}

