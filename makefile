TARGETS := funcs vars
OBJDIR := obj

SOURCES := funcs.c \
           vars_parse_functions.c \
           funcs_parse_functions.c \
           common_parse_functions.c \
           symbol_table_functions.c

COMMON_HEADERS := symbol_target_structs.h

OBJECTS := $(addprefix $(OBJDIR)/, \
             funcs.o \
             funcs_parse_functions.o \
             vars_parse_functions.o \
             common_parse_functions.o \
             symbol_table_functions.o \
	     )

$(TARGETS): $(OBJECTS)
	gcc $(OBJECTS) -o $@ 

$(OBJDIR)/%.o : %.c %.h $(COMMON_HEADERS)
	gcc -Wall -g -c $< -o $@

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean

clean:
	rm -f $(TARGETS)
	rm -f $(OBJDIR)/*.o

