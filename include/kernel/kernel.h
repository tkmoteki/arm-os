/*
 * ・kernel.hはメインファイルkernel.cのhファイルである. 
 * ・主に，タスクが使用するシステムコールのプロトタイプ，サービスコール，プロトタイプ，
 *   ユーザタスクの
 *   プロトタイプ，kernel.cのプロトタイプ，タスク状態管理マクロ，タスク待ち要因管理マクロ，
 *   タスク取得情報マクロ，タスクコントロールブロック，レディーキューとなる.
 * ・間違った使用をした時(カーネルオブジェクトを取得したままタスクを休止状態または
 *   未登録状態にしてしまった時)カーネルが自動で解放してくれるためにタスク取得情報を
 *   追加する.(最後に取得したオブジェクト1つのみ)
 *   これが，TCBのget_infoである. 複数のカーネルオブジェクトの自動解放機能の動作保証を
 *   するならば，取得情報を配列化する方法があるが，メモリを消費するため実装していない.
 * ・また，上記のカーネルオブジェクト自動解放機能は静的型mutex，動的型mutexともに
 *   プロトコルありを指定したとき，すべてのパターンについては正常動作の保証はしない.
 * ・また，タスクコントロールブロックでの実行中変わらない情報はDRAM節約の観点から
 *   RAMまたはROMに保存すべきだが，初学者向けOSという事ですべてDRAMに保存する事とする.
 *  これがTCBのkz_initである.
 * ・待ち情報として，TCBにwait_infoの領域を設定している.待ちポインタは双方向としている.
 *   さらに，wobjpとして待ち要因となっているTCBには対象カーネルオブジェクトのポインタ
 *   を記録している.これは
 *   kz_ter_tsk()やkz_rel_wai()システムコールの動作向上のためである.さらに待ち要因
 *   としてタイマ関連を含むことがあるので，tobjp欄を設けいている. 
 *   また，何の待ち要因かを記録する方法は各フラグとしていて，タスク状態管理のflags
 *   とまとめている.
 *   待ち要因の記録はwait_infoに別途設けてもよいが，タスク状態管理のflagsの残りの領域
 *   がもったいないので(shortでやれば，13ビット無駄)わざと分けていない.
 * ・待ち要因管理は一度に2つ以上の要因をもつ事はありえないので，取得情報のように配列化
 *   しなくても動作の保証ができる.
 */



#ifndef _KERNEL_H_INCLUDED_
#define _KERNEL_H_INCLUDED_


#include "include/kernel/defines.h"
#include "include/kernel/syscall.h"
//#include "interrupt.h"


/*! タスク状態管理マクロ */
/*! LSBから3ビット目までをタスクの状態として使用 */
#define STATE_CLEAR                   (0 << 0)    /*! タスク状態を初期化 */
#define TASK_WAIT                     (0 << 0)    /*! 0なら待ち状態 */
#define TASK_READY                    (1 << 0)    /*! 実行可能状態または実行状態 */
#define TASK_DORMANT                  (1 << 1)    /*! 休止状態 */

/*! タスク待ち要因管理マクロ */
/*
 * タスク待ち要因管理マクロは静的型と動的型で分けない
 * (カーネルオブジェクトは属性で静的か動的かを記録している)
 */
/*! LSBから4ビット目からタスクの待ち要因として使用 */
#define TASK_WAIT_TIME_SLEEP          (1 << 3)    /*! 起床待ち(kz_tslp_tsk()) */
#define TASK_WAIT_TIME_DELAY          (1 << 4)    /*! 時間経過待ち(kz_dly_tsk()) */
#define TASK_WAIT_SEMAPHORE           (1 << 5)    /*! セマフォ待ち */
#define TASK_WAIT_MUTEX               (1 << 6)    /*! mutex待ち */
#define TASK_WAIT_VIRTUAL_MUTEX       (1 << 7)    /*! virtual mutex待ち */
#define TASK_WAIT_MAILBOX             (1 << 8)    /*! mail box待ち */

