/* private interface */
#include "memory.h"
#include "ready.h"
#include "scheduler.h"

/* public interface */
/* include/kernel */
#include "kernel.h"
/* include/lib/c */
#include "lib.h"


/*! キュー構造のレディーブロックの初期化 */
static void rquecb_init(RQUECB *rcb, int len);

/*! 2分木のレディーブロックの初期化 */
static void btreercb_init(BTREERCB *bcb, int len, BTREE_ATR atr);

/*! 下位4ビットのパターンを記録するlsb_4bits_tableの初期化 */
static void bitmap_init(void);

/*! カレントタスク(実行状態TCB)を単一のレディーキュー先頭または，レディーキュー情報ブロックinit_queから抜き出す */
static ER get_current_singleque(void);

/*! カレントタスク(実行状態TCB)を単一のレディーキューの末尾または，レディーキュー情報ブロックinit_queへ繋げる */
static ER put_current_singleque(void);

/*! 指定されたTCBを単一のレディーキューまたは，レディーキュー情報ブロックinit_queから抜き取る */
static void get_tsk_singleque(TCB *worktcb);

/*! カレントタスク(実行状態TCB)を優先度レベルのレディーキュー先頭または，レディーキュー情報ブロックinit_queから抜き出す */
static ER get_current_prique(void);

/*! カレントタスク(実行状態TCB)を優先度レベルのレディーキューの末尾または，レディーキュー情報ブロックinit_queへ繋げる */
static ER put_current_prique(void);

/*! 指定されたTCBを優先度レベルのレディーキューまたは，レディーキュー情報ブロックinit_queから抜き取る */
static void get_tsk_prique(TCB *worktcb);

/*! activキューかexpiredキューかのビットマップのセット */
static void set_tmout_prique_bitmap(UINT8 flag);

/*! activキューかexpiredキューかのビットマップのクリア */
static void clear_tmout_prique_bitmap(TCB *worktcb, UINT8 flag);

/*! カレントタスク(実行状態TCB)をタイムアウトと優先度レベルのレディーキュー先頭または，レディーキュー情報ブロックinit_queから抜き出す */
static ER get_current_tmprique(void);

/*! カレントタスク(実行状態TCB)をタイムアウトと優先度レベルのactivレディーキューの末尾または，レディーキュー情報ブロックinit_queへ繋げる */
static ER put_current_tmpri_activque(void);

/*! 指定されたTCBをタイムアウトと優先度レベルのレディーキューまたは，レディーキュー情報ブロックinit_queから抜き取る */
static void get_tsk_tmprique(TCB *worktcb, UINT8 flag);

/*! 動的優先度枯渇状態をチェックする関数 */
static BOOL check_priority_exhaustion(int min_pri, int tsk_num);

/*! カレントタスク(実行状態TCB)を2分探索木または，レディーキュー情報ブロックinit_queへ繋げる */
static ER put_current_btree(void);

/*! 2分探索木に挿入したTCBを整列リストへ挿入する関数 */
static void put_sortlist(void);

/*! 動的優先度を調節する関数(2分木) */
static void adjust_priority_btree(void);

/*! 指定されたタスク(カレントタスクも含まれる)を2分探索木または，レディーキュー情報ブロックinit_queから抜き出す */
static ER get_tskcurrent_btree(TCB *tcb);

/*! 2分探索木に挿入したTCBを整列リストから抜き取る関数 */
static void get_sortlist(TCB *tcb);

/*! カレントタスク(実行状態TCB)を優先度ごとの2分探索木または，レディーキュー情報ブロックinit_queへ繋げる */
static ER put_current_pribtree(void);

/*! 2分探索木に挿入したTCBを優先度ごとの整列リストへ挿入する関数 */
static void put_pri_sortlist(void);

/*! 動的優先度を調節する関数(優先度ごとの2分木) */
static void adjust_priority_pribtree(void);

/*! 指定されたタスク(カレントタスクも含まれる)を2分探索木または，レディーキュー情報ブロックinit_queから抜き出す */
static ER get_tskcurrent_pribtree(TCB *tcb);

/*! 2分探索木に挿入したTCBを整列リストから抜き取る関数 */
static void get_pri_sortlist(TCB *tcb);



/*!
 * キュー構造のレディーブロックの初期化
 * *rcb : 初期化するキュー構造のレディーブロックポインタ
 * len : 長さ(配列の場合使用)
 */
static void rquecb_init(RQUECB *rcb, int len)
{
  int i;
  
  for (i = 0; i <= len; i++) {
    (rcb + i)->head = (rcb + i)->tail = NULL;
  }
}


/*!
 * 2分木のレディーブロックの初期化
 * *bcb : 初期化する2分木のレディーブロックポインタ
 * len : 長さ(配列の場合使用)
 * atr : 2分木の属性(実行時間,デッドライン,余裕時刻)
 */
static void btreercb_init(BTREERCB *bcb, int len, BTREE_ATR atr)
{
  int i;
  
  for (i = 0; i <= len; i++) {
    (bcb + i)->atr = atr;
    (bcb + i)->slist_head = (bcb + i)->tree_head = NULL;
    (bcb + i)->min_priority = -1;
    (bcb + i)->tsk_num = 0;
  }
}


/*!
 * レディーキューの初期化
 * typeはenumでやっているので，パラメータチェックはいらない
 * type : スケジューラのタイプ
 * (返却値)E_NOMEM : メモリ不足
 * (返却値)E_OK : 正常終了
 */
ER ready_init(void)
{
  RDYCB *rqcb;
  SCHDUL_TYPE type = mg_schdul_info.type;
  
  rqcb = (RDYCB *)get_mpf_isr(sizeof(*rqcb)); /* 動的メモリ取得 */
  if (rqcb == NULL) {
    return E_NOMEM;
  }
  memset(rqcb, 0, sizeof(*rqcb));
  
  mg_ready_info.entry = rqcb; /* レディー情報ブロックの設定 */
  
  /* First Come First Sarvedとラウンドロビンスケジューリングの時は単一のレディーキュー */
  if (type == FCFS_SCHEDULING || type == RR_SCHEDULING) {
    mg_ready_info.type = SINGLE_READY_QUEUE;
    /* キュー構造のレディーブロックのポインタ */
    rquecb_init(&mg_ready_info.entry->un.single.ready, 0);
    /* ビットマップの処理はない */
  }
  /* 簡易O(1)スケジューリング時はタイムアウトと優先度レベルごとのレディーキュー */
  else if (type == ODRONE_SCHEDULING) {
    mg_ready_info.type = TIMEOUT_PRIORITY_READYQUE;
    mg_ready_info.entry->un.tmout_pri.activ = &mg_ready_info.entry->un.tmout_pri.activ_ready;
    mg_ready_info.entry->un.tmout_pri.expired = &mg_ready_info.entry->un.tmout_pri.expired_ready;
    /* キュー構造のレディーブロックのポインタ */
    rquecb_init(mg_ready_info.entry->un.tmout_pri.activ_ready.que, PRIORITY_NUM);
    rquecb_init(mg_ready_info.entry->un.tmout_pri.expired_ready.que, PRIORITY_NUM);
    bitmap_init(); /*! ビットマップの初期化 */
  }
  /* Fair Schedulerは実行時間の2分探索木のレディー構造 */
  else if (type == FR_SCHEDULING) {
    mg_ready_info.type = BINARY_TREE;
    /* 2分木のレディーブロックの初期化(属性は実行時間) */
    btreercb_init(&mg_ready_info.entry->un.btree.ready, 0, EXECTION_TIME);
  }
  /* Priority Fair Schedulerは実行時間の優先度レベルの2分探索木のレディー構造 */
  else if (type == PFR_SCHEDULING) {
    mg_ready_info.type = PRIORITY_BINARY_TREE;
    btreercb_init(mg_ready_info.entry->un.pri_btree.ready, PRIORITY_NUM, EXECTION_TIME);
    bitmap_init(); /*! ビットマップの初期化 */
  }
  /* EDFスケジューリングはデッドライン2分探索木のレディー構造 */
  else if (type == EDF_SCHEDULING) {
    mg_ready_info.type = BINARY_TREE;
    /* 2分木のレディーブロックの初期化(属性はデッドライン時刻) */
    btreercb_init(&mg_ready_info.entry->un.btree.ready, 0, DEADLINE_TIME);
  }
  /* LLFスケジューリングフロートタイム2分探索木のレディー構造 */
  else if (type == LLF_SCHEDULING) {
    mg_ready_info.type = BINARY_TREE;
    /* 2分木のレディーブロックの初期化(属性は余裕時刻) */
    btreercb_init(&mg_ready_info.entry->un.btree.ready, 0, FLOAT_TIME);
  }
  /*
   * 優先度スケジューリングとラウンドロビン×優先度スケジューリング，Multilevel Feedback Queue,
   * Rate Monotonic,Deadline Monotonicは優先度レベルごとのレディーキュー
   */
  else {
    mg_ready_info.type = PRIORITY_READY_QUEUE;
    /* キュー構造のレディーブロックのポインタ */
    rquecb_init(mg_ready_info.entry->un.pri.ready.que, PRIORITY_NUM);
    bitmap_init(); /*! ビットマップの初期化 */
  }
  
  return E_OK;
}


