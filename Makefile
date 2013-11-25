#
# CFLAGS
# -nostdinc         : ヘッダファイルのための標準システムディレクトリを検索しない
# -nostdlib         : 標準ライブラリ等を使用しない
# -fno-builtin      : ビルドイン関数の無効
# -fsigned-char     : char型を自動的にunsinged charとしない
# ABIオプション
# -mabi=apcs-gnu    : GNU ABIインターフェース
# imabi=aapcs-linux : Version ? EABIインターフェース
# なし              : Version ? EABIインターフェース
# アーキテクチャ
# -march            : 指定したCPUだけで動作するコードを生成 -mcpuより速いコード生成
# -mcpu             : 同系列のCPUでも動作するコード生成
# -mtune            : -mcpuの別名(gcc3.4以降)


PREFIX  = $(HOME)/bin/CodeSourcery/Sourcery_G++_Lite
BINDIR  = $(PREFIX)/bin


ARCH      = arm-none-eabi
ADDNAME   = $(ARCH)-


AR      = $(BINDIR)/$(ADDNAME)ar
AS      = $(BINDIR)/$(ADDNAME)as
CC      = $(BINDIR)/$(ADDNAME)gcc
LD      = $(BINDIR)/$(ADDNAME)ld
NM      = $(BINDIR)/$(ADDNAME)nm

OBJCOPY = $(BINDIR)/$(ADDNAME)objcopy
OBJDUMP = $(BINDIR)/$(ADDNAME)objdump
RANLIB  = $(BINDIR)/$(ADDNAME)ranlib
STRIP   = $(BINDIR)/$(ADDNAME)strip


TARGET = bin/uImage


#依存関係より，-oを加えない
CFLAGS = -Wall -nostdlib -fno-builtin -fsigned-char
CFLAGS += -march=armv7-a
CFLAGS += -I.
CFLAGS += -g# デバッガ対応(Jtag+openOCD+remoteGDB)
CFLAGS += -Os


CFLAGS += -DKERNEL
CFLAGS += -DTSK_LIBRARY
CFLAGS += -DLOG
#CFLAGS += -DDEBUG_LEVEL1 -DARCH_CPU_LIB_MAIN
CFLAGS += -DKERNEL_MSG
CFLAGS += -DCONFIG_MMC -DCONFIG_RELOC_FIXUP_WORKS -DCONFIG_DOS_PARTITION -D__LITTLE_ENDIAN
#CFLAGS += クロック入力?


LFLAGS = -static -T kernel/kernel.scr


#include directive
#下位のmkファイルへのパラメータ渡し
ASM_SOURCES :=
C_SOURCES :=

ARCH_CPU_DIR := arch/cpu/
include $(ARCH_CPU_DIR)build.mk

TARGET_DRIVER_DIR := target/driver/
include $(TARGET_DRIVER_DIR)build.mk

ARCH_GCC_DIR := arch/gcc/
include $(ARCH_GCC_DIR)build.mk

KERNEL_DIR := kernel/
include $(KERNEL_DIR)build.mk

KERNEL_SVC_DIR := kernel_svc/
include $(KERNEL_SVC_DIR)build.mk

NET_DIR := net/
include $(NET_DIR)build.mk

CLIB_DIR := c_lib/
include $(CLIB_DIR)build.mk

TSKLIB_DIR := tsk_lib/
include $(TSKLIB_DIR)build.mk


ASM_OBJ := $(ASM_SOURCES:.S=.o)
C_OBJ := $(C_SOURCES:.c=.o)
OBJS += $(ASM_OBJ) $(C_OBJ)
OBJS := $(addprefix objs/,$(OBJS))


all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)


# startup.oとmain.oコンポーネントを先に生成
# -先に生成しないと，シンボルがずれ，エントリーアドレスから起動できなくなる
objs/%.o : $(ARCH_CPU_DIR)%.S
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(ARCH_CPU_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(TARGET_DRIVER_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(ARCH_GCC_DIR)%.S
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(ARCH_GCC_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(KERNEL_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(KERNEL_SVC_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(NET_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(CLIB_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@

objs/%.o : $(TSKLIB_DIR)%.c
	$(CC) -c $(CFLAGS) $< -o $@


bin : $(TARGET)
	$(OBJCOPY) -O binary $(TARGET) $(TARGET).bin

exe :
	cp bin/uImage.bin ~
	sudo chmod 777 /dev/ttyUSB0
	minicom -o


#make imageを使用する時は，以下にu-bootのmkimageバイナリファイルをおく
image : $(TARGET).bin
	../u-boot/tools/mkimage \
	-A arm -O linux -T kernel -C none \
	-n "Angstrom/2.6.32/beagleboard" -a 0x80008000 -e 0x80008000 \
	-d $(TARGET).bin $(TARGET)


clean :
	rm -f $(OBJS) $(TARGET) $(TARGET).bin $(TARGET)~ $(TARGET).bin~
	rm -f *~ $(ARCH_CPU_DIR)*.*~ $(TARGET_DRIVER_DIR)*.*~ $(ARCH_GCC_DIR)*.*~ $(KERNEL_DIR)*.*~ \
	$(KERNEL_SVC_DIR)*.*~ $(NET_DIR)*.*~ $(CLIB_DIR)*.*~ $(TSKLIB)*.*~