#define TASK_STATE_INFO               (0x07 << 0) /*! タスク状態の抜き取り */
#define TASK_WAIT_ONLY_TIME           (3 << 3)    /*! タイマ要因のみ(tslp_tsk()とdly_tsk()) */


/*! タスクのカーネルオブジェクト取得情報管理マクロ */
/*
 * ・セマフォ及びmutexを取得したままタスクが死んだ時自動解放するため
 * ・カーネルオブジェクト取得情報管理マクロは静的型と動的型で分けない(カーネルオブジェクトは属性で静的か動的かを記録している)
 */
#define TASK_GET_SEMAPHORE            (1 << 1)    /*! セマフォ */
#define TASK_GET_MUTEX                (1 << 2)    /*! mutex */
/* メールボックス取得情報はない */

#define TASK_NOT_GET_OBJECT           (0 << 0)    /*! タスクは何も取得していない場合 */


#define TASK_NAME_SIZE                16          /*!  タスク名の最大値! */
#define INIT_TASK_ID                  0           /*!  initタスクIDは0とする */


/*!
 * スレッドのスタート・アップ(thread_init())に渡すパラメータ.実行時に内容が変わらないところ
 */ 
typedef struct _task_init_infomation {
  int tskid;                            /*! タスクID */
  TSK_FUNC func;                        /*! タスクのメイン関数 */
  int argc;                             /*! タスクのメイン関数に渡すargc */
  char **argv;                          /*! タスクのメイン関数に渡すargv */
  char name[TASK_NAME_SIZE]   ;         /*! タスク名 */
  int priority;                         /*! 起動時の優先度(優先度変更のシステムコールがあるため) */
} TSK_INITCB;


/*! タスクの待ち情報管理構造体 */
typedef struct _task_wait_infomation {
  struct _task_struct *wait_next;       /*! 待ちオブジェクトへの次ポインタ */
  struct _task_struct *wait_prev;       /*! 待ちオブジェクトへの前ポインタ */
  TMR_OBJP tobjp;                       /*! タイマ関連の待ち要因がある時に使用する領域．対象タイマコントロールブロックを記録 */
  WAIT_OBJP wobjp;                      /*! 待ち行列につながれている対象コントロールブロック */
} TSK_WAIT_INFOCB;


/*! タスクのカーネルオブジェクト取得情報(休止状態または未登録状態に遷移する時に使用する) */
typedef struct _task_get_infomation {
  int flags;                            /*! 取得フラグ */
  GET_OBJP gobjp;                       /*! 取得しているカーネルオブジェクトのポインタ */
} TSK_GET_INFOCB;


/*! タスクコンテキスト */
typedef struct _task_interrupt_infomation {
  INTR_TYPE type;                       /*! 割込みの種類 */
  UINT32 sp;                            /*! タスクスタックへのポインタ */
} TSK_INTR_INFOCB;


/*! システムコール用バッファ */
typedef struct _task_systemcall_infomation {
  SYSCALL_TYPE flag;                    /*! システムコールかサービスコールかを判別 */
  ISR_TYPE type;                        /*! システムコールのID */
  OBJP ret;                             /*! システムコール返却値を格納しておくポインタ */
  SYSCALL_PARAMCB *param;               /*! システムコールのパラメータパケットへのポインタ(タスク間通信の受信処理に使用) */
} TSK_SYSCALL_INFOCB;


/*! スケジューラによって依存する情報 */
/*
 * readyque_flagは優先度とタイムアウトごとのレディーキューを持つスケジューラに対してのみ設定，処理する
 * 他のレディーキュー構造に対しては無視(ACTIV_READYマクロとEXPIRED_READYマクロの値しか設定されない).
 */
