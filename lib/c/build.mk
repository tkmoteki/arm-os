#lib/c/build.mk

# my C library
C_SOURCES += lib.c vsprintf.c ctype.c

C_DIR_INCLUDE += -Iinclude/c -Iinclude/kernel -Iinclude/driver
