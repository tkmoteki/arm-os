/*
 * ・task.cにはタスク管理機能割込みサービスルーチン，タスク付属同期機能の
 *   割込みサービスルーチンの順に各割込みサービスルーチン及び，主にそれらが使用する
 *   内部関数が記述されている
 * -タスク管理機能
 *  ・実装しているシステムコールcre_tsk():タスク生成，del_tsk():タスク排除，
 *    sta_tsk():タスク起動，ext_tsk():自タスクの終了,
 *    exd_tsk():自タスクの終了と排除，ter_tsk():タスクの強制終了.
 *    chg_pri():タスクの優先度変更，get_pri():タスクの優先度参照となる.
 *
 *  ・acre_tsk()は静的と動的を組み合わせた方法である.返却値にTCBのポインタを返却し,
 *    sta_tsk()などタスクIDを指定するものはすべて返却されたポインタを使用する.
 *    acre_tsk()の属性(高級言語インターフェースやアセンブリ言語インターフェース)
 *    はよくわからないので，指定していない.またacre_tsk()はタスク生成を行うだけで,
 *    起動はしない(そのような属性を用意していない).よって起動するときは必ず,
 *    sta_tsk()を発行する.
 *
 *  ・システムコール処理(ter_tsk():タスクの強制終了)
 *    自タスク以外のタスクを強制終了する(実行可能状態と待ち状態にあるもの)
 *    実行可能状態にあるものはレディーキューから抜き取る処理,
 *    待ち状態にあるものは待ち要件を調べ該当の待ちキューから抜き取る
 *    強制終了なので，待ち状態に入っているシステムコールの返却値は書き換えない
 * -タスク付属同期機能
 *  ・実装しているシステムコールはslp_tsk():起床待ち,
 *    tslp_tsk():起床待ち(タイムアウトあり)，wup_tsk():タスクの起床，
 *    rel_wai():待ち状態強制解除，dly_tsk():自タスクの遅延となる.
 *  ・wup_tsk()について
 *    起動要求キューイング機能はない.また任意の待ち要因がある時は起床できない
 *  E_ILUSEの返却値はμITRON4.0にはないものである.act_tsk()との整合性を考えて
 *    自タスクの起床もできるとあるが，よくわからんのでE_ILUSEとした.  
 *   また，何らかの待ち要因を持っている時はwup_tsk()はできない
 *   (ただし，タイムアウトスリープは起床できる.このときのtslp_tsk()の
 *   返却値はE_OKとなる)
 *  ・違うシステムコールで前に呼んだ待ちになるシステムコールの返却値を書き換える方法
 *   (代表的なものにrel_wai)
 *   待ち状態強制解除を行った場合，μITRON4.0ではE_RLWAIを待ち状態に入った
 *   システムコールの返却値として返すとある.これはTCBのwait_infoにwait_erというメンバ
 *   を追加し，待ち強制解除(rel_wai)された時，このメンバのポインタを経由して，
 *   書き換えることで可能となる.
 *   この書き換える方法自体はrel_waiだけでなく，tslp_tskなど待ち状態となる
 *   すべてのシステムコールに適用している.
 *  ・dly_tsk()
 *   μITRON仕様にはtslp_tsk()とdly_tsk()の違いは待ちの間にwup_tsk()が発行
 *   できるかできないかで，dly_tsk()はできない.また0がmsecに指定された場合は
 *   ポーリングとならずに，最初のタイムスティックで待ち解除を行う.
 *   これはあんま意味がないので，実装しない.                                          *
 */

/* private interface */
#include "memory.h"
#include "ready.h"
#include "scheduler.h"
#include "task_manage.h"

/* public interface */
/* include/kernel */
#include "debug.h"
#include "kernel.h"
#include "scr_symbols.h"
/* include/cpu */
#include "cpu_cntrl.h"
/* include/lib/c */
#include "lib.h"

//#include "target/driver/gp_timer.h"
//#include "timer_callrte.h"


/*! タスクの領域確保と初期化(タスクのfreeリストを作成する) */
static ER dynamic_tsk_init(void);

/*! tsk alocリストからタスクコントロールブロックを抜き取る */
static void get_aloclist(TCB *tcb);

/*! TCBスケジューリング依存ブロックの初期化 */
static void tsk_schdul_infocb_init(TCB *tcb, int rate, int rel_exetim, int deadtim, int floatim);

/*! TCBレディー依存ブロックの初期化 */
static void tsk_ready_infocb_init(TCB *tcb);

/*! タスクの優先度を初期化 */
static void tsk_priority_init(TCB *tcb, int priority);

/*! タスクのスタック領域の確保 */
static ER get_tsk_stack(TCB *tcb, int stacksize);

/*! タスク(スレッド)の終了の手続きをする関数 */
static void tsk_endup(void);

/*! タスク(スレッド)のスタートアップ */
static void tsk_startup(TCB *tcb);

/*! タスクスタックの初期化をする関数 */
static void tsk_stack_init(TCB *tcb);

/*! 非タスクコンテキスト用の優先度変更関数 */
static void chg_pri_isyscall_isr(TCB *tcb, int tskpri);

/*! タスクコンテキスト用の優先度変更関数 */
static void chg_pri_syscall_isr(TCB *tcb, int tskpri);