/*!
 * 下位4ビットのパターンを記録するlsb_4bits_tableの初期化
 * 優先度レベルごとにレディーキューを持つスケジューリング法のみ初期化をする
 */
static void bitmap_init(void)
{
  int i, *p;
  READY_TYPE type = mg_ready_info.type;

  /* 優先度レベルごとのレディーキューか */
  if (type == PRIORITY_READY_QUEUE) {
    p = mg_ready_info.entry->un.pri.lsb_4bits_table;
  }
  /* タイムアウトと優先度レベルごとのレディーキューか */
  else if (type == TIMEOUT_PRIORITY_READYQUE) {
    p = mg_ready_info.entry->un.tmout_pri.lsb_4bits_table;
  }
  /* 優先度レベルの2分探索木のレディー構造か */
  else if (type == PRIORITY_BINARY_TREE) {
    p = mg_ready_info.entry->un.pri_btree.lsb_4bits_table;
  }
  /* 以外 */
  else {
    return;
  }

  *p = -PRIORITY_NUM; /* エラー処理として使用 */

  /* ここから初期化処理 */
  for (i = 1; i < LSB_4BIT_PATTERN; i++) {
    if (i & BIT_SERCH_NUNBER) {
      *(p + i) = 0;
    }
    else if (i & BIT_SERCH_NUNBER << 1) {
      *(p + i) = 1;
    }
    else if (i & BIT_SERCH_NUNBER << 2) {
      *(p + i) = 2;
    }
    else {
      *(p + i) = 3;
    }
  }
}


/*!
 * カレントタスク(実行状態TCB)をどのタイプのレディーキューから抜き取るか分岐
 * typeはenumでやっているので，パラメータチェックはいらない
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(レディーに存在しない．つまり対象タスクが実行状態ではない)
 * (返却値)E_OK : 正常終了
 */
ER getcurrent(void)
{
  READY_TYPE type = mg_ready_info.type;
  
  /* process_init_tsk()の呼び出し時のみ評価される */
  if (current == NULL) {
    return E_ILUSE;
  }
  /* First Come First Sarvedとラウンドロビンスケジューリング */
  else if (type == SINGLE_READY_QUEUE) {
    return get_current_singleque();
  }
  /* 簡易O(1)スケジューリング */
  else if (type == TIMEOUT_PRIORITY_READYQUE) {
    return get_current_tmprique();
  }
  /* Earliest Deadline FirstスケジューリングとLeast Laxity Firstスケジューリング,Fairスケジューリング */
  else if (type == BINARY_TREE) {
    return get_tskcurrent_btree(current);
  }
  /* Priority Fiar スケジューリング */
  else if (type == PRIORITY_BINARY_TREE) {
    return get_tskcurrent_pribtree(current);
  }
  /*
   * 優先度スケジューリングとラウンドロビン×優先度スケジューリング，Muletilevel Feedback Queue,
   * Rate Monotonic,Deadline Monotonic
   */
  else {
    return get_current_prique();
  }
}

/*!
 * カレントタスク(実行状態TCB)をどのタイプのレディーキューへつなげるか分岐
 * typeはenumでやっているので，パラメータチェックはいらない
 * flag : どのレディーキューか(※タイムアウトと優先度レベルのレディーキューをもっているものにしか適用しない)
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(すでにレディーに存在している．つまり対象タスクが実行状態となっている)
 * (返却値)E_OK : 正常終了
 */
ER putcurrent(void)
{
  READY_TYPE type = mg_ready_info.type;
  
  /* process_init_tsk()の呼び出し時のみ評価される */
  if (current == NULL) {
    return E_ILUSE;
  }
  /* First Come First Sarvedとラウンドロビンスケジューリング */
  else if (type == SINGLE_READY_QUEUE) {
    return put_current_singleque();
  }
  /*
   * 簡易O(1)スケジューリング(つねにactivキューへ繋げる.
   * expiredキューにつなげる時はスケジューラのタイムブロックでタイムアウトした時)
   */
  else if (type == TIMEOUT_PRIORITY_READYQUE) {
    /* activキュー */
    return put_current_tmpri_activque();
  }
  /* Earliest Deadline FirstスケジューリングとLeast Laxity Firstスケジューリング,Fairスケジューリング */
  else if (type == BINARY_TREE) {
    return put_current_btree();
  }
  /* Priority Fair スケジューリング */
  else if (type == PRIORITY_BINARY_TREE) {
    return put_current_pribtree();
  }
  /*
   * 優先度スケジューリングとラウンドロビン×優先度スケジューリング，Muletilevel Feedback Queue,
   * Rate Monotonic,Deadline Monotonic
   */
  else {
    return put_current_prique();
  }
}


/*!
 * 指定されたTCBをどのタイプのレディーキューから抜き取るか分岐
 * typeはenumでやっているので，パラメータチェックはいらない
 * 抜き取られた後はcurrentに設定される
 * O(1)スケジューリングの時はactivキューかexpiredキューかのtypeを求める(※優先度とタイムアウトキューをもっている場合のみに使用)
 * worktcb : 抜き取るTCB
 * (返却値)E_OK : 正常終了
 * (返却値)E_PAR : パラメータエラー
 */
