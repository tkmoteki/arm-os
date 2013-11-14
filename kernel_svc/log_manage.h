#ifndef _LOG_MANAGE_H_INCLUDED_
#define _LOG_MANAGE_H_INCLUDED_


#include "kernel/defines.h"


#define CONTEXT_LOG_ONESIZE 92    /* コンテキストログ1つあたりの大きさ */


/* ログの数 */
UINT32 log_counter;

/*! ログ管理機構の初期化 */
void log_mechanism_init(void);

/*! コンテキストスイッチングのログの出力(Linux jsonログを参考(フォーマット変換は行わない)) */
void get_log(OBJP log_tcb);


#endif
