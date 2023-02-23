


#include "symbol_target_structs.h"
#include "funcs_parse_functions.h"

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
    if (bufptr[1] == '^') {
        bufptr += 2;
    }

    return (void *)bufptr;
}

void *parse_symbol_type( char *bufptr )
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

   };

   return (void *)rval; 
}

void *parse_vars_line_number( char *bufptr )
{
   return (void *)strtol(bufptr, NULL, 10);
}
#if 0
void *parse_funcs_line_number( char *bufptr )
{
   return (void *)strtol(strchr(bufptr, ':')+1, NULL, 10);
}
#endif
void * parse_default( char *bufptr )
{
   return (void *)bufptr;
}

bool skip_vars_symbol(symbol_def_t *s_table)
{
    return false;
}
#if 0
bool skip_funcs_symbol(symbol_def_t *s_table)
{
    return ((s_table->sym_type != func) && (s_table->sym_type != var));
}
#endif

void print_vars_file_symbols_line(symbol_def_t *s_table)
{
    printf("%ld\t%s\t%s\n", s_table->linenum, s_table->filename, s_table->prototype);
//    printf("%d\t%s\n", s_table->linenum, s_table->prototype);

}

#if 0
void print_vars_file_reference_line(symbol_def_t *s_table)
{
    printf("%ld: %s\n", s_table->linenum, s_table->prototype);
//    printf("%d\t%s\n", s_table->linenum, s_table->prototype);

}
#endif

#if 0
void print_funcs_file_symbols_line(symbol_def_t *s_table)
{
//    if (s_table->sym_type == func) {
//        printf("%d\t%s\n", s_table->linenum, s_table->name);
        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", s_table->index, s_table->name, s_table->prototype, s_table->filename, s_table->linenum);
//        printf("\nFile: %s\n%d: \nFunction \n%ld: %s", s_table->filename, s_table->index, s_table->linenum, s_table->prototype);
//    }
}
#endif

#if 0
void print_funcs_file_reference_line(symbol_def_t *s_table)
{
//    if (s_table->sym_type == func) {
//        printf("%d\t%s\n", s_table->linenum, s_table->name);
//        printf("%d: %s\n\t%s\n\tSource File: %s \tReference Function Line Number: %ld\n\n", s_table->index, s_table->name, s_table->prototype, s_table->filename, s_table->linenum);
        printf("%s\n%ld: %s\n", s_table->filename, s_table->linenum, s_table->prototype);
//    }
}
#endif

//symbol_def_t *funcs_symbol_table_head = NULL;
symbol_def_t *vars_symbol_table_head = NULL;

symbol_def_t *get_symbol_table_indexed(symbol_def_t **symbol_table_head, const uint32_t index) {

   symbol_def_t *s_table = NULL;

   if (NULL != *symbol_table_head) {
       symbol_def_t *ptr = *symbol_table_head;


       // Walk the list starting at head->next until the 
       // last element, whose next pointer is NULL
       while ((index != ptr->index) && (NULL != ptr->next)) {
           ptr = ptr->next;
       }

       if (index == ptr->index) {
           s_table = ptr;
       }
   }
    
//   printf("%s:  %s\n", __func__, s_table->name);

   return s_table;
}





