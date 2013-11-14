/*
 * ・defines.hは型の定義や静的パラメータ定義を行うhファイルである
 * ・μITRON4.0準拠スタンダードプロファイルのようなすべての引数の型定義までは行って
 *   いない.返却値の型(ERとER_BOOL)とエラーコードのマクロは実装している.
 *  -ERは機能としてのエラーコード，ER_BOOLは真偽のエラーコードとなる
 *  -ER_UINTだけで実装するとわかりずらくなるので，具体的なオブジェクトの型は実装
 *   している(kz_thread_id_tなど).
 *  -E_BOVRはのうちユーザスタックオーバーフローはE_NOMEMとしている.
 * ・返却値であるエラーコードのマクロE_QOVRについて
 *   このOSには強制待ち状態と二重待ち状態はない，起動キューイングの機能はない
 *   しかし，カーネルオブジェクトに対して上限数を越えた解放要求の可能性があるため，
 *   そのような
 *   時に使用するものとする．(例セマフォでセマフォ最大値以上の解放要求がきた時)
 * ・実装独自のエラーコードEV_NDLについて
 *   これはカーネルオブジェクトを取得しているタスクが存在するのに排除要求が出された
 *   場合に返す返却値である
 * ・実装独自のエラーコードEV_NPLについて
 *   これはmutexを使用する場合にプロトコルを選択していなかった場合のエラーコード    *
 */



#ifndef _DEFINES_H_INCLUDED_
#define _DEFINES_H_INCLUDED_


#include "arch/cpu/intr.h"
#include "c_lib/lib.h"


/*! 型の定義 */
/*! 型と返却値は略称名称を用いる */
/*
 * ERはcharで，R_BOOLはcharで十分だが，ディスパッチ禁止状態でシステムコールの返却値のE_CTX
 * をポインタで書き換える時に，その時のポインタ型が一貫性を持っていた方がいいので(kernel.cのsyscall_proc()のところ)，
 * ER ER_ID R_BOOLともにsigned charとしている
 */
typedef unsigned char           UINT8;                  /*! プロセッサに自然な符号なし8ビット整数 */
typedef unsigned short          UINT16;                 /*! プロセッサに自然な符号なし16ビット整数 */
typedef unsigned long           UINT32;                 /*! プロセッサに自然な符号なし32ビット整数 */
typedef signed short            ER;                     /*! 機能としてのエラーコード */
typedef signed char             ER_BOOL;                /*! 真偽または機能としてのエラーコードを返却 */
typedef signed short            ER_ID;                  /*! 機能としてのエラーコードまたはID(0,1,2,・・・)を返却 */
typedef signed long             ER_VLE;                 /*! 機能としてのエラーコードまたは何か値(正数値)を返却 */
typedef unsigned long           OBJP;                   /*! カーネルオブジェクトのポインタ抽象型(OS内部で使用) */
typedef signed short            BOOL;                   /*! 真偽 */
typedef signed long             R_VLE;                  /*! 何か整数値のみ返却 */
typedef unsigned long           GET_OBJP;               /*! TCBが取得しているオブジェクトポインタを記録 */
typedef unsigned long           WAIT_OBJP;              /*! TCBの待ち要因となるオブジェクトポインタを記録 */
typedef unsigned long           TMR_OBJP;               /*! タイマオブジェクトのポインタを記録 */
typedef unsigned long           TMRRQ_OBJP;             /*! タイマを要求したオブジェクトのポインタを記録 */
typedef ER (*SOFTVEC_HANDL)(INTRPT_TYPE type, UINT32 sp);     /*! ソフトウェアベクタのハンドラ型 */
typedef int (*TSK_FUNC)(int argc, char *argv[]);        /*! TCBが呼ぶスレッドメインルーチンを記録 */
typedef void (*IR_HANDL)(void);                         /*! 割込みハンドラ */
typedef void (*TMR_CALLRTE)(void *argv);                /*! タイマコールバックルーチン */


