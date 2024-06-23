#include "funcs.h"
#include "symbol_table_functions.h"



//*****************************************************************************
// Function: get_symbol_indexed
//
// Description: finds the symbol def struct at the given index for the given
//              linked list.
//
// Parameters: symbol_list_head - defines the linked list.
//
//             index - index number for the symbol definition to return.
//
// Return: pointer to struct symbol_def at the given index.
//
//*****************************************************************************
struct symbol_def *get_symbol_indexed(struct symbol_def **symbol_list_head, const uint32_t index)
{

   struct symbol_def *symbol = NULL;

   if (NULL != *symbol_list_head) {
       struct symbol_def *ptr = *symbol_list_head;

       // Walk the list starting at head->next until the
       // last element, whose next pointer is NULL
       while ((index != ptr->header.index) && (NULL != ptr->header.next)) {
           ptr = ptr->header.next;
       }

       // check for the correct index.
       if (index == ptr->header.index) {
           symbol = ptr;
       }
   }

   return symbol;
}


//*****************************************************************************
// Function: append_symbol
//
// Description: Append the given struct symbol_def to the linked list
//              whose head is in the struct symbol_def struct.
//
// Parameters: symbol_ptr - the struct symbol_def to append,
//
// Return: returns the index at which the node was appended.
//
//*****************************************************************************
uint32_t append_symbol(struct symbol_def *symbol_ptr)
{

     uint32_t index = 0;

    if (NULL != symbol_ptr) {
        symbol_ptr->header.next = NULL;

        // If first element, set head.
        if (NULL == *symbol_ptr->header.head) {
            symbol_ptr->header.index = 1;
            *symbol_ptr->header.head = symbol_ptr;
        }
        // else add to tail.
        else {
            struct symbol_def *ptr = *symbol_ptr->header.head;

            // index at head->next is 2.
            index = 2;

            // Walk the list starting at head->next until the
            // last element, whose next pointer is NULL
            while (NULL != ptr->header.next) {
                ptr = ptr->header.next;
                index++;
            }

            // Set the index for the added element.
            symbol_ptr->header.index = index;
            ptr->header.next = symbol_ptr;
        }
    }
    return index;
}

//*****************************************************************************
// Function: deallocate_symbol_list
//
// Description: Free the linked list of struct symbol_def structs.
//
// Parameters: symbol_list_head - pointer to the list head.
//
// Return: void.
//
//*****************************************************************************
void deallocate_symbol_list(struct symbol_def **symbol_list_head)
{

    struct symbol_def *ptr = NULL;
    struct symbol_def *next_ptr = NULL;

    ptr = *symbol_list_head;

    while (NULL != ptr) {

        next_ptr = ptr->header.next;

        // Deallocate the last table pointer.
        if (NULL != ptr->header.line_bufptr) {
           free(ptr->header.line_bufptr);
        }
        free(ptr);
        ptr = next_ptr;
    }
    *symbol_list_head = NULL;
}


