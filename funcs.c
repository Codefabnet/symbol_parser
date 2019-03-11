


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void
write_data (FILE * stream)
{
/*    
  int i;
  for (i = 0; i < 100; i++)
    fprintf (stream, "%d\n", i);
  if (ferror (stream))
    {
      fprintf (stderr, "Output to stream failed.\n");
      exit (EXIT_FAILURE);
    }
*/

    fprintf (stream, "test.c\n");
}


enum symboltype {
    macro = 0,  // d
    var,    // v
    func,   // f
    proto,  // p
    strct,  // s
    enm,    // g
    member, // m
    enmm    // e
};

//char *parse_proto_string( char *bufptr )
void * parse_proto_string( char *bufptr )
{
    char *ds_ptr = NULL;

    if ((NULL != (ds_ptr = strchr(bufptr, ';')))) {
        *(ds_ptr + 1) = '\0';
    }
    if ((NULL != (ds_ptr = strchr(bufptr, '$')))) {
        if (*(ds_ptr - 1) != ';') {
            *ds_ptr++ = ';';
        }
        *ds_ptr = '\0';
    }
    if (bufptr[0] == '/') {
        bufptr += 2;
    }

    return (void *)bufptr;
}

 void * parse_symbol_type( char *bufptr )
{
   uint32_t rval = 0;

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

   };

   return (void *)rval; 
}

void *parse_vars_line_number( char *bufptr )
{
   return (void *)strtol(bufptr, NULL, 10);
}

void *parse_line_number( char *bufptr )
{
   return (void *)strtol(strchr(bufptr, ':')+1, NULL, 10);
}

void * parse_default( char *bufptr )
{
   return (void *)bufptr;
}

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
   void **symbol;
   char *delimiter;
   void *(*parse_function)(char *bufptr);
}line_schema_t;

typedef void (*print_file_symbols_function)(symbol_def_t *s_table);

typedef symbol_def_t *(*symbol_table_alloc_func_t)(void);
typedef void(*symbol_table_dealloc_func_t)(void);

#define BUFSIZE 128
struct symbol_def {
//typedef struct symbol_def {
    struct symbol_def *next;
    char *bufptr;
    uint8_t count;
    line_schema_t line_schema[last_plus_one];
    char *name;
    char *filename;
    char *prototype;
    enum symboltype sym_type;
    int linenum;
    print_file_symbols_function print_function;
    symbol_table_dealloc_func_t dealloc_function;
//}symbol_def_t;
};


void print_vars_file_symbols_line(symbol_def_t *s_table)
{
//    printf("%d\t%s\t%s\n", s_table->linenum, s_table->filename, s_table->prototype);
    printf("%d\t%s\n", s_table->linenum, s_table->prototype);

}

void print_funcs_file_symbols_line(symbol_def_t *s_table)
{
    if (s_table->sym_type == func) {
//        printf("%d\t%s\n", s_table->linenum, s_table->name);
        printf("%s, in file: %s\n", s_table->prototype, s_table->filename);
    }
}

symbol_def_t *funcs_symbol_table_head = NULL;
symbol_def_t *vars_symbol_table_head = NULL;

symbol_def_t *allocate_symbol_table( symbol_def_t **symbol_table_head ) {

    symbol_def_t *s_table_ptr = NULL;

    s_table_ptr = malloc(sizeof(symbol_def_t));
    if (NULL != s_table_ptr) {
        s_table_ptr->next = NULL;

        if (NULL == *symbol_table_head) {
            *symbol_table_head = s_table_ptr;
        }
        else {
            symbol_def_t *ptr = *symbol_table_head;
            while (ptr->next != NULL) {
                ptr = ptr->next;
            }
            ptr->next = s_table_ptr;
        }
    }
    return s_table_ptr;
}

void deallocate_symbol_table( symbol_def_t **symbol_table_head )
{

    symbol_def_t *ptr = NULL;
    symbol_def_t *prev_ptr = NULL;

    ptr = *symbol_table_head;

    while (NULL != ptr) {

        prev_ptr = ptr;
        ptr = ptr->next;

        // Deallocate the last table pointer.
        if (NULL != prev_ptr->bufptr) {
           free(prev_ptr->bufptr);
        }
        free(prev_ptr);
    }
    *symbol_table_head = NULL;
}

void deallocate_funcs_symbol_table(void)
{
    deallocate_symbol_table(&funcs_symbol_table_head);
}

void deallocate_vars_symbol_table(void)
{
    deallocate_symbol_table(&vars_symbol_table_head);
}

symbol_def_t *allocate_funcs_symbol_table() {

   symbol_def_t *s_table_ptr;

   s_table_ptr = allocate_symbol_table(&funcs_symbol_table_head);

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
                                                          .parse_function = parse_symbol_type};
   s_table_ptr->line_schema[linenum_f_idx]   = (line_schema_t) {.symbol = (void**)&s_table_ptr->linenum,
                                                          .delimiter = "\t",
                                                          .parse_function = parse_line_number};
   s_table_ptr->line_schema[null_term_f_idx] = (line_schema_t) {.symbol = NULL,
                                                          .delimiter = NULL,
                                                          .parse_function = NULL};
   s_table_ptr->print_function = print_funcs_file_symbols_line;
   s_table_ptr->dealloc_function = deallocate_funcs_symbol_table;
   return s_table_ptr;

}

symbol_def_t *allocate_vars_symbol_table() {

   symbol_def_t *s_table_ptr;

   s_table_ptr = allocate_symbol_table(&vars_symbol_table_head);

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
   s_table_ptr->print_function = print_vars_file_symbols_line;
   s_table_ptr->dealloc_function = deallocate_vars_symbol_table;
   return s_table_ptr;

}

