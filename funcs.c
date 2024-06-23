#include "funcs.h"

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
 
   // TODO: not sure this makes sense for anything but funcs_parse_functions.
   if (select_symbol_from_file) {
 
      run_parse(parse_functions, true);
 
      // TODO: not sure this makes sense for anything but funcs_parse_functions.
      printf("Select a symbol in %s\n", parse_functions->target_name);
      selected = get_symbol_selection(parse_functions);
 
      if (NULL == selected) {
         deallocate_parser(&funcs_parse_functions);
         return EXIT_SUCCESS;
      }
 
      set_target_name(&vars_parse_functions, selected->name, strlen(selected->name) + 1);
      deallocate_parser(&funcs_parse_functions);
 
   }
 
   // List locations of the symbol selected above, in vars_parse_functions.name.

   // loop for symbols selected for edit until empty return is entered below.
   do {
       find_symbols();

       // Pick a symbol, from the displayed list, by index number.
       printf("Select a location of symbol to edit\n");
       selected = get_symbol_selection(&vars_parse_functions);

       if (NULL != selected) {

          vars_parse_functions.reference_print_function(selected);

          // Open the location of the selected symbol in vim.
          run_vim(selected);
       }

       // Clean up the lists.
       deallocate_parser(&funcs_parse_functions);
       deallocate_parser(&vars_parse_functions);

   // Repeat for next selection, empty return from select quits.
   } while (NULL != selected);

   printf("\n");

   return EXIT_SUCCESS;
}

