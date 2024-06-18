#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "funcs.h"


////////////////////////////////////////////////////////////////////////////////
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
//  parse_functions - Symbol parser helper functions.
//
//  stream - The output from the poeration command.
//
//  print_entries - print switch determines if the parse detail are printed.
//
////////////////////////////////////////////////////////////////////////////////
void read_data(const parse_functions_t *const parse_functions,
                FILE * stream, bool print_entries)
{
    size_t bufsize = READ_BUFSIZE;
    int line_char_count = 0;
    symbol_def_t *s_table_ptr;
    char *line_bufptr = NULL;

    line_bufptr = malloc(READ_BUFSIZE);

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

    s_table_ptr->header.line_bufptr = line_bufptr;
    s_table_ptr->header.line_char_count = line_char_count;

    while (line_char_count != -1) {
        int i=0;
        char *tmp_bufptr;

        s_table_ptr->header.head = parse_functions->head;

        // Parse a ctag line from file
        tmp_bufptr = s_table_ptr->header.line_bufptr;
        if (parse_functions->line_schema[0].delimiter) {
           do {
              int symbol_idx = parse_functions->line_schema[i].symbol_idx;
              tmp_bufptr = strtok(tmp_bufptr,
                  parse_functions->line_schema[i].delimiter);
              *s_table_ptr->symbol[symbol_idx] =
                  (void*)parse_functions->line_schema[i].parse_function(tmp_bufptr);
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
        line_bufptr = malloc(READ_BUFSIZE);
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
        s_table_ptr->header.line_bufptr = line_bufptr;
        s_table_ptr->header.line_char_count = line_char_count;
    }
}


bool run_parse(const parse_functions_t *const parse_functions,
               const bool print_list)
{
    FILE *output;
    char command[120];

    // Clear the function symbols linked list.
    parse_functions->dealloc_function();

    snprintf(command, sizeof(command),
             parse_functions->command_string,
             parse_functions->target_name);

    printf("\n");
    output = popen(command, "r");
    if (!output) {
        fprintf(stderr, "incorrect parameters or too many files.\n");
        return false;
    }

    read_data(parse_functions, output, print_list);
    pclose(output);

    return true;
}


#define VAR_LEN 80
int main(int argc, char **argv)
{
  char var_target[VAR_LEN];
  // bool find_variables = false;
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
          vars_parse_functions.target_name = argv[1];
      }
      // Default to "read_data" if no symbol name is given.
      else {
          vars_parse_functions.target_name = "read_data";
      }
//      find_variables = true;

      run_parse(&vars_parse_functions,
                true);

  }
  // Called as the "funcs" application.
  // Search in the given file for a list of symbols (functions).
  else {

      // Use the filename from the command line if present.
      if (2 <= argc) {
          funcs_parse_functions.target_name = argv[1];
      }
      // Default to "funcs.c" if no filename is given
      else {
          funcs_parse_functions.target_name = "funcs.c";
      }

     run_parse(&funcs_parse_functions, true);


     char c;
     while ('\n' != (c = getchar())) {

       index = (index * 10) + (uint8_t)c - 0x30;

//       find_variables = true;
     }


//     if (false == find_variables) {
//         funcs_parse_functions.dealloc_function();
//         return EXIT_SUCCESS;
//     }

     s_table_in_target = get_symbol_table_indexed(&funcs_symbol_table_head, index);
     s_table_target = copy_s_table_data (s_table_in_target);

     // var_target will be used in the grep command below.
     strncpy(var_target, s_table_target->name, VAR_LEN);

  }

//********************************************************************************
// Run the grep command for the symbol selected above, in var_target
//********************************************************************************
  vars_parse_functions.target_name = var_target;
  run_parse(&vars_parse_functions, false);

  vars_ptr = vars_symbol_table_head;

//  printf("vars_ptr->filename: %s\n", vars_ptr->filename);

//********************************************************************************
// Parse the grep output
//********************************************************************************
   while (NULL != vars_ptr && vars_ptr->filename != NULL) {
 
       // For the first, and each unique filename in the vars linked list
       // get the function list for the given filename.
       if ((NULL == symbol_filename) ||
           (strcmp(symbol_filename, vars_ptr->filename) != 0)) {
           symbol_filename = vars_ptr->filename;
           funcs_parse_functions.target_name = symbol_filename;
  
           run_parse(&funcs_parse_functions, false);
  
           // Resetting the func_ptr head after rerunning the funcs
           // parse with new filename.
           funcs_ptr = funcs_symbol_table_head;
  
  
       }
  
       // Resetting the func_ptr head the last run funcs parse
       funcs_ptr = funcs_symbol_table_head;
  
       while (NULL != funcs_ptr) {
  
           if ((vars_ptr->linenum == funcs_ptr->linenum)) { // &&
               printf("\n%d) Defined here:\n", vars_ptr->header.index);
               funcs_parse_functions.reference_print_function(funcs_ptr);
               vars_parse_functions.print_function(vars_ptr);
               vars_ptr->sym_type = funcs_ptr->sym_type;
               sym_type_to_find = funcs_ptr->sym_type;
           }
   
           if (funcs_ptr->linenum < vars_ptr->linenum &&
               (NULL == funcs_ptr->header.next ||
               funcs_ptr->header.next->linenum > vars_ptr->linenum)) {
   
               if (func == sym_type_to_find) {
                   printf("\n%d) Called by:\n", vars_ptr->header.index);
               }
               else {
                   printf("\n%d) Referenced here:\n", vars_ptr->header.index);
               }
   
           }
           else {
               funcs_ptr = funcs_ptr->header.next;
               continue;
           }
   
           funcs_parse_functions.reference_print_function(funcs_ptr);
           printf(" :\n");
           vars_parse_functions.reference_print_function(vars_ptr);
           funcs_ptr = funcs_symbol_table_head;
           break;
       }
  
       vars_ptr = vars_ptr->header.next;
       printf("\n\n");
       if (vars_ptr) {
          printf("%s: vars_ptr->filename: %s\n", __func__, vars_ptr->filename);
       }
       else {
          printf("%s: vars_ptr->filename: NULL\n", __func__);
       }
   }

   char c;
   index = 0;
   while ('\n' != (c = getchar())) {

     index = (index * 10) + (uint8_t)c - 0x30;
   }
   if (0 < index) {
       s_table_in_target = get_symbol_table_indexed(&vars_symbol_table_head, index);
       vars_parse_functions.reference_print_function(s_table_in_target);


       int pid = fork();
       if (0 == pid) {

           char linenum_cmd[8];
           char find_cmd[48];

           snprintf(linenum_cmd, sizeof(linenum_cmd),
                    "+%ld",
                    s_table_in_target->linenum);

           snprintf(find_cmd, sizeof(find_cmd),
                    "/%s",
                    s_table_in_target->name);
           printf("%s\n", find_cmd);
           execlp("vim", "vim", "-R", s_table_in_target->filename, linenum_cmd, "-c", find_cmd, NULL);
       } else if (-1 != pid)
       {
           wait(NULL);
       }
   }
   printf("\n");



   if (s_table_target) {
      deallocate_symbol_table(&s_table_target);
   }
   return EXIT_SUCCESS;
}

