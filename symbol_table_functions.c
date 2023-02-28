#include "funcs.h"
#include "symbol_target_structs.h"


symbol_def_t *get_symbol_table_indexed(symbol_def_t **symbol_table_head, const uint32_t index) {

   symbol_def_t *s_table = NULL;

   if (NULL != *symbol_table_head) {
       symbol_def_t *ptr = *symbol_table_head;


       // Walk the list starting at head->next until the 
       // last element, whose next pointer is NULL
       while ((index != ptr->header.index) && (NULL != ptr->header.next)) {
           ptr = ptr->header.next;
       }

       if (index == ptr->header.index) {
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
        s_table_ptr->header.next = NULL;

        // If first element, set head.
        if (NULL == *s_table_ptr->header.head) {
            s_table_ptr->header.index = 0;
            *s_table_ptr->header.head = s_table_ptr;
        }
        // else add to tail.
        else {
            symbol_def_t *ptr = *s_table_ptr->header.head;

            // index at head->next is 1.
            index = 1;  

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
        ptr = ptr->header.next;

        // Deallocate the last table pointer.
        if (NULL != prev_ptr->header.line_bufptr) {
           free(prev_ptr->header.line_bufptr);
        }
        free(prev_ptr);
    }
    *symbol_table_head = NULL;
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


    s_table_out_ptr->header.line_bufptr = malloc(s_table_in_ptr->header.line_char_count);
    s_table_out_ptr->header.line_char_count = s_table_in_ptr->header.line_char_count;
    memcpy(s_table_out_ptr->header.line_bufptr, s_table_in_ptr->header.line_bufptr, s_table_in_ptr->header.line_char_count);
//    strncpy(s_table_out_ptr->line_bufptr, s_table_in_ptr->line_bufptr, s_table_in_ptr->line_char_count);

    s_table_out_ptr->name = (char *)s_table_out_ptr->header.line_bufptr + ((char *)s_table_in_ptr->name - (char *)s_table_in_ptr->header.line_bufptr),
#if 0
printf("%s: %p, %p, %p, %p\n", __func__, s_table_out_ptr->line_bufptr, s_table_out_ptr->name, s_table_in_ptr->line_bufptr, s_table_in_ptr->name);
printf("%s: in - %s\n", __func__, s_table_in_ptr->name);
printf("%s: out - %s\n", __func__, s_table_out_ptr->name);
#endif
    s_table_out_ptr->filename = s_table_out_ptr->header.line_bufptr + (s_table_in_ptr->filename - s_table_in_ptr->header.line_bufptr);
    s_table_out_ptr->prototype = s_table_out_ptr->header.line_bufptr + ((char *)s_table_in_ptr->prototype - (char *)s_table_in_ptr->header.line_bufptr),
    s_table_out_ptr->sym_type = s_table_out_ptr->sym_type = s_table_in_ptr->sym_type;
    s_table_out_ptr->linenum = s_table_in_ptr->linenum;
//    s_table_out_ptr->print_function = print_funcs_file_symbols_line;
//    s_table_out_ptr->reference_print_function = print_funcs_file_reference_line;

    return s_table_out_ptr;
}