/*!
 * タスク情報(mg_tsk_info)の初期化
 * -この関数は，kernelオブジェクトの初期化時に呼ばれる
 * (返却値)E_NOMEM : メモリが取得できない
 * (返却値)E_OK : 正常終了
 */
ER starup_tsk_init(void)
{
  mg_tsk_info.counter = 0;
  mg_tsk_info.power_count = 0;
  mg_tsk_info.tskid_num = TASK_ID_NUM;

  return tsk_init(); /* task ID変換テーブルの領域確保と初期化 */
}


/*!
 * タスクの初期化(task ID変換テーブルの領域確保と初期化)
 * -この関数は，kernel_obj_init()で呼ばれる場合と，taskIDが足らなくなった場合に呼ばれる
 * (返却値)E_NOMEM : メモリが取得できない(この返却値は)
 * (返却値)E_OK : 正常終了
 */
ER tsk_init(void)
{
  int tskids, i;
  TCB *tcb;
  
  tskids = mg_tsk_info.tskid_num; /* 倍の領域を確保する(start_init_tsk()の時はデフォルト通り) */
  
  mg_tsk_info.id_table = (TCB **)get_mpf_isr(sizeof(tcb) * tskids); /* 変換テーブルの動的メモリ確保 */
  if (mg_tsk_info.id_table == NULL) {
    return E_NOMEM; /* initタスクの時は，start_init_tsk()関数内でOSをスリープさせる */
  }
  /* taskID変換テーブルの初期化(メモリにNULLを埋めるのにmemset()は使用できない) */
  for (i = 0; i < tskids; i++) {
    mg_tsk_info.id_table[i] = NULL;
  }
  /* TCBの静的型配列の作成及び初期化とTCBの動的型freeリストの作成と初期化 */
  if (E_NOMEM == dynamic_tsk_init()) {
    return E_NOMEM; /* initタスクの時は，start_init_tsk()関数内でOSをスリープさせる */
  }
  
  return E_OK;
}


/*!
 * タスクの領域確保と初期化(タスクのfreeリストを作成する)
 * (返却値)E_NOMEM : メモリが取得できない
 * (返却値)E_OK : 正常終了
 */
static ER dynamic_tsk_init(void)
{
  int tskids, i;
  TCB *tcb;
  
  mg_tsk_info.freehead = mg_tsk_info.alochead = NULL;
  
  /* start_init_tsk()の時以外 */
  if (mg_tsk_info.power_count) {
    tskids = TASK_ID_NUM << (mg_tsk_info.power_count - 1); /* 現在と同じ領域を確保する */
  }
  /* start_init_tsk()の時 */
  else {
    tskids = mg_tsk_info.tskid_num;
  }
  
  for (i = 0; i < tskids; i++) {
    tcb = (TCB *)get_mpf_isr(sizeof(*tcb)); /* ノードのメモリ確保 */
    /*メモリが取得できない*/
    if(tcb == NULL) {
      return E_NOMEM;  /* initタスクの時は，start_init_tsk()関数内でOSをスリープさせる */
    }
    memset(tcb, -1, sizeof(*tcb)); /* 確保したノードを初期化 */
    /* freeキューの作成 */
    tcb->free_next = mg_tsk_info.freehead;
    tcb->free_prev = NULL;
    /* NULLを経由するとCodeSourceryはエラー(データアボートを発行する) */
    if (tcb->free_next != NULL) {
      tcb->free_next->free_prev = tcb;
    }
    mg_tsk_info.freehead = tcb;
  }
  
  return E_OK;
}


/*!
 * tsk alocリストからタスクコントロールブロックを抜き取る
 * tcb : 抜き取るTCB
 */
static void get_aloclist(TCB *tcb)
{
  /* 先頭から抜きとる */
  if (tcb == mg_tsk_info.alochead) {
    mg_tsk_info.alochead = tcb->free_next;
    mg_tsk_info.alochead->free_prev = NULL;
  }
  /* それ以外から抜き取る */
  else {
    tcb->free_prev->free_next = tcb->free_next;
    /* NULLを経由するとgcc4はエラー(データアボートを発行する) */
    if (tcb->free_prev->free_next != NULL) {
      tcb->free_next->free_prev = tcb->free_prev;
    }
  }
}


/*!
 * TCBスケジューリング依存ブロックの初期化
 * *tcb : スケジューリング依存ブロックの初期化をするTCB
 * rate : 周期
 * rel_exetim : 実行時間(仮想)
 * deadtim : デッドライン時刻
 * floatim : 余裕時間
 */