ER get_tsk_readyque(TCB *worktcb)
{
  READY_TYPE type = mg_ready_info.type;
  UINT8 ready_type;

  /* First Come First Sarvedとラウンドロビンスケジューリング */
  if (type == SINGLE_READY_QUEUE) {
    get_tsk_singleque(worktcb);
    return E_OK;
  }
  /* 簡易O(1)スケジューリング */
  else if (type == TIMEOUT_PRIORITY_READYQUE) {
    ready_type = worktcb->schdul_info.un.odrone_schdul.readyque_flag; /* どのレディーキューにいるか(O(1)スケジューリング時) */
    /* activキューの場合 */
    if (ready_type == ACTIV_READY) {
      get_tsk_tmprique(worktcb, ready_type);
      return E_OK;
    }
    /* expiredキューの場合の設定 */
    else if (ready_type == EXPIRED_READY) {
      get_tsk_tmprique(worktcb, ready_type);
      return E_OK;
    }
    /* 以外 */
    else {
      return E_PAR;
    }
  }
  /* Earliest Deadline FirstスケジューリングとLeast Laxity Firstスケジューリング,Fairスケジューリング */
  else if (type == BINARY_TREE) {
    get_tskcurrent_btree(worktcb); /* get_currentから呼ばれるのと同じ */
    return E_OK;
  }
  /* Priority Fiar スケジューリング */
  else if (type == PRIORITY_BINARY_TREE) {
    return get_tskcurrent_pribtree(worktcb);
  }
  /*
   * 優先度スケジューリングとラウンドロビン×優先度スケジューリング，Muletilevel Feedback Queue,
   * Rate Monotonic,Deadline Monotonic
   */
  else {
    get_tsk_prique(worktcb);
    return E_OK;
  }
}


/*!
 * カレントタスク(実行状態TCB)を単一のレディーキュー先頭または，レディーキュー情報ブロックinit_queから抜き出す
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(レディーに存在しない．つまり対象タスクが実行状態ではない)
 * (返却値)E_OK : 正常終了
 */
static ER get_current_singleque(void)
{
  TCB **next = &current->ready_info.un.que_ready.ready_next;
  TCB **prev = &current->ready_info.un.que_ready.ready_prev;
  RQUECB *p = &mg_ready_info.entry->un.single.ready;

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在するTCBか */
  else if (!(current->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return E_OBJ;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queから抜く) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = NULL;
    current->state &= ~TASK_READY;
    return E_OK;
  }
  /* レディキューから抜き取り */
  else {
    /* カレントスレッドは必ず先頭にあるはずなので，先頭から抜き出す */
    p->head = *next;
    p->head->ready_info.un.que_ready.ready_prev = NULL;
    /* レディーキュータスクがない場合 */
    if (p->head == NULL) {
      p->tail = NULL;
      /* ビットマップの処理はない */
    }
    current->state &= ~TASK_READY;
    *next = *prev = NULL;
    return E_OK;
  }
}


/*!
 * カレントタスク(実行状態TCB)を単一のレディーキューの末尾または，レディーキュー情報ブロックinit_queへ繋げる
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(すでにレディーに存在している．つまり対象タスクが実行状態となっている)
 * (返却値)E_OK : 正常終了
 */
static ER put_current_singleque(void)
{
  RQUECB *p = &mg_ready_info.entry->un.single.ready;

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在するTCBか */
  else if (current->state & TASK_READY) {
    /*すでにある場合は無視*/
    return E_OBJ;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queへつなぐ) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = current;
    current->state |= TASK_READY;
    return E_OK;
  }
  /* レディーキュー末尾に繋げる */
  else {
    if (p->tail) {
      current->ready_info.un.que_ready.ready_prev = p->tail;
      p->tail->ready_info.un.que_ready.ready_next = current;
    }
    else {
      p->head = current;
    }
    p->tail = current;

    /* ビットマップ処理はいらない */
    current->state |= TASK_READY;

    return E_OK;
  }
}


/*!
 * 指定されたTCBを単一のレディーキューまたは，レディーキュー情報ブロックinit_queから抜き取る
 * 単一のレディーキューの場合，chg_pri()からは呼ばれないが(この場合はE_NOSPT)，
 * ter_tsk()などからも呼ばれるためこの関数を追加した
 * 抜き取られた後はcurrentに設定される
 * worktcb : 抜き取るTCB
 */
static void get_tsk_singleque(TCB *worktcb)
{
  TCB **next = &worktcb->ready_info.un.que_ready.ready_next;
  TCB **prev = &worktcb->ready_info.un.que_ready.ready_prev;
  RQUECB *p = &mg_ready_info.entry->un.single.ready;

  /* すでにレディーに存在しないTCBか */
  if (!(worktcb->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return;
  }
  /* レディーキューの先頭を抜き取る */
  else if (worktcb == p->head) {
    p->head = *next;
    p->head->ready_info.un.que_ready.ready_prev = NULL;
    /* レディーにタスクが一つの場合 */
    if (worktcb == p->tail) {
      /* ビットマップ処理はいらない */
      p->tail = NULL;
    }
  }
  /* レディーキューの最後から抜き取る */
  else if (worktcb == p->tail) {
    p->tail = *prev;
    (*prev)->ready_info.un.que_ready.ready_next = NULL;
  }
  /* レディーキューの中間から抜き取る */
  else {
    (*prev)->ready_info.un.que_ready.ready_next = *next;
    (*next)->ready_info.un.que_ready.ready_prev = *prev;
  }
  current = worktcb; /* 優先度変更するタスクはそれぞれの待ち行列の後に挿入するのでcurrentにしておく */
  *next = *prev = NULL;
  current->state &= ~TASK_READY; /* スレッドの状態をスリープ状態にしておく */
}


/*!
 * カレントタスク(実行状態TCB)を優先度レベルのレディーキュー先頭または，レディーキュー情報ブロックinit_queから抜き出す
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(レディーに存在しない．つまり対象タスクが実行状態ではない)
 * (返却値)E_OK : 正常終了
 */
static ER get_current_prique(void)
{
  TCB **next = &current->ready_info.un.que_ready.ready_next;
  TCB **prev = &current->ready_info.un.que_ready.ready_prev;
  RQUECB *p = &mg_ready_info.entry->un.pri.ready.que[current->priority];

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在しないTCBか */
  else if (!(current->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return E_OBJ;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queから抜く) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = NULL;
    current->state &= ~TASK_READY;
    return E_OK;
  }
  /* レディキューから抜き取り */
  else {
    /* カレントスレッドは必ず先頭にあるはずなので，先頭から抜き出す */
    p->head = *next;
    /* NULLチェック(NULLポインタを経由してデータを参照しようとするとデータアボートとなる) */
    if (p->head != NULL) {
      p->head->ready_info.un.que_ready.ready_prev = NULL;
    }
    /* レディーキュータスクがない場合 */
    else {
      p->tail = NULL;
      /* ビットマップの対象ビット(優先度ビット)を落とす */
      mg_ready_info.entry->un.pri.ready.bitmap &= ~(1 << current->priority);
    }
    current->state &= ~TASK_READY;
    *next = *prev = NULL;
    return E_OK;
  }
}


/*!
 * カレントタスク(実行状態TCB)を優先度レベルのレディーキューの末尾または，レディーキュー情報ブロックinit_queへ繋げる
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(すでにレディーに存在している．つまり対象タスクが実行状態となっている)
 * (返却値)E_OK : 正常終了
 */
static ER put_current_prique(void)
{
  RQUECB *p = &mg_ready_info.entry->un.pri.ready.que[current->priority];
  
  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在するTCBか */
  else if (current->state & TASK_READY) {
    /*すでにある場合は無視*/
    return E_OBJ;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queへつなぐ) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = current;
    current->state |= TASK_READY;
    return E_OK;
  }
  /* レディーキュー末尾に繋げる */
  else {
    if (p->tail) {
      current->ready_info.un.que_ready.ready_prev = p->tail;
      p->tail->ready_info.un.que_ready.ready_next = current;
    }
    else {
      p->head = current;
    }
    p->tail = current;
    /* ビットマップの対象ビット(優先度ビット)をセット */
    mg_ready_info.entry->un.pri.ready.bitmap |= (1 << current->priority);
    current->state |= TASK_READY;
    return E_OK;
  }
}


/*!
 * 指定されたTCBを優先度レベルのレディーキューまたは，レディーキュー情報ブロックinit_queから抜き取る
 * 抜き取られた後はcurrentに設定される
 * worktcb : 抜き取るTCB
 * p : 抜き取る対象であるレディーキューのポインタ
 */
