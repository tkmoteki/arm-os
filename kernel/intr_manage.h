#ifndef _INTR_MANAGE_H_
#define _INTR_MANAGE_H_

/* include/kernel */
#include "defines.h"


/*! 割込みハンドラ */
IR_HANDL exter_handlers[EXTERNAL_INTERRUPT_NUM];

/*! システムコールの処理(def_inh():割込みハンドラの定義) */
ER def_inh_isr(INTRPT_TYPE type, IR_HANDL handler);


#endif
