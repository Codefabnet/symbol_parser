#if !defined(__SYMBOL_TARGET_STRUCTS_H__)
#define __SYMBOL_TARGET_STRUCTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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


enum funcs_fields {
    name_f_idx        = 0,
    filename_f_idx    = 1,
    prototype_f_idx   = 2,
    symboltype_f_idx  = 3,
    linenum_f_idx     = 4,
    null_term_f_idx   = 5,
    last_plus_one_f_idx = 6,
    last_plus_one = last_plus_one_f_idx
};

enum vars_fields {
    filename_v_idx    = 0,
    linenum_v_idx     = 1,
    prototype_v_idx   = 2,
    null_term_v_idx   = 3,
    last_plus_one_v_idx = 4
};

typedef struct symbol_def symbol_def_t;

typedef struct line_schema {
//   void **symbol;
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
   const line_schema_t line_schema[last_plus_one];
   const symbol_table_dealloc_func_t dealloc_function;
   const symbol_table_alloc_func_t alloc_function;
   const print_file_symbols_function print_function;
   const print_file_symbols_function reference_print_function;
   const symbol_skip_function skip_function;

} parse_functions_t;




#define BUFSIZE 256
//struct symbol_def {
typedef struct symbol_def {
    symbol_def_t *next;
    symbol_def_t **head;
    uint32_t index;
    char *line_bufptr;
    uint8_t line_char_count;
    line_schema_t line_schema[last_plus_one];
    char *name;
    char *filename;
    char *prototype;
    enum symboltype sym_type;
    uint64_t linenum;
//    print_file_symbols_function print_function;
//    print_file_symbols_function reference_print_function;
//    symbol_skip_function skip_function;
//    symbol_table_dealloc_func_t dealloc_function;
}symbol_def_t;
//};



symbol_def_t *get_symbol_table_indexed(symbol_def_t **symbol_table_head, const uint32_t index);
uint32_t append_symbol_table( symbol_def_t *s_table_ptr);
void deallocate_symbol_table( symbol_def_t **symbol_table_head);
symbol_def_t *copy_s_table_data(symbol_def_t *s_table_in_ptr); 

#endif //!defined(__SYMBOL_TARGET_STRUCTS_H__)
