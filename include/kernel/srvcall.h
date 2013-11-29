#ifndef _SRVCALL_H_INCLUDED_
#define _SRVSCALL_H_INCLUDED_

#include "include/kernel/defines.h"
#include "include/kernel/syscall.h"


/* サービスコール(ユーザタスクが呼ぶシステムコールのプロトタイプ，実体はsrvcall.cにある) */
/*! mv_acre_tsk():タスクコントロールブロックの生成(ID自動割付) */
ER_ID mv_acre_tsk(SYSCALL_PARAMCB *par);
          
/*! mv_del_tsk():スレッドの排除 */
ER mv_del_tsk(ER_ID tskid);

/*! mz_ter_tsk():スレッドの強制終了 */
ER mv_ter_tsk(ER_ID tskid);

/*! mv_get_pri():スレッドの優先度取得 */
ER mv_get_pri(ER_ID tskid, int *p_tskpri);

/*! mv_def_inh():割込みハンドラの定義 */
ER_ID mv_def_inh(INTRPT_TYPE type, IR_HANDL handler);

/*! mv_sel_schdul():スケジューラの切り替え */
ER mv_sel_schdul(SCHDUL_TYPE type, long param);


#endif
