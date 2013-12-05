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
//#include "defines.h" /* このosの型の定義 */
#include "syscall.h"
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* sample_tsk6 */
int sample_tsk6_main(int argc, char *argv[])
{
  SYSCALL_PARAMCB tsk2_param;
  int tskpri;

  /* 中優先度タスク生成のパラメータ設定 */
  tsk2_param.un.acre_tsk.func = SAMPLE_TSK7_ENTRY_ADDR;   /* タスク起動番地 */
  tsk2_param.un.acre_tsk.name = "sample_tsk7";      /* タスク名 */
  tsk2_param.un.acre_tsk.priority = 3;              /* タスク優先度 */
  tsk2_param.un.acre_tsk.stacksize = 0x100;         /* タスクスタックサイズ */
  tsk2_param.un.acre_tsk.argc = 0;                  /* タスクへ渡すパラメータ */
  tsk2_param.un.acre_tsk.argv = NULL;               /* タスクへ渡すパラメータ */

  puts("sample_tsk6 started.\n");

  /* 中優先度タスク(sample_tsk7)をタスクを未登録状態から休止状態へ */
  sample_tsk7_id = mz_acre_tsk(&tsk2_param); /* タスク生成のシステムコール */
  /* 生成できた場合 */
  if (sample_tsk7_id > E_NG) {
    puts("sample_tsk6 create tsk(sample_tsk7).\n");
  }

  mz_get_pri(sample_tsk6_id, &tskpri);
  putxval(tskpri, 0);
  puts("sample_tsk6 get priority.\n");

  puts("sample_tsk6 exited.\n");

  return 0;
}
