
#target/driver/build.mk

# driver
C_SOURCES += uart.c gp_timer.c mmc.c part.c part_dos.c

DRIVER_DIR_INCLUDE += -Iinclude/driver -Iinclude/kernel -Iinclude/fs