static void tsk_schdul_infocb_init(TCB *tcb, int rate, int rel_exetim, int deadtim, int floatim)
{
  SCHDUL_TYPE schdul_type = mg_schdul_info.type;
  SCHDUL_DEP_INFOCB *p_schcb = &tcb->schdul_info;

  /* タスクごとにスライスタイムを決定するスライスタイム型スケジューラの場合 */
  if (schdul_type == RR_SCHEDULING || schdul_type == RR_PRI_SCHEDULING) {
    p_schcb->un.slice_schdul.tm_slice = -1; /* タスクタイムスライスは初期設定は-1 */
  }
  /* レディーキューを2種類もつ簡易O(1)スケジューラの場合 */
  else if (schdul_type == ODRONE_SCHEDULING) {
    p_schcb->un.odrone_schdul.readyque_flag = 0; /* まだレディーにつながれていないので，0をセット */
  }
  /* Fair Schedulerの場合 */
  else if (schdul_type == FR_SCHEDULING || schdul_type == PFR_SCHEDULING) {
    p_schcb->un.fr_schdul.rel_exetim = 0;
  }
  /* Rate Monotonicスケジューリングの場合(周期と実行時間のみ使用) */
  else if (schdul_type == RM_SCHEDULING) {
    p_schcb->un.rt_schdul.rate = rate;
    p_schcb->un.rt_schdul.rel_exetim = rel_exetim;
    /* Rate Monotonicの展開スケジューリングをするための値をセット */
    //set_unrolled_schedule_val(rate, rel_exetim);
  }
  /* Deadline Monotonicスケジューリングの場合(周期と実行時間,デッドラインを使用) */
  else if (schdul_type == DM_SCHEDULING) {
    p_schcb->un.rt_schdul.rate = rate;
    p_schcb->un.rt_schdul.rel_exetim = rel_exetim;
    p_schcb->un.rt_schdul.deadtim = deadtim;
    /* Deadline Monotonicの展開スケジューリングをするための値をセット */
    //set_unrolled_schedule_val(deadtim, rel_exetim);
  }
  /* Earliest Deadline Firstスケジューリングの場合(デッドライン時刻のみ使用) */
  else if (schdul_type == EDF_SCHEDULING) {
    p_schcb->un.rt_schdul.deadtim = deadtim;
    p_schcb->un.rt_schdul.tobjp = 0;
  }
  /* Least Laxity Firstスケジューリングの場合(余裕時刻のみ使用) */
  else if (schdul_type == LLF_SCHEDULING) {
    p_schcb->un.rt_schdul.floatim = floatim;
    p_schcb->un.rt_schdul.tobjp = 0;
  }
  /* 上記以外のスケジューリング */
  else {
    /* 処理なし */
  }
}


/*!
 * TCBレディー依存ブロックの初期化
 * *tcb : レディー依存ブロックの初期化をするTCB
 */
static void tsk_ready_infocb_init(TCB *tcb)
{
  READY_TYPE ready_type = mg_ready_info.type;
  READY_DEP_INFOCB *p_rdycb = &tcb->ready_info;

  /* レディーが2分木の場合 */
  if (ready_type == BINARY_TREE || ready_type == PRIORITY_BINARY_TREE) {
    p_rdycb->un.btree_ready.parent = p_rdycb->un.btree_ready.left_next = p_rdycb->un.btree_ready.right_next =                             p_rdycb->un.btree_ready.sort_next = p_rdycb->un.btree_ready.sort_prev = NULL;
    p_rdycb->un.btree_ready.dynamic_prio = -1;
  }
  /* レディーがキューの場合 */
  else {
    p_rdycb->un.que_ready.ready_next = p_rdycb->un.que_ready.ready_prev = NULL;
  }
}


/*!
 * タスクの優先度を初期化
 * -スケジューリングによって優先度の扱いが異なる
 * -init.priorityは優先度変更があるため
 * *tcb : 優先度を初期化するTCB
 * priority : 初期化する優先度
 */
static void tsk_priority_init(TCB *tcb, int priority)
{
  SCHDUL_TYPE schdul_type = mg_schdul_info.type;

  /* 
   * 静的優先度を使用しないスケジューリングの場合
   * なお，init.priorityは作成時の優先度を記録(優先度変更のシステムコールと休止状態があるため)
   */
  if (schdul_type == FCFS_SCHEDULING || schdul_type == RR_SCHEDULING || schdul_type == EDF_SCHEDULING                                   || schdul_type == LLF_SCHEDULING || schdul_type == FR_SCHEDULING) {
    tcb->init.priority = tcb->priority = -1;
  }
  /* 優先度を使用するスケジューリングの場合 */
  else {
    tcb->init.priority = tcb->priority = priority;
  }
}


/*!
 * タスクのスタック領域の確保
 * -タスクスタックはheapメモリを使用しない(スタックトレースをしやすくするため)
 * -リンカスクリプトで定義しているタスクスタック領域は，再利用を考えて，リスト管理をした方が良い
 * *tcb : スタック領域を確保するTCB
 * stacksize : 確保するスタックサイズ
 * (返却値)E_NOMEM : スタック領域が確保できない
 * (返却値)E_OK : スタック領域が確保完了
 */
static ER get_tsk_stack(TCB *tcb, int stacksize)
{
  static char *p_stack = (char *)&_tskstack;

  tcb->stacksize = stacksize;

  /* タスクスタック領域を獲得 */
  memset(p_stack, 0, stacksize);
  p_stack += stacksize; /* タスクスタック領域を切り出す */
  
  /* タスクスタック空間検査 */
  if (p_stack > (char *)(0x90000000 + 0x00002000)) {
    KERNEL_OUTMSG("task stack over flow.\n");
    return E_NOMEM;
  }

  tcb->stack = p_stack; /* タスクスタックの上方アドレスの設定 */

  return E_OK; /* ローカルstatic変数は返却できないので，エラーコードを返却 */
}


/*!
 * タスクの終了の手続きをする関数
 * -タスクの終了はタスク処理の延長となるのでシステムコールで終了させると実装が簡単になる
 */
