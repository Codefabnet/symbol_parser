


#include <stdio.h>
#include <stdlib.h>
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
    }

}

void print_file_symbols_line(struct symbol_def *s_table) {


    printf("%d\t%s\t\t%s\n", s_table->linenum, s_table->name, s_table->prototype);

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


    while (count != -1) {
//        printf("%d: %s\n", count, bufptr);
//        printf("%s\n", strtok(bufptr, ":"));

        bufptr = strtok(bufptr, ":"); 
        s_table.name = bufptr;
//        printf("name = %s\n", s_table.name);

//        printf("%s\n", strtok(NULL, ":"));
//        printf("%d\n", s_table.linenum = strtol(strtok(NULL, ":"), NULL, 10));

        bufptr = strtok(NULL, ":");
        s_table.linenum = strtol(bufptr, NULL, 10);
//        printf("line number = %d\n", s_table.linenum);

        bufptr = strtok(NULL, "\n"); 
        s_table.prototype = bufptr;
//        printf("prototype = %s\in", s_table.prototype);
        print_file_symbols_line(&s_table);
#if 0
        int i=0;
//        printf("-----------------------------------------------\n");
//        printf("%d: %s\n", count, bufptr);

        // first field: name.    
        bufptr = strtok(bufptr, "\t"); 
        s_table.name = bufptr;
    //    printf("%s\n", bufptr);


        // second field: file.    
        bufptr = strtok(NULL, "\t"); 
        s_table.filename = bufptr;
    //    printf("%s\n", bufptr);


        // thrid field: prototype.    
        bufptr = strtok(NULL, "\t"); 
    //    printf("%s\n", bufptr);
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
        s_table.prototype = bufptr;

        // fourth field: file.    
        bufptr = strtok(NULL, "\t"); 
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


        // fifth field: file.    
        bufptr = strtok(NULL, "\t"); 
        s_table.linenum = strtol(strchr(bufptr, ':')+1, NULL, 10);
//        print_file_symbols_table(&s_table);
        print_file_symbols_function(&s_table);
#endif
        count = getline(&bufptr, &bufsize, stream);
    }
}


int
main (int argc, char **argv)
{
  FILE *output;
  char command[120];
  char *vartofind;

  if (2 >= argc) {
      vartofind = argv[1];
  }
  else {
      vartofind = "funcs.c";
  }
  //  -u to turn off sort, 
  snprintf(command, sizeof(command), "grep --include=*.c -IRn %s *", vartofind);
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

