/* public interface */
/* include/kernel */
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */

/* 高優先度タスク(優先度1で起動される) */
int sample_tsk5_main(int argc, char *argv[])
{
  int tskpri;
  ER ercd;

  puts("sample_tsk5 started.\n");

  ercd = mz_get_pri(sample_tsk5_id, &tskpri); /* タスク優先度取得システムコール */
  /* 優先度取得できた場合 */
  if (ercd == E_OK) {
    putxval(tskpri, 0);
    puts(" sample_tsk5 task priority(sample_tsk5)\n");
  }

  ercd = mz_chg_pri(sample_tsk5_id, 4); /* タスク優先度変更システムコール */
  /* タスクの優先度変更できた場合 */
  if (ercd == E_OK) {
    puts("sample_tsk5 change priority(sample_tsk5).\n");
  }

  return 0;
}
