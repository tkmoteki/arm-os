#fs/fat/build.mk

#ファイルシステム
C_SOURCES += fat.c

FAT_DIR_INCLUDE += -Iinclude/fs -Iinclude/kernel -Iinclude/driver -Iinclude/c
