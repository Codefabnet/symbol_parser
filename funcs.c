

#include "funcs.h"

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
//void read_data (symbol_table_alloc_func_t s_table_alloc, FILE * stream, bool print_entries)
void read_data (const parse_functions_t *const parse_functions, FILE * stream, bool print_entries)
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
    s_table_ptr = parse_functions->alloc_function();

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
        if (parse_functions->line_schema[0].delimiter) {
           do {

              tmp_bufptr = strtok(tmp_bufptr, parse_functions->line_schema[i].delimiter); 
              s_table_ptr->name = (void*)parse_functions->line_schema[i].parse_function(tmp_bufptr); 
              tmp_bufptr = NULL;
           } while (parse_functions->line_schema[++i].delimiter);
        }
        if (!parse_functions->skip_function(s_table_ptr)) {
            append_symbol_table(s_table_ptr);

            if (print_entries) {
                // Print the table of ctag elemente.
                parse_functions->print_function(s_table_ptr);
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
        s_table_ptr = parse_functions->alloc_function();
        if (NULL == s_table_ptr) {
            break;
        }
        s_table_ptr->line_bufptr = line_bufptr;
        s_table_ptr->line_char_count = line_char_count;
    }
//    s_table_ptr->dealloc_function();    
}


bool run_parse(const parse_functions_t *const parse_functions,
               const char *const symbol_filename,
               const bool print_list)
{
    FILE *output;
    char command[120];

    // Clear the function symbols linked list.
    parse_functions->dealloc_function();
//    printf("\n\n********%s\n", parse_string);

    snprintf(command, sizeof(command), parse_functions->command_string, symbol_filename);
//    printf("%s\n%s/t/t%s\n", command, parse_string, symbol_filename);

    printf("\n");
    output = popen (command, "r");
    if (!output) {
        fprintf (stderr, "incorrect parameters or too many files.\n");
        return false;
    }

    read_data (parse_functions, output, print_list);
    pclose (output);

    return true;
}


#define VAR_LEN 80
int
main (int argc, char **argv)
{
  FILE *output;
  char command[120];
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

     read_data (&vars_parser_functions, output, true);

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

     run_parse(&funcs_parser_functions,
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
  run_parse(&vars_parser_functions,
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

        run_parse(&funcs_parser_functions,
                  symbol_filename, false); 

        // Resestting the func_ptr head after rerunning the funcs parse with new filename
        funcs_ptr = funcs_symbol_table_head;


     }

     // Resestting the func_ptr head the last run funcs parse
     funcs_ptr = funcs_symbol_table_head;

     while (NULL != funcs_ptr) {

        if ((vars_ptr->linenum == funcs_ptr->linenum)) { // &&
            printf("\nDefined here:\n");
            funcs_parser_functions.reference_print_function(funcs_ptr);
            vars_parser_functions.print_function(vars_ptr);
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

        funcs_parser_functions.reference_print_function(funcs_ptr);
        printf(" :\n");
        vars_parser_functions.reference_print_function(vars_ptr);
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

