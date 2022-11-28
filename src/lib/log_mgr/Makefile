# Julia Connelly
# 605.614 System Development in the UNIX Environment
# Makefile
# Builds the log_mgr library. Supports targets it, install, clean, depend.

PROJECT_DIR = ../../..
include $(PROJECT_DIR)/common.mk

INCLUDE = $(PROJECT_DIR)/include
LIB = $(PROJECT_DIR)/lib

VPATH = $(INCLUDE)
CPPFLAGS = -I $(INCLUDE)

ARCHIVE = liblog_mgr.a
SOURCES = $(wildcard *.c)
OBJECTS = $(subst .c,.o,$(SOURCES))
DEPENDS = $(subst .c,.d,$(SOURCES))

it: $(ARCHIVE)

install: $(LIB)/$(ARCHIVE)
$(LIB)/$(ARCHIVE): $(ARCHIVE)
	$(INSTALL) $(ARCHIVE) $(LIB)

clean:
	$(RM) -f $(ARCHIVE) $(OBJECTS) $(DEPENDS)

$(ARCHIVE): $(OBJECTS)
	$(AR) $@ $?

%.d: %.c
	$(CC) -M $(CPPFLAGS) $^ > $@
	
depend: $(DEPENDS)

include $(DEPENDS)