static void get_tsk_prique(TCB *worktcb)
{
  TCB **next = &worktcb->ready_info.un.que_ready.ready_next;
  TCB **prev = &worktcb->ready_info.un.que_ready.ready_prev;
  RQUECB *p = &mg_ready_info.entry->un.pri.ready.que[worktcb->priority];

  /* すでにレディーに存在しないTCBか */
  if (!(worktcb->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return;
  }
  /* レディーキューの先頭を抜き取る */
  else if (worktcb == p->head) {
    p->head = *next;
    p->head->ready_info.un.que_ready.ready_prev = NULL;
    /* レディーにタスクが一つの場合 */
    if (worktcb == p->tail) {
      /* ビットマップの対象ビット(優先度ビット)を落とす */
      mg_ready_info.entry->un.pri.ready.bitmap &= ~(1 << worktcb->priority);
      p->tail = NULL;
    }
  }
  /* レディーキューの最後から抜き取る */
  else if (worktcb == p->tail) {
    p->tail = *prev;
    (*prev)->ready_info.un.que_ready.ready_next = NULL;
  }
  /* レディーキューの中間から抜き取る */
  else {
    (*prev)->ready_info.un.que_ready.ready_next = *next;
    (*next)->ready_info.un.que_ready.ready_prev = *prev;
  }
  current = worktcb; /* 優先度変更するタスクはそれぞれの待ち行列の後に挿入するのでcurrentにしておく */
  *next = *prev = NULL;
  current->state &= ~TASK_READY; /* スレッドの状態をスリープ状態にしておく */
}


/*!
 * カレントタスク(実行状態TCB)をタイムアウトと優先度レベルのレディーキュー先頭または，レディーキュー情報ブロックinit_queから抜き出す
 * 処理自体はget_current_priqueとほとんど同じ(ビットマップのとこの処理だけ異なる)．get_current_priqueに組み込むと複雑になるかもと思い追加した
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(レディーに存在しない．つまり対象タスクが実行状態ではない)
 * (返却値)E_OK : 正常終了
 */
static ER get_current_tmprique(void)
{
  TCB **next = &current->ready_info.un.que_ready.ready_next;
  TCB **prev = &current->ready_info.un.que_ready.ready_prev;
  RQUECB *p = &mg_ready_info.entry->un.tmout_pri.activ->que[current->priority];

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在するTCBか */
  else if (!(current->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return E_OBJ;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queから抜く) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = NULL;
    current->state &= ~TASK_READY;
    return E_OK;
  }
  /* レディキューから抜き取り */
  else {
    /* カレントスレッドは必ず先頭にあるはずなので，先頭から抜き出す */
    p->head = *next;
    p->head->ready_info.un.que_ready.ready_prev = NULL;
    /* レディーキュータスクがない場合 */
    if (p->head == NULL) {
      p->tail = NULL;
      /* getの場合はつねにactivキューからとなる */
      mg_ready_info.entry->un.tmout_pri.activ->bitmap &= ~(1 << current->priority); /* ビットマップの対象ビット(優先度ビット)を落とす */
    }
    current->state &= ~TASK_READY;
    *next = *prev = NULL;
    return E_OK;
  }
}


/*!
 * activキューかexpiredキューかのビットマップのセット
 * flag : activキューかexpiredキューか
 */
static void set_tmout_prique_bitmap(UINT8 flag)
{
  UINT32 *p = &mg_ready_info.entry->un.tmout_pri.activ->bitmap;

  /* activキューの場合の設定 */
  if (flag == ACTIV_READY) {
    /* ビットマップの対象ビット(優先度ビット)をセット */
    *p |= (1 << current->priority);
  }
  /* expiredキューの場合の設定 */
  else {
    /* ビットマップの対象ビット(優先度ビット)をセット */
    *p |= (1 << current->priority);
  }
}


/*!
 * activキューかexpiredキューかのビットマップのクリア
 * flag : activキューかexpiredキューか
 */
static void clear_tmout_prique_bitmap(TCB *worktcb, UINT8 flag)
{
  UINT32 *p = &mg_ready_info.entry->un.tmout_pri.activ->bitmap;

  /* activキューの場合の設定 */
  if (flag == ACTIV_READY) {
    /* ビットマップの対象ビット(優先度ビット)をセット */
    *p &= ~(1 << worktcb->priority);
  }
  /* expiredキューの場合の設定 */
  else {
    /* ビットマップの対象ビット(優先度ビット)をセット */
    *p &= ~(1 << worktcb->priority);
  }
}


/*!
 * カレントタスク(実行状態TCB)をタイムアウトと優先度レベルのactivレディーキューの末尾または，レディーキュー情報ブロックinit_queへ繋げる
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(すでにレディーに存在している．つまり対象タスクが実行状態となっている)
 * (返却値)E_OK : 正常終了
 */
static ER put_current_tmpri_activque(void)
{
  UINT8 *p_flag = &current->schdul_info.un.odrone_schdul.readyque_flag;
  RQUECB *p = &mg_ready_info.entry->un.tmout_pri.activ->que[current->priority];

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在するTCBか */
  else if (current->state & TASK_READY) {
    /*すでにある場合は無視*/
    return E_OBJ;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queへつなぐ) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = current;
    current->state |= TASK_READY;
    *p_flag |= ACTIV_READY; /* actevレディーキューにいる記録を設定 */
    return E_OK;
  }
  /* レディーキュー末尾に繋げる */
  else {
    if (p->tail) {
      current->ready_info.un.que_ready.ready_prev = p->tail;
      p->tail->ready_info.un.que_ready.ready_next = current;
    }
    else {
      p->head = current;
    }
    p->tail = current;
    set_tmout_prique_bitmap(ACTIV_READY); /* 対象ビットマップのフラグをセット */
    current->state |= TASK_READY;
    *p_flag |= ACTIV_READY; /* actevレディーキューにいる記録を設定 */

    return E_OK;
  }
}


/*! タスク(実行状態TCB)をタイムアウトと優先度レベルのexpiredレディーキューの末尾へ繋げる */
ER put_current_tmpri_expiredque(void)
{
  RQUECB *p = &mg_ready_info.entry->un.tmout_pri.expired->que[current->priority];
  
  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }
  /* すでにレディーキューに存在するTCBか */
  else if (current->state & TASK_READY) {
    /*すでにある場合は無視*/
    return E_OBJ;
  }
  /* ～initタスクがexpiredキューにつなぐことはない(スライスタイムはないため)～ */
  /* レディーキュー末尾に繋げる */
  else {
    if (p->tail) {
      current->ready_info.un.que_ready.ready_prev = p->tail;
      p->tail->ready_info.un.que_ready.ready_next = current;
    }
    else {
      p->head = current;
    }
    p->tail = current;
    set_tmout_prique_bitmap(EXPIRED_READY); /* 対象ビットマップのフラグをセット */
    current->state |= TASK_READY;
    current->schdul_info.un.odrone_schdul.readyque_flag |= EXPIRED_READY; /* expiredレディーキューにいる記録を設定 */

    return E_OK;
  }
}

/*!
 * 指定されたTCBをタイムアウトと優先度レベルのレディーキューまたは，レディーキュー情報ブロックinit_queから抜き取る
 * 抜き取られた後はcurrentに設定される
 * worktcb : 抜き取るTCB
 * flag : activキューかexpiredキューか
 */
