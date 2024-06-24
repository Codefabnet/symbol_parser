#include "symbol_target_structs.h"

struct symbol_def *get_symbol_indexed(struct symbol_def **symbol_list_head, const uint32_t index);
uint32_t append_symbol( struct symbol_def *symbol_ptr);
void deallocate_symbol_list( struct symbol_def **symbol_list_head);

