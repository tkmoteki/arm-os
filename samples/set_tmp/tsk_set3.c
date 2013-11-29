/*
 * 本タスクセットは，同研究室で開発されたタスク可視化ツールのデモに使用するタスクセットである
 * 使用システムコール群
 * -mz_acre_tsk() : タスク生成
 * -mz_sta_tsk()    : タスク起動
 * -mz_slp_tsk()  : 自タスク起床待ちシステムコール
 * -mz_wup_tsk()  : タスク起床システムコール
 * -mz_rel_wai()  : タスク待ち状態強制解除システムコール
 */

#ifdef TSK_LIBRARY

/* public interface */
/* include/kernel */
//#include "defines.h" /* このosの型の定義 */
/* include/lib/c */
//#include "lib.h"      /* 標準ライブラリの定義 */
#include "tsk_obj_id.h" /* リソース情報 */


/* sample_tsk6 */
int sample_tsk6_main(int argc, char *argv[])
{
  
  puts("sample_tsk6 started.\n");

  puts("sample_tsk6 exited.\n");

  return 0;
}

#endif
