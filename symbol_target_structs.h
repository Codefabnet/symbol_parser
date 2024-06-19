#if !defined(__SYMBOL_TARGET_STRUCTS_H__)
#define __SYMBOL_TARGET_STRUCTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// kinds of ctags
enum symboltype {
    macro = 0,  // d
    var,        // v
    func,       // f
    proto,      // p
    strct,      // s
    enm,        // g
    member,     // m
    enmm,       // e
    tdef,       // t
    invalid_type
};

// Line read from ctags output...
// |-symbol name-|-filename-|-prototype-|-symbol type-|-line number-|
// indexes into the the symbol array member of the symbol_def struct.
// Used to store pointers to the symbol attributes members of the 
// symbol_def struct.
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
//********************************************* 
// struct line_schema
//********************************************* 
// structure defining the fields of a line
// representing a symbol, output from ctags
//********************************************* 
typedef struct line_schema {
   enum symbol_fields symbol_idx;
   // delimmiter for the given field in a line.
   char *delimiter;
   // parser function for the given field in a the line.
   void *(*parse_function)(char *bufptr);
}line_schema_t;

// helper function typedefs
typedef void (*print_file_symbols_function)(symbol_def_t *s_table);
typedef bool (*symbol_skip_function)(symbol_def_t *s_table);
typedef symbol_def_t *(*symbol_table_alloc_func_t)(void);
typedef void(*symbol_table_dealloc_func_t)(void);

//************************************************************ 
// struct parse_functions
//************************************************************ 
// Structure of parser helper functions used to parse each
// line of source code.
//************************************************************ 
typedef struct parse_functions {
   // template string for the read file command.
   const char *command_string;

   // name of the symbol or file parsed.
   char *target_name;

   // linked list of symbols
   symbol_def_t **head;

   // schema defines the sections of the line output from the
   // read file operation.
   line_schema_t line_schema[last_plus_one];

   // helper functions
   const symbol_table_dealloc_func_t    dealloc_function;
   const symbol_table_alloc_func_t      alloc_function;
   const print_file_symbols_function    print_function;
   const print_file_symbols_function    reference_print_function;
   const symbol_skip_function           skip_function;

} parse_functions_t;

//******************************************* 
// struct symbol_def_hdr
//******************************************* 
// Linked list node struct with information
// about the line read from a source file.
//******************************************* 
typedef struct symbol_def_hdr {
    symbol_def_t *next;
    symbol_def_t **head;
    uint32_t index;
    char *line_bufptr;
    uint8_t line_char_count;
} symbol_def_hdr_t;

#define READ_BUFSIZE 256
//******************************************************************************
// struct symbol_def
//******************************************************************************
// Symbol definition structure, contains symbol information parsed from a line 
// of ctags output, including symbol location. Each line of output from ctags
// repesents a symbol.
//******************************************************************************
typedef struct symbol_def {
    symbol_def_hdr_t header;

    // Array of pointers to the symbol attribute members of symbol_def
    // The array elements are initializd in the alloc_XXX_symbol_table
    // function for the given parser type (funcs, vars).
    void **symbol[SYMBOL_LIST_LAST_PLUS_ONE];

    // Symbol attributes from the ctags output for the symbol.
    char *name;
    char *filename;
    char *prototype;
    enum symboltype sym_type;
    uint64_t linenum;
}symbol_def_t;


symbol_def_t *get_symbol_table_indexed(symbol_def_t **symbol_table_head, const uint32_t index);
uint32_t append_symbol_table( symbol_def_t *s_table_ptr);
void deallocate_symbol_table( symbol_def_t **symbol_table_head);

#endif //!defined(__SYMBOL_TARGET_STRUCTS_H__)