#define NULL    ((void *)0)
#define TRUE    1
#define FALSE     0
//#define DISABLE                   0                     /*! 無効 */
#define TMO_FEVR                  -1                      /*! not tiemr */
#define TMO_POL                   0                       /*! タイマポーリング */
#define TMR_EFFECT                1                       /*! タイマ有効値 */
#define STATIC                    0                       /*! 内部でのデータ構造(静的) */
#define DYNAMIC                   1                       /*! 内部でのデータ構造(動的) */


/*! エラーコードの定義 */
#define E_OK                      0                       /*! 正常終了 */
#define E_NG                      -1                      /*! 関数異常終了(失敗) */
#define E_NOSPT                   -9                      /*! 未サポート機能 */
#define E_PAR                     -17                     /*! システムコールパラメータエラー */
#define E_ID                      -18                     /*! ID不正番号 */
#define E_CTX                     -25                     /*! コンテキストエラー(ディスパッチ禁止状態でシステムコール発行) */
#define E_ILUSE                   -28                     /*! 不正使用 */
#define E_NOMEM                   -33                     /*! メモリ不足 */
#define E_NOID                    -34                     /*! 割付可能なIDがない(主にID自動割付システムコールに使用) */
#define E_OBJ                     -41                     /*! オブジェクト状態エラー */
#define E_NOEXS                   -42                     /*! オブジェクト未生成 */
#define E_QOVR                    -43                     /*! キューイングオーバフロー(冒頭のコメントを参照) */
#define E_RLWAI                   -49                     /*! 待ち状態強制解除 */
#define E_TMOUT                   -50                     /*! ポーリング失敗またはタイムアウト */


/*! 実装独自のエラーコード */
#define EV_NORTE                  -97                     /*! 対象ルーチンまたはハンドラが未登録 */
#define EV_NDL                    -99                     /*! カーネルオブジェクトを排除できない */


/*! ここから静的パラメーター定義 */
#define SERIAL_DEFAULT_DEVICE     1                       /*! シリアルドライバ */
#define HARD_TIMER_DEFAULT_DEVICE 0                       /*! ハードタイマドライバ */
#define SOFT_TIMER_DEFAULT_DEVICE 1                       /*! ソフトタイマドライバ */
#define TASK_ID_NUM               2                       /*! タスク数 */
/* (現在では32までしか設定できない．また32以下も設定できない(ビットサーチが問題)) */
#define PRIORITY_NUM              32                      /*! 優先度数 */
#define SEMAPHORE_ID_NUM          2                       /*! セマフォ資源数 */
#define MAILBOX_ID_NUM            4                       /*! メールボックス資源数 */
#define MASSAGE_PRIORITY_NUM      16                      /*! メッセージ優先度数 */
#define MUTEX_ID_NUM              2                       /*! ミューテックス資源数 */
#define ALARM_ID_NUM              2                       /*! アラームハンドラ資源数 */
#define CYCLE_ID_NUM              2                       /*! 周期ハンドラ資源数 */


/*
 * マクロではなくenumでやることによってE_RSTARのチェックはいらなくなる
 */


/*! 割込みの種類 */
typedef enum {
  EXCEPTION                 = 0,  /*! ソフトウェアエラー */
  SYSCALL_INTERRUPT,              /*! システムコール */
  SERIAL_INTERRUPT,               /*! シリアル割込み */
  TIMER_INTERRUPT,                /*! タイマ割込み */
  NMI_INTERRUPT,                  /*! NMI割込み */
} INTR_TYPE;


/*! システムコールの種類 */
typedef enum {
  MZ_VOID                   = 0,  /*! sta_tsk()で新規起動するタスクに設定する */
  MZ_SYSCALL,                     /*! タスクコンテキスト用システムコール(タスクスイッチングあり) */
  MZ_ISYSCALL,                    /*! 非タスクコンテキスト用システムコール */
} SYSCALL_TYPE;


