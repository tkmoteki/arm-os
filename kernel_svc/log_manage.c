/* private interface */

/* public interface */
/* include/kernel_svc */
#include "log_manage.h"
/* include/kernel */
#include "debug.h"
#include "kernel.h"
#include "message.h"
#include "scr_symbols.h"
/* include/lib/c */
#include "lib.h"


#define LOG_BUFFER_MAX      4096  /* ログバッファの最大 */
#define CONTEXT_LOG_ONESIZE 92    /* コンテキストログ1つあたりの大きさ */


volatile unsigned long *logbuf;


/*! ログ管理機構の初期化 */
void log_mechanism_init(void)
{
  logbuf = &_logbuffer_start;
  log_counter = 0;
  
  logbuf++;
}


/*!
 * コンテキストスイッチングのログの出力(Linux jsonログを参考(フォーマット変換は行わない))
 * log_tcb : コンテキストスイッチング対象TCB
 */
void get_log(OBJP log_tcb)
{
  static UINT32 tmp = 0;
  TCB *work = (TCB *)log_tcb;
  UINT32 tskid, priority, state;

  /* 実行可能タスクが決定していない時(init_tsk生成等で呼ばれる) */
  if (work == NULL) {
    return;
  }
  /* ログバッファがオーバーフローした場合 */
  else if (LOG_BUFFER_MAX < ((log_counter + 1) * CONTEXT_LOG_ONESIZE)) {
    KERNEL_OUTMSG("error: get_log() \n");
    down_system(); /* kernelのフリーズ */
  }
  else {
    /* 処理なし */
  }

  tskid = (UINT32)work->init.tskid;
  priority = (UINT32)work->priority;
  state = (UINT32)work->state;
  
  /*
   * prevのログをメモリセグメントへ
   * ・prevログ
   *  システムコールを発行したタスクの次(ISR適用後(currentが切り替わるものは，適用前のログとなる))の状態
   */
  if (log_counter == tmp) {
    DEBUG_L1_KERNEL_SVC_LOG_MANAGE_OUTMSG("ddddd.\n");
    memset((char *)logbuf, 0, TASK_NAME_SIZE * 4);
    strcpy((char *)logbuf, work->init.name);
    logbuf += TASK_NAME_SIZE;
    *logbuf = tskid;
    logbuf++;
    //*logbuf = secs; //時間
    //logbuf++;
    *logbuf = tskid;
    logbuf++;
    *logbuf = priority;
    logbuf++;
    *logbuf = state;
    logbuf++;
    tmp = log_counter;
    log_counter++;
  }
  /*
   * nextのログをメモリセグメントへ
   * ・nextログ
   *  次にディスパッチされるタスクの状態
   */
  else {
    DEBUG_L1_KERNEL_SVC_LOG_MANAGE_OUTMSG("eeeee.\n");
    *logbuf = tskid;
    logbuf++;
    *logbuf = priority;
    logbuf++;
    *logbuf = state;
    logbuf++;
    tmp = log_counter;
  }
}
