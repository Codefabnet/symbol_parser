


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

struct symbol_def {
    char *name;
    char *filename;
    char *prototype;
    enum symboltype sym_type;
    int linenum;
};

void print_file_symbols_function(struct symbol_def *s_table) {

    if (s_table->sym_type == func) {
        printf("%d\t%s\n", s_table->linenum, s_table->name);
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
    }

}

void print_file_symbols_line(struct symbol_def *s_table) {


    printf("%d\t%s\n", s_table->linenum, s_table->name);

}

void print_file_symbols_table(struct symbol_def *s_table) {

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
uint32_t parse_proto_string( char *bufptr )
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

    return (uint32_t)bufptr;
}

uint32_t parse_symbol_type( char *bufptr )
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

   return rval; 
}

uint32_t parse_line_number( char *bufptr )
{
   return strtol(strchr(bufptr, ':')+1, NULL, 10);
}

uint32_t parse_default( char *bufptr )
{
   return (uint32_t)bufptr;
}

typedef struct line_schema_t {
   void **symbol;
   char *delimiter;
   uint32_t(*parse_function)(char *bufptr);
}line_schema;


#define BUFSIZE 120
char buf[BUFSIZE];

void read_data (FILE * stream)
{
    char *bufptr = &buf[0];
    size_t bufsize = BUFSIZE;
    int count = 0;
    struct symbol_def s_table;
    char *ds_ptr = NULL;

    count = getline(&bufptr, &bufsize, stream);

    line_schema funcs_schema[] = {
       {(void**)&s_table.name, "\t", parse_default},
       {(void**)&s_table.filename, "\t", parse_default},
       {(void**)&s_table.prototype, "\t", parse_proto_string},
       {(void**)&s_table.sym_type, "\t", parse_symbol_type},
       {(void**)&s_table.linenum, "\t", parse_line_number},
       {NULL, NULL, NULL}
    };

    while (count != -1) {
        int i=0;
//        printf("-----------------------------------------------\n");
//        printf("%d: %s\n", count, bufptr);

        if (funcs_schema[0].delimiter) {
           do {
              if (i != 0) {
                  bufptr = NULL;
              }
              bufptr = strtok(bufptr, funcs_schema[i].delimiter); 
              *funcs_schema[i].symbol = (void*)funcs_schema[i].parse_function(bufptr); 
           } while (funcs_schema[++i].delimiter);
        }
#if 0        
        // first field: name.    
        bufptr = strtok(bufptr, "\t"); 
        s_table.name = bufptr;
    //    printf("%s\n", bufptr);


        // second field: file.    
        bufptr = strtok(NULL, "\t"); 
        s_table.filename = bufptr;
    //    printf("%s\n", bufptr);


        // third field: prototype.    
        bufptr = strtok(NULL, "\t"); 
    //    printf("%s\n", bufptr);
#if 0
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
#endif
        s_table.prototype = (char *)parse_proto_string(bufptr);

        // fourth field: file.    
        bufptr = strtok(NULL, "\t"); 
        s_table.sym_type = parse_symbol_type(bufptr);
#if 0
        switch ((char)*bufptr) {
            case 'd':
                s_table.sym_type = macro;
                break;

            case 'v':
                s_table.sym_type = var;
                break;

            case 'f':
                s_table.sym_type = func;
                break;

            case 'p':
                s_table.sym_type = proto;
                break;

            case 's':
                s_table.sym_type = strct;
                break;

            case 'm':
                s_table.sym_type = member;
                break;

            case 'g':
                s_table.sym_type = enm;
                break;

            case 'e':
                s_table.sym_type = enmm;
                break;

       };
#endif


        // fifth field: file.    
        bufptr = strtok(NULL, "\t"); 
//        s_table.linenum = strtol(strchr(bufptr, ':')+1, NULL, 10);
        s_table.linenum = parse_line_number(bufptr);
//        print_file_symbols_table(&s_table);
#endif
        print_file_symbols_function(&s_table);
        count = getline(&bufptr, &bufsize, stream);
    }
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

