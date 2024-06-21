#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "funcs.h"


//*****************************************************************************
// Function: read_data
//
// Description: The common file parser function.
//              The s_tablet_alloc function, of the given parse_function struct,
//              determines if a "vars" or "funcs" parse is performed.
//
//              Parsed lines are stored in the symbol_def_t list of the given
//              parse_functions struct.
//
// Parameters: parse_functions - Symbol parser helper functions, vars or funcs.
//
//             stream - The output from the file operation command.
//
//             print_entries - Print switch determines if the parse details
//                             are printed.
//
// Return: void
//
//*****************************************************************************
void read_data(const parse_functions_t *const parse_functions,
                FILE * stream, bool print_entries)
{
    size_t bufsize = READ_BUFSIZE;
    int line_char_count = 0;
    symbol_def_t *s_table_ptr;
    char *line_bufptr = NULL;

    line_bufptr = malloc(READ_BUFSIZE);

    if (NULL == line_bufptr) {
        printf("%s: out of memory\n", __FUNCTION__);
        funcs_parse_functions.dealloc_function();
        vars_parse_functions.dealloc_function();
        exit(EXIT_FAILURE);
    }

    // Get the output line to be parsed.
    line_char_count = getline(&line_bufptr, &bufsize, stream);

    if (line_char_count > 250) {
        printf("line_char_count = %d\n", line_char_count);
    }

    // TODO: handle all error cases for getline.
    if (-1 == line_char_count) {
       free (line_bufptr);
       return;
    }

    // allocate the first list element.
    s_table_ptr = parse_functions->alloc_function();

    if (NULL == s_table_ptr) {
        printf("%s: symbol_def_t alloc failed\n", __FUNCTION__);
        funcs_parse_functions.dealloc_function();
        vars_parse_functions.dealloc_function();
        exit(EXIT_FAILURE);
    }

    // Each symbol definition list entry has a pointer to the line
    // of output parsed.
    s_table_ptr->header.line_bufptr = line_bufptr;
    s_table_ptr->header.line_char_count = line_char_count;

    // Check for EOF/error while reading file operation output.
    while (line_char_count != -1) {
        int i=0;
        char *tmp_bufptr;

        s_table_ptr->header.head = parse_functions->head;

        // Parse a line of output from file operation.
        tmp_bufptr = s_table_ptr->header.line_bufptr;

        // Break up the line and populate symbol definition struct based on
        // the line schema array, one array element per section of the line.
        // |-symbol name-|-filename-|-prototype-|-symbol type-|-line number-|
        if (parse_functions->line_schema[0].delimiter) {
           do {
              // symbol_index gives the offset into the symbol array containing
              // a pointer to the symbol_def struct member used to store the
              // value from this line section.
              int symbol_idx = parse_functions->line_schema[i].symbol_idx;

              // Get the line section as defined by the delimiter in the schema.
              tmp_bufptr = strtok(tmp_bufptr,
                      parse_functions->line_schema[i].delimiter);

              // Store the value from this section as parsed by the function
              // pointed to in the schema.
              *s_table_ptr->symbol[symbol_idx] =
                  (void*)parse_functions->line_schema[i].parse_function(tmp_bufptr);

              // terminate the string for this section.
              tmp_bufptr = NULL;

             // The line schema ends with a NULL delimiter.
           } while (parse_functions->line_schema[++i].delimiter);
        }

        // Certain symbols may be skipped.
        if (!parse_functions->skip_function(s_table_ptr)) {

            // Append the symbol, just parsed, to the list in the
            // parser_functions struct.
            append_symbol_table(s_table_ptr);

            if (print_entries) {
                // Print the table of ctag element.
                parse_functions->print_function(s_table_ptr);
            }
        }


        // Set up for the next line of output from the file operation.
        line_bufptr = malloc(READ_BUFSIZE);

        if (NULL == line_bufptr)
        {
            printf("%s: out of memory\n", __FUNCTION__);
            funcs_parse_functions.dealloc_function();
            vars_parse_functions.dealloc_function();
            exit(EXIT_FAILURE);
        }

        line_char_count = getline(&line_bufptr, &bufsize, stream);

        // TODO: handle all error cases for getline.
        if (-1 == line_char_count) {
           free (line_bufptr);
           break;
        }

        // Get and initialize the next symbol_def list entry.
        s_table_ptr = parse_functions->alloc_function();
        if (NULL == s_table_ptr) {
            break;
        }
        s_table_ptr->header.line_bufptr = line_bufptr;
        s_table_ptr->header.line_char_count = line_char_count;
    }
}


