# Julia Connelly
# 605.614 System Development in the UNIX Environment
# Makefile
# Builds the my_fortune executable. Supports targets it, install, clean, depend.

PROJECT_DIR = ../../..
include $(PROJECT_DIR)/common.mk

INCLUDE = $(PROJECT_DIR)/include
BIN = $(PROJECT_DIR)/bin

CPPFLAGS = -I $(INCLUDE)

EXECUTABLE = my_fortune
SOURCES = $(wildcard *.c)
OBJECTS = $(subst .c,.o,$(SOURCES))
DEPENDS = $(subst .c,.d,$(SOURCES))

it: $(EXECUTABLE)

install: $(BIN)/$(EXECUTABLE)
$(BIN)/$(EXECUTABLE): $(EXECUTABLE)
	$(INSTALL) $(EXECUTABLE) $(BIN)

clean:
	$(RM) -f $(OBJECTS) $(EXECUTABLE) $(DEPENDS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ -lrt

%.d: %.c
	$(CC) -M $(CPPFLAGS) $^ > $@
	
depend: $(DEPENDS)

include $(DEPENDS)