static void get_tsk_tmprique(TCB *worktcb, UINT8 flag)
{
  TCB **next = &worktcb->ready_info.un.que_ready.ready_next;
  TCB **prev = &worktcb->ready_info.un.que_ready.ready_prev;
  RQUECB *p = &mg_ready_info.entry->un.tmout_pri.expired->que[current->priority];

  /* すでにレディーに存在しないTCBか */
  if (!(worktcb->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return;
  }
  /* initタスクの場合(レディーキュー情報ブロックのinit_queへつなぐ) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = NULL;
    current = worktcb; /* 下記のキューから抜き取る時と同様にcurrentに設定しておき，一貫性を保つ */
    current->state &= ~TASK_READY; /* スレッドの状態をスリープ状態にしておく */
  }
  /* initタスク以外 */
  else {
    /* レディーキューの先頭を抜き取る */
    if (worktcb == p->head) {
      p->head = *next;
      p->head->ready_info.un.que_ready.ready_prev = NULL;
      /* レディーにタスクが一つの場合 */
      if (worktcb == p->tail) {
        clear_tmout_prique_bitmap(worktcb, flag); /* 対象ビットマップのフラグをクリア */
        current->schdul_info.un.odrone_schdul.readyque_flag &= ~flag; /* どのレディーキューにいるか記録取り消し */
        p->tail = NULL;
      }
    }
    /* レディーキューの最後から抜き取る */
    else if (worktcb == p->tail) {
      p->tail = *prev;
      (*prev)->ready_info.un.que_ready.ready_next = NULL;
    }
    /* レディーキューの中間から抜き取る */
    else {
      (*prev)->ready_info.un.que_ready.ready_next = *next;
      (*next)->ready_info.un.que_ready.ready_prev = *prev;
    }
    current = worktcb; /* 優先度変更するタスクはそれぞれの待ち行列の後に挿入するのでcurrentにしておく */
    *next = *prev = NULL;
    current->state &= ~TASK_READY; /* スレッドの状態をスリープ状態にしておく */
  }
}


/*!
 * 動的優先度枯渇状態をチェックする関数
 * 繰り返しタスクが起動→DORMANTを繰り返す優先度が枯渇する事がある
 */
static BOOL check_priority_exhaustion(int min_pri, int tsk_num)
{
  if (min_pri == PRIORITY_NUM / 2 && tsk_num != PRIORITY_NUM / 2) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}


/*!
 * カレントタスク(実行状態TCB)を2分探索木または，レディーキュー情報ブロックinit_queへ繋げる
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(すでに2分探索木に存在している．つまり対象タスクが実行状態となっている)
 * (返却値)E_OK : 正常終了
 */
static ER put_current_btree(void)
{
  TCB *tmp;
  int tmp_tim;
  int c_tim;
  BTREE_ATR atr = mg_ready_info.entry->un.btree.ready.atr;
  TCB **b_head = &mg_ready_info.entry->un.btree.ready.tree_head;
  TCB **s_head = &mg_ready_info.entry->un.btree.ready.slist_head;
  TCB **l_next;
  TCB **r_next;

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }

  /* すでにレディーキューに存在するTCBか */
  else if (current->state & TASK_READY) {
    /*すでにある場合は無視*/
    return E_OBJ;
  }

  /* initタスクの場合(レディーキュー情報ブロックのinit_queへつなぐ) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = current;
    current->state |= TASK_READY;
    return E_OK;
  }

  /* rootへ設定 */
  else if (*b_head == NULL) {
    current->ready_info.un.btree_ready.dynamic_prio = 1;
    *s_head = *b_head = current;
    /* ビットマップ処理はいらない */
    current->state |= TASK_READY;
    return E_OK;
  }

  /* ここからレディー操作(root以外) */
  else {
    tmp = *b_head;
    /* 2分木のタイプによって比較する対象データを設定(実行時間,デッドライン時刻か余裕時刻か) */
    if (atr == EXECTION_TIME) {
      c_tim = current->schdul_info.un.fr_schdul.rel_exetim;
    }
    else if (atr == DEADLINE_TIME) {
      c_tim = current->schdul_info.un.rt_schdul.deadtim;
    }
    else {
      c_tim = current->schdul_info.un.rt_schdul.floatim;
    }
    while (1) {
      l_next = &tmp->ready_info.un.btree_ready.left_next;
      r_next = &tmp->ready_info.un.btree_ready.right_next;
      
      /* 2分木のタイプによって比較される対象データを設定(実行時間,デッドライン時刻か余裕時刻か) */
      if (atr == EXECTION_TIME) {
        tmp_tim = tmp->schdul_info.un.fr_schdul.rel_exetim;
      }
      else if (atr == DEADLINE_TIME) {
        tmp_tim = tmp->schdul_info.un.rt_schdul.deadtim;
      }
      else {
        tmp_tim = tmp->schdul_info.un.rt_schdul.floatim;
      }

      /* deadlineが小さいならば左部分木へ */
      if (tmp_tim > c_tim) {
        /* 葉を探す */
        if (*l_next != NULL) {
          tmp = *l_next;
        }
        /* 挿入 */
        else {
          *l_next = current;
          mg_ready_info.entry->un.btree.ready.tsk_num++;
          break;
        }
      }

      /* deadlineが大きいならば右部分木へ */
      else if (tmp_tim < c_tim) {
        /* 葉を探す */
        if (*r_next != NULL) {
          tmp = *r_next;
        }
        /* 挿入 */
        else {
          *r_next = current;
          mg_ready_info.entry->un.btree.ready.tsk_num++;
          break;
        }
      }

      /* deadlineが同じ場合は右部分木へ */
      else {
        /* 挿入 */
        current->ready_info.un.btree_ready.right_next = *r_next;
        /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
        if (*r_next != NULL) {
          (*r_next)->ready_info.un.btree_ready.parent = current;
        }
        *r_next = current;
        mg_ready_info.entry->un.btree.ready.tsk_num++;
        break;
      }
    }
    current->ready_info.un.btree_ready.parent = tmp; /* 親を記録 */
    current->state |= TASK_READY;

    put_sortlist(); /* 整列リストへ挿入 */
    return E_OK;
  }
}


/*! 2分探索木に挿入したTCBを整列リストへ挿入する関数 */
static void put_sortlist(void)
{
  TCB *cur_par = current->ready_info.un.btree_ready.parent;
  TCB **cp_snext = &cur_par->ready_info.un.btree_ready.sort_next;
  TCB **cp_sprev = &cur_par->ready_info.un.btree_ready.sort_prev;
  TCB **s_head = &mg_ready_info.entry->un.btree.ready.slist_head;

  /* 左部分木につながれた場合，親が整列リストでは次となっいる */
  if (cur_par->ready_info.un.btree_ready.left_next == current) {
    current->ready_info.un.btree_ready.sort_next = cur_par;
    current->ready_info.un.btree_ready.sort_prev = *cp_sprev;
    /* 先頭へ挿入する場合 */
    if (*s_head == cur_par) {
      *s_head= current;
    }
    /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
    else {
      (*cp_sprev)->ready_info.un.btree_ready.sort_next = current;
    }
    *cp_sprev = current;
    adjust_priority_btree(); /* 優先度の調整 */
  }

  /* 右部分木につながれた場合，親が整列リストでは前となっている */
  else {
    current->ready_info.un.btree_ready.sort_prev = current->ready_info.un.btree_ready.parent;
    current->ready_info.un.btree_ready.sort_next = *cp_snext;
    /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
    if (*cp_snext != NULL) {
      (*cp_snext)->ready_info.un.btree_ready.sort_prev = current;
    }
    *cp_snext = current;
    adjust_priority_btree(); /* 動的優先度の調整 */
  }

}