static void tsk_endup(void)
{
  mz_ext_tsk(); /* システムコール(自タスクの終了) */
}


/*!
 * タスク(スレッド)のスタートアップ
 * -argcとargvをR1~R2レジスタ経由でもらう事ができるが，引数が増えるため，TCBで渡す事にする.
 * *tcb: スタートアップするTCB
 */
static void tsk_startup(TCB *tcb)
{
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("D:task_manage:tsk_startup ");
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE((unsigned long)tcb->init.func, 0);
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" tcb->init.func value(addr).\n");

  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("D:task_manage:tsk_startup ");
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(tcb->init.name);
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" out task name.\n");
 
  /* タスク(スレッド)のメイン関数を呼び出す.タスクはすべて最初はここから呼ばれる */
  tcb->init.func(tcb->init.argc, tcb->init.argv);
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("D:task_manage:tsk_startup init.func clear.\n");
  tsk_endup(); /* タスクの終了 */
}


/*!
 * タスクスタックの初期化をする関数
 * *tcb : タスクスタック初期化をするTCB
 */
static void tsk_stack_init(TCB *tcb)
{
  UINT32 *sp;

  sp = (UINT32 *)tcb->stack;

  *(--sp) = (UINT32)tsk_endup; /* R14_SYS:タスクの戻り先 */
  *(--sp) = 0; /* R12 */
  *(--sp) = 0; /* R11 */
  *(--sp) = 0; /* R10 */
  *(--sp) = 0; /* R9 */
  *(--sp) = 0; /* R8 */
  *(--sp) = 0; /* R7 */
  *(--sp) = 0; /* R6 */
  *(--sp) = 0; /* R5 */
  *(--sp) = 0; /* R4 */
  *(--sp) = 0; /* R3 */
  *(--sp) = 0; /* R2 */
  *(--sp) = (UINT32)tsk_startup; /* PC */
  *(--sp) = 0; /* R1 */
  *(--sp) = (UINT32)tcb; /* R0(タスクスタートアップ(tsk_startup()に渡す引数の設定)) */

  /* 優先度が0の場合は割込み禁止とする */
  /* 優先度が0以外の場合 */
  if (tcb->priority) {
    *(--sp) = (UINT32)(CPSR_SYS_MODE | 0);
  }
  /* 優先度が0の場合 */
  else {
    *(--sp) = (UINT32)(CPSR_SYS_MODE | IRQ_DISABLE | FIQ_DISABLE);
  }

  tcb->intr_info.sp = (UINT32)sp; /* タスクスタック下方アドレスの設定 */
}


/*!
 * システムコールの処理(acre_tsk():タスクの生成(ID自動割付))
 * タスクの状態としては未登録状態から休止状態に移行
 * func : タスクのメイン関数
 * *name : タスクの名前
 * priority : タスクの優先度
 * stacksize : ユーザスタック(タスクのスタック)のサイズ
 * rate : 周期
 * rel_exetim : 実行時間(仮想)
 * deadtim : デッドライン時刻
 * floatim : 余裕時間
 * argc : タスクのメイン関数の第一引数
 * *argv[] : タスクのメイン関数の第二引数
 * (返却値)E_NOMEM : メモリが確保できない
 * (返却値)tcb : 正常終了(作成したタスクコントロールブロックへのポインタ)
 */
OBJP acre_tsk_isr(TSK_FUNC func, char *name, int priority,
                  int stacksize, int rate, int rel_exetim, int deadtim, int floatim, int argc, char *argv[])
{
  TCB *tcb; /* 新規作成するTCB(タスクコントロールブロック) */
    
  /* 動的型taskの場合 */
 
  tcb = mg_tsk_info.freehead; /* free headからノードを与える(抜き取るわけではない) */
  mg_tsk_info.freehead = tcb->free_next; /* free headを一つ進める */
  /* aloc headが空の場合 */
  if (mg_tsk_info.alochead == NULL) {
    mg_tsk_info.alochead = tcb;
  }

  /* TCBの設定 */
  tcb->state    &= STATE_CLEAR; /* init()で-1が設定されているので，一度0クリア */
  tcb->state    |= TASK_DORMANT; /* タスクを休止状態へ */
  tcb->get_info.flags = tcb->get_info.gobjp = 0; /* 取得情報を初期化 */
  tcb->wait_info.tobjp = tcb->wait_info.wobjp = 0; /* 待ち情報を初期化 */
  tcb->wait_info.wait_next = tcb->wait_info.wait_prev = NULL; /* 待ちポインタをNULLに */

  /* TCBの設定(実行時に変化しない内容) */
  tcb->init.tskid = mg_tsk_info.counter;
  tcb->init.func = func;
  tcb->init.argc = argc;
  tcb->init.argv = argv;
  strcpy(tcb->init.name, name);
    
  tsk_schdul_infocb_init(tcb, rate, rel_exetim, deadtim, floatim); /* TCBのスケジューリング依存情報ブロックの初期化 */

  tsk_ready_infocb_init(tcb); /* TCBのスケジューリング依存情報ブロックの初期化 */
  
  tsk_priority_init(tcb, priority); /* タスクの優先度を初期化 */

  /* タスクのスタック領域を確保 */
  if (E_NOMEM == get_tsk_stack(tcb, stacksize)) {
    return E_NOMEM;
  }

  tsk_stack_init(tcb); /* タスクスタックの初期化 */

  return (OBJP)tcb;
}


