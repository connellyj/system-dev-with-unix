# Julia Connelly
# 605.614 System Development in the UNIX Environment
# Makefile
# Builds the install_and_monitor executable. Supports targets it, install, clean, depend.

PROJECT_DIR = ../../..
include $(PROJECT_DIR)/common.mk

INCLUDE = $(PROJECT_DIR)/include
LIB = $(PROJECT_DIR)/lib
BIN = $(PROJECT_DIR)/bin

CPPFLAGS = -I $(INCLUDE)

EXECUTABLE = install_and_monitor
SOURCES = $(wildcard *.c)
OBJECTS = $(subst .c,.o,$(SOURCES))
DEPENDS = $(subst .c,.d,$(SOURCES))

it: $(EXECUTABLE)

install: $(BIN)/$(EXECUTABLE)
$(BIN)/$(EXECUTABLE): $(EXECUTABLE)
	$(INSTALL) $(EXECUTABLE) $(BIN)

clean:
	$(RM) -f $(OBJECTS) $(EXECUTABLE) $(DEPENDS)

$(EXECUTABLE): $(OBJECTS) $(LIB)/libthread_mgr.a $(LIB)/liblog_mgr.a
	$(CC) -pthread $^ -o $@

%.d: %.c
	$(CC) -M $(CPPFLAGS) $^ > $@
	
depend: $(DEPENDS)

include $(DEPENDS)