typedef struct _scheduler_depend_infomation {
  union {
    /*! タイムスライス型スケジューラに依存する情報 */
    struct {
      int tm_slice;                     /*! タスクのタイムスライス(タイムスライスが絡まないスケジューリングの時は-1となる) */
    } slice_schdul;
    /*! 2種類のレディーキューを持つ簡易O(1)スケジューラに依存する情報 */
    struct {
      UINT8 readyque_flag;              /*! どのレディーキューにいるか */
    } odrone_schdul;
    /* 公平配分型スケジューラに依存する情報 */
    struct {
      int rel_exetim;
    } fr_schdul;
    /* リアルタイム型スケジューラに依存する情報 */
    struct {
      int rel_exetim;                   /*! 実行時間(RM専用のメンバで簡単化のため相対デッドライン時間とする) */
      int rate;                         /*! 周期 */
      int deadtim;                      /*! デッドライン時刻 */
      int floatim;                      /*! 余裕時刻 */
      TMR_OBJP tobjp;                   /*! スケジューラが使用するソフトタイマオブジェクト(EDF,LLF時使用) */
    } rt_schdul;
  } un;
} SCHDUL_DEP_INFOCB;


/*! レディーによって依存する内容(静的優先度はキュー構造のレディー,動的優先度は2分木と整列リストのレディー) */
typedef struct _ready_depend_infomation {
  union {
    /*! キュー構造のレディー */
    struct {
      struct _task_struct *ready_next; /*! レディーの次ポインタ */
      struct _task_struct *ready_prev; /*! レディーの前ポインタ */
    } que_ready;
    /*! 2分木と整列リスト(動的優先度管理のため)のレディー */
    struct {
      int dynamic_prio;                 /*! 動的優先度 */
      struct _task_struct *parent;      /*! 親 */
      struct _task_struct *left_next;   /*! 左ポインタ */
      struct _task_struct *right_next;  /*! 右ポインタ */
      struct _task_struct *sort_next;   /*! 整列リストの次ポインタ */
      struct _task_struct *sort_prev;   /*! 整列リストの前ポインタ */
    } btree_ready;
  } un;
} READY_DEP_INFOCB;


/*!
 * タスクコントロールブロック(TCB)
 * free listにREADY_DEP_INFOCBのポインタを使用するとごちゃごちゃになるので,別途free list専用メンバを追加
 */
typedef struct _task_struct {
  struct _task_struct *free_next;   /*! free listの次ポインタ */
  struct _task_struct *free_prev;   /*! free listの前ポインタ */
  READY_DEP_INFOCB ready_info;      /*! レディーごとに依存する内容 */
  int priority;                     /*! 静的優先度 */
  int stacksize;
  char *stack;                      /*! スタックリンカスクリプトに定義されているユーザスタック領域のポインタ */
  UINT16 state;                     /*! 状態フラグ */
  TSK_INITCB init;                  /*! 実行時に内容が変わらないところ */
  TSK_WAIT_INFOCB wait_info;        /*! 待ち情報管理 */
  TSK_GET_INFOCB get_info;          /*! 取得情報管理 */
  TSK_INTR_INFOCB intr_info;        /*! 割込み情報(ここはつねに変動する) */
  TSK_SYSCALL_INFOCB syscall_info;  /*! システムコール情報管理 */
  SCHDUL_DEP_INFOCB schdul_info;    /*! スケジューラごとに依存する情報 */
} TCB;


/*! タスク情報 */
struct _task_infomation {
  TCB **id_table;                   /*! task ID変換テーブルへのheadポインタ(可変長配列として使用) */
  TCB *freehead;                    /*! taskリンクドfreeリストのキュー(freeheadポインタを記録) */
  TCB *alochead;                    /*! taskリンクドalocリストのキュー(alocheadポインタを記録) */
  int counter;                      /*! 次の割付可能ID */
  int tskid_num;                    /*! 現在のタスク資源ID数 */
  int power_count;                  /*! task IDが足らなくなった回数(<<で倍増やしていく).この変数で可変長配列のサイズが求められる */
} mg_tsk_info;


