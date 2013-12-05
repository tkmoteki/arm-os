/*
 * 本タスクセットは，同研究室で開発されたタスク可視化ツールのデモに使用するタスクセットである
 * 使用システムコール群
 * -mz_acre_tsk() : タスク生成
 * -mz_sta_tsk()    : タスク起動
 * -mz_slp_tsk()  : 自タスク起床待ちシステムコール
 * -mz_wup_tsk()  : タスク起床システムコール
 * -mz_rel_wai()  : タスク待ち状態強制解除システムコール
 */

/* public interface */
/* include/kernel */
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */


/* sample_tsk8 */
int sample_tsk8_main(int argc, char *argv[])
{
  ER ercd;

  puts("sample_task8 started.\n");

  ercd = mz_slp_tsk(); /* 自タスクの起床待ちシステムコール */
  /* 自タスクが起床された場合 */
  if (ercd == E_RLWAI) {
    puts("sample_tsk8 rel_wai (by sample_tsk7).\n");
  }

  return 0;
}
