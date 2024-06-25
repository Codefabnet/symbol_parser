#include "funcs.h"

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
   struct parse_functions *parse_functions;
   bool select_symbol_from_file;
   struct symbol_def *selected;
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
       strncpy(target_name, argv[1], strlen(argv[1]) + 1);
       set_target_name(parse_functions, argv[1], strlen(argv[1]) + 1);
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

   if (select_symbol_from_file) {

      // Run ctags for the filename in target_name, build a
      // list of symbols in that file.
      run_parse(&funcs_parse_functions, true);

      // Select a struct symbol_def from that list of symbols.
      printf("Select a symbol in %s\n", parse_functions->target_name);
      selected = get_symbol_selection(parse_functions);

      // No symbol was selected.
      if (NULL == selected) {
         deallocate_parser(&funcs_parse_functions);
         return EXIT_SUCCESS;
      }

      // Save the symbol name to be used as the target for the vars search.
      strncpy(target_name, selected->name, strlen(selected->name) + 1);
   }

   // Grep to find all locations of the symbol selected above,
   //  list and open in editor. loop for symbols selected for edit until empty return is entered below.
   do {
       find_symbols(target_name);

       // Pick a symbol, from the displayed list, by index number.
       printf("Select a location of symbol to edit\n");
       selected = get_symbol_selection(&vars_parse_functions);

       if (NULL != selected) {

          vars_parse_functions.reference_print_function(selected);

          // Open the location of the selected symbol in vim.
          run_vim(selected);
       }

   // Repeat for next selection, empty return from select quits.
   } while (NULL != selected);

   // Clean up the lists.
   deallocate_parser(&funcs_parse_functions);
   deallocate_parser(&vars_parse_functions);

   printf("\n");

   return EXIT_SUCCESS;
}

