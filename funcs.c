


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

enum funcs_fields {
    name        = 0,
    filename    = 1,
    prototype   = 2,
    symboltype  = 3,
    linenum     = 4,
    last_plus_one = 5
};

#define BUFSIZE 120
struct symbol_def {
    struct symbol_def *next;
    char *bufptr;
    uint8_t count;
    char *name;
    char *filename;
    char *prototype;
    enum symboltype sym_type;
    int linenum;
};

typedef struct symbol_def symbol_def_t;

symbol_def_t *symbol_table_head = NULL;


symbol_def_t *allocate_symbol_table() {

    symbol_def_t *s_table_ptr = NULL;

    s_table_ptr = malloc(sizeof(symbol_def_t));
    if (NULL != s_table_ptr) {
        s_table_ptr->next = NULL;
        s_table_ptr->bufptr = malloc(BUFSIZE);

        if (NULL == symbol_table_head) {
            symbol_table_head = s_table_ptr;
        }
        else {
            symbol_def_t *ptr = symbol_table_head;
            while (ptr->next != NULL) {
                ptr = ptr->next;
            }
            ptr->next = s_table_ptr;
        }
    }
    return s_table_ptr;
}

void deallocate_symbol_table(void) {

    symbol_def_t *ptr = NULL;
    symbol_def_t *prev_ptr = NULL;

    if (NULL == symbol_table_head) {
        return;
    }
    ptr = symbol_table_head->next;

    // Look for the end of the list.
    while (NULL != ptr->next) {

        // If more than one table on list, keep track of the 2nd to the last ptr.
        // Use this to NULL terminate the list after deleting the last ptr.
        prev_ptr = ptr->next;
        ptr = ptr->next;
    }

    // If only one table on the list, NULL the head.
    if (ptr == symbol_table_head) {
        symbol_table_head = NULL;
    }

    // Deallocate the last table pointer.
    free(ptr->bufptr);
    free(ptr);

    // If there was more than one table on the list, prev_ptr will now point at the last
    // table on the list.
    if (NULL != prev_ptr) {

        // NULL terminate the list.
        prev_ptr->next = NULL;
    }
}

void print_file_symbols_function(symbol_def_t *s_table) {

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

void print_file_symbols_line(symbol_def_t *s_table) {


    printf("%d\t%s\n", s_table->linenum, s_table->name);

}

void print_file_symbols_table(symbol_def_t *s_table) {

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

   };

   return (void *)rval; 
}

void *parse_line_number( char *bufptr )
{
   return (void *)strtol(strchr(bufptr, ':')+1, NULL, 10);
}

void * parse_default( char *bufptr )
{
   return (void *)bufptr;
}

typedef struct line_schema_t {
   void **symbol;
   char *delimiter;
   void *(*parse_function)(char *bufptr);
}line_schema;



void read_data (FILE * stream)
{
    size_t bufsize = BUFSIZE;
    int count = 0;
    symbol_def_t *s_table_ptr;
    char *bufptr; // = &s_table.buffer[0];
    char *ds_ptr = NULL;

    s_table_ptr = allocate_symbol_table(bufptr);
    count = getline(&s_table_ptr->bufptr, &bufsize, stream);
    if (-1 == count) {
        deallocate_symbol_table();    
        return;
    }
    s_table_ptr->count = count;

    line_schema funcs_schema[] = {
       {(void**)&s_table_ptr->name, "\t", parse_default},
       {(void**)&s_table_ptr->filename, "\t", parse_default},
       {(void**)&s_table_ptr->prototype, "\t", parse_proto_string},
       {(void**)&s_table_ptr->sym_type, "\t", parse_symbol_type},
       {(void**)&s_table_ptr->linenum, "\t", parse_line_number},
       {NULL, NULL, NULL}
    };

    while (count != -1) {
        int i=0;

        if (funcs_schema[0].delimiter) {
           do {
              if (i != 0) {
                  bufptr = NULL;
              }
              else {
                  bufptr = s_table_ptr->bufptr;
              }
              bufptr = strtok(bufptr, funcs_schema[i].delimiter); 
              *funcs_schema[i].symbol = (void*)funcs_schema[i].parse_function(bufptr); 
           } while (funcs_schema[++i].delimiter);
        }
        print_file_symbols_function(s_table_ptr);

        s_table_ptr = allocate_symbol_table(bufptr);
        if (NULL == s_table_ptr) {
            break;
        }
        count = getline(&s_table_ptr->bufptr, &bufsize, stream);
        if (-1 != count) {
            s_table_ptr->count = count;

            funcs_schema[name].symbol       = (void**)&s_table_ptr->name;
            funcs_schema[filename].symbol   = (void**)&s_table_ptr->filename;
            funcs_schema[prototype].symbol  = (void**)&s_table_ptr->prototype;
            funcs_schema[symboltype].symbol = (void**)&s_table_ptr->sym_type;
            funcs_schema[linenum].symbol    = (void**)&s_table_ptr->linenum;

        }
        else {
            deallocate_symbol_table();    
        }

    }
    deallocate_symbol_table();    
}


int
main (int argc, char **argv)
{
  FILE *output;
  char command[120];
  char *filetoparse;

  if (2 >= argc) {
      filetoparse = argv[1];
  }
  else {
      filetoparse = "funcs.c";
  }
  //  -u to turn off sort, 
  snprintf(command, sizeof(command), "echo %s | ctags --sort=no --c-kinds=+p --filter=yes --fields=nk", filetoparse);
  output = popen (command, "r");
  if (!output) {
      fprintf (stderr, "incorrect parameters or too many files.\n");
      return EXIT_FAILURE;
  }
//  write_data (output);

  read_data (output);
  if (pclose (output) != 0) {
      fprintf (stderr, "Could not run more or other error.\n");
  }
  return EXIT_SUCCESS;
}

