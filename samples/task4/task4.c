/*
 * sample_tsk4~sample_tsk6は，主にタスク管理システムコール群を使用する
 * サンプルシナリオ
 * 使用システムコール
 * -mz_run_tsk() : タスク生成と起動システムコール
 * -mz_get_pri() : タスク優先度取得システムコール
 * -mz_chg_pri() : タスク優先度変更システムコール
 */


/* public interface */
/* include/kernel */
#include "defines.h" /* このosの型の定義 */
#include "syscall.h"    /* システムコール及びユーザタスクのシステムコールの定義 */
/* include/lib/c */
#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* 低優先度タスク(優先度3で起動される) */
int sample_tsk4_main(int argc, char *argv[])
{
  SYSCALL_PARAMCB tsk5_param;

  /* 中優先度タスク生成のパラメータ設定 */
  tsk5_param.un.acre_tsk.func = SAMPLE_TSK5_ENTRY_ADDR;     /* タスク起動番地 */
  tsk5_param.un.acre_tsk.name = "sample_tsk5";        /* タスク名 */
  tsk5_param.un.acre_tsk.priority = 1;              /* タスク優先度 */
  tsk5_param.un.acre_tsk.stacksize = 0x100;         /* タスクスタックサイズ */
  tsk5_param.un.acre_tsk.argc = 0;                  /* タスクへ渡すパラメータ */
  tsk5_param.un.acre_tsk.argv = NULL;               /* タスクへ渡すパラメータ */


  puts("sample_tsk4 started.\n");

  /*
   * タスクの生成と起動はmz_acre_tsk()とmz_sta_tsk()で行うとタスクがスイッチングするため，
   * 実行結果がわかりずらくなるので注意する
   */
  puts("sample_tsk4 create running in (sample_tsk5).\n");
  /* sample_tsk5のタスクを生成と起動をする */
  sample_tsk5_id = mz_run_tsk(&tsk5_param);
  puts("sample_tsk4 create runnning out (sample_tsk5).\n");

  return 0;
}