/*!
 * 動的優先度を調節する関数(2分木)
 * 優先度枯渇状態の場合は整列リストの先頭から動的優先度を調節する
 */
static void adjust_priority_btree(void)
{
  TCB *tmp;
  int priority;
  TCB **s_prev = &current->ready_info.un.btree_ready.sort_prev;
  BTREERCB *p_rdy = &mg_ready_info.entry->un.btree.ready;

  /* 動的優先度枯渇状態の場合 */
  if (check_priority_exhaustion(p_rdy->min_priority, p_rdy->tsk_num)) {
    tmp = mg_ready_info.entry->un.btree.ready.slist_head;
    priority = 1;
  }
  /* 動的優先度枯渇状態でない場合 */
  else {
    tmp = current;
    /*
     * 整列リストの先頭以外に挿入した時(挿入した箇所から，
     * 整列リストの後のTCBの動的優先度を調整するために，挿入したTCBの動的優先度を決定)
     */
    if (*s_prev != NULL) {
      priority = (*s_prev)->ready_info.un.btree_ready.dynamic_prio + 1;
    }
    /* 整列リスト先頭に挿入した時(整列リストのすべてのTCBの優先度を調整するために，優先度を1へ設定している) */
    else {
      priority = 1;
    }
  }

  /* 整列リストを進めて，動的優先度を調整 */
  for (; tmp != NULL; tmp = tmp->ready_info.un.btree_ready.sort_next, priority++) {
    if (PRIORITY_NUM <= priority) {
      KERNEL_OUTMSG("priority starvation");
      /* kernel sleep */
      down_system();
    }
    tmp->ready_info.un.btree_ready.dynamic_prio = priority;
  }
  mg_ready_info.entry->un.btree.ready.min_priority = priority; /* 最低動的優先度を記録 */

}


/*!
 * 指定されたタスク(カレントタスクも含まれる)を2分探索木または，レディーキュー情報ブロックinit_queから抜き出す
 * 実行状態タスクを抜き取るのと，レディーに存在するタスクを抜き取るのと同じなので，この関数一つにまとめた
 * tcb : 抜き取る対象であるTCBのポインタ
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(レディーに存在しない．つまり対象タスクが実行状態ではない)
 * (返却値)E_OK : 正常終了
 */
static ER get_tskcurrent_btree(TCB *tcb)
{
  TCB *tmp;
  TCB **b_head = &mg_ready_info.entry->un.btree.ready.tree_head;
  TCB **l_next = &tcb->ready_info.un.btree_ready.left_next;
  TCB **r_next = &tcb->ready_info.un.btree_ready.right_next;
  TCB **p_par = &tcb->ready_info.un.btree_ready.parent;

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((tcb->syscall_info.flag != MZ_VOID) && (tcb->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }

  /* すでにレディーキューに存在するTCBか */
  else if (!(tcb->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return E_OBJ;
  }

  /* initタスクの場合(レディーキュー情報ブロックのinit_queから抜く) */
  else if (tcb->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = NULL;
    if (tcb != current) {
      current = tcb;
    }
    tcb->state &= ~TASK_READY;
    return E_OK;
  }

  /* ここからレディー操作 */
  else {
    /* 左右両方に子がある時(左部分木の最もdeadlineが大きいTCBを代替とする) */
    if (*l_next && *r_next) {
      /* 置き換えるTCBを見つける */
      tmp = *l_next;
      while (tmp->ready_info.un.btree_ready.right_next) {
        tmp = tmp->ready_info.un.btree_ready.right_next;
      }
  
      /* 抜き取るTCBの親のチェック */
      /* rootを抜き取る時は，headを更新 */
      if (tcb == *b_head) {
        *b_head = tmp;
      }
      /* root以外の時(抜き取るTCBが親の左に繋がれている) */
      else if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
        (*p_par)->ready_info.un.btree_ready.left_next = tmp; /* 親の左を更新 */
      }
      /* root以外の時(抜き取るTCBが親の右に繋がれている) */
      else {
        (*p_par)->ready_info.un.btree_ready.right_next = tmp; /* 親の右を更新 */
      }
  
      /* 左右両方に子がいる場合の抜き取りなので，右部分木の親は必ず更新 */
      (*r_next)->ready_info.un.btree_ready.parent = tmp;
      
      /* 置き換えるTCBの親のチェック */
      /* 抜き取るTCBの左部分木にTCBが二つ以上ある場合 */
      if (tmp->ready_info.un.btree_ready.parent->ready_info.un.btree_ready.right_next == tmp) {
        tmp->ready_info.un.btree_ready.parent->ready_info.un.btree_ready.right_next = NULL; /* 親の右を更新 */
        tmp->ready_info.un.btree_ready.left_next = *l_next;
      }
      /* 抜き取るTCBの左部分木にTCBが一つしかない場合 */
      else {
        *l_next = NULL; /* 親の左を更新 */
      }
      
      /* 置き換えるTCBの各ポインタを更新 */
      tmp->ready_info.un.btree_ready.parent = *p_par;
      tmp->ready_info.un.btree_ready.right_next = *r_next;
    }
  
    /* 左だけ子がある時 */
    else if (*l_next) {
      /* rootの時 */
      if (tcb == *b_head) {
        *b_head = *l_next;
        (*l_next)->ready_info.un.btree_ready.parent = NULL;
      }
      /* root以外の時 */
      else {
        /* 抜き取るTCBが親の左に繋がれている */
        if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
          (*p_par)->ready_info.un.btree_ready.left_next = *l_next;
        }
        /* 抜き取るTCBが親の右に繋がれている */
        else {
          (*p_par)->ready_info.un.btree_ready.right_next = *l_next;
        }
        /* 一つ上の親へ */
        (*l_next)->ready_info.un.btree_ready.parent = *p_par;
      }
    }
  
    /* 右だけ子がある時 */
    else if (*r_next) {
      /* rootの時 */
      if (tcb == *b_head) {
        *b_head = *r_next;
        (*r_next)->ready_info.un.btree_ready.parent = NULL;
      }
      /* root以外の時 */
      else {
        /* 抜き取るTCBが親の左に繋がれている */
        if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
          (*p_par)->ready_info.un.btree_ready.left_next = *r_next;
        }
        /* 抜き取るTCBが親の右に繋がれている */
        else {
          (*p_par)->ready_info.un.btree_ready.right_next = *r_next;
        }
        /* 一つ上の親へ */
        (*r_next)->ready_info.un.btree_ready.parent = *p_par;
      }
    }
    
    /* 子を持たない時 */
    else {
      /* rootの時 */
      if (tcb == *b_head) {
        *b_head = NULL;
        /* ビットマップの処理はない */
      }
      /* root以外の時(抜き取るTCBが親の左に繋がれている) */
      else if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
        (*p_par)->ready_info.un.btree_ready.left_next = NULL;
      }
      /* root以外の時(抜き取るTCBが親の右に繋がれている) */
      else {
        (*p_par)->ready_info.un.btree_ready.right_next = NULL;
      }
    }
  
    tcb->state &= ~TASK_READY;
    get_sortlist(tcb); /* 整列リストから抜き取り */
    if (tcb != current) {
      current = tcb;
    }
    *p_par = *l_next = *r_next = NULL;
    return E_OK;
  }
}


/*! 2分探索木に挿入したTCBを整列リストから抜き取る関数 */
static void get_sortlist(TCB *tcb)
{
  TCB **s_head = &mg_ready_info.entry->un.btree.ready.slist_head;
  TCB **s_next = &tcb->ready_info.un.btree_ready.sort_next;
  TCB **s_prev = &tcb->ready_info.un.btree_ready.sort_prev;

  /* 抜き取るTCBが整列リストの先頭の時 */
  if (*s_head == tcb) {
    *s_head = *s_next;
  }
  /* それ以外の時 */
  else {
    (*s_prev)->ready_info.un.btree_ready.sort_next = *s_next;
  }
  
  /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
  if (*s_next != NULL) {
    (*s_next)->ready_info.un.btree_ready.sort_prev = *s_prev;
  }
  /* 最後尾の時 */
  else {
    /* 処理なし */
  }
}


