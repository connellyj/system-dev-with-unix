# Julia Connelly
# 605.614 System Development in the UNIX Environment
# Makefile
# Builds all project libraries and executables. Supports targets it, install, clean, depend.

include common.mk

it:
	$(MAKE_IT)

install:
	$(MAKE_INSTALL)

clean:
	rm -f bin/* lib/*
	$(MAKE_CLEAN)

depend:
	$(MAKE_DEPEND)