//*****************************************************************************
// Function: run_parse
//
// Description: Runs the file operation, as defined by the command string
//              in the given parse functions srtuct. Call read_data to parse
//              the output.
//
// Parameters: parse_functions - Symbol parser helper functions, vars or funcs.
//
//             print_list - Print switch determines if the parse details
//                          are printed.
//
// Return: true if the file operation command successfully ran.
//
//*****************************************************************************
bool run_parse(const parse_functions_t *const parse_functions,
               const bool print_list)
{
    FILE *output;
    char command[120];

    // Clear the function symbols linked list.
    parse_functions->dealloc_function();

    // build the file operation command from the command string in the parse
    // functions struct.
    snprintf(command, sizeof(command),
             parse_functions->command_string,
             parse_functions->target_name);

    printf("\n");

    // run the file operation command.
    output = popen(command, "r");
    if (!output) {
        fprintf(stderr, "incorrect parameters or too many files.\n");
        return false;
    }

    // call read_data with the command output.
    read_data(parse_functions, output, print_list);
    pclose(output);

    return true;
}


//*****************************************************************************
// Function: run_vim
//
// Description: Uses vim to open the given filename at the given location, with
//              the given symbol name loaded into '/' search. Next search 'n'
//              will goto the next occurrence of the given symbol.
//
// Parameters: symbol_in_target - Symbol definition struct containing the
//                                filename, line number and symbol name used
//                                to run vim.
//
// Return: void
//
//*****************************************************************************
void run_vim(symbol_def_t *symbol_in_target)
{
   // Spawn the process to run the vim command in.
   int pid = fork();

   // Use the Child process for the command.
   if (0 == pid) {
       char linenum_cmd[8];
       char find_cmd[48];

       // Build the string containing the line number for the vim command.
       snprintf(linenum_cmd, sizeof(linenum_cmd),
                "+%ld",
                symbol_in_target->linenum);


       // Build the string containing the find command.
       snprintf(find_cmd, sizeof(find_cmd),
                "/%s",
                symbol_in_target->name);

       // Run the vim command in the new process.
       execlp("vim", "vim", "-R",
               symbol_in_target->filename,
               "-c",
               find_cmd,
               linenum_cmd,
               NULL);

   // If fork succeeded.
   } else if (-1 != pid)
   {
       // Wait for child to complete.
       wait(NULL);
   }
}


//*****************************************************************************
// Function: get_symbol_selection
//
// Description: Gets user input used to find the symbol definition struct
//              in the parse_functions list of symbols.
//
// Parameters: parse_functions - Pointer to a struct containing the relevant
//                               parsing information.
//
// Return: Pointer to the symbol definition struct, if a symbol was selected.
//
//*****************************************************************************
symbol_def_t *get_symbol_selection(parse_functions_t *parse_functions)
{
   char c;
   int index = 0;
   symbol_def_t *selected = NULL;

   while ('\n' != (c = getchar())) {

     index = (index * 10) + (uint8_t)c - 0x30;
   }
   if (0 != index) {
       selected = get_symbol_table_indexed(parse_functions->head, index);
   }
   return selected;
}

