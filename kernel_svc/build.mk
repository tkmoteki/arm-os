#kernel_svc/build.mk

# カーネル拡張機能
C_SOURCES += log_manage.c

KERNEL_SVC_DIR_INCLUDE += -Iinclude/kernel -Iinclude/kernel_svc -Iinclude/c