//symbol_def_t *allocate_symbol_table( symbol_def_t **symbol_table_head ) {
uint32_t append_symbol_table( symbol_def_t *s_table_ptr) {

//    symbol_def_t *s_table_ptr = NULL;
     uint32_t index = 0;

//    s_table_ptr = malloc(sizeof(symbol_def_t));
    if (NULL != s_table_ptr) {
        s_table_ptr->next = NULL;

        // If first element, set head.
        if (NULL == *s_table_ptr->head) {
            s_table_ptr->index = 0;
            *s_table_ptr->head = s_table_ptr;
        }
        // else add to tail.
        else {
            symbol_def_t *ptr = *s_table_ptr->head;

            // index at head->next is 1.
            index = 1;  

            // Walk the list starting at head->next until the 
            // last element, whose next pointer is NULL
            while (NULL != ptr->next) {
                ptr = ptr->next;
                index++;
            }

            // Set the index for the added element.
            s_table_ptr->index = index;
            ptr->next = s_table_ptr;
        }
    }
    s_table_ptr->sym_type = invalid_type;
    return index;
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
        if (NULL != prev_ptr->line_bufptr) {
           free(prev_ptr->line_bufptr);
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

// The schema array and the operation functions determine the parse type, in this case a "funcs" parse.
symbol_def_t *allocate_funcs_symbol_table() {

   symbol_def_t *s_table_ptr;

   s_table_ptr = malloc(sizeof(symbol_def_t));
//   s_table_ptr = allocate_symbol_table(&funcs_symbol_table_head);

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
                                                          .parse_function = parse_funcs_line_number};
   s_table_ptr->line_schema[null_term_f_idx] = (line_schema_t) {.symbol = NULL,
                                                          .delimiter = NULL,
                                                          .parse_function = NULL};
   s_table_ptr->print_function = print_funcs_file_symbols_line;
   s_table_ptr->reference_print_function = print_funcs_file_reference_line;
   s_table_ptr->skip_function = skip_funcs_symbol;
   s_table_ptr->dealloc_function = deallocate_funcs_symbol_table;
   s_table_ptr->head = &funcs_symbol_table_head;
   return s_table_ptr;

}

// The schema array and the operation functions determine the parse type, in this case a "vars" parse.
symbol_def_t *allocate_vars_symbol_table() {

   symbol_def_t *s_table_ptr;

   s_table_ptr = malloc(sizeof(symbol_def_t));
//   s_table_ptr = allocate_symbol_table(&vars_symbol_table_head);

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
   s_table_ptr->reference_print_function = print_vars_file_reference_line;
   s_table_ptr->skip_function = skip_vars_symbol;
   s_table_ptr->dealloc_function = deallocate_vars_symbol_table;
   s_table_ptr->head = &vars_symbol_table_head;
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

    };
    printf("line number = %ld\n", s_table->linenum);

}

