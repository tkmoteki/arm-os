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
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* sample_tsk7 */
int sample_tsk7_main(int argc, char *argv[])
{
  SYSCALL_PARAMCB tsk3_param;
  ER ercd;

  /* 高優先度タスク生成のパラメータ設定 */
  tsk3_param.un.acre_tsk.func = SAMPLE_TSK8_ENTRY_ADDR;   /* タスク起動番地 */
  tsk3_param.un.acre_tsk.name = "sample_tsk8";      /* タスク名 */
  tsk3_param.un.acre_tsk.priority = 1;              /* タスク優先度 */
  tsk3_param.un.acre_tsk.stacksize = 0x100;         /* タスクスタックサイズ */
  tsk3_param.un.acre_tsk.argc = 0;                  /* タスクへ渡すパラメータ */
  tsk3_param.un.acre_tsk.argv = NULL;               /* タスクへ渡すパラメータ */

  puts("sample_tsk7 started.\n");

  /* 高優先度タスク(sample_tsk8)を未登録状態から休止状態へ */
  sample_tsk8_id = mz_acre_tsk(&tsk3_param); /* タスク生成のシステムコール */
  /* 生成できた場合 */
  if (sample_tsk8_id > E_NG) {
    puts("sample_tsk7 create tsk(sample_tsk8).\n");
  }

  puts("sample_tsk7 running in (sample_tsk8).\n");
  /* 高優先度タスク(sample_tsk8)を休止状態から実行可能状態へ(高優先度タスクに処理が移る) */
  mz_sta_tsk(sample_tsk8_id); /* タスク起動のシステムコール */
  puts("sample_tsk7 running out (sample_tsk8).\n");

  ercd = mz_slp_tsk(); /* 自タスクの起床待ちシステムコール */
  /* 自タスクが起床された場合 */
  if (ercd == E_OK) {
    puts("sample_tsk7 wake up OK(by sample_tsk6).\n");
  }

  mz_rel_wai(sample_tsk8_id); /* タスク強制待ち解除システムコール */

  return 0;
}
