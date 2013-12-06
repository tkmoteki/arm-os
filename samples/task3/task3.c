/* public interface */
/* include/kernel */
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"    /* システムコール及びユーザタスクのシステムコールの定義 */
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* 高優先度タスク(優先度5で起動される) */
int sample_tsk3_main(int argc, char *argv[])
{
  puts("sample_tsk3 started.\n");

  /* 自タスク(中優先度タスク(sample_tsk2)を実行状態から未登録状態へ) */
  mz_ext_tsk(); /* タスク終了と排除システムコール */

  /* 中優先度タスクに実行が移る */

  return 0;
}
