
#arch/cpu/build.mk

# target依存部
ASM_SOURCES += startup.S 
C_SOURCES += main.c intr_cntrl.c intr_hadle.c

CPU_DIR_INCLUDE += -Iinclude/cpu -Iinclude/kernel -Iinclude/c -Iinclude/driver
