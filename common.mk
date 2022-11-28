# Julia Connelly
# 605.614 System Development in the UNIX Environment
# common.mk
# Makefile with commonly used macros and variables.

INSTALL = install
AR = ar cr
MAKE = make
RM = rm
CC = cc

MAKE_SUB_DIRS = ls -d -1 ./**/* | grep Makefile | xargs dirname | sort -r | xargs -I dir make -C dir
MAKE_IT = $(MAKE_SUB_DIRS) it
MAKE_INSTALL = $(MAKE_SUB_DIRS) install
MAKE_CLEAN = $(MAKE_SUB_DIRS) clean
MAKE_DEPEND = $(MAKE_SUB_DIRS) depend
