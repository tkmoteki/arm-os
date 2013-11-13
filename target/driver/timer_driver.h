#ifndef _TIMER_DRIVER_H_INCLUDED_
#define _TIMER_DRIVER_H_INCLUDED_


/*! 周期タイマスタート */
void start_cycle_timer(int index, int usec);

/*! ワンショットタイマスタート */
void start_oneshot_timer(int index, int usec);

/*! 周期タイマ割込み満了処理 */
void expire_cycle_timer(int index);

/*! ワンショットタイマ割込み満了処理 */
void expire_oneshot_timer(int index);

/*! タイマの停止 */
void stop_timer(int index);

/*! タイマキャンセル */
void cancel_timer(int index);

/*! タイマ動作中か検査する関数 */
ER_VLE get_timervalue(int index);


#endif
