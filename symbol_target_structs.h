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
    symboltype_idx  = 3,
    symbol_enum_idx = 4,
    linenum_idx     = 5,
    null_term_idx   = 6,
    last_plus_one_idx = 7,
    last_plus_one = last_plus_one_idx,
    SYMBOL_LIST_LAST_PLUS_ONE = last_plus_one_idx
};

struct symbol_def;
//********************************************* 
// struct line_schema
//********************************************* 
// structure defining the fields of a line
// representing a symbol, output from ctags
//********************************************* 
struct line_schema {
   enum symbol_fields symbol_idx;
   // delimmiter for the given field in a line.
   char *delimiter;
   // parser function for the given field in a the line.
   void *(*parse_function)(char *bufptr);
};

struct parse_functions;
// helper function typedefs
typedef void (*print_file_symbols_function)(struct symbol_def *symbol);
typedef bool (*symbol_skip_function)(struct symbol_def *symbol);
typedef struct symbol_def *(*symbol_alloc_func_t)(void);
typedef void(*symbol_dealloc_func_t)(struct parse_functions);

//************************************************************ 
// struct parse_functions
//************************************************************ 
// Structure of parser helper functions used to parse each
// line of source code.
//************************************************************ 
struct parse_functions {
   // template string for the read file command.
   const char *command_string;

   // name of the symbol or file parsed.
   char *target_name;

   // linked list of symbols
   struct symbol_def **head;

   // schema defines the sections of the line output from the
   // read file operation.
   struct line_schema line_schema[last_plus_one];

   // helper functions
   const symbol_dealloc_func_t    dealloc_function;
   const symbol_alloc_func_t      alloc_function;
   const print_file_symbols_function    print_function;
   const print_file_symbols_function    reference_print_function;
   const symbol_skip_function           skip_function;

};

//******************************************* 
// struct symbol_def_hdr
//******************************************* 
// Linked list node struct with information
// about the line read from a source file.
//******************************************* 
struct symbol_def_hdr {
    struct symbol_def *next;
    struct symbol_def **head;
    uint32_t index;
    char *line_bufptr;
    uint8_t line_char_count;
};

#define READ_BUFSIZE 124
//******************************************************************************
// struct symbol_def
//******************************************************************************
// Symbol definition structure, contains symbol information parsed from a line 
// of ctags output, including symbol location. Each line of output from ctags
// repesents a symbol.
//******************************************************************************
struct symbol_def {
    struct symbol_def_hdr header;

    // Array of pointers to the symbol attribute members of symbol_def
    // The array elements are initializd in the alloc_XXX_symbol
    // function for the given parser type (funcs, vars).
    void **symbol[SYMBOL_LIST_LAST_PLUS_ONE];

    // Symbol attributes from the ctags output for the symbol.
    char *name;
    char *filename;
    char *prototype;
    char *symboltype;
    enum symboltype sym_type;
    uint64_t linenum;
    struct symbol_def *link_index;
};


struct symbol_def *get_symbol_indexed(struct symbol_def **symbol_list_head, const uint32_t index);
uint32_t append_symbol( struct symbol_def *symbol_ptr);
void deallocate_symbol_list( struct symbol_def **symbol_list_head);

#endif //!defined(__SYMBOL_TARGET_STRUCTS_H__)