/*! ディスパッチ情報 */
struct _dispatcher_infomation {
  char flag;                        /*! ディスパッチ状態フラグ */
  void (*func)(UINT32 *context);    /*! ディスパッチルーチン(1つだけしかないからtypedefしない) */
} dsp_info;


TCB *current; /*! カレントスレッド(実行状態のスレッドの事) */

/* システムコール(ユーザタスクが呼ぶシステムコールのプロトタイプ，実体はsyscall.cにある) */
/*! mz_acre_tsk():タスクコントロールブロックの生成(ID自動割付) */
ER_ID mz_acre_tsk(SYSCALL_PARAMCB *par);

/*! mz_del_tsk():スレッドの排除 */
ER mz_del_tsk(ER_ID tskid);

/*! mz_sta_tsk():スレッドの起動 */
ER mz_sta_tsk(ER_ID tskid);

/*! mz_run_tsk():スレッドの生成(ID自動割付)と起動 */
ER_ID mz_run_tsk(SYSCALL_PARAMCB *par);

/*! mz_ext_tsk():自タスクの終了 */
void mz_ext_tsk(void);

/*! mz_exd_tsk():自スレッドの終了と排除 */
void mz_exd_tsk(void);

/*! mz_ter_tsk():スレッドの強制終了 */
ER mz_ter_tsk(ER_ID tskid);

/*! mz_get_pri():スレッドの優先度取得 */
ER mz_get_pri(ER_ID tskid, int *p_tskpri);

/*! mz_chg_pri():スレッドの優先度変更 */
ER mz_chg_pri(ER_ID tskid, int tskpri);

/*! chg_slt():タスクタイムスライスの変更 */
ER mz_chg_slt(SCHDUL_TYPE type, ER_ID tskid, int slice);

/*! get_slt():タスクタイムスライスの取得 */
ER mz_get_slt(SCHDUL_TYPE type, ER_ID tskid, int *p_slice);

/*! mz_slp_tsk():自タスクの起床待ち */
ER mz_slp_tsk(void);

/*! mz_wup_tsk():タスクの起床 */
ER mz_wup_tsk(ER_ID tskid);

/*! mz_rel_wai():待ち状態強制解除 */
ER mz_rel_wai(ER_ID tskid);

/*! mz_get_mpf():動的メモリ獲得 */
void* mz_get_mpf(int size);

/*! mz_rel_mpf():動的メモリ解放 */
int mz_rel_mpf(void *p);

/*! mz_def_inh():割込みハンドラの定義 */
ER mz_def_inh(INTRPT_TYPE type, IR_HANDL handler);

/* 非タスクコンテキストから呼ぶシステムコールのプロトタイプ，実体はsyscall.cにある) */
/*! mz_iacre_tsk():タスクの生成 */
ER mz_iacre_tsk(SYSCALL_PARAMCB *par);
/*! mz_ista_tsk():タスクの起動 */
ER mz_ista_tsk(ER_ID tskid);

/*! mz_ichg_pri():タスクの優先度変更 */
ER mz_ichg_pri(ER_ID tskid, int tskpri);

/*! mz_iwup_tsk():タスクの起床 */
ER mz_iwup_tsk(ER_ID tskid);


/* サービスコール(ユーザタスクが呼ぶシステムコールのプロトタイプ，実体はsrvcall.cにある) */
/*! mv_acre_tsk():タスクコントロールブロックの生成(ID自動割付) */
ER_ID mv_acre_tsk(SYSCALL_PARAMCB *par);
          
/*! mv_del_tsk():スレッドの排除 */
ER mv_del_tsk(ER_ID tskid);

/*! mz_ter_tsk():スレッドの強制終了 */
ER mv_ter_tsk(ER_ID tskid);

/*! mv_get_pri():スレッドの優先度取得 */
ER mv_get_pri(ER_ID tskid, int *p_tskpri);

/*! mv_def_inh():割込みハンドラの定義 */
ER_ID mv_def_inh(INTRPT_TYPE type, IR_HANDL handler);