/*! スケジューリングの種類 */
typedef enum {
  FCFS_SCHEDULING           = 0,  /*! FCFSスケジューリング ~既存の方式~ */
  RR_SCHEDULING,                  /*! ラウンドロビンスケジューリング ~既存の方式~ */
  ITRON_RR_SCHEDULING,            /*! ITRON型ラウンドロビンスケジューリング(toppersカーネル参考) ~既存の方式~ */
  PRI_SCHEDULING,                 /*! 優先度スケジューリング ~既存の方式~ */
  RR_PRI_SCHEDULING,              /*! ラウンドロビン×優先度スケジューリング ~既存の方式~ */
  MFQ_SCHEDULING,                 /*! Multilevel Feedback Queue(BSDスケジューラ参考) ~既存の方式~ */
  ODRONE_SCHEDULING,              /*! 簡易O(1)スケジューリング(Linuxカーネル2.6.10参考) ~既存の方式~ */
  FR_SCHEDULING,                  /*! 公平配分スケジューリング(Linuxカーネル2.6.23参考(赤黒木ではないver.)) ~既存の方式~ */
  PFR_SCHEDULING,                 /*! 優先度公平配分スケジューリング ~新規の方式~*/
  RM_SCHEDULING,                  /*! Rate Monotonicスケジューリング ~既存の方式~ */
  DM_SCHEDULING,                  /*! Deadline Monotonicスケジューリング ~既存の方式~ */
  EDF_SCHEDULING,                 /*! Earliest Deadline Firstスケジューリング ~既存の方式~ */
  LLF_SCHEDULING,                 /*! Least Laxity Firstスケジューリング ~既存の方式~ */
} SCHDUL_TYPE;


/*! システムロールバック属性(スケジューラの切り替えは等に使用する) */
typedef enum {
  TA_VOIDCHG                = 0,  /*! initのみで使用する属性(何もしない) */
  TA_EXECHG,                      /*! OSをリロードせずに，ユーザタスク実行中にinit生成ルーチンへロールバック */
  TA_EXITCHG,                     /*! OSをリロードせずに，ユーザタスクがすべて終了してからinit生成ルーチンへロールバック */
} ROL_ATR;


/*! レディーの種類 */
typedef enum {  
  SINGLE_READY_QUEUE        = 0,  /*! 単一のキュー型レディー */
  PRIORITY_READY_QUEUE,           /*! 優先度レベルのキュー型レディー */
  TIMEOUT_PRIORITY_READYQUE,      /*! 優先度とタイムアウトレベルのキュー型レディー */
  BINARY_TREE,                    /*! 単一のツリー型レディー */
  PRIORITY_BINARY_TREE,           /*! 優先度レベルのツリー型レディー */
} READY_TYPE;


/*! OS内部で管理するsemaphoreデータ構造定義 */
typedef enum {
  STATIC_SEMAPHORE          = 0,  /*! 静的型 */
  DYNAMIC_SEMAPHORE,              /*! 動的型 */
} SEM_TYPE;


/*! セマフォ待ちタスクをレディーへ戻す属性の定義 */
typedef enum {
  SEM_TA_TFIFO              = 0,  /*! FIFO順 */
  SEM_TA_TPRI,                    /*! 優先度順 */
} SEM_ATR;


/*! OS内部で管理するmailboxデータ構造定義 */
typedef enum {
  STATIC_MAILBOX            = 0,  /*! 静的型 */
  DYNAMIC_MAILBOX,                /*! 動的型 */
} MBX_TYPE;


/*! mailboxの各メッセージ属性の定義 */
typedef enum {
  MSG_TA_MFIFO              = 0,  /*! メッセージをFIFO順で管理 */
  MSG_TA_MPRI,                    /*! メッセージを優先度順で管理 */
} MBX_MATR;


/*! mailbox待ちタスクをレディーへ戻す属性の定義 */
typedef enum {
  MBX_TA_TFIFO              = 0,  /*! 待ちタスクをFIFO順で戻す */
  MBX_TA_TPRI,                    /*! 待ちタスクを優先度順で戻す */
} MBX_WATR;


/*! メッセージヘッダの定義 */
typedef struct _type_massage {
  struct _type_massage *next;     /*! メッセージ次ポインタ */
  struct _type_massage *prev;     /*! メッセージ前ポインタ */
  int msgpri;                     /*! メッセージ優先度 */
} T_MSG;


/*! OS内部で管理するmutexデータ構造定義 */
typedef enum{
  STATIC_MUTEX              = 0,  /*! 静的型 */
  DYNAMIC_MUTEX,                  /*! 動的型 */
} MTX_TYPE;