/*!
 * システムコール処理(del_tsk():タスクの排除)
 * 休止状態にあるタスクを未登録状態にする(自タスクの排除は認めない)
 * *tcb : 排除するタスクコントロールブロックへのポインタ
 * (返却値)E_OK : 正常終了
 * (返却値)E_OBJ : エラー終了(タスクがその他の状態)
 */
ER del_tsk_isr(TCB *tcb)
{
  /* 本来ならスタックも解放して再利用できるようにすべきだが省略．*/
  
  /* 休止状態の場合(排除) */
  if (tcb->state == TASK_DORMANT) {
    memset(tcb, -1, sizeof(*tcb)); /* ノードを初期化 */
    /* タスクalocリストから抜き取りfreeリストへ挿入 */
    get_aloclist(tcb); /* alocリストから抜き取り */
    /* freeリスト先頭へ挿入 */
    tcb->free_next = mg_tsk_info.freehead;
    tcb->free_prev = mg_tsk_info.freehead->free_prev;
    mg_tsk_info.freehead = tcb->free_next->free_prev = tcb->free_prev->free_next = tcb;
    
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("delete task contorol block for interrput handler\n");
    
    return E_OK;
  }
  /* その他の状態 */
  else {
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("not delete.\n");
    return E_OBJ;
  }
}


/*!
 * システムコール処理(sta_tsk():タスクの起動)
 * タスクの状態としては休止状態から実行可能状態へ移行
 * *tcb : 起動するtcb
 * (返却値)E_OK : 正常終了
 * (返却値)E_OBJ : エラー終了(タスクが休止状態ではない)
 */
ER sta_tsk_isr(TCB *tcb)
{
  DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" sta_tsk_isr start.\n");
 
  current = tcb;
  current->syscall_info.flag = MZ_VOID;
  /* sta_tsk()のシステムコールは休止状態の時の使用可能 */
  if (current->state == TASK_DORMANT) {
    current->state &= ~TASK_DORMANT; /* 休止状態解除 */
    putcurrent(); /* 起動要求タスクをレディーへ */
    return E_OK;
  }
  /* 休止状態ではない場合 */
  else {
    return E_OBJ;
  }
}


/*!
 * システムコールの処理(acre_tsk():タスクの生成(ID自動割付)と起動)
 * -タスクの状態としては未登録状態から休止状態に移行後，実行可能状態へ移行
 * func : タスクのメイン関数
 * *name : タスクの名前
 * priority : タスクの優先度
 * stacksize : ユーザスタック(タスクのスタック)のサイズ
 * rate : 周期
 * rel_exetim : 実行時間(仮想)
 * deadtim : デッドライン時刻
 * floatim : 余裕時間
 * argc : タスクのメイン関数の第一引数
 * *argv[] : タスクのメイン関数の第二引数
 * (返却値)rcd(E_PAR) : システムコールの引数不正
 * (返却値)rcd(E_NOID) : 動的メモリが取得できない(割付可能なIDがない)
 * (返却値)rcd(E_NOMEM) : メモリが確保できない
 * (返却値)rcd : 自動割付したID
 * (返却値)ercd(E_ID) : エラー終了(タスクIDが不正)
 * (返却値)ercd(E_NOEXS) : エラー終了(対象タスクが未登録)
 * (返却値)ercd(E_OBJ) : エラー終了(タスクが休止状態ではない)
 */
ER_ID run_tsk_isr(SYSCALL_PARAMCB *p)
{
  ER_ID acre_rcd; /* 自動割付IDまたはエラーコードを記録 */
  ER sta_rcd;
  SYSCALL_PARAMCB acre_ptr, sta_ptr;

  acre_ptr.un.acre_tsk.func = p->un.run_tsk.func;
  acre_ptr.un.acre_tsk.name = p->un.run_tsk.name;
  acre_ptr.un.acre_tsk.priority = p->un.run_tsk.priority;
  acre_ptr.un.acre_tsk.stacksize = p->un.run_tsk.stacksize;
  acre_ptr.un.acre_tsk.rate = p->un.run_tsk.rate;
  acre_ptr.un.acre_tsk.rel_exetim = p->un.run_tsk.rel_exetim;
  acre_ptr.un.acre_tsk.deadtim = p->un.run_tsk.deadtim;
  acre_ptr.un.acre_tsk.floatim = p->un.run_tsk.floatim;
  acre_ptr.un.acre_tsk.argc = p->un.run_tsk.argc;
  acre_ptr.un.acre_tsk.argv = p->un.run_tsk.argv;

  /* タスク生成ルーチン呼び出し */
  kernelrte_acre_tsk(&acre_ptr);
  sta_ptr.un.sta_tsk.tskid = acre_rcd = acre_ptr.un.acre_tsk.ret;
  /* 生成できなかった場合はkernelrte_acre_tsk()のエラーコードを返却 */
  if (acre_rcd < E_OK) {
    return acre_rcd;
  }

  /* タスク起動ルーチン呼び出し(kernelrte_sta_tsk()のputcurrent()はすでにレディーへ存在するので，無視される) */
  kernelrte_sta_tsk(&sta_ptr);
  sta_rcd = sta_ptr.un.sta_tsk.ret;

  /* 起動できた場合はIDを返却 */
  if (sta_rcd == E_OK) {
    return acre_rcd;
  }
  /* 起動できなかった場合はkernelrte_sta_tsk()のエラーコードを返却 */
  else {
    return (ER_ID)sta_rcd;
  }
}


