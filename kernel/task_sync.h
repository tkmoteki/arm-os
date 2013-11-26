#ifndef _TASK_SYNC_H_
#define _TASK_SYNC_H_

/* include/kernel */
#include "defines.h"


/*! システムコールの処理(slp_tsk():自タスクの起床待ち) */
ER slp_tsk_isr(void);

/*! システムコールの処理(wup_tsk():タスクの起床) */
ER wup_tsk_isr(TCB *tcb);

/*! システムコールの処理(rel_wai():待ち状態強制解除) */
ER rel_wai_isr(TCB *tcb);


#endif