void print_funcs_file_symbols_function(symbol_def_t *s_table) {

    if (s_table->sym_type == func) {
        printf("%d\t%s\n", s_table->linenum, s_table->name);
//        printf("%s\n", __FUNCTION__);
//        printf("%s\n", s_table->bufptr);
//        printf("%d\n", s_table->linenum);
//        printf("%s\n", s_table->name);
#if 0
        printf("%\t%s\n", s_table->prototype);
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

       };
#endif
    }

}
#if 0
void print_funcs_file_symbols_line(symbol_def_t *s_table) {


    printf("%d\t%s\n", s_table->linenum, s_table->name);

}
#endif
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

    };
    printf("line number = %d\n", s_table->linenum);

}


void read_data (symbol_table_alloc_func_t s_table_alloc, FILE * stream)
{
    size_t bufsize = BUFSIZE;
    int count = 0;
    symbol_def_t *s_table_ptr;
    char *bufptr = NULL;

    bufptr = malloc(BUFSIZE);
    count = getline(&bufptr, &bufsize, stream);
    if (-1 == count) {
       free (bufptr);         
       return;
    }
    s_table_ptr = s_table_alloc();
    if (NULL == s_table_ptr) {
        return;
    }
    s_table_ptr->bufptr = bufptr;
    s_table_ptr->count = count;

    while (count != -1) {
        int i=0;
        char *bufptr;

        bufptr = s_table_ptr->bufptr;
        if (s_table_ptr->line_schema[0].delimiter) {
           do {

              bufptr = strtok(bufptr, s_table_ptr->line_schema[i].delimiter); 
              *s_table_ptr->line_schema[i].symbol = (void*)s_table_ptr->line_schema[i].parse_function(bufptr); 
              bufptr = NULL;
           } while (s_table_ptr->line_schema[++i].delimiter);
        }
//        s_table_ptr->print_function(s_table_ptr);

        bufptr = malloc(BUFSIZE);
        count = getline(&bufptr, &bufsize, stream);
        if (-1 == count) {
           free (bufptr);         
           break;
        }
        s_table_ptr = s_table_alloc();
        if (NULL == s_table_ptr) {
            break;
        }
        s_table_ptr->bufptr = bufptr;
        s_table_ptr->count = count;
    }
//    s_table_ptr->dealloc_function();    
}


int
main (int argc, char **argv)
{
  FILE *output;
  char command[120];
  char *filetoparse = NULL;
  char *vartofind = NULL;
  bool find_variables = false;
  symbol_table_alloc_func_t alloc_func;
  symbol_table_dealloc_func_t dealloc_func;
  symbol_def_t *vars_ptr;
  symbol_def_t *funcs_ptr;
  char *symbol_filename = NULL;

  if (strstr(argv[0], "vars") != NULL) {
      if (2 >= argc) {
          vartofind = argv[1];
      }
      else {
          vartofind = "read_data";
      }
      find_variables = true;

      alloc_func = allocate_vars_symbol_table;
      dealloc_func = deallocate_vars_symbol_table;

      //  -u to turn off sort, 
      snprintf(command, sizeof(command), "grep --include=*.c -IRn %s *", vartofind);
  }
  else {
      if (2 >= argc) {
          filetoparse = argv[1];
      }
      else {
          filetoparse = "funcs.c";
      }

      alloc_func = allocate_funcs_symbol_table;
      dealloc_func = deallocate_funcs_symbol_table;

      snprintf(command, sizeof(command), "echo %s | ctags --sort=no --c-kinds=+p --filter=yes --fields=nk", filetoparse);
  }

  output = popen (command, "r");
  if (!output) {
      fprintf (stderr, "incorrect parameters or too many files.\n");
      return EXIT_FAILURE;
  }
//  write_data (output);

  read_data (alloc_func, output);

  if (pclose (output) != 0) {
      fprintf (stderr, "Could not run more or other error.\n");
  }

  if (false == find_variables) {
      dealloc_func();
      return EXIT_SUCCESS;
  }


  vars_ptr = vars_symbol_table_head;
  while (NULL != vars_ptr && vars_ptr->filename != NULL) {
     if (symbol_filename == NULL ||
        strcmp(symbol_filename, vars_ptr->filename) != 0) {
        symbol_filename = vars_ptr->filename;    

        alloc_func = allocate_funcs_symbol_table;
        dealloc_func = deallocate_funcs_symbol_table;

        deallocate_funcs_symbol_table();

        snprintf(command, sizeof(command), "echo %s | ctags --sort=no --c-kinds=+p --filter=yes --fields=nk", symbol_filename);

        output = popen (command, "r");
        if (!output) {
            fprintf (stderr, "incorrect parameters or too many files.\n");
            return EXIT_FAILURE;
        }

        read_data (alloc_func, output);
        pclose (output);

        funcs_ptr = funcs_symbol_table_head;
     }

     while (NULL != funcs_ptr) {
        
        if (vars_ptr->linenum == funcs_ptr->linenum ||
            (funcs_ptr->linenum < vars_ptr->linenum &&
            (NULL == funcs_ptr->next ||
            funcs_ptr->next->linenum > vars_ptr->linenum))) {

            if (func == vars_ptr->sym_type) {
               if (vars_ptr->linenum == funcs_ptr->linenum) {
                  printf("\nDefined here:\n"); 
               }
               else {
                  printf("\nCalled by:\n"); 
               }
            }
            else {
               printf("\nReferenced here:\n"); 
            }

            funcs_ptr->print_function(funcs_ptr);
            vars_ptr->print_function(vars_ptr);
            funcs_ptr = funcs_symbol_table_head;
            break;
        }
        funcs_ptr = funcs_ptr->next;
     }


    vars_ptr = vars_ptr->next;
  }      


  return EXIT_SUCCESS;
}