/*!
 * システムコールの処理(ext_tsk():自タスクの終了)
 * タスクの状態としては実行状態から休止状態へ移行
 * 自タスクの終了は資源を取得しているものは解放する処理をする(ただし，取得したオブジェクト1つの時のみ)
 * リターンパラメータはあってはならない
 */
void ext_tsk_isr(void)
{
  TCB *tmpcurrent;
  //TMRCB *tbf;
  //SCHDUL_TYPE schdul_type = mg_schdul_info.type;
  //TMR_OBJP *p_tobjp = &current->schdul_info.un.rt_schdul.tobjp;
  
  /* 
   * カーネルオブジェクトを取得しているかチェック.
   * release_objectの後のコールスタックでcurrentが書き換えられる場合があるので，
   * 呼び出したタスク(自タスク)を一時退避させておく
   */
  if (current->get_info.flags != TASK_NOT_GET_OBJECT) {
    tmpcurrent = current;
    //release_object(current); /* 実体はkernel.cにある */
    current = tmpcurrent;
  }
  
  /* システムコール発行時タスクはカレントタスクから抜かれてくるのでstateは0になっている */
  current->state |= TASK_DORMANT; /* タスクを休止状態へ */
  KERNEL_OUTMSG(current->init.name);
  KERNEL_OUTMSG(" DORMANT.\n");

  /*
   * EDFかLLFで，タイマブロックが作成されている場合
   * EDFまたはLLFは休止状態または未登録状態時にデッドラインタイマを解除する
   * (スライス型スケジューリングのようにプリエンプト時ではない)
   */
  //if ((schdul_type == EDF_SCHEDULING || schdul_type == LLF_SCHEDULING) && *p_tobjp != 0) {
  //tbf = (TMRCB *)*p_tobjp;
  /* タイマブロックを排除 */
  //delete_tmrcb_diffque(tbf);
  //*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
  //}
  
  current->priority = current->init.priority; /* タスクの優先度を起動状態へ戻す */
  tsk_stack_init(current); /* ユーザスタックを起動時に戻す */
}


/*!
 * システムコールの処理(exd_tsk():自タスクの終了と排除)
 * TCBが排除されるので返却値はなしにする
 */
void exd_tsk_isr(void)
{
  TCB *tmpcurrent;
  //TMRCB *tbf;
  //SCHDUL_TYPE schdul_type = mg_schdul_info.type;
  //TMR_OBJP *p_tobjp = &current->schdul_info.un.rt_schdul.tobjp;
  
  /*
   * カーネルオブジェクトを取得しているかチェック.
   * release_objectの後のコールスタックでcurrentが書き換えられる場合があるので，
   * 呼び出したタスク(自タスク)を一時退避させておく
   */
  if (current->get_info.flags != TASK_NOT_GET_OBJECT) {
    tmpcurrent = current;
    //release_object(current); /* 実体はkernel.cにある */
    current = tmpcurrent;
  }
  
  /* 本来ならスタックも解放して再利用できるようにすべきだが省略． */
  KERNEL_OUTMSG(current->init.name);
  KERNEL_OUTMSG(" EXIT.\n");

  /*
   * EDFかLLFで，タイマブロックが作成されている場合
   * EDFまたはLLFは休止状態または未登録状態時にデッドラインタイマを解除する
   * (スライス型スケジューリングのようにプリエンプト時ではない)
   */
  //if ((schdul_type == EDF_SCHEDULING || schdul_type == LLF_SCHEDULING) && *p_tobjp != 0) {
  //tbf = (TMRCB *)*p_tobjp;
  /* タイマブロックを排除 */
  //delete_tmrcb_diffque(tbf);
  //*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
  //}
  
  memset(current, -1, sizeof(*current)); /* ノードの初期化 */
  /* タスクalocリストから抜き取りfreeリストへ挿入 */
  get_aloclist(current); /* alocリストから抜き取り */
  /* freeリスト先頭へ挿入 */
  current->free_next = mg_tsk_info.freehead;
  current->free_prev = mg_tsk_info.freehead->free_prev; /* NULLが入る */
  mg_tsk_info.freehead = current->free_next->free_prev = current->free_prev->free_next = current;
  mg_tsk_info.id_table[current->init.tskid] = NULL; /* ID変換テーブルのクリア */
}


/*!
 * システムコール処理(ter_tsk():タスクの強制終了)
 * 自タスク以外のタスクを強制終了する(実行可能状態と待ち状態にあるもの)
 * 実行可能状態にあるものはレディーキューから抜き取る処理，
 * 待ち状態にあるものは待ち要件を調べ該当の待ちキューから抜き取る
 * 強制終了なので，待ち状態に入っているシステムコールの返却値は書き換えない
 * tskid : 強制終了するタスクコントロールブロックへのポインタ
 * (返却値)E_ILUSE : エラー終了(タスクが実行状態.つまり自タスク)
 * (返却値)E_OBJ : エラー終了(タスクが休止状態)
 * (返却値)E_OK : 正常終了(タスクが実行可能状態または待ち状態)
 */
