#ifndef _SCR_SYMBOLS_H_INCLUDED_
#define _SCR_SYMBOLS_H_INCLUDED_

#include "include/kernel/defines.h"

/*! リンカのシンボルを参照 */

/* sramvector section(lowvector) addr */
extern volatile UINT32 _sramvector_start;
extern volatile UINT32 _sramvector_end;
  
/* text section addr */
extern volatile UINT32 _text_start;

/* rodata section addr */
extern volatile UINT32 _rodata_start;
  
/* data section addr */
extern volatile UINT32 _data_start;
extern volatile UINT32 _data_end;
  
/* bss section addr */
extern volatile UINT32 _bss_start;
extern volatile UINT32 _bss_end;
  
/* heap section addr */
extern volatile UINT32 _heap;

/* suhdul_area section addr */
extern volatile UINT32 _schdul_area;

/* tskstack_area section addr */
extern volatile UINT32 _tskstack;

/* stack section addr */
extern volatile UINT32 _sys_stack;
extern volatile UINT32 _svc_stack;
extern volatile UINT32 _irq_stack;
extern volatile UINT32 _fiq_stack;
extern volatile UINT32 _abt_stack;
extern volatile UINT32 _und_stack;

/* logbuffer section addr */
extern volatile UINT32 _logbuffer_start;
extern volatile UINT32 _logbuffer_end;

/* tskbuffer section addr */
extern volatile UINT32 _tskbuffer_start;
extern volatile UINT32 _tskbuffer_end;

/* dramvector section addr(highvector) */
extern volatile UINT32 _dramvector_start;
extern volatile UINT32 _dramvector_end;


/* section size */
/* Boot ROM */
#define ROMALL_SIZE     0x00100000 /* 1MB */
#define ROM_SIZE        (0x00100000 - 0x00014021)
/* low vector */
#define ROMVECTOR_SIZE  0x00000020 

/* SRAM(64KB) */
#define SRAMALL_SIZE    0x00010000
#define SRAM_SIZE       (0x00010000 - 0x40)
/* low vetor時の中継テーブル */
#define SRAMVECTOR_SIZE 0x00000040 /* 64B */

/* DDRSDRAM (512MB) */
#define DRAMALL_SIZE    0x20000000
#define DRAM_SIZE       (0x10000000 - 0x200000)
#define TSKSTACK_SIZE   0x00002000
#define STACK_SIZE      0x00001000
#define LOGBUFFER_SIZE  0x00001000 /* temp(4096B) */
#define TSKBUFFER_SIZE  0x00001000 /* temp(4096B) */
/* high vector */
#define DRAMVECTOR_SIZE 0x00000040


#endif
