/* private interface */

/* public interface */
/* include/cpu */
#include "cpu_cntrl.h"

#define VECTOR_TABLE 0x4020FFC4   /*! lowvectorの先頭アドレス */


/* ・ベクタの設定は.textセクションに配置した間接ジャンプ命令をlowvectorベースアドレスへコピーする方式とする．*/
/* ・リンカスクリプトでvectorセクションを設けて，間接ジャンプ命令を配置する場合，以下のようにする．*/
/*  -リンカスクリプトで定義するvectorセクションは，読み取り属性とする(例 : vector(r)) */
/*  -アセンブラでは属性を変更する(例 : .sectiron .vector "a"または .section .vector"ax")．ただし，elfフォーマットに限る */


  .arm                    /* 出力アーキテクチャの指定 */
  .section .text          /* セクションのセット */
  .align 0
  //.global _start          /* 外部参照ラベル */
_start:
  b _reset                /* ラベル_restへ無条件ジャンプ */
  ldr pc, _und            /* 擬似命令 _undラベルをpcへロード(間接ジャンプ) */
  ldr pc, _swi            /* 擬似命令 _swiラベルをpcへロード(間接ジャンプ) */
  ldr pc, _pabort         /* 擬似命令 _pabortラベルをpcへロード(間接ジャンプ) */
  ldr pc, _dabort         /* 擬似命令 _dabortラベルをpcへロード(間接ジャンプ) */
  ldr pc, _unused         /* 擬似命令 _unusedラベルをpcへロード(間接ジャンプ) */
  ldr pc, _irq            /* 擬似命令 _irqラベルをpcへロード(間接ジャンプ) */
  ldr pc, _fiq            /* 擬似命令 _fiqラベルをpcへロード(間接ジャンプ) */
_und:
  .word _und_intr         /* 擬似命令 und_intrのアドレスをメモリへ書きこむ */
_swi:
  .word _swi_intr         /* 擬似命令 und_intrのアドレスをメモリへ書きこむ */
_pabort:
  .word _pabort_intr      /* 擬似命令 und_intrのアドレスをメモリへ書きこむ */
_dabort:
  .word _dabort_intr      /* 擬似命令 und_intrのアドレスをメモリへ書きこむ */
_unused:
  nop                     /* 使用しない */
_irq:
  .word _irq_intr         /* 擬似命令 und_intrのアドレスをメモリへ書きこむ */
_fiq:
  .word _fiq_intr         /* 擬似命令 und_intrのアドレスをメモリへ書きこむ */


/* スタートアップ */
_reset:
/* lowvectorへコピーするアドレスの決定 */
vector_set:
  ldr r0, =_start         /* ラベル_startのアドレスをロード */
  ldr r1, =VECTOR_TABLE   /* lowvectorの先頭アドレスをロード */
  mov r2, #14             /* lowvector上限オフセットの指定(減算カウンタの指定) */

/* 分岐先をramvectorへコピーする */
vector_copy:
  ldr     r3,[r0]         /* _startの中身をロード */
  str     r3,[r1]         /* ロードした中身をlowvectorのアドレスの中身へ書きこむ */
  add     r0,r0,#4        /* オフセットの指定(次のベクタの中身へ) */
  add     r1,r1,#4        /* lowvectorのオフセット指定(次のlowvectorのアドレスへ) */
  sub     r2,r2,#1        /* 減算カウンタ */
  cmp     r2,#0           /* 減算カウンタが0になるまでループ */
  bne     vector_copy     /* フラグを見てvector_copyへジャンプ */

/* 割込みスタックの初期化 */
/* 
* ・CPUモードを設定してから各種スタックの設定を行う
* ・CPSRへCPUモードの設定をすると割込みマスクビットがクリアされるので注意
* (よって，CPUモードの設定とともに割込みマスクビットを立てている)
*/
stack_setup:
  msr cpsr_fsxc, #(CPSR_SYS_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをシステムモード，外部割込み(IRQとFIQ)禁止 */
  ldr sp, =_sys_stack                                         /* システムモードスタックの設定(0x90002000を設定) */

  msr cpsr_fsxc, #(CPSR_UND_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードを未定義モード，外部割込み(IRQとFIQ)禁止 */
  ldr sp, =_und_stack                                         /* 未定義モードスタックの設定(0x90003000を設定) */

  msr cpsr_fsxc, #(CPSR_ABT_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをアボートモード，外部割込み(IRQとFIQ)禁止 */
  ldr sp, =_abt_stack                                         /* アボートモードスタックの設定(0x90000ff0を設定) */

  msr cpsr_fsxc, #(CPSR_FIQ_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをFIQモード，外部割込み(IRQとFIQ)禁止 */
  ldr sp, =_fiq_stack                                         /* FIQモードスタックの設定(0x90000fe0を設定) */

  msr cpsr_fsxc, #(CPSR_IRQ_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをIRQモード，外部割込み(IRQとFIQ)禁止 */
  ldr sp, =_irq_stack                                         /* IRQモードスタックの設定(0x90000fd0を設定) */

  msr cpsr_fsxc, #(CPSR_SVC_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをSVCモード，外部割込み(IRQとFIQ)禁止 */
  ldr sp, =_svc_stack                                         /* SVCモードスタックの設定(0x90002fc0を設定) */
