/* private interface */

/* public interface */
/* include/kernel */
#include "isyscall.h"
#include "kernel.h"


/*
 * interrput syscall
 * 非タスクコンテキストから呼び出すシステムコール(タスクの切り替えは行わない)
 */
/*
 * パラメータ類の退避割込みは使用しない(mz_acre_tsk():タスクコントロールブロックの生成(ID自動割付))
 * acre_tsk()システムコールはパラメータ数が多いので,とりあえず構造体×共用体でやった(他のもやったほうがいいのかな～).
 * *par : ユーザ側で定義されたシステムコールバッファポインタ
 * (返却値)E_PAR : システムコールの引数不正
 * (返却値)E_NOID : 動的メモリが取得できない(割付可能なIDがない)
 * (返却値)E_NOMEM : ユーザスタックが確保できない
 * (返却値)tskid : 正常終了
 */
ER mz_iacre_tsk(SYSCALL_PARAMCB *par)
{
  SYSCALL_PARAMCB param;
  /* iacre_tskの延長でget_tsk_readyque()が呼ばれるとcurrentが書き換えられるので一時退避 */
  TCB *tmptcb = current;
  /*
   * システムコール割込みハンドラの延長で非タスクコンテキスト用システムコールが呼ばれた時は，
   * syscall_info.flagが書き換えられるため退避
   */
  SYSCALL_TYPE tmp_flag = current->syscall_info.flag;
  
  /* パラメータ退避 */
  param.un.acre_tsk.func = par->un.acre_tsk.func;
  param.un.acre_tsk.name = par->un.acre_tsk.name;
  param.un.acre_tsk.priority = par->un.acre_tsk.priority;
  param.un.acre_tsk.stacksize = par->un.acre_tsk.stacksize;
  param.un.acre_tsk.rate = par->un.acre_tsk.rate;
  param.un.acre_tsk.rel_exetim = par->un.acre_tsk.rel_exetim;
  param.un.acre_tsk.deadtim = par->un.acre_tsk.deadtim;
  param.un.acre_tsk.floatim = par->un.acre_tsk.floatim;
  param.un.acre_tsk.argc = par->un.acre_tsk.argc;
  param.un.acre_tsk.argv = par->un.acre_tsk.argv;
  /* トラップは発行しない(単なる関数呼び出し) */
  isyscall_intr(ISR_TYPE_IACRE_TSK, &param);

  /* 実行状態タスクを前の状態へ戻す */
  current = tmptcb;
  current->syscall_info.flag = tmp_flag;

  return param.un.acre_tsk.ret;
}


/*!
 * 割込み出入り口前のパラメータ類の退避(mz_sta_tsk():スレッドの起動)
 * tskid : 起動するタスクID
 * (返却値)E_ID : エラー終了(タスクIDが不正)
 * (返却値)E_NOEXS エラー終了(対象タスクが未登録)
 * (返却値)E_OK : 正常終了
 * (返却値)E_OBJ : エラー終了(タスクが休止状態ではない)
 */
ER mz_ista_tsk(ER_ID tskid)
{
  SYSCALL_PARAMCB param;
  /* ichg_priの延長でget_tsk_readyque()が呼ばれるとcurrentが書き換えられるので一時退避 */
  TCB *tmptcb = current;
  /*
   * システムコール割込みハンドラの延長で非タスクコンテキスト用システムコールが呼ばれた時は，
   * syscall_info.flagが書き換えられるため退避
   */
  SYSCALL_TYPE tmp_flag = current->syscall_info.flag;
  
  /* パラメータ退避 */
  param.un.sta_tsk.tskid = tskid;
  /* トラップは発行しない(単なる関数呼び出し) */
  isyscall_intr(ISR_TYPE_ISTA_TSK, &param);

  /* 実行状態タスクを前の状態へ戻す */
  current = tmptcb;
  current->syscall_info.flag = tmp_flag;

  return param.un.sta_tsk.ret;
}


/*!
 * パラメータ類の退避割込みは使用しない(mz_ichg_pri():スレッドの優先度変更)
 * tskid : 優先度を変更するタスクID
 * tskpri : 変更する優先度
 * (返却値)E_ID : エラー終了(タスクIDが不正)
 * (返却値)E_NOEXS : エラー終了(タスクが未登録状態)
 * (返却値)E_PAR : エラー終了(tskpriが不正)
 * (返却値)E_OBJ : エラー終了(タスクが休止状態)
 * (返却値)E_OK : 正常終了
 */
ER mz_ichg_pri(ER_ID tskid, int tskpri)
{
  SYSCALL_PARAMCB param;
  /* ichg_priの延長でget_tsk_readyque()が呼ばれるとcurrentが書き換えられるので一時退避 */
  TCB *tmptcb = current;
  /*
   * システムコール割込みハンドラの延長で非タスクコンテキスト用システムコールが呼ばれた時は，
   * syscall_info.flagが書き換えられるため退避
   */
  SYSCALL_TYPE tmp_flag = current->syscall_info.flag;
 
  /* パラメータ退避 */
  param.un.chg_pri.tskid = tskid;
  param.un.chg_pri.tskpri = tskpri;
  /* トラップは発行しない(単なる関数呼び出し) */
  isyscall_intr(ISR_TYPE_CHG_PRI, &param);

  /* 実行状態タスクを前の状態へ戻す */
  current = tmptcb;
  current->syscall_info.flag = tmp_flag;
  
  return param.un.chg_pri.ret;
}


/*!
 * 割込み出入り口前のパラメータ類の退避(mz_iwup_tsk():タスクの起床)
 * tskid : タスクの起床するタスクID
 * (返却値)E_ID : エラー終了(タスクIDが不正)
 * (返却値)E_NOEXS : エラー終了(タスクが未登録状態)
 * (返却値)E_OBJ : 対象タスクが休止状態
 * (返却値)E_ILUSE : システムコール不正使用(要求タスクが実行状態または，何らかの待ち行列につながれている)
 * (返却値)E_OK : 正常終了
 */
ER mz_iwup_tsk(ER_ID tskid)
{
  SYSCALL_PARAMCB param;
  /* iwup_tskの延長でget_tsk_readyque()が呼ばれるとcurrentが書き換えられるので一時退避 */
  TCB *tmptcb = current;
  /*
   * システムコール割込みハンドラの延長で非タスクコンテキスト用システムコールが呼ばれた時は，
   * syscall_info.flagが書き換えられるため退避
   */
  SYSCALL_TYPE tmp_flag = current->syscall_info.flag;

  /* パラメータ退避 */
  param.un.wup_tsk.tskid = tskid;
  /* トラップは発行しない(単なる関数呼び出し) */
  isyscall_intr(ISR_TYPE_WUP_TSK, &param);

  /* 実行状態タスクを前の状態へ戻す */
  current = tmptcb;
  current->syscall_info.flag = tmp_flag;

  return param.un.wup_tsk.ret;
}