#define TARGET_NAME_SIZE 48
//*****************************************************************************
// Function: main
//
// Description: There are two build targets for this source:
//
//              vars - Takes the name of a symbol s a parameter and greps
//                     for all occurrences of the symbol name in the CWD.
//
//              funcs- Takes a filename as a parameter and uses ctags
//                     in filter mode to list symbols in the given file.
//                     A symbol can then be selected for the grep operation.
//
//              A list of locations for the given symbol is displayed. A
//              location can then be selected for opening in vim.
//
// Parameters: argc - count of arguments
//             argv[] - array of argument values
//
// Return: int status
//
//*****************************************************************************
int main(int argc, char **argv)
{
   parse_functions_t *parse_functions;
   symbol_def_t *vars_ptr;
   symbol_def_t *funcs_ptr;
   char *symbol_filename = NULL;
   bool select_symbol_from_file;
   symbol_def_t *selected;
   char target_name[TARGET_NAME_SIZE];
 
   // Called as the "vars" application.
   // Search files in the current directory for a given symbol name.
   if (strstr(argv[0], "vars") != NULL) {
 
       parse_functions = &vars_parse_functions;
       select_symbol_from_file = false;
   } else {
 
       parse_functions = &funcs_parse_functions;
       select_symbol_from_file = true;
   }
 
   if (2 == argc) {
       strncpy(target_name, argv[1], TARGET_NAME_SIZE);
       parse_functions->target_name = (char *)&target_name;
   }
   else {
       printf("No target selected\n");
       if (select_symbol_from_file) {
           printf("Add filename parameter for a list of symbols in that file.");
       } else {
           printf("Add symbol name parameter for locations where a symbol is used.");
       }
       printf("\n");
 
       return 1;
   }
 
   // TODO: not sure this makes sense for anything but funcs_parse_functions.
   if (select_symbol_from_file) {
 
      run_parse(parse_functions, true);
 
      // TODO: not sure this makes sense for anything but funcs_parse_functions.
      printf("Select a symbol in %s\n", parse_functions->target_name);
      selected = get_symbol_selection(parse_functions);
 
      if (NULL == selected) {
         funcs_parse_functions.dealloc_function();
         return EXIT_SUCCESS;
      }
 
      strncpy(target_name, selected->name, TARGET_NAME_SIZE);
      vars_parse_functions.target_name = (char *)&target_name;
      funcs_parse_functions.dealloc_function();
 
   }
 
   // List locations of the symbol selected above, in vars_parse_functions.name.

   // loop for symbols selected for edit until empty return is entered below.
   do {
       // Get the symbol list.
       run_parse(&vars_parse_functions, false);

       // Get first list entry
       vars_ptr = vars_symbol_table_head;

       enum symboltype sym_type_to_find = invalid_type;

       // Walk the list of locations for the given symbol.
       while (NULL != vars_ptr && vars_ptr->filename != NULL) {

           // For the first, and each unique filename in the vars linked list
           // get the list of symbols for the filename in current list entry.
           if ((NULL == symbol_filename) ||
               (strcmp(symbol_filename, vars_ptr->filename) != 0)) {
               symbol_filename = vars_ptr->filename;

               // Set the file to parse.
               funcs_parse_functions.target_name = symbol_filename;

               // Get all symbols for the given file.
               run_parse(&funcs_parse_functions, false);
           }

           // Get the first list entry for the symbols in the given file.
           funcs_ptr = funcs_symbol_table_head;

           // For all symbols in current file look for the given symbol.
           while (NULL != funcs_ptr) {

               // If the line number reported by ctags and grep are the
               // same, this is where the symbol is defined.
               if ((vars_ptr->linenum == funcs_ptr->linenum)) {
                   printf("\n%d) Defined here:\n", vars_ptr->header.index);
                   funcs_parse_functions.reference_print_function(funcs_ptr);
                   vars_parse_functions.print_function(vars_ptr);
                   vars_ptr->sym_type = funcs_ptr->sym_type;
                   sym_type_to_find = funcs_ptr->sym_type;
               }

               // Check for the given symbol between entries in the funcs list.
               if (funcs_ptr->linenum < vars_ptr->linenum &&
                   (NULL == funcs_ptr->header.next ||
                   funcs_ptr->header.next->linenum > vars_ptr->linenum)) {

                   // if the funcs list entry is a function, assume calling the
                   // given symbol.
                   if (func == sym_type_to_find) {
                       printf("\n%d) Called by:\n", vars_ptr->header.index);
                   }
                   // Otherwise a "reference."
                   else {
                       printf("\n%d) Referenced here:\n", vars_ptr->header.index);
                   }

               }
               // The given symbol is not in the range for the current funcs entry.
               else {
                   funcs_ptr = funcs_ptr->header.next;
                   continue;
               }

               funcs_parse_functions.reference_print_function(funcs_ptr);
               printf(" :\n");
               vars_parse_functions.reference_print_function(vars_ptr);

               // Go back to the first entry in the funcs list, break for the next
               // location for the given symbol.
               funcs_ptr = funcs_symbol_table_head;
               break;
           }

           // Get the list entry for the next location of the given symbol.
           vars_ptr = vars_ptr->header.next;
           printf("\n\n");
           if (vars_ptr) {
              printf("%s: vars_ptr->filename: %s\n", __func__, vars_ptr->filename);
           }
           else {
              printf("%s: vars_ptr->filename: NULL\n", __func__);
           }
       }

       // Pick a symbol_def, from the displayed list, by index number.
       printf("Select a location of symbol to edit\n");
       selected = get_symbol_selection(&vars_parse_functions);

       if (NULL != selected) {

          vars_parse_functions.reference_print_function(selected);

          // Open the location of the selected symbol in vim.
          run_vim(selected);
       }

       // Clean up the lists.
       funcs_parse_functions.dealloc_function();
       vars_parse_functions.dealloc_function();

   // Repeat for next selection, empty return from select quits.
   } while (NULL != selected);

   printf("\n");

   return EXIT_SUCCESS;
}

