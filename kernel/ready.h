#ifndef _READY_H_INCLUDED_
#define _READY_H_INCLUDED_

#include "defines.h"
#include "kernel.h"


/*! 2分木のタイプ(何のデータを元に管理されているか) */
typedef enum {
  EXECTION_TIME = 0,                          /*! 実行時間で管理 */
  DEADLINE_TIME,                              /*! デッドライン時刻で管理 */
  FLOAT_TIME,                                 /*! 余裕時刻で管理 */
} BTREE_ATR;


/*! レディーキュー型 */
typedef struct _ready_queue_infomation {
  TCB *head;                                  /*! レディー先頭ポインタ */
  TCB *tail;                                  /*! レディー最尾ポインタ */
} RQUECB;


/*! レディーキュー型(優先度付き待ち行列) */
typedef struct _priority_ready_queue_infomation {
  UINT32 bitmap;                              /*! ビットマップ(検索高速化のため) */
  RQUECB que[PRIORITY_NUM];                   /*! 優先度レベルでキューを配列化 */
} PRIRQUECB;


/*! レディー(2分探索木)型 */
typedef struct _binary_tree_ready_infomation {
  BTREE_ATR atr;                              /*! 対象順序項目(デッドライン時刻や余裕時間) */
  TCB *slist_head;                            /*! 整列リストの先頭 */
  TCB *tree_head;                             /*! 2分探索木の先頭 */
  int min_priority;                           /*! 2分探索木に存在する最低動的優先度(優先度枯渇防止) */
  int tsk_num;                                /*! 2分探索木に存在するタスク数 */
} BTREERCB;


#define OTHER_READY (1 << 0)                  /*! 優先度とタイムアウトごとのレディーキュー以外にタスクを操作する */
/*! レディーキューコントロールブロック */
/*
 * ～レディーキューごとにもっておく情報～
 * ビットパターンテーブルとビットマップはレディー構造に個別に置いておかなくて大丈夫だが，
 * 見やすくするのと，拡張性を考慮してつめておく
 */
typedef struct {
  union {
    /*! 単一レベルのキュー */
    struct {
      RQUECB ready;                           /*! 単一レディーキュー */
    } single;
    /*! 優先度ごとのキュー */
    struct {
#define LSB_4BIT_PATTERN 16                   /*! LSBから下位4ビットのビットパターン */
#define BIT_SERCH_NUNBER 0x01                 /*! ビットサーチする番号(bitmap_init()で使用) */
      int lsb_4bits_table[LSB_4BIT_PATTERN];  /*! LSBから下位4ビットのパターンを記録する配列(メモリ効率のためPRIQUEには置いとかない) */
      PRIRQUECB ready;                        /*! 優先度レベルでキュー */
    } pri;
    /*! タイムアウトと優先度ごとのキュー */
    struct {
      int lsb_4bits_table[LSB_4BIT_PATTERN];  /*! LSBから下位4ビットのパターンを記録する配列 */
#define ACTIV_READY (2 << 0)                  /*! 実行状態レディーキューにタスクを操作 */
#define EXPIRED_READY (3 << 0)                /*! 満了(タイムアウト)状態レディーキューにタスクを操作 */
      PRIRQUECB *activ;                       /*! 実行状態レディーキューへのポインタ */
      PRIRQUECB *expired;                     /*! 満了状態レディーキューへのポインタ */
      PRIRQUECB activ_ready;                  /*! 実行状態レディーキュー */
      PRIRQUECB expired_ready;                /*! 満了状態レディーキュー */
    } tmout_pri;
    /* 2分木と整列リストのキュー */
    struct {
      BTREERCB ready;
    } btree;
    /* 優先度ごとの2分木と整列リストのキュー */
    struct {
#define LSB_4BIT_PATTERN 16                   /*! LSBから下位4ビットのビットパターン */
#define BIT_SERCH_NUNBER 0x01                 /*! ビットサーチする番号(bitmap_init()で使用) */
      int lsb_4bits_table[LSB_4BIT_PATTERN];  /*! LSBから下位4ビットのパターンを記録する配列(メモリ効率のためPRIQUEには置いとかない) */
      UINT32 bitmap;                          /*! ビットマップ(検索高速化のため) */
      BTREERCB ready[PRIORITY_NUM];           /*! 優先度レベルでツリーを配列化 */
    } pri_btree;
  } un;
} RDYCB;


/*! レディーキュー情報ブロック */
/*
 * ～レディーキューで共通にもっていおく情報～
 * 複数のレディーキュー構造をもっているので(単一のレディーキュー，優先度レディーキュー)，
 * init(優先度0番)は別途異なるところへつないでおく
 * (よって，優先度レディーキューの優先度レベルの配列インデックス0には何もつながれない)
 */
struct _readyque_infomation {
  READY_TYPE type;                          /*! レディーキューのタイプ */
  TCB *init_que;                            /*! initタスク専用領域 */
  RDYCB *entry;                             /*! 対象レディーキューブロックへのポインタ */
} mg_ready_info;


/*! レディーキューの初期化 */
ER ready_init(void);

/*! カレントタスク(実行状態TCB)をどのタイプのレディーキューから抜き取るか分岐 */
ER getcurrent(void);

/*! カレントタスク(実行状態TCB)をどのタイプのレディーキューへつなげるか分岐 */
ER putcurrent(void);

/*! 指定されたTCBをレディーキューから抜き取る */
ER get_tsk_readyque(TCB *worktcb);

/*! タスク(実行状態TCB)をタイムアウトと優先度レベルのexpiredレディーキューの末尾へ繋げる */
ER put_current_tmpri_expiredque(void);


#endif