/*!
 * カレントタスク(実行状態TCB)を優先度ごとの2分探索木または，レディーキュー情報ブロックinit_queへ繋げる
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(すでに2分探索木に存在している．つまり対象タスクが実行状態となっている)
 * (返却値)E_OK : 正常終了
 */
static ER put_current_pribtree(void)
{
  TCB *tmp;
  int tmp_tim;
  int c_tim;
  BTREE_ATR atr = mg_ready_info.entry->un.pri_btree.ready[current->priority].atr;
  TCB **b_head = &mg_ready_info.entry->un.pri_btree.ready[current->priority].tree_head;
  TCB **s_head = &mg_ready_info.entry->un.pri_btree.ready[current->priority].slist_head;
  TCB **l_next;
  TCB **r_next;

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((current->syscall_info.flag != MZ_VOID) && (current->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }

  /* すでにレディーキューに存在するTCBか */
  else if (current->state & TASK_READY) {
    /*すでにある場合は無視*/
    return E_OBJ;
  }

  /* initタスクの場合(レディーキュー情報ブロックのinit_queへつなぐ) */
  else if (current->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = current;
    current->state |= TASK_READY;
    return E_OK;
  }

  /* rootへ設定 */
  else if (*b_head == NULL) {
    current->ready_info.un.btree_ready.dynamic_prio = 1;
    *s_head = *b_head = current;
    /* ビットマップの対象ビット(優先度ビット)をセット */
    mg_ready_info.entry->un.pri_btree.bitmap |= (1 << current->priority);
    current->state |= TASK_READY;
    return E_OK;
  }

  /* ここからレディー操作(root以外) */
  else {
    tmp = *b_head;
    /* 2分木のタイプによって比較する対象データを設定(実行時間,デッドライン時刻か余裕時刻か) */
    if (atr == EXECTION_TIME) {
      c_tim = current->schdul_info.un.fr_schdul.rel_exetim;
    }
    else if (atr == DEADLINE_TIME) {
      c_tim = current->schdul_info.un.rt_schdul.deadtim;
    }
    else {
      c_tim = current->schdul_info.un.rt_schdul.floatim;
    }
    while (1) {
      l_next = &tmp->ready_info.un.btree_ready.left_next;
      r_next = &tmp->ready_info.un.btree_ready.right_next;
      
      /* 2分木のタイプによって比較される対象データを設定(実行時間,デッドライン時刻か余裕時刻か) */
      if (atr == EXECTION_TIME) {
        tmp_tim = tmp->schdul_info.un.fr_schdul.rel_exetim;
      }
      else if (atr == DEADLINE_TIME) {
        tmp_tim = tmp->schdul_info.un.rt_schdul.deadtim;
      }
      else {
        tmp_tim = tmp->schdul_info.un.rt_schdul.floatim;
      }

      /* deadlineが小さいならば左部分木へ */
      if (tmp_tim > c_tim) {
        /* 葉を探す */
        if (*l_next != NULL) {
          tmp = *l_next;
        }
        /* 挿入 */
        else {
          *l_next = current;
          mg_ready_info.entry->un.pri_btree.ready[current->priority].tsk_num++;
          break;
        }
      }

      /* deadlineが大きいならば右部分木へ */
      else if (tmp_tim < c_tim) {
        /* 葉を探す */
        if (*r_next != NULL) {
          tmp = *r_next;
        }
        /* 挿入 */
        else {
          *r_next = current;
          mg_ready_info.entry->un.pri_btree.ready[current->priority].tsk_num++;
          break;
        }
      }

      /* deadlineが同じ場合は右部分木へ */
      else {
        /* 挿入 */
        current->ready_info.un.btree_ready.right_next = *r_next;
        /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
        if (*r_next != NULL) {
          (*r_next)->ready_info.un.btree_ready.parent = current;
        }
        *r_next = current;
        mg_ready_info.entry->un.pri_btree.ready[current->priority].tsk_num++;
        break;
      }
    }
    current->ready_info.un.btree_ready.parent = tmp; /* 親を記録 */
    
    current->state |= TASK_READY; /* タスクの状態をレディーへ */

    put_pri_sortlist(); /* 整列リストへ挿入 */
    return E_OK;
  }
}


/*! 2分探索木に挿入したTCBを優先度ごとの整列リストへ挿入する関数 */
static void put_pri_sortlist(void)
{
  TCB *cur_par = current->ready_info.un.btree_ready.parent;
  TCB **cp_snext = &cur_par->ready_info.un.btree_ready.sort_next;
  TCB **cp_sprev = &cur_par->ready_info.un.btree_ready.sort_prev;
  TCB **s_head = &mg_ready_info.entry->un.pri_btree.ready[current->priority].slist_head;

  /* 左部分木につながれた場合，親が整列リストでは次となっいる */
  if (cur_par->ready_info.un.btree_ready.left_next == current) {
    current->ready_info.un.btree_ready.sort_next = cur_par;
    current->ready_info.un.btree_ready.sort_prev = *cp_sprev;
    /* 先頭へ挿入する場合 */
    if (*s_head == cur_par) {
      *s_head= current;
    }
    /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
    else {
      (*cp_sprev)->ready_info.un.btree_ready.sort_next = current;
    }
    *cp_sprev = current;
    adjust_priority_pribtree(); /* 優先度の調整 */
  }

  /* 右部分木につながれた場合，親が整列リストでは前となっている */
  else {
    current->ready_info.un.btree_ready.sort_prev = current->ready_info.un.btree_ready.parent;
    current->ready_info.un.btree_ready.sort_next = *cp_snext;
    /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
    if (*cp_snext != NULL) {
      (*cp_snext)->ready_info.un.btree_ready.sort_prev = current;
    }
    *cp_snext = current;
    adjust_priority_pribtree(); /* 動的優先度の調整 */
  }

}


/*!
 * 動的優先度を調節する関数(優先度ごとの2分木)
 * 優先度枯渇状態の場合は整列リストの先頭から動的優先度を調節する
 */
static void adjust_priority_pribtree(void)
{
  TCB *tmp;
  int priority;
  TCB **s_prev = &current->ready_info.un.btree_ready.sort_prev;
  BTREERCB *p_rdy = &mg_ready_info.entry->un.pri_btree.ready[current->priority];

  /* 動的優先度枯渇状態の場合 */
  if (check_priority_exhaustion(p_rdy->min_priority, p_rdy->tsk_num)) {
    tmp = mg_ready_info.entry->un.pri_btree.ready[current->priority].slist_head;
    priority = 1;
  }
  /* 動的優先度枯渇状態でない場合 */
  else {
    tmp = current;
    /*
     * 整列リストの先頭以外に挿入した時(挿入した箇所から，
     * 整列リストの後のTCBの動的優先度を調整するために，挿入したTCBの動的優先度を決定)
     */
    if (*s_prev != NULL) {
      priority = (*s_prev)->ready_info.un.btree_ready.dynamic_prio + 1;
    }
    /* 整列リスト先頭に挿入した時(整列リストのすべてのTCBの優先度を調整するために，優先度を1へ設定している) */
    else {
      priority = 1;
    }
  }

  /* 整列リストを進めて，動的優先度を調整 */
  for (; tmp != NULL; tmp = tmp->ready_info.un.btree_ready.sort_next, priority++) {
    if (PRIORITY_NUM <= priority) {
      KERNEL_OUTMSG("priority starvation");
      /* kernel sleep */
      down_system();
    }
    tmp->ready_info.un.btree_ready.dynamic_prio = priority;
  }
  mg_ready_info.entry->un.pri_btree.ready[current->priority].min_priority = priority; /* 最低動的優先度を記録 */

}


/*!
 * 指定されたタスク(カレントタスクも含まれる)を2分探索木または，レディーキュー情報ブロックinit_queから抜き出す
 * 実行状態タスクを抜き取るのと，レディーに存在するタスクを抜き取るのと同じなので，この関数一つにまとめた
 * tcb : 抜き取る対象であるTCBのポインタ
 * (返却値)E_ILUSE : 不正使用(サービスコールの時はE_ILUSEが返却される)
 * (返却値)E_OBJ : オブジェクトエラー(レディーに存在しない．つまり対象タスクが実行状態ではない)
 * (返却値)E_OK : 正常終了
 */
static ER get_tskcurrent_pribtree(TCB *tcb)
{
  TCB *tmp;
  TCB **b_head = &mg_ready_info.entry->un.pri_btree.ready[tcb->priority].tree_head;
  TCB **l_next = &tcb->ready_info.un.btree_ready.left_next;
  TCB **r_next = &tcb->ready_info.un.btree_ready.right_next;
  TCB **p_par = &tcb->ready_info.un.btree_ready.parent;

  /* 非タスクコンテキスト用のシステムコールか，またはサービスコールか */
  if ((tcb->syscall_info.flag != MZ_VOID) && (tcb->syscall_info.flag != MZ_SYSCALL)) {
    return E_ILUSE;
  }

  /* すでにレディーキューに存在するTCBか */
  else if (!(tcb->state & TASK_READY)) {
    /* すでに無い場合は無視 */
    return E_OBJ;
  }

  /* initタスクの場合(レディーキュー情報ブロックのinit_queから抜く) */
  else if (tcb->init.tskid == INIT_TASK_ID) {
    mg_ready_info.init_que = NULL;
    if (tcb != current) {
      current = tcb;
    }
    tcb->state &= ~TASK_READY;
    return E_OK;
  }

  /* ここからレディー操作 */
  else {
    /* 左右両方に子がある時(左部分木の最もdeadlineが大きいTCBを代替とする) */
    if (*l_next && *r_next) {
      /* 置き換えるTCBを見つける */
      tmp = *l_next;
      while (tmp->ready_info.un.btree_ready.right_next) {
        tmp = tmp->ready_info.un.btree_ready.right_next;
      }
  
      /* 抜き取るTCBの親のチェック */
      /* rootを抜き取る時は，headを更新 */
      if (tcb == *b_head) {
        *b_head = tmp;
      }
      /* root以外の時(抜き取るTCBが親の左に繋がれている) */
      else if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
        (*p_par)->ready_info.un.btree_ready.left_next = tmp; /* 親の左を更新 */
      }
      /* root以外の時(抜き取るTCBが親の右に繋がれている) */
      else {
        (*p_par)->ready_info.un.btree_ready.right_next = tmp; /* 親の右を更新 */
      }
  
      /* 左右両方に子がいる場合の抜き取りなので，右部分木の親は必ず更新 */
      (*r_next)->ready_info.un.btree_ready.parent = tmp;
      
      /* 置き換えるTCBの親のチェック */
      /* 抜き取るTCBの左部分木にTCBが二つ以上ある場合 */
      if (tmp->ready_info.un.btree_ready.parent->ready_info.un.btree_ready.right_next == tmp) {
        tmp->ready_info.un.btree_ready.parent->ready_info.un.btree_ready.right_next = NULL; /* 親の右を更新 */
        tmp->ready_info.un.btree_ready.left_next = *l_next;
      }
      /* 抜き取るTCBの左部分木にTCBが一つしかない場合 */
      else {
        *l_next = NULL; /* 親の左を更新 */
      }
      
      /* 置き換えるTCBの各ポインタを更新 */
      tmp->ready_info.un.btree_ready.parent = *p_par;
      tmp->ready_info.un.btree_ready.right_next = *r_next;
    }
  
    /* 左だけ子がある時 */
    else if (*l_next) {
      /* rootの時 */
      if (tcb == *b_head) {
        *b_head = *l_next;
        (*l_next)->ready_info.un.btree_ready.parent = NULL;
      }
      /* root以外の時 */
      else {
        /* 抜き取るTCBが親の左に繋がれている */
        if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
          (*p_par)->ready_info.un.btree_ready.left_next = *l_next;
        }
        /* 抜き取るTCBが親の右に繋がれている */
        else {
          (*p_par)->ready_info.un.btree_ready.right_next = *l_next;
        }
        /* 一つ上の親へ */
        (*l_next)->ready_info.un.btree_ready.parent = *p_par;
      }
    }
  
    /* 右だけ子がある時 */
    else if (*r_next) {
      /* rootの時 */
      if (tcb == *b_head) {
        *b_head = *r_next;
        (*r_next)->ready_info.un.btree_ready.parent = NULL;
      }
      /* root以外の時 */
      else {
        /* 抜き取るTCBが親の左に繋がれている */
        if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
          (*p_par)->ready_info.un.btree_ready.left_next = *r_next;
        }
        /* 抜き取るTCBが親の右に繋がれている */
        else {
          (*p_par)->ready_info.un.btree_ready.right_next = *r_next;
        }
        /* 一つ上の親へ */
        (*r_next)->ready_info.un.btree_ready.parent = *p_par;
      }
    }
    
    /* 子を持たない時 */
    else {
      /* rootの時 */
      if (tcb == *b_head) {
        *b_head = NULL;
        /* ビットマップの対象ビット(優先度ビット)を落とす */
        mg_ready_info.entry->un.pri_btree.bitmap &= ~(1 << tcb->priority);
      }
      /* root以外の時(抜き取るTCBが親の左に繋がれている) */
      else if ((*p_par)->ready_info.un.btree_ready.left_next == tcb) {
        (*p_par)->ready_info.un.btree_ready.left_next = NULL;
      }
      /* root以外の時(抜き取るTCBが親の右に繋がれている) */
      else {
        (*p_par)->ready_info.un.btree_ready.right_next = NULL;
      }
    }
  
    tcb->state &= ~TASK_READY;
    get_pri_sortlist(tcb); /* 整列リストから抜き取り */
    if (tcb != current) {
      current = tcb;
    }
    *p_par = *l_next = *r_next = NULL;
    return E_OK;
  }
}


/*! 2分探索木に挿入したTCBを整列リストから抜き取る関数 */
static void get_pri_sortlist(TCB *tcb)
{
  TCB **s_head = &mg_ready_info.entry->un.pri_btree.ready[tcb->priority].slist_head;
  TCB **s_next = &tcb->ready_info.un.btree_ready.sort_next;
  TCB **s_prev = &tcb->ready_info.un.btree_ready.sort_prev;

  /* 抜き取るTCBが整列リストの先頭の時 */
  if (*s_head == tcb) {
    *s_head = *s_next;
  }
  /* それ以外の時 */
  else {
    (*s_prev)->ready_info.un.btree_ready.sort_next = *s_next;
  }
  
  /* NULLポインタを経由するとgccはsegmentation faultとなるので，追加 */
  if (*s_next != NULL) {
    (*s_next)->ready_info.un.btree_ready.sort_prev = *s_prev;
  }
  /* 最後尾の時 */
  else {
    /* 処理なし */
  }
}
