#include "funcs.h"
#include "funcs_parse_functions.h"

// symbol_def_t list head pointer for funcs_parse_functions struct.
symbol_def_t *funcs_symbol_table_head = NULL;

// Command string to list all symbols (see man ctags - kinds) for a given file.
const char *const funcs_command_string = "echo %s | ctags --sort=no --c-kinds=+p --filter=yes --fields=nk";

// helper functions, data structs and schema for funcs parser.
parse_functions_t funcs_parse_functions = { 
    .command_string = funcs_command_string,
    .head = &funcs_symbol_table_head,
    .alloc_function = allocate_funcs_symbol_table,
    .dealloc_function = deallocate_funcs_symbol_table,
    .line_schema = {{.symbol_idx = name_idx,
                       .delimiter = "\t",
                       .parse_function = parse_default},
                      {.symbol_idx = filename_idx,
                       .delimiter = "\t",
                       .parse_function = parse_default},
                      {.symbol_idx = prototype_idx,
                       .delimiter = "\t",
                       .parse_function = parse_proto_string},
                      {.symbol_idx = symbol_type_idx,
                       .delimiter = "\t",
                       .parse_function = parse_funcs_symbol_type},
                      {.symbol_idx = linenum_idx,
                       .delimiter = "\t",
                       .parse_function = parse_funcs_line_number},
                      {.symbol_idx = null_term_idx,
                       .delimiter = NULL,
                       .parse_function = NULL}},
    .print_function = print_funcs_file_symbols_line,
    .reference_print_function = print_funcs_file_reference_line,
    .skip_function = skip_funcs_symbol
};

void *parse_funcs_line_number( char *bufptr )
{
   return (void *)strtol(strchr(bufptr, ':')+1, NULL, 10);
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


bool skip_funcs_symbol(symbol_def_t *s_table)
{
//    return ((s_table->sym_type != func) && (s_table->sym_type != var));
    return ((s_table->sym_type != func)); // &&
//            (s_table->sym_type != var)  &&
//            (s_table->sym_type != strct)  &&
//            (s_table->sym_type != tdef)  &&
//            (s_table->sym_type != macro));
}

void deallocate_funcs_symbol_table(void)
{
    deallocate_symbol_table(&funcs_symbol_table_head);
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
        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", s_table->header.index, s_table->name, s_table->prototype, s_table->filename, s_table->linenum);
//        printf("\nFile: %s\n%d: \nFunction \n%ld: %s", s_table->filename, s_table->index, s_table->linenum, s_table->prototype);
//    }
}


void print_funcs_file_reference_line(symbol_def_t *s_table)
{
//    if (s_table->sym_type == func) {
//        printf("%d\t%s\n", s_table->linenum, s_table->name);
//        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", s_table->index, s_table->name, s_table->prototype, s_table->filename, s_table->linenum);
        printf("%s\n%ld: %s\n",
                s_table->filename,
                s_table->linenum,
                s_table->prototype);
//    }
}


// The schema array and the operation functions determine the parse type, in this case a "funcs" parse.
symbol_def_t *allocate_funcs_symbol_table() {

   symbol_def_t *s_table_ptr;

   s_table_ptr = malloc(sizeof(symbol_def_t));

   if (NULL == s_table_ptr) {
       return NULL;
   }
//   s_table_ptr = allocate_symbol_table(&funcs_symbol_table_head);
   s_table_ptr->symbol[name_idx]        = (void**)&s_table_ptr->name;
   s_table_ptr->symbol[filename_idx]    = (void**)&s_table_ptr->filename;
   s_table_ptr->symbol[prototype_idx]   = (void**)&s_table_ptr->prototype;
   s_table_ptr->symbol[symbol_type_idx] = (void**)&s_table_ptr->sym_type;
   s_table_ptr->symbol[linenum_idx]     = (void**)&s_table_ptr->linenum;
   s_table_ptr->symbol[null_term_idx]   = NULL;
#if 0
   s_table_ptr->line_schema[name_f_idx]      = (line_schema_t) {.symbol = (void**)&s_table_ptr->name,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_default};
   s_table_ptr->line_schema[filename_f_idx]  = (line_schema_t) {.symbol = (void**)&s_table_ptr->filename,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_default};
   s_table_ptr->line_schema[prototype_f_idx] = (line_schema_t) {.symbol = (void**)&s_table_ptr->prototype,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_proto_string};
   s_table_ptr->line_schema[symboltype_f_idx]  = (line_schema_t) {.symbol = (void**)&s_table_ptr->sym_type,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_funcs_symbol_type};
   s_table_ptr->line_schema[linenum_f_idx]   = (line_schema_t) {.symbol = (void**)&s_table_ptr->linenum,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_funcs_line_number};
   s_table_ptr->line_schema[null_term_f_idx] = (line_schema_t) {.symbol = NULL,
                                                          .delimiter = NULL,
                                                          .parse_function = NULL};
#endif

#if 0
   s_table_ptr->print_function = print_funcs_file_symbols_line;
   s_table_ptr->reference_print_function = print_funcs_file_reference_line;
   s_table_ptr->skip_function = skip_funcs_symbol;
   s_table_ptr->dealloc_function = deallocate_funcs_symbol_table;
   s_table_ptr->head = &funcs_symbol_table_head;
   return s_table_ptr;
#endif
   return s_table_ptr;

}

void print_funcs_file_symbols_table(symbol_def_t *s_table) {

    printf("name = %s\n", s_table->name);
    printf("file = %s\n", s_table->filename);
    printf("prototype = %s\n", s_table->prototype);
    switch (s_table->sym_type) {
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
    printf("line number = %ld\n", s_table->linenum);

}