ER ter_tsk_isr(TCB *tcb)
{
  //TMRCB *tbf;
  //SCHDUL_TYPE schdul_type = mg_schdul_info.type;
  //TMR_OBJP *p_tobjp = &tcb->schdul_info.un.rt_schdul.tobjp;

  /* 実行状態 */
  if (current == tcb) {
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("not temination activ.\n");
    return E_ILUSE;
  }
  /* 休止状態の場合 */
  else if (tcb->state == TASK_DORMANT) {
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("not termination dormant.\n");
    return E_OBJ;
  }
  /* ここから強制終了処理 */
  else {
    /* 実行可能状態 */
    if (tcb->state == TASK_READY) {
      get_tsk_readyque(tcb); /* レディーキューから抜き取る(呼び出した後はcurrentに設定されている) */
      //release_object(tcb); /* 何らかのカーネルオブジェクトを取得したままの場合，自動解放する */ 
    }
    /* 待ち状態(何らかの待ち行列につながれている時は対象タスクを待ち行列からはずす) */
    else {
      //get_tsk_waitque(tcb, (tcb->state & ~TASK_STATE_INFO));
      /* タイマブロックを持っているものは対象タイマブロックを排除する */
      if (tcb->wait_info.tobjp != 0) {
        //delete_tmrcb_diffque((TMRCB *)tcb->wait_info.tobjp);
        tcb->wait_info.tobjp = 0; /* クリアにしておく */
      }
      /*
       * カーネルオブジェクトを取得しているかチェック.
       * release_objectの後のコールスタックでcurrentが書き換えられる場合があるので，
       * 一時退避させておく
       */
      if (tcb->get_info.flags != TASK_NOT_GET_OBJECT) {
        //release_object(tcb); /* 何らかのカーネルオブジェクトを取得したままの場合，自動解放する */
      }
    }
    tcb->state |= TASK_DORMANT; /* タスクを休止状態へ */
    KERNEL_OUTMSG(tcb->init.name);
    KERNEL_OUTMSG(" DORMANT.\n");

    /*
     * EDFかLLFで，タイマブロックが作成されている場合
     * EDFまたはLLFは休止状態または未登録状態時にデッドラインタイマを解除する
     * (スライス型スケジューリングのようにプリエンプト時ではない)
     */
    //if ((schdul_type == EDF_SCHEDULING || schdul_type == LLF_SCHEDULING) && *p_tobjp != 0) {
    //tbf = (TMRCB *)*p_tobjp;
    /* タイマブロックを排除 */
    //delete_tmrcb_diffque(tbf);
    //*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
    //}
  
    tcb->priority = tcb->init.priority; /* タスクの優先度を起動状態へ戻す */
    tsk_stack_init(tcb); /* ユーザスタックを起動時に戻す */
    
    return E_OK;
  }
}


/*!
 * システムコールの処理(get_pri():タスクの優先度取得)
 * tskid : 優先度を参照するタスクコントロールブロックへのポインタ
 * *p_tskpri : 参照優先度を格納するポインタ(実体はユーザタスク側で宣言されているもの)
 * (返却値)E_OBJ : エラー終了(対象タスクが休止状態)
 * (返却値)E_OK : 正常終了
 */
ER get_pri_isr(TCB *tcb, int *p_tskpri)
{
  /* 休止状態の場合 */
  if (tcb->state == TASK_DORMANT) {
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("not change priority is tsk dormant for interrput hanler.\n");
    return E_OBJ;
  }
  /* タスクの優先度を設定 */
  else {
    *p_tskpri = tcb->priority;
    return E_OK;
  }
}


/*!
 * システム・コールの処理(chg_pri():タスクの優先度変更)
 * タスクの状態が未登録状態，休止状態の場合は変更できない
 * 同じレベルでの優先度変更は変更対象タスクはそれぞれの待ち行列の最後に接続される
 * E_NOEXSはμITRONにはない
 * E_PARは上限優先度違反ではなく(シーリング値はシステムコールの引数指定しない)デフォルト時の優先度個数とする
 * (defines.hに記述してあるやつ)
 * tcb : 変更対象のタスクコントロールブロックへのポインタ
 * tskpri : 変更する優先度
 * (返却値)E_PAR : エラー終了(tskpriが不正)
 * (返却値)E_OBJ : エラー終了(タスクが休止状態)
 * (返却値)E_OK : 正常終了
 */
ER chg_pri_isr(TCB *tcb, int tskpri)
{
  /*優先度は有効化か*/
  if (tskpri < 0 || PRIORITY_NUM < tskpri) {
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("not change priority is deffer tskpri fot interrput handler.\n");
    return E_PAR;
  }
  /*休止状態の場合*/
  else if (tcb->state == TASK_DORMANT) {
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("not change priority is tsk dormant for interrput hanler.\n");
    return E_OBJ;
  }
  /*その他の場合優先度を変更する*/
  else {
    /* 実行状態タスクがタスクコンテキスト用システムコールを呼んだ場合 */
    if (current->syscall_info.flag == MZ_SYSCALL) {
      getcurrent(); /* システムコール発行タスクをレディーから抜き取る */
      chg_pri_syscall_isr(tcb, tskpri); /*優先度変更*/
    }
    /* 実行状態タスクが非タスクコンテキスト用システムコールを呼んだ場合 */
    else if (current->syscall_info.flag == MZ_ISYSCALL) {
      chg_pri_isyscall_isr(tcb, tskpri); /*優先度変更*/
    }
    return E_OK;
  }
}


