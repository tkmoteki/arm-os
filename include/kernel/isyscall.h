#ifndef _ISYSCALL_H_INCLUDED_
#define _ISYSCALL_H_INCLUDED_

#include "include/kernel/defines.h"
#include "include/kernel/syscall.h"


/* 非タスクコンテキストから呼ぶシステムコールのプロトタイプ，実体はsyscall.cにある) */
/*! mz_iacre_tsk():タスクの生成 */
ER mz_iacre_tsk(SYSCALL_PARAMCB *par);
/*! mz_ista_tsk():タスクの起動 */
ER mz_ista_tsk(ER_ID tskid);

/*! mz_ichg_pri():タスクの優先度変更 */
ER mz_ichg_pri(ER_ID tskid, int tskpri);

/*! mz_iwup_tsk():タスクの起床 */
ER mz_iwup_tsk(ER_ID tskid);


#endif