symbol_def_t *copy_s_table_data(symbol_def_t *s_table_in_ptr) 
{
    symbol_def_t *s_table_out_ptr;

    if (NULL == s_table_in_ptr) {
        return NULL;
    }
   
    s_table_out_ptr = malloc(sizeof(symbol_def_t));

    if (NULL == s_table_out_ptr) {
        return NULL;
    }

    memset(s_table_out_ptr, 0, sizeof(*s_table_out_ptr));


    s_table_out_ptr->line_bufptr = malloc(s_table_in_ptr->line_char_count);
    s_table_out_ptr->line_char_count = s_table_in_ptr->line_char_count;
    memcpy(s_table_out_ptr->line_bufptr, s_table_in_ptr->line_bufptr, s_table_in_ptr->line_char_count);
//    strncpy(s_table_out_ptr->line_bufptr, s_table_in_ptr->line_bufptr, s_table_in_ptr->line_char_count);

    s_table_out_ptr->name = (char *)s_table_out_ptr->line_bufptr + ((char *)s_table_in_ptr->name - (char *)s_table_in_ptr->line_bufptr),
#if 0
printf("%s: %p, %p, %p, %p\n", __func__, s_table_out_ptr->line_bufptr, s_table_out_ptr->name, s_table_in_ptr->line_bufptr, s_table_in_ptr->name);
printf("%s: in - %s\n", __func__, s_table_in_ptr->name);
printf("%s: out - %s\n", __func__, s_table_out_ptr->name);
#endif
    s_table_out_ptr->filename = s_table_out_ptr->line_bufptr + (s_table_in_ptr->filename - s_table_in_ptr->line_bufptr);
    s_table_out_ptr->prototype = s_table_out_ptr->line_bufptr + ((char *)s_table_in_ptr->prototype - (char *)s_table_in_ptr->line_bufptr),
    s_table_out_ptr->sym_type = s_table_out_ptr->sym_type = s_table_in_ptr->sym_type;
    s_table_out_ptr->linenum = s_table_in_ptr->linenum;
    s_table_out_ptr->print_function = print_funcs_file_symbols_line;
    s_table_out_ptr->reference_print_function = print_funcs_file_reference_line;

    return s_table_out_ptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// The read_data function is the common file parser function.
//
//  The s_tablet_alloc function determines if a "vars" or "funcs"
//  parse is performed.
//
//  Parsed lines are stored in a symbol_def_t object linked list
//  starting at funcs_symbol_table_head or vars_symbol_table_head 
//  depending on the parse type, as determined by the s_table_alloc
//  function.
//
//  s_table_alloc - Pointer to the function which allocates the s_table list elements.
//
//  stream - The output from the poeration command.
//
//  print_entries - print switch determines if the parse detail are printed.
//
//////////////////////////////////////////////////////////////////////////////////////////
void read_data (symbol_table_alloc_func_t s_table_alloc, FILE * stream, bool print_entries)
{
    size_t bufsize = BUFSIZE;
    int line_char_count = 0;
    symbol_def_t *s_table_ptr;
    char *line_bufptr = NULL;

    line_bufptr = malloc(BUFSIZE);

    line_char_count = getline(&line_bufptr, &bufsize, stream);

    if (line_char_count > 250) {
        printf("line_char_count = %d\n", line_char_count);
    }

    // todo: handle all error cases for getline.
    if (-1 == line_char_count) {
       free (line_bufptr);         
       return;
    }

    // allocate the first list element.
    s_table_ptr = s_table_alloc();

    if (NULL == s_table_ptr) {
        return;
    }

    s_table_ptr->line_bufptr = line_bufptr;
    s_table_ptr->line_char_count = line_char_count;

    while (line_char_count != -1) {
        int i=0;
        char *tmp_bufptr;

        // Parse a ctag line from file
        tmp_bufptr = s_table_ptr->line_bufptr;
        if (s_table_ptr->line_schema[0].delimiter) {
           do {

              tmp_bufptr = strtok(tmp_bufptr, s_table_ptr->line_schema[i].delimiter); 
              *s_table_ptr->line_schema[i].symbol = (void*)s_table_ptr->line_schema[i].parse_function(tmp_bufptr); 
              tmp_bufptr = NULL;
           } while (s_table_ptr->line_schema[++i].delimiter);
        }
        if (!s_table_ptr->skip_function(s_table_ptr)) {
            append_symbol_table(s_table_ptr);

            if (print_entries) {
                // Print the table of ctag elemente.
                s_table_ptr->print_function(s_table_ptr);
            }
        }


        // Set up for the next ctag line.
        line_bufptr = malloc(BUFSIZE);
        line_char_count = getline(&line_bufptr, &bufsize, stream);

        // todo: handle all error cases for getline.
        if (-1 == line_char_count) {
           free (line_bufptr);         
           break;
        }
        s_table_ptr = s_table_alloc();
        if (NULL == s_table_ptr) {
            break;
        }
        s_table_ptr->line_bufptr = line_bufptr;
        s_table_ptr->line_char_count = line_char_count;
    }
//    s_table_ptr->dealloc_function();    
}


bool run_parse(const symbol_table_alloc_func_t alloc_func,
               const symbol_table_dealloc_func_t dealloc_func,
               const char *const parse_string,
               const char *const symbol_filename,
               const bool print_list)
{
    FILE *output;
    char command[120];

    // Clear the function symbols linked list.
    dealloc_func();
//    printf("\n\n********%s\n", parse_string);

    snprintf(command, sizeof(command), parse_string, symbol_filename);
//    printf("%s\n%s/t/t%s\n", command, parse_string, symbol_filename);

    printf("\n");
    output = popen (command, "r");
    if (!output) {
        fprintf (stderr, "incorrect parameters or too many files.\n");
        return false;
    }

    read_data (alloc_func, output, print_list);
    pclose (output);

    return true;
}


#define VAR_LEN 80
int
main (int argc, char **argv)
{
  FILE *output;
  char command[120];
  const char *vars_command_string = "grep --include=*.c -IRn %s *"; 
  const char *funcs_command_string = "echo %s | ctags --sort=no --c-kinds=+p --filter=yes --fields=nk";
  char *filetoparse = NULL;
//  char var_target[VAR_LEN];
//  char filename_target[VAR_LEN];
  char *var_target;
  char *filename_target;
  enum symboltype sym_type_target; 
  uint64_t linenum_target;
  print_file_symbols_function target_print_function = print_funcs_file_symbols_line;
  bool find_variables = false;
  symbol_table_alloc_func_t alloc_func;
  symbol_table_dealloc_func_t dealloc_func;
  symbol_def_t *vars_ptr;
  symbol_def_t *funcs_ptr;
  char *symbol_filename = NULL;
  enum symboltype sym_type_to_find = invalid_type;
  symbol_def_t *s_table_target;
  symbol_def_t *s_table_in_target;
  uint32_t index = 0;

  // Called as the "vars" application.
  // Search files in the current directory for a given symbol name.
  if (strstr(argv[0], "vars") != NULL) {

      // Use the symbol name from the command line if present.
      if (2 >= argc) {
          strncpy(var_target, argv[1], VAR_LEN);
      }
      // Default to "read_data" if no symbol name is given.
      else {
          strncpy(var_target, "read_data", VAR_LEN);
      }
      find_variables = true;

      // The alloc function builds a symbol table struct and populates the schema function array.
      alloc_func = allocate_vars_symbol_table;
      dealloc_func = deallocate_vars_symbol_table;

      //  -u to turn off sort, 
      snprintf(command, sizeof(command), "grep --include=*.c -IRn %s *", &var_target[0]);

     output = popen (command, "r");
     if (!output) {
         printf ("incorrect parameters or too many files.\n");
         return EXIT_FAILURE;
     }
//  write_data (output);

     read_data (alloc_func, output, true);

  }
  // Called as the "funcs" application.
  // Search in the given file for a list of symbols (functions).
  else {

      // Use the filename from the command line if present.
      if (2 <= argc) {
          filetoparse = argv[1];
      }
      // Default to "funcs.c" if no filename is given
      else {
          filetoparse = "funcs.c";
      }

     run_parse(allocate_funcs_symbol_table,
               deallocate_funcs_symbol_table,
               funcs_command_string,
               filetoparse, true); 


     char c;
     while ('\n' != (c = getchar())) {

       index = (index * 10) + (uint8_t)c - 0x30;

       find_variables = true;
     }


     if (false == find_variables) {
         dealloc_func();
         return EXIT_SUCCESS;
     }

//     symbol_def_t *s_table = get_symbol_table_indexed(&funcs_symbol_table_head, index);
//     s_table_target = get_symbol_table_indexed(&funcs_symbol_table_head, index);
     s_table_in_target = get_symbol_table_indexed(&funcs_symbol_table_head, index);
//     printf("%s: s_table_in_target->name =  %s\n", __func__, s_table_in_target->name);
     s_table_target = copy_s_table_data (s_table_in_target);
//     s_table_target = copy_s_table_data (get_symbol_table_indexed(&funcs_symbol_table_head, index));

//     // var _target will be used in the grep command below.
     var_target = s_table_target->name;

//     // filenaame_target will be used to compare with the grep output below.
     filename_target = s_table_target->filename;
//printf("%s, %s\n", var_target, filename_target);


     linenum_target = s_table_target->linenum;
     sym_type_target = s_table_target->sym_type;
  }

//////////////////////////////////////////////////////////////////////////////////////
// Run the grep command for the sysbol selected above, in var_target
//
//
//////////////////////////////////////////////////////////////////////////////////////
  run_parse(allocate_vars_symbol_table,
            deallocate_vars_symbol_table,
            vars_command_string,
            var_target, false); 

  vars_ptr = vars_symbol_table_head;

  printf("vars_ptr->filename: %s\n", vars_ptr->filename);

//////////////////////////////////////////////////////////////////////////////////////
// Parse the grep output
//
//
//////////////////////////////////////////////////////////////////////////////////////
  while (NULL != vars_ptr && vars_ptr->filename != NULL) {

     // For the first, and each unique filename in the vars linked list
     // get the function list for the given filename.
     if ((NULL == symbol_filename) || (strcmp(symbol_filename, vars_ptr->filename) != 0)) {
        symbol_filename = vars_ptr->filename;

        run_parse(allocate_funcs_symbol_table,
                  deallocate_funcs_symbol_table,
                  funcs_command_string,
                  symbol_filename, false); 

        // Resestting the func_ptr head after rerunning the funcs parse with new filename
        funcs_ptr = funcs_symbol_table_head;


     }

     // Resestting the func_ptr head the last run funcs parse
     funcs_ptr = funcs_symbol_table_head;

     while (NULL != funcs_ptr) {

        if ((vars_ptr->linenum == funcs_ptr->linenum)) { // &&
            printf("\nDefined here:\n");
            funcs_ptr->reference_print_function(funcs_ptr);
            vars_ptr->print_function(vars_ptr);
            vars_ptr->sym_type = s_table_target->sym_type;
            sym_type_to_find = s_table_target->sym_type;
        }

        if (funcs_ptr->linenum < vars_ptr->linenum &&
            (NULL == funcs_ptr->next ||
            funcs_ptr->next->linenum > vars_ptr->linenum)) {

            if (func == sym_type_to_find) {
                printf("\nCalled by:\n");
            }
            else {
                printf("\nReferenced here:\n");
            }

        }
        else {
            funcs_ptr = funcs_ptr->next;
            continue; 
        }

        funcs_ptr->reference_print_function(funcs_ptr);
        printf(" :\n");
        vars_ptr->reference_print_function(vars_ptr);
        funcs_ptr = funcs_symbol_table_head;
        break;
     }

    vars_ptr = vars_ptr->next;
    printf("\n\n");
    if (vars_ptr) {
       printf("%s: vars_ptr->filename: %s\n", __func__, vars_ptr->filename);
    }
    else {
       printf("%s: vars_ptr->filename: NULL\n", __func__);
    }
  }
  if(s_table_target) {
    deallocate_symbol_table(&s_table_target);   
  }
  return EXIT_SUCCESS;
}

