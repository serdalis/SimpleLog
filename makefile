WARNINGS := -Wall -Wextra 
CFLAGS ?= -g $(WARNINGS)

OBJDIR := Build
SRCDIR := SimpleLog

ifeq ($(VERBOSE),1)
	SILENCER := 
else
	SILENCER := @
endif

all: main

createdir:
	$(SILENCER)mkdir -p $(OBJDIR)

main: $(OBJS) 
	#@echo " LINK $^"
	$(SILENCER)$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c | createdir
	#@echo " CC $<"
	$(SILENCER)$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	$(SILENCER)$(RM) -f *~ core main
	$(SILENCER)$(RM) -r $(OBJDIR)