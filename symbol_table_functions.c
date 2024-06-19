#include "funcs.h"
#include "symbol_table_functions.h"


//*****************************************************************************
// Function: get_symbol_table_indexed
//
// Description: finds the symbol def struct at the given index for the given
//              linked list.
//
// Parameters: symbol_table_head - defines the linked list.
//
//             index - index number for the symbol definition to return.
//
// Return: pointer to symbol_def_t at the given index.
//
//*****************************************************************************
symbol_def_t *get_symbol_table_indexed(symbol_def_t **symbol_table_head, const uint32_t index)
{

   symbol_def_t *s_table = NULL;

   if (NULL != *symbol_table_head) {
       symbol_def_t *ptr = *symbol_table_head;

       // Walk the list starting at head->next until the
       // last element, whose next pointer is NULL
       while ((index != ptr->header.index) && (NULL != ptr->header.next)) {
           ptr = ptr->header.next;
       }

       // check for the correct index.
       if (index == ptr->header.index) {
           s_table = ptr;
       }
   }

   return s_table;
}


//*****************************************************************************
// Function: append_symbol_table
//
// Description: Append the given symbol_def_t to the linked list
//              whose head is in the symbol_def_t struct.
//
// Parameters: symbol_table_ptr - the symbol_def_t to append,
//
// Return: returns the index at which the node was appended.
//
//*****************************************************************************
uint32_t append_symbol_table(symbol_def_t *s_table_ptr)
{

     uint32_t index = 0;

    if (NULL != s_table_ptr) {
        s_table_ptr->header.next = NULL;

        // If first element, set head.
        if (NULL == *s_table_ptr->header.head) {
            s_table_ptr->header.index = 1;
            *s_table_ptr->header.head = s_table_ptr;
        }
        // else add to tail.
        else {
            symbol_def_t *ptr = *s_table_ptr->header.head;

            // index at head->next is 2.
            index = 2;

            // Walk the list starting at head->next until the
            // last element, whose next pointer is NULL
            while (NULL != ptr->header.next) {
                ptr = ptr->header.next;
                index++;
            }

            // Set the index for the added element.
            s_table_ptr->header.index = index;
            ptr->header.next = s_table_ptr;
        }
    }
    return index;
}

//*****************************************************************************
// Function: deallocate_symbol_table
//
// Description: Free the linked list of symbol_def_t structs.
//
// Parameters: symbol_table_head - pointer to the list head.
//
// Return: void.
//
//*****************************************************************************
void deallocate_symbol_table(symbol_def_t **symbol_table_head)
{

    symbol_def_t *ptr = NULL;
    symbol_def_t *prev_ptr = NULL;

    ptr = *symbol_table_head;

    while (NULL != ptr) {

        prev_ptr = ptr;
        ptr = ptr->header.next;

        // Deallocate the last table pointer.
        if (NULL != prev_ptr->header.line_bufptr) {
           free(prev_ptr->header.line_bufptr);
        }
        free(prev_ptr);
    }
    *symbol_table_head = NULL;
}


