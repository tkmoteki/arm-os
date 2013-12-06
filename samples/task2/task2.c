/* public interface */
/* include/kernel */
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"    /* システムコール及びユーザタスクのシステムコールの定義 */
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* 中優先度タスク(優先度3で起動される) */
int sample_tsk2_main(int argc, char *argv[])
{
  SYSCALL_PARAMCB tsk3_param;
  ER ercd;
  
  /* 高優先度タスク生成のパラメータ設定 */
  tsk3_param.un.acre_tsk.func = SAMPLE_TSK3_ENTRY_ADDR;     /* タスク起動番地 */
  tsk3_param.un.acre_tsk.name = "sample_tsk3";        /* タスク名 */
  tsk3_param.un.acre_tsk.priority = 1;              /* タスク優先度 */
  tsk3_param.un.acre_tsk.stacksize = 0x100;         /* タスクスタックサイズ */
  tsk3_param.un.acre_tsk.argc = 0;                  /* タスクへ渡すパラメータ */
  tsk3_param.un.acre_tsk.argv = NULL;               /* タスクへ渡すパラメータ */

  puts("sample_tsk2 started.\n");

  /* 高優先度タスクを未登録状態から休止状態へ */
  sample_tsk3_id = mz_acre_tsk(&tsk3_param); /* タスク生成のシステムコール */
  /* タスクが生成できた場合 */
  if (sample_tsk3_id > E_NG) {
    puts("sample_tsk2 create task(sample_tsk3).\n");
  }

  puts("sample_tsk2 create running in (sample_tsk3).\n");
  /* 高優先度タスク(sample_task1)を休止状態から実行可能状態へ(高優先度タスクに処理が移る) */
  mz_sta_tsk(sample_tsk3_id); /* タスク起動のシステムコール */
  puts("sample_tsk2 create running out (sample_tsk3).\n");

  /* 低優先度タスク(sample_tsk1)を実行可能状態から休止状態へ */
  //ercd = mz_ter_tsk(sample_tsk1_id); /* タスク強制終了システムコール */
  /* 強制終了できた場合 */
  //if (ercd == E_OK) {
  //puts("sample_tsk2 termination task(sample_tsk1).\n");
  //}

  /* 低優先度タスク(sample_tsk1)を休止状態から未登録状態へ */
  ercd = mz_del_tsk(sample_tsk3_id); /* タスク排除システムコール */
  if (ercd == E_OK) {
    puts("sample_tsk2 delete task(sample_tsk3).\n");
  }

  /* 自タスク(中優先度タスク(sample_tsk2)を実行状態から未登録状態へ) */
  mz_exd_tsk(); /* タスク終了と排除システムコール */

  /* ここには戻ってこない */

  return 0;
}
