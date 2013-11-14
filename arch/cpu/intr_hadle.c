#include "kernel/defines.h"
#include "cpu_cntrl.h"
#include "intr_cntrl.h"
#include "intr_hadle.h"
#include "kernel/kernel.h"
#include "c_lib/lib.h"
#include "target/driver/serial_driver.h"
#include "kernel/syscall.h"



/*!
 * 未定義命令ハンドラ
 * sp : スタックポインタ
 */
void intr_und(unsigned long sp)
{
  unsigned long pc, spsr;

  /* コンテキストから，PC及びSPSRを得る */
  spsr = *(unsigned long *)(sp + 0);
  pc = *(unsigned long *)(sp + 12);

  KERNEL_OUTMSG("undefined instruction at ");
  /* Thumbモードの場合 */
  if ((spsr & THUMB_INSTRUCTION)) {
    KERNEL_OUTVLE((pc - 2), 0); /* 例外を発生させたPCの値を表示 */
  }
  /* ARMモードの場合 */
  else {
    KERNEL_OUTVLE((pc - 4), 0); /* 例外を発生させたPCの値を表示 */
  }
  putc('\n');

  down_system(); /* OSをフリーズ  */
}


/*!
 * SVC割込みハンドラ
 * sp : スタックポインタ
 */
void intr_swi(unsigned long sp)
{
  ISR_TYPE type;
  unsigned long pc, spsr;

  /* コンテキストから，PC及びSPSRを得る */
  spsr = *(unsigned long *)(sp + 0);
  pc = *(unsigned long *)(sp + 12);

  /* Thumbモードの場合 */
  if (spsr & THUMB_INSTRUCTION) {
    type = *(unsigned short *)(pc - 2); /* 例外を発生させたPCの値を表示 */
  }
  /* ARMモードの場合 */
  else {
    type = *(unsigned long *)(pc - 4); /* 例外を発生させたPCの値を表示 */
  }

  /* 有効なベクタ番号は，0〜255までとする */
  type &= 0xff;
  DEBUG_OUTVLE(type, 0);
  DEBUG_OUTMSG(" swi number.\n");

  syscall_intr(type, sp); /* ISR呼び出し */

  context_switching(SYSCALL_INTERRUPT); /* タスクの切り替えを行う(スケジューラとディスパッチャ呼び出し) */
}


/*!
 * プリフェッチアボートハンドラ
 * sp : スタックポインタ
 */
void intr_pabort(unsigned long sp)
{
  unsigned long pc;

  /* 保存されたコンテキストから，PCを得る */
  pc = *(unsigned long *)(sp + 12);

  KERNEL_OUTMSG("prefetch abort at ");
  KERNEL_OUTVLE(pc - 4, 0);
  putc('\n');

  down_system(); /* OSをフリーズ  */
}


/*!
 * データアボートハンドラ
 * sp : スタックポインタ
 */
void intr_dabort(unsigned long sp)
{
  unsigned long pc;

  /* 保存されたコンテキストから，PCを得る */
  pc = *(unsigned long *)(sp + 12);

  /* 例外を発生させたPCの値を表示 */
  KERNEL_OUTMSG("data abort at ");
  KERNEL_OUTVLE(pc - 8, 0);
  putc('\n');

  down_system(); /* OSをフリーズ */
}


/*!
 * IRQハンドラ
 * sp : スタックポインタ
 */
void intr_irq(unsigned long sp)
{
  INTR_TYPE type;

  type = (INTR_TYPE)REG32_READ(INTCPS_SIR_IRQ) & 0x7F; /* 現在有効化したIRQ番号を取得(INTCPS_SIR_IRQレジスタは7ビット目で管理) */
  DEBUG_OUTVLE(type, 0);
  DEBUG_OUTMSG(" interrupt number.\n");
  external_intr(type, sp);
  /*
   * 割込みコントローラのアサート取り消し
   * -具体的には，INTCPS_CONTROL[0] NEWIRQAGRビットを1にし，同期バリアを張る
   *  このようにする事で，INTCPS_CONTROL[0] NEWIRQAGRビットを1→CPSRのIRQ割込み有効化の順序が保証される
   *  これは，IRQのネストが発生を防ぐためである(上記の順序が逆になると，ネストし，割込みハンドラ内で無限ループする)．
   */
  REG32_WRITE(INTCPS_CONTROL, REG32_READ(INTCPS_CONTROL) | 0x01);
  
  /* 割込みコントローラとCPSRの同期バリア */
  __asm__ volatile("mov r0, #0\n\t"
                   "mcr p15, 0, r0, c7, c10, 4\n");

  context_switching(SERIAL_INTERRUPT); /* タスクの切り替えを行う(スケジューラとディスパッチャ呼び出し) */
}
