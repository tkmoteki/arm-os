/*
 * task.hにはタスク管理機能及び，タスク付属同期機能の割込みサービスルーチン
 * 及び主にそれが使用する内部関数が記述されたtask.cのヘッダファイルである.
 * ・このヘッダファイルにはタスク管理機能割込みサービスルーチン，タスク同期
 *   付属機能の割込みサービスルーチンの順に各割込みサービスルーチンの
 *   プロトタイプが記述されている.
 */



#ifndef _TASK_MANAGE_H_
#define _TASK_MANAGE_H_


#include "defines.h"


/*! タスク情報(mg_tsk_info)の初期化 */
ER starup_tsk_init(void);

/*! タスクの初期化(task ID変換テーブルの領域確保と初期化) */
ER tsk_init(void);

/*! システムコールの処理(acre_tsk():タスクの生成(ID自動割付)) */
OBJP acre_tsk_isr(TSK_FUNC func, char *name, int priority, int stacksize, 
                  int rate, int exetim, int deadtim, int floatim, int argc, char *argv[]);
         
/*! システムコール処理(del_tsk():タスクの排除) */
ER del_tsk_isr(TCB *tcb);

/*! システムコール処理(sta_tsk():タスクの起動) */
ER sta_tsk_isr(TCB *tcb);

/*! システムコールの処理(run_tsk():タスクの生成(ID自動割付)と起動) */
ER_ID run_tsk_isr(SYSCALL_PARAMCB *p);

/*! システムコールの処理(ext_tsk():自タスクの終了) */
void ext_tsk_isr(void);

/*! システムコールの処理(exd_tsk():自タスクの終了と排除) */
void exd_tsk_isr(void);

/*! システムコール処理(ter_tsk():タスクの強制終了) */
ER ter_tsk_isr(TCB *tcb);

/*! システムコールの処理(get_pri():タスクの優先度取得) */
ER get_pri_isr(TCB *tcb, int *p_tskpri);

/*! システム・コールの処理(chg_pri():タスクの優先度変更) */
ER chg_pri_isr(TCB *tcb, int tskpri);

/*! タスクの優先度を変更する関数 */
void chg_pri_tsk(TCB *tcb, int tskpri);

/*! システムコールの処理(get_id():タスクID取得) */
ER_ID get_tid_isr(void);


#endif