/*!
 * タスクコンテキスト用の優先度変更関数
 * システムコール発行タスク(実行状態タスク)はレディーから抜き取られてくる
 * *tcb : 優先度変更対象タスク
 * tskpri : 変更する優先度
 */
static void chg_pri_syscall_isr(TCB *tcb, int tskpri)
{
  /*
   * 優先度変更タスクが実行状態の場合，実行状態タスクはレディーから抜き取られてくるので，
   * 優先度を変更してから，実行状態タスクをレディーへ戻す
   */
  if (tcb == current) {
    current->priority = tskpri; /* 実行状態タスクの優先度変更 */
    putcurrent(); /* システムコール発行タスクをレディーへ */
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("tskid ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->init.tskid, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->priority, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" change priority activ(context syscall) for interruput handler.\n");
  }
  /*
   * 優先度変更タスクが実行可能状態(レディーに存在する)の場合，実行状態タスクはレディーから抜き取られてくるので，
   * 実行状態タスクをレディーへ戻してから(get_tsk_readyque()実行後，抜き取りタスクはcurrentに設定される)，
   * 実行可能状態タスクをレディーから抜き取り，優先度を変更して，レディーへ戻す
   */
  else if (tcb->state & TASK_READY) {
    putcurrent(); /* システムコール発行タスクをレディーへ */
    get_tsk_readyque(tcb); /* レディーキューから抜き取る関数(scheduler.cにある) */
    current->priority = tskpri; /* 実行可能状態タスクの優先度を変更 */
    putcurrent(); /* 変更したタスクをレディーへ */
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("tskid ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->init.tskid, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->priority, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" change priority ready(context syscall) for interruput handler.\n");
  }
  /*
   * 優先度変更タスクが待ち状態(レディーに存在しない)の場合，実行状態タスクはレディーから抜き取られてくるので，
   * 待ち状態タスクの優先度を変更して，実行状態タスクをレディーへ戻す
   */
  else {
    putcurrent(); /* システムコール発行タスクをレディーへ */
    tcb->priority = tskpri; /* 待ち状態タスクの優先度変更 */
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("tskid ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(tcb->init.tskid, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(tcb->priority, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" change priority sleep(context syscall) for interruput handler.\n");
  }
}


/*!
 * 非タスクコンテキスト用の優先度変更関数
 * -システムコール発行タスク(実行状態タスク)はレディーから抜き取られてくる場合と抜き取られない場合がある
 * 抜き取られる場合 : システムコール割込みの非タスクコンテキストで呼ばれる
 * 抜き取られない場合 : シリアル割込みハンドラの非タスクコンテキストで呼ばれる(シリアル割込みはタスクを切り替えず，
 *                     かつユーザが自作できるものとしてしている)
 * *tcb : 優先度変更対象タスク
 * tskpri : 変更する優先度
 */
static void chg_pri_isyscall_isr(TCB *tcb, int tskpri)
{
  /*
   * /優先度変更タスクが実行状態の場合，実行状態タスクはレディーから抜き取られる場合と
   * 抜き取られない場合がある
   * /抜き取られる場合は優先度を変更するのみ
   * /抜き取られない場合は一度レディーから実行状態タスクを抜き取り，優先度を変更して，再度レディーへ戻す
   */
  if (tcb == current) {
    /* 抜き取られない場合 */
    if (tcb->intr_info.type == SERIAL_INTERRUPT) {
      getcurrent(); /* 非タスクコンテキスト用システムコール発行タスクをレディーへ */
    }
    current->priority = tskpri; /* 実行状態タスクの優先度変更 */
    /* 抜き取られない場合 */
    if (tcb->intr_info.type == SERIAL_INTERRUPT) {
      putcurrent(); /* 非タスクコンテキスト用システムコール発行タスクをレディーへ */
    }
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("tskid ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->init.tskid, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->priority, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" change priority activ(not context syscall) for interruput handler.\n");
  }
  /*
   * 優先度変更タスクが実行可能状態(レディーに存在する)の場合，実行状態タスクはレディーから抜き取られないので，
   * 実行可能状態タスクをレディーから抜き取り，優先度を変更して，レディーへ戻す
   */
  else if (tcb->state & TASK_READY) {
    get_tsk_readyque(tcb); /* レディーキューから抜き取る関数(scheduler.cにある) */
    current->priority = tskpri;
    putcurrent();
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("tskid ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->init.tskid, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(current->priority, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" change priority ready(not context syscall) for interruput handler.\n");
  }
  /*
   * 優先度変更タスクが待ち状態(レディーに存在しない)の場合，実行状態タスクはレディーから抜き取られないので，
   * 待ち状態タスクの優先度を変更のみ
   */
  else {
    tcb->priority = tskpri;
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG("tskid ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(tcb->init.tskid, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" ");
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTVLE(tcb->priority, 0);
    DEBUG_L1_KERNEL_TASK_MANAGE_OUTMSG(" change priority sleep(not context syscall) for interruput handler.\n");
  }
}
