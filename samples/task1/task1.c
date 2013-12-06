/*
 * sample_tsk1~sample_tsk3は，主にタスク管理システムコール群を使用する
 * サンプルシナリオ
 * 使用システムコール
 * -mz_acre_tsk() : タスク生成システムコール
 * -mz_sta_tsk() : タスク起動システムコール
 * -mz_del_tsk() : タスク排除システムコール
 * -mz_ext_tsk() : 自タスクの終了システムコール
 * -mz_exd_tsk() : 自タスクの終了と排除システムコール
 * -mz_ter_tsk() : タスク強制終了システムコール
 */


/* public interface */
/* include/kernel */
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"    /* システムコール及びユーザタスクのシステムコールの定義 */
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* 低優先度タスク(優先度5で起動される) */
int sample_tsk1_main(int argc, char *argv[])
{
  SYSCALL_PARAMCB tsk2_param;
  ER ercd;

  /* 中優先度タスク生成のパラメータ設定 */
  tsk2_param.un.acre_tsk.func = SAMPLE_TSK2_ENTRY_ADDR;   /* タスク起動番地 */
  tsk2_param.un.acre_tsk.name = "sample_tsk2";      /* タスク名 */
  tsk2_param.un.acre_tsk.priority = 3;              /* タスク優先度 */
  tsk2_param.un.acre_tsk.stacksize = 0x100;         /* タスクスタックサイズ */
  tsk2_param.un.acre_tsk.argc = 0;                  /* タスクへ渡すパラメータ */
  tsk2_param.un.acre_tsk.argv = NULL;               /* タスクへ渡すパラメータ */


  puts("sample_tsk1 started.\n");

  /* 中優先度タスクを未登録状態から休止状態へ */
  sample_tsk2_id = mz_acre_tsk(&tsk2_param); /* タスク生成のシステムコール */
  /* タスクが生成できた場合 */
  if (sample_tsk2_id > E_NG) {
    puts("sample_tsk1 create tsk(sample_tsk2).\n");
  }

  puts("sample_tsk1 create running in (sample_tsk2).\n");
  /* 中優先度タスクを休止状態から実行可能状態へ(中優先度タスクに処理が移る) */
  ercd = mz_sta_tsk(sample_tsk2_id); /* タスク起動のシステムコール */
  puts("sample_tsk1 create running out (sample_tsk2).\n");

  /* ここには戻ってこない */

  return 0;
}