/* これ以降CPUモードはSVCモード，外部割込み無効として動く */

startup_main:
  mov r0, #0  /* Cの関数への引数をなしとする */
  b main      /* OSのmain関数(Cの関数)へジャンプ */

/* 暴走用 */
1:
  b   1b

  .pool


/* 割込みの入り口 */
.macro  INTERRPUT_ENTR
  stmfd sp!, {r0, r1, lr}
  mrs r0, spsr
  stmfd sp!, {r0}
  mrs r1, cpsr

  msr cpsr_c, #(CPSR_SYS_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをシステムモード，外部割込み(IRQとFIQを禁止にする) */
  stmfd sp!, {r2-r12, r14}
  sub sp, sp, #0x10
  mov r0, sp

  orr r1, r1, #(IRQ_DISABLE | FIQ_DISABLE)
  msr cpsr_c, r1
  ldmfd sp!, {r1-r4}

  msr cpsr_c, #(CPSR_SVC_MODE | IRQ_DISABLE | FIQ_DISABLE) /* CPUモードをシステムモード，外部割込み(IRQとFIQを禁止にする) */
  stm r0, {r1-r4}
.endm

/* 割込みの出口 */
.macro  INTERRPUT_EXIT
  ldmfd r0!, {r1-r3, r12}
  stmfd sp!, {r1-r3, r12}

  msr cpsr_c, #(CPSR_SYS_MODE | IRQ_DISABLE | FIQ_DISABLE)
  mov sp, r0
  ldmfd sp!, {r2-r12, r14}

  msr cpsr_c, #(CPSR_SVC_MODE | IRQ_DISABLE | FIQ_DISABLE)
  ldmfd sp!, {r0}
  msr spsr_fsxc, r0
  ldmfd sp!, {r0, r1, pc}^
.endm


/* 未定義命令割込みハンドラ呼び出しの出入り口関数 */
  .global _und_intr
_und_intr:
  INTERRPUT_ENTR
  mov r4, r0      /* コンテキストのスタックポインタを設定 */
  bl  intr_und    /* Cの関数へジャンプ */
  mov r0, r4      /* コンテキストのスタックポインタを復旧 */
  INTERRPUT_EXIT


/* SVC割込みハンドラ呼び出しの出入り口関数 */
  .global _swi_intr
_swi_intr:
  INTERRPUT_ENTR
  mov r4, r0      /* コンテキストのスタックポインタを設定 */
  bl  intr_swi    /* Cの関数へジャンプ */
  mov r0, r4      /* コンテキストのスタックポインタを設定 */
  INTERRPUT_EXIT


/* プリフェッチアボートハンドラ呼び出しの出入り口関数 */
  .global _pabort_intr
_pabort_intr:
  INTERRPUT_ENTR
  mov r4, r0      /* コンテキストのスタックポインタを設定 */
  bl  intr_pabort /* Cの関数へジャンプ */
  mov r0, r4      /* コンテキストのスタックポインタを設定 */
  INTERRPUT_EXIT


/* データアボートハンドラ呼び出しの出入り口関数 */
  .global _dabort_intr
_dabort_intr:
  INTERRPUT_ENTR
  mov r4, r0      /* コンテキストのスタックポインタを設定 */
  bl  intr_dabort /* Cの関数へジャンプ */
  mov r0, r4      /* コンテキストのスタックポインタを設定 */
  INTERRPUT_EXIT


/* IRQハンドラ呼び出しの出入り口関数 */
  .global _irq_intr
_irq_intr:
  sub lr, lr, #4  /* 正常に戻るため減算 */
  INTERRPUT_ENTR
  mov r4, r0      /* コンテキストのスタックポインタを設定 */
  bl  intr_irq    /* Cの関数へジャンプ */
  mov r0, r4      /* コンテキストのスタックポインタを設定 */
  INTERRPUT_EXIT


/* Fast IRQハンドラ呼び出しの出入り口関数(使用しない) */
  .global _fiq_intr
_fiq_intr:
  nop


/* ディスパッチ */
  .global dispatch
dispatch:
  ldr r0, [r0]
  INTERRPUT_EXIT

  .pool
