#ifndef _DEBUG_H_INCLUDED_
#define _DEBUG_H_INCLUDED_

#include "include/c/lib.h"

/*! デバックマクロ */
/*
 * -DEBUG_LEVEL1(低級なデバッグ)
 *   -テストハーネスと相性が悪いstaticスコープのテスト
 * -DEBUG_LEVEL2(高級なデバッグ)
 *   -ログ出力
 */
//#ifdef DEBUG_LEVEL1

#if defined(DEBUG_LEVEL1) && defined(ARCH_CPU_STARTUP)
#define DEBUG_L1_ARCH_CPU_STARTUP_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_CPU_STARTUP_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_CPU_STARTUP_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_CPU_STARTUP_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(ARCH_CPU_LIB_MAIN)
#define DEBUG_L1_ARCH_CPU_MAIN_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_CPU_MAIN_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_CPU_MAIN_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_CPU_MAIN_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(ARCH_CPU_INTER_CNTRL)
#define DEBUG_L1_ARCH_CPU_INTER_CNTRL_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_CPU_INTER_CNTRL_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_CPU_INTER_CNTRL_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_CPU_INTER_CNTRL_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(ARCH_CPU_INTR_HADLE)
#define DEBUG_L1_ARCH_CPU_INTR_HADLE_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_CPU_INTR_HADLE_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_CPU_INTR_HADLE_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_CPU_INTR_HADLE_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(ARCH_GCC_DIVSI3)
#define DEBUG_L1_ARCH_GCC_DIVSI3_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_GCC_DIVSI3_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_GCC_DIVSI3_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_GCC_DIVSI3_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(ARCH_GCC_UDIVSI3)
#define DEBUG_L1_ARCH_GCC_UDIVSI3_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_GCC_UDIVSI3_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_GCC_UDIVSI3_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_GCC_UDIVSI3_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(ARCH_GCC_DIV0)
#define DEBUG_L1_ARCH_GCC_DIV0_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_ARCH_GCC_DIV0_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_ARCH_GCC_DIV0_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_ARCH_GCC_DIV0_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(FS_FAT_FAT)
#define DEBUG_L1_FS_FAT_FAT_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_FS_FAT_FAT_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_FS_FAT_FAT_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_FS_FAT_FAT_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_KERNLE)
#define DEBUG_L1_KERNEL_KERNLE_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_KERNLE_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_KERNLE_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_KERNLE_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_SYSCALL)
#define DEBUG_L1_KERNEL_SYSCALL_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_SYSCALL_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_SYSCALL_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_SYSCALL_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_SCHDUL)
#define DEBUG_L1_KERNEL_SCHDUL_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_SCHDUL_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_SCHDUL_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_SCHDUL_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_READY)
#define DEBUG_L1_KERNEL_READY_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_READY_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_READY_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_READY_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_MEMORY)
#define DEBUG_L1_KERNEL_MEMORY_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_MEMORY_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_MEMORY_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_MEMORY_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_TASK_MANAGE)
#define DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_INTR_MANAGE)
#define DEBUG_L1_KERNEL_INTR_MANAGE_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_INTR_MANAGE_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_INTR_MANAGE_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_INTR_MANAGE_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_TASK_SYNC)
#define DEBUG_L1_KERNEL_TASK_SYNC_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_TASK_SYNC_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_TASK_SYNC_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_TASK_SYNC_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_MULTI_TIMER)
#define DEBUG_L1_KERNEL_MULTI_TIMER_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_MULTI_TIMER_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_MULTI_TIMER_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_MULTI_TIMER_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_COMMAND)
#define DEBUG_L1_KERNEL_COMMAND_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_COMMAND_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_COMMAND_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_COMMAND_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_INIT_TSK)
#define DEBUG_L1_KERNEL_INIT_TSK_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_INIT_TSK_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_INIT_TSK_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_INIT_TSK_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(KERNEL_SVC_LOG_MANAGE)
#define DEBUG_L1_KERNEL_SVC_LOG_MANAGE_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_KERNEL_SVC_LOG_MANAGE_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_KERNEL_SVC_LOG_MANAGE_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_KERNEL_SVC_LOG_MANAGE_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(LIB_C_LIB)
#define DEBUG_L1_LIB_C_LIB_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_LIB_C_LIB_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_LIB_C_LIB_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_LIB_C_LIB_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(LIB_C_VSPRINTF)
#define DEBUG_L1_LIB_C_VSPRINTF_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_LIB_C_VSPRINTF_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_LIB_C_VSPRINTF_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_LIB_C_VSPRINTF_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(LIB_C_CTYPE)
#define DEBUG_L1_LIB_C_CTYPE_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_LIB_C_CTYPE_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_LIB_C_CTYPE_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_LIB_C_CTYPE_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(NET_XMODEM)
#define DEBUG_L1_NET_XMODEM_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_NET_XMODEM_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_NET_XMODEM_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_NET_XMODEM_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(TARGET_DRIVER_UART)
#define DEBUG_L1_TARGET_DRIVER_UART_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_TARGET_DRIVER_UART_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_TARGET_DRIVER_UART_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_TARGET_DRIVER_UART_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(TARGET_DRIVER_GP_TIMER)
#define DEBUG_L1_TARGET_DRIVER_GP_TIMER_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_TARGET_DRIVER_GP_TIMER_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_TARGET_DRIVER_GP_TIMER_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_TARGET_DRIVER_GP_TIMER_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(TARGET_DRIVER_MMC)
#define DEBUG_L1_TARGET_DRIVER_MMC_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_TARGET_DRIVER_MMC_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_TARGET_DRIVER_MMC_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_TARGET_DRIVER_MMC_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(TARGET_DRIVER_PART)
#define DEBUG_L1_TARGET_DRIVER_PART_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_TARGET_DRIVER_PART_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_TARGET_DRIVER_PART_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_TARGET_DRIVER_PART_OUTMSG(testmsg)
#endif

#if defined(DEBUG_LEVEL1) && defined(TARGET_DRIVER_PART_DOS)
#define DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn)) /* debug message */
#define DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG(testmsg) (puts(testmsg)) /* debug value */
#else
#define DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTVLE(testvalue, testcolumn)
#define DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG(testmsg)
#endif


#ifdef DEBUG_LEVEL2

#endif


#endif
