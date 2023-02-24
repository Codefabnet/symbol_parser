OBJDIR := obj

SOURCES := funcs.c vars_parse_function.c funcs_parse_functions.c common_parse_functions.c
#OBJECTS = $(subst .c,.o,$(OBJDIR)$(SOURCES))
OBJECTS := $(addprefix $(OBJDIR)/,funcs.o funcs_parse_functions.o vars_parse_functions.o common_parse_functions.o)

#funcs: funcs.o vars_parse_functions.o funcs_parse_functions.o common_parse_functions.o
funcs: $(OBJECTS)
	gcc $(OBJECTS) -o $@ 

$(OBJDIR)/%.o : %.c
	gcc -g -c $< -o $@
#	$(COMPLIE.c) $(OUTPUT_OPTION) $<

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

#$(OBJECTS): $(SOURCES)
#	gcc -g -c $< -o $@

#$(OBJDIR)/funcs.o: funcs.c funcs.h
#	gcc -g -c funcs.c -o $@
#
#$(OBJDIR)/funcs_parse_functions.o: funcs_parse_functions.c funcs_parse_functions.h
#	gcc -g -c funcs_parse_functions.c -o $@
#
#$(OBJDIR)/vars_parse_functions.o: vars_parse_functions.c vars_parse_functions.h
#	gcc -g -c vars_parse_functions.c -o $@
#
#$(OBJDIR)/common_parse_functions.o: common_parse_functions.c common_parse_functions.h
#	gcc -g -c common_parse_functions.c -o $@
#
.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o

