
#arch/gcc/build.mk

# 言語処理系依存部
# システムライブラリ(by arm)
ASM_SOURCES += _divsi3.S _udivsi3.S
C_SOURCES += div0.c

GCC_DIR_INCLUDE += -I
