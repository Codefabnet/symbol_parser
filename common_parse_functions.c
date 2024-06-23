#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "funcs.h"

//*****************************************************************************
// Function: set_target_name
//
// Description: #include "common_parse_functions.h"
//
// Parameters:
//
// Return:
//
//*****************************************************************************
void set_target_name(struct parse_functions *const parser_functions,
                     const char *const target_name, const int name_length)
{
    char *new_name = NULL;

    new_name = malloc(name_length);
    if (NULL != new_name) {
        strncpy(new_name, target_name, name_length);
    }
    parser_functions->target_name = new_name;
}

//*****************************************************************************
// Function: free_target_name
//
// Description: }
//
// Parameters:
//
// Return:
//
//*****************************************************************************
void free_target_name(struct parse_functions *const parser_functions)
{
    if (NULL != parser_functions->target_name) {
        free(parser_functions->target_name);
    }
    parser_functions->target_name = NULL;
}

//*****************************************************************************
// Function: deallocate_parser
//
// Description: }
//
// Parameters:
//
// Return:
//
//*****************************************************************************
void deallocate_parser(struct parse_functions *const parser_functions)
{
    free_target_name(parser_functions);
    deallocate_symbol_list(parser_functions->head);
}

void * parse_default( char *bufptr )
{
   return (void *)bufptr;
}

void * parse_proto_string( char *bufptr )
{

    while (' ' == *bufptr) {
        bufptr++;
    }
    return (void *)bufptr;
}

#if 0
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
#endif

//*********************************************************************************
// Function: read_data
//
// Description: The common file parser function.
//              The symbol alloc function, of the given parse_function struct,
//              determines if a "vars" or "funcs" parse is performed.
//
//              Parsed lines are stored in the symbol_def list of the given
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
//*********************************************************************************
void read_data(const struct parse_functions *const parse_functions,
                FILE * stream, const bool print_entries)
{
    size_t bufsize = READ_BUFSIZE;
    int line_char_count = 0;
    struct symbol_def *symbol_ptr;
    char *line_bufptr = NULL;

    line_bufptr = malloc(READ_BUFSIZE);

    if (NULL == line_bufptr) {
        printf("%s: out of memory\n", __FUNCTION__);
        deallocate_parser(&funcs_parse_functions);
        deallocate_parser(&vars_parse_functions);
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
    symbol_ptr = parse_functions->alloc_function();

    if (NULL == symbol_ptr) {
        printf("%s: symbol_def alloc failed\n", __FUNCTION__);
        deallocate_parser(&funcs_parse_functions);
        deallocate_parser(&vars_parse_functions);
        exit(EXIT_FAILURE);
    }

    // Each symbol definition list entry has a pointer to the line
    // of output parsed.
    symbol_ptr->header.line_bufptr = line_bufptr;
    symbol_ptr->header.line_char_count = line_char_count;

    // Check for EOF/error while reading file operation output.
    while (line_char_count != -1) {
        int i=0;
        char *tmp_bufptr;

        symbol_ptr->header.head = parse_functions->head;

        // Parse a line of output from file operation.
        tmp_bufptr = symbol_ptr->header.line_bufptr;

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
              *symbol_ptr->symbol[symbol_idx] =
                  (void*)parse_functions->line_schema[i].parse_function(tmp_bufptr);

              // terminate the string for this section.
              tmp_bufptr = NULL;

             // The line schema ends with a NULL delimiter.
           } while (parse_functions->line_schema[++i].delimiter);
        }

        // Certain symbols may be skipped.
        if (!parse_functions->skip_function(symbol_ptr)) {

            // Append the symbol, just parsed, to the list in the
            // parser_functions struct.
            append_symbol(symbol_ptr);

            if (print_entries) {
                // Print the table of ctag element.
                parse_functions->print_function(symbol_ptr);
            }
        }


        // Set up for the next line of output from the file operation.
        line_bufptr = malloc(READ_BUFSIZE);

        if (NULL == line_bufptr)
        {
            printf("%s: out of memory\n", __FUNCTION__);
            deallocate_parser(&funcs_parse_functions);
            deallocate_parser(&vars_parse_functions);
            exit(EXIT_FAILURE);
        }

        bufsize = READ_BUFSIZE;

        line_char_count = getline(&line_bufptr, &bufsize, stream);

        // TODO: handle all error cases for getline.
        if (-1 == line_char_count) {
           free (line_bufptr);
           break;
        }

        // Get and initialize the next symbol_def list entry.
        symbol_ptr = parse_functions->alloc_function();
        if (NULL == symbol_ptr) {
            break;
        }
        symbol_ptr->header.line_bufptr = line_bufptr;
        symbol_ptr->header.line_char_count = line_char_count;
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
bool run_parse(const struct parse_functions *const parse_functions,
               const bool print_list)
{
    FILE *output;
    char command[120];

    // Clear the function symbols linked list.
    deallocate_symbol_list(parse_functions->head);

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
void run_vim(struct symbol_def *symbol_in_target)
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
       execlp("gvim", "gvim", "-R",
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
struct symbol_def *get_symbol_selection(struct parse_functions *parse_functions)
{
   char c;
   int index = 0;
   struct symbol_def *selected = NULL;

   while ('\n' != (c = getchar())) {

     index = (index * 10) + (uint8_t)c - 0x30;
   }
   if (0 != index) {
       selected = get_symbol_indexed(parse_functions->head, index);
   }
   // If the first char is 'U' push the symbol at the index and use the name
   // member as the target_name for the new parse.

//   strncpy(target_name, selected->name, TARGET_NAME_SIZE);
//   find_symbol():
   return selected;
}

void find_symbols(const char *const target_name)
{
    struct symbol_def *vars_ptr;
    struct symbol_def *funcs_ptr;
    char *symbol_filename = NULL;

    deallocate_parser(&funcs_parse_functions);
    deallocate_parser(&vars_parse_functions);

    set_target_name(&vars_parse_functions,
            target_name,
            strlen(target_name) + 1);

    // Get the symbol list.
    run_parse(&vars_parse_functions, false);

    // Get first list entry
    vars_ptr = vars_symbol_list_head;

    enum symboltype sym_type_to_find = invalid_type;

    // Walk the list of locations for the given symbol.
    while (NULL != vars_ptr && vars_ptr->filename != NULL) {

        // For the first, and each unique filename in the vars linked list
        // get the list of symbols for the filename in current list entry.
        if ((NULL == symbol_filename) ||
            (strcmp(symbol_filename, vars_ptr->filename) != 0)) {
            symbol_filename = vars_ptr->filename;

            // Set the file to parse.
            set_target_name(&funcs_parse_functions,
                    symbol_filename,
                    strlen(symbol_filename) + 1);

            // Get all symbols for the given file.
            run_parse(&funcs_parse_functions, false);
        }

        // Get the first list entry for the symbols in the given file.
        funcs_ptr = funcs_symbol_list_head;

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

            vars_ptr->link_index = funcs_ptr;

            funcs_parse_functions.print_function(funcs_ptr);
            funcs_parse_functions.reference_print_function(funcs_ptr);
            printf(" :\n");
            vars_parse_functions.reference_print_function(vars_ptr);

            // Go back to the first entry in the funcs list, break for the next
            // location for the given symbol.
            funcs_ptr = funcs_symbol_list_head;
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
}

