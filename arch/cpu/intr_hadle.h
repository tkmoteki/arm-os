/* 割り込みハンドラを周り */

#ifndef _INTR_HANDLE_H_INCLUDED_
#define _INTR_HANDLE_H_INCLUDED_


/*!  未定義命令ハンドラ */
void intr_und(unsigned long sp);

/*! SVC割込みハンドラ */
void intr_swi(unsigned long sp);

/*! プリフェッチアボートハンドラ */
void intr_pabort(unsigned long sp);

/*! データアボートハンドラ */
void intr_dabort(unsigned long sp);

/*! IRQハンドラ */
void intr_irq(unsigned long sp);


#endif
