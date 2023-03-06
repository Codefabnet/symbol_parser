#if !defined(__SYMBOL_TARGET_STRUCTS_H__)
#define __SYMBOL_TARGET_STRUCTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum symboltype {
    macro = 0,  // d
    var,    // v
    func,   // f
    proto,  // p
    strct,  // s
    enm,    // g
    member, // m
    enmm,   // e
    tdef,   // t
    invalid_type
};


enum symbol_fields {
    name_idx        = 0,
    filename_idx    = 1,
    prototype_idx   = 2,
    symbol_type_idx = 3,
    linenum_idx     = 4,
    null_term_idx   = 5,
    last_plus_one_idx = 6,
    last_plus_one = last_plus_one_idx,
    SYMBOL_LIST_LAST_PLUS_ONE = last_plus_one_idx
};

typedef struct symbol_def symbol_def_t;

typedef struct line_schema {
   enum symbol_fields symbol_idx;
   char *delimiter;
   void *(*parse_function)(char *bufptr);
}line_schema_t;

typedef void (*print_file_symbols_function)(symbol_def_t *s_table);
typedef bool (*symbol_skip_function)(symbol_def_t *s_table);
typedef symbol_def_t *(*symbol_table_alloc_func_t)(void);
typedef void(*symbol_table_dealloc_func_t)(void);

typedef struct parse_functions {

   const char *command_string;
   symbol_def_t **head;
   line_schema_t line_schema[last_plus_one];
   const symbol_table_dealloc_func_t dealloc_function;
   const symbol_table_alloc_func_t alloc_function;
   const print_file_symbols_function print_function;
   const print_file_symbols_function reference_print_function;
   const symbol_skip_function skip_function;

} parse_functions_t;



typedef struct symbol_def_hdr {
    symbol_def_t *next;
    symbol_def_t **head;
    uint32_t index;
    char *line_bufptr;
    uint8_t line_char_count;
} symbol_def_hdr_t;

#define BUFSIZE 256
typedef struct symbol_def {
    symbol_def_hdr_t header;
// todo: use struct offsets in bytes to access symbol addresses directly.
    // Symbol address lookup array to convert from symnol indexes in the
    // parser_functions' line_schema to struct member addresses in the
    // symbol_def.
    // The array elements are initializd in the alloc_XXX_symbol_table
    // function for the given parser type (funcs, vatrs).
    void **symbol[SYMBOL_LIST_LAST_PLUS_ONE];
//    size_t **symbol[SYMBOL_LIST_LAST_PLUS_ONE];
    char *name;
    char *filename;
    char *prototype;
    enum symboltype sym_type;
    uint64_t linenum;
}symbol_def_t;


symbol_def_t *get_symbol_table_indexed(symbol_def_t **symbol_table_head, const uint32_t index);
uint32_t append_symbol_table( symbol_def_t *s_table_ptr);
void deallocate_symbol_table( symbol_def_t **symbol_table_head);
symbol_def_t *copy_s_table_data(symbol_def_t *s_table_in_ptr); 

#endif //!defined(__SYMBOL_TARGET_STRUCTS_H__)
