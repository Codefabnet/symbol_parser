############################################################
# All targets
############################################################

TARGETS := funcs vars

############################################################
# Common build rules
############################################################

OBJDIR := ./obj
OUTPUTDIR := ./output
SOURCEDIR := ./source

COMPILEFLAGS := -g -Wall

all: $(TARGETS)

$(OUTPUTDIR):
	mkdir $(OUTPUTDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

############################################################

HEADERS := $(addprefix $(SOURCEDIR)/, \
            funcs.h \
            funcs_parse_functions.h \
            vars_parse_functions.h \
            common_parse_functions.h \
            symbol_definition_functions.h \
            symbol_target_structs.h \
        )

OBJECTS := $(addprefix $(OBJDIR)/, \
             funcs.o \
             funcs_parse_functions.o \
             vars_parse_functions.o \
             common_parse_functions.o \
             symbol_definition_functions.o \
	     )

$(TARGETS): $(OBJECTS)
	gcc $(OBJECTS) -o $(OUTPUTDIR)/$@ 

$(OBJDIR)/%.o : $(SOURCEDIR)/%.c $(HEADERS)
	gcc -Wall -g -c $< -o $@

############################################################

$(TARGETS): | $(OUTPUTDIR)

$(OBJECTS): | $(OBJDIR)

clean:
	rm -rf $(OUTPUTDIR)
	rm -rf $(OBJDIR)

