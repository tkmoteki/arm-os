
#kernel/build.mk

# target非依存部
# kernel source
C_SOURCES += kernel.c syscall.c scheduler.c ready.c memory.c task_manage.c intr_manage.c task_sync.c multi_timer.c command.c

# task
C_SOURCES += init_tsk.c