/*! mutex待ちタスクをレディーへ戻す属性の定義 */
/* 優先度逆転機構を搭載したときもFIFOと優先度順を選べるようにする */
typedef enum {
  MTX_TA_TFIFO              = 0,  /*! FIFO順 */
  MTX_TA_TPRI,                    /*! 優先度順 */
} MTX_ATR;


/*! 優先度逆転機構の定義 */
typedef enum {
  TA_VOIDPCL                = 0,  /*! プロトコルなし ~既存の方式~ */
  TA_DYHIGHLOC,                   /*! 遅延型最高値固定プロトコル ~既存の方式~ */
  TA_INHERIT,                     /*! 優先度継承プロトコル ~既存の方式~ */
  TA_CEILING,                     /*! 優先度上限プロトコル ~既存の方式~ */
  TA_IMHIGHLOC,                   /*! 即時型最高値固定プロトコル ~既存の方式~ */
  TA_STACK,                       /*! スタックリソースポリシー ~既存の方式~ */
  TA_VINHERIT,                    /*! virtual priority inheritance(仮想優先度継承プロトコル) ~新規の方式~ */
} PIVER_TYPE;


/*! OS内部で管理するcycle handlerデータ構造定義 */
typedef enum {
  STATIC_CYCLE_HANDLER      = 0,  /*! 静的型 */
  DYNAMIC_CYCLE_HANDLER,          /*! 動的型 */
} CYC_TYPE;


/*! OS内部で管理するalarm handlerデータ構造定義 */
typedef enum {
  STATIC_ALARM_HANDLER      = 0,  /*! 静的型 */
  DYNAMIC_ALARM_HANDLER,          /*! 動的型 */
} ALM_TYPE;


/*! 8bit幅のレジスタ操作マクロ */
#define REG8_READ(adr)    *((volatile unsigned char *)(adr))                  /*! 各レジスタから読み出し */
#define REG8_WRITE(adr, dat)  {*((volatile unsigned char *)(adr)) = (dat);}   /*! 各レジスタから書き出し */

/*! 16bit幅のレジスタ操作マクロ */
#define REG16_READ(adr)   *((volatile unsigned char *)(adr))                  /*! 各レジスタから読み出し */
#define REG16_WRITE(adr, dat) {*((volatile unsigned char *)(adr)) = (dat);}   /*! 各レジスタから書き出し */

/* 32bit幅のレジスタ操作マクロ */
#define REG32_READ(addr)  *((volatile unsigned int *)(addr))                  /*! 各レジスタから読み出し */
#define REG32_WRITE(addr, dat)  {*((volatile unsigned int *)(addr)) = (dat);} /*! 各レジスタから書き出し */


/**********************************
 * 以下のマクロはMakefileで指定する *
 **********************************/

/* サンプルタスクセットライブラリ選択マクロ */
/* #define TSK_LIBRARY */


/* ログ出力マクロ */
/* #define LOG */

#ifdef LOG
/* コンテキストスイッチングのログ出力マクロ */
#define LOG_CONTEXT(log_tcb) (get_log((OBJP)log_tcb))
#else
#define LOG_CONTEXT(log_tcb)
#endif


/*! デバックマクロ */
/* #define DEBUG */

#ifdef DEBUG
/* debug message */
#define DEBUG_OUTVLE(testvalue, testcolumn) (putxval(testvalue, testcolumn))
/* debug value */
#define DEBUG_OUTMSG(testmsg) (puts(testmsg))
#else
#define DEBUG_OUTVLE(testvalue, testcolumn)
#define DEBUG_OUTMSG(testmsg)
#endif


/*! kernel message */
/* #define KERNEL_MSG */

#ifdef KERNEL_MSG
/* kernel message */
#define KERNEL_OUTVLE(kernelvalue, kernelcolumn) (putxval(kernelvalue, kernelcolumn))
/* kernel value */
#define KERNEL_OUTMSG(kernelmsg) (puts(kernelmsg))
#else
#define KERNEL_OUTVLE(kernelvalue, kernelcolumn)
#define KERNEL_OUTMSG(kernelmsg)
#endif


#endif