/*! mv_sel_schdul():スケジューラの切り替え */
ER mv_sel_schdul(SCHDUL_TYPE type, long param);


/*! initタスク */
int start_threads(int argc, char *argv[]);
ER_ID idle_id;

#ifdef TSK_LIBRARY
/*! ユーザタスク及び資源情報 */
int sample_tsk1_main(int argc, char *argv[]);
int sample_tsk2_main(int argc, char *argv[]);
int sample_tsk3_main(int argc, char *argv[]);
int sample_tsk4_main(int argc, char *argv[]);
int sample_tsk5_main(int argc, char *argv[]);
int sample_tsk6_main(int argc, char *argv[]);
int sample_tsk7_main(int argc, char *argv[]);
int sample_tsk8_main(int argc, char *argv[]);
extern ER_ID sample_tsk1_id;
extern ER_ID sample_tsk2_id;
extern ER_ID sample_tsk3_id;
extern ER_ID sample_tsk4_id;
extern ER_ID sample_tsk5_id;
extern ER_ID sample_tsk6_id;
extern ER_ID sample_tsk7_id;
extern ER_ID sample_tsk8_id;
#endif


/* kernel.cの関数を定義 */
/*! tskid変換テーブル設定処理(acre_tsk():タスクコントロールブロックの生成(ID自動割付)) */
void kernelrte_acre_tsk(SYSCALL_PARAMCB *p);

/*! tskid変換テーブル設定処理(del_tsk():スレッドの排除) */
void kernelrte_del_tsk(SYSCALL_PARAMCB *p);

/*! tskid変換テーブル設定処理(sta_tsk():スレッドの起動) */
void kernelrte_sta_tsk(SYSCALL_PARAMCB *p);

/*! tskid変換テーブル設定処理(ter_tsk():スレッドの強制終了) */
void kernelrte_ter_tsk(SYSCALL_PARAMCB *p);

/*! tskid変換テーブル設定処理(get_pri():スレッドの優先度取得) */
void kernelrte_get_pri(SYSCALL_PARAMCB *p);

/*! 変換テーブル設定処理はいらない(def_inh():割込みハンドラの定義) */
void kernelrte_def_inh(SYSCALL_PARAMCB *p);

/*! 変換テーブル設定処理はいらない(sel_schdul():スケジューラの切り替え) */
void kernelrte_sel_schdul(SYSCALL_PARAMCB *p);

/*! 非タスクコンテキスト用システムコール呼び出しライブラリ関数 */
void isyscall_intr(ISR_ITYPE type, SYSCALL_PARAMCB *param);

/*! スケジューラとディスパッチャの呼び出し */
void context_switching(INTR_TYPE type);

/*! 外部割込みハンドラを呼び出す準備 */
ER external_intr(INTR_TYPE type, UINT32 sp);

/*! システムコール割込みハンドラ(ISR)を呼び出す準備 */
void syscall_intr(ISR_TYPE type, UINT32 sp);

/*! トラップ発行(システムコール) */
void issue_trap_syscall(ISR_TYPE type, SYSCALL_PARAMCB *param, OBJP ret);

/*! kernelオブジェクトの初期化を行う */
void kernel_obj_init(void);

/*! initタスクの生成と起動を行う */
void start_init_tsk(TSK_FUNC func, char *name, int priority, int stacksize, int argc, char *argv[]);

/*! kernelの初期化を行う */
void kernel_init(TSK_FUNC func, char *name, int priority, int stacksize, int argc, char *argv[]);

/*! カーネルオブジェクトを取得しているか検査し，取得しているならば各解放関数を呼ぶ */
//ER release_object(TCB *checktcb);

/*! どの待ち行列からTCBを抜き取るか分岐する */
//ER get_tsk_waitque(TCB *worktcb, UINT16 flags);

/*! OSの致命的エラー時 */
void down_system(void);


#endif
