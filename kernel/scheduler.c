#include "arch/cpu/intr.h"
#include "debug.h"
#include "scheduler.h"
#include "kernel.h"
#include "memory.h"
#include "scr_symbols.h"
#include "task_manage.h"
//#include "timer_callrte.h"
//#include "target/driver/timer_driver.h"
#include "ready.h"


/*! スケジューラ情報メモリセグメントへ書き込み */
static ER write_schdul(SCHDUL_TYPE type, long param);

static ER switch_schduler(SCHDUL_TYPE type);

/*! スケジューラ情報の読み取り設定 */
static void read_schdul(SCHDULCB *schcb);

/*! First Come First Sarvedスケジューラ */
static void schedule_fcfs(void);

/*! 優先度ビットップを検索する */
static ER_VLE bit_serch(UINT32 bitmap);

/*! 優先度スケジューラ */
static void schedule_ps(void);

/*! RMスケジューラミスハンドラ(条件よりスケジュールできない) */
static void rmschedule_miss_handler(void);

/*! Rate Monotonic */
static void schedule_rms(void);

/*! DMスケジューラミスハンドラ(条件よりスケジュールできない) */
static void dmschedule_miss_handler(void);

/*! Deadline Monotonic */
static void schedule_dms(void);


/*!
 * システムコール処理(sel_schdul():スケジューラの切り替え)
 * type : スケジューラのタイプ
 * param : スケジューラが使用する情報
 * (返却値)E_PAR : パラメータエラー
 * (返却値)E_OK : 正常終了
 */
ER sel_schdul_isr(SCHDUL_TYPE type, long param)
{
  ER ercd;

  /* initタスク生成ルーチンに戻すため，スケジューラ情報メモリセグメントへ書き込み */
  ercd = write_schdul(type, param); /* スケジューラ情報メモリセグメントへ書き込み */
  /* スケジューラを切り替える() */
  if (ercd == E_OK) {
    /*
     * スケジューラを切り替える(rol_sys()が発行されない場合は，後のシステムコールから
     * スケジューラを切り替える(sel_schdulからは切り替えない)ので，その準備をする
     */
    //ercd = switch_schdulr(type);
  }
  return ercd;
}


/*!
 * スケジューラ情報メモリセグメントへ書き込み
 * type : スケジューラのタイプ
 * param : スケジューラが使用する情報
 * (返却値)E_PAR : パラメータエラー
 * (返却値)E_OK : 正常終了
 */
static ER write_schdul(SCHDUL_TYPE type, long param)
{
  char *schdul_info = (char *)&_schdul_area;
  UINT32 *p = (UINT32 *)schdul_info;

  *(--p) = type;  /* スケジューラのタイプを退避 */
  
  /* FCFSスケジューリング */
  if (type == FCFS_SCHEDULING) {
    *(--p) = -1;
    *(--p) = (UINT32)schedule_fcfs; /* スケジューラコントロールブロックの設定 */
  }
  
  /* ラウンドロビンスケジューリング */
  else if (type == RR_SCHEDULING) {
    /* パラメータチェック */
    if (param < TMR_EFFECT) {
      return E_PAR;
    }
    /* スケジューラコントロールブロックの設定 */
    *(--p) = param;
    //*(--p) = (UINT32)schedule_rr;
  }
  
  /* 優先度スケジューリング */
  else if (type == PRI_SCHEDULING) {
    *(--p) = -1;
    *(--p) = (UINT32)schedule_ps; /* スケジューラコントロールブロックの設定 */
  }
  
  /* ラウンドロビン×優先度スケジューリング */
  else if (type == RR_PRI_SCHEDULING) {
    /* パラメータチェック */
    if (param < TMR_EFFECT) {
      return E_PAR;
    }
    *(--p) = param;
    //*(--p) = (UINT32)schedule_rps;
  }
  
  /* Multilevel Feedback Queue */
  else if (type == MFQ_SCHEDULING) {
    /* パラメータチェック */
    if (param < TMR_EFFECT) {
      return E_PAR;
    }
    /* スケジューラコントロールブロックの設定 */
    *(--p) = param;
    //*(--p) = (UINT32)schedule_mfq;
  }
  
  /* 簡易O(1)スケジューリング */
  else if (type == ODRONE_SCHEDULING) {
    /* パラメータチェック */
    if (param < TMR_EFFECT) {
      return E_PAR;
    }
    *(--p) = param;
    //*(--p) = (UINT32)schedule_odrone;
  }
  
  /* Fair Scheduler */
  else if (type == FR_SCHEDULING) {
    /* パラメータチェック */
    if (param < TMR_EFFECT) {
      return E_PAR;
    }
    *(--p) = param;
    //*(--p) = (UINT32)schedule_fr;
  }
  
  /* Priority Fair Scheduler */
  else if (type == PFR_SCHEDULING) {
    /* パラメータチェック */
    if (param < TMR_EFFECT) {
      return E_PAR;
    }
    *(--p) = param;
    //*(--p) = (UINT32)schedule_pfr;
  }
  
  /* Rate Monotonic */
  else if (type == RM_SCHEDULING) {
    *(--p) = -1;
    *(--p) = (UINT32)schedule_rms; /* スケジューラコントロールブロックの設定 */
  }
  
  /* Deadline Monotonic */
  else if (type == DM_SCHEDULING) {
    *(--p) = -1;
    *(--p) = (UINT32)schedule_dms; /* スケジューラコントロールブロックの設定 */
  }

  /* Earliest Deadline First */
  else if (type == EDF_SCHEDULING) {
    *(--p) = -1;
    //*(--p) = (UINT32)schedule_edf; /* スケジューラコントロールブロックの設定 */
  }
  
  /* Least Laxity First */
  else if (type == LLF_SCHEDULING) {
    *(--p) = -1;
    //*(--p) = (UINT32)schedule_llf; /* スケジューラコントロールブロックの設定 */
  }
  
  return E_OK;
}


static ER switch_schduler(SCHDUL_TYPE type)
{
  DEBUG_L1_KERNEL_SCHDUL_OUTMSG("coming soon.\n");

  /*SCHDULCB *next_schcb;

    next_schcb = (SCHDULCB *)get_mpf_isr(sizeof(*next_schcb));
    if (next_schcb == NULL) {
    return E_NOMEM;
    }
    memset(next_schcb, 0, sizeof(*next_schcb));

    if (type == RM_SCHEDULING)   {
    if (is_switch_rm_schdulr()) {
    remove_ready(next_schcb);
    }
    else {
    return E_NOSWT;
    }
    }
    else if (type == DM_SCHEDULING)  {
    if (is_switch_dm_schdulr()) {
    remove_ready(next_schcb);
    }
    else {
    return E_NOSWT;
    }
    }
    else if (type == EDF_SCHEDULING)   {
    if (is_switch_edf_schdulr())  {
    remove_ready(next_schcb);
    }
    else {
    return E_NOSWT;
    }
    }
    else if (type == LLF_SCHEDULING)   {
    if (is_switch_llf_schdulr())  {
    remove_ready(next_schcb);
    }
    else {
    return E_NOSWT;
    }
    }
    else {
    remove_ready(next_schcb);
    } */
  return E_NOSPT;
}


/*!
 * スケジューラ情報を読み取り設定
 * *schcb : 設定するスケジューラコントロールブロック
 */
static void read_schdul(SCHDULCB *schcb)
{
  SCHDUL_TYPE type;
  char *schdul_info = (char *)&_schdul_area;
  UINT32 *p = (UINT32 *)schdul_info;
  long dummy;
  
  mg_schdul_info.type = type = *(--p);  /* スケジューラタイプを復旧 */
  
  /* FCFSスケジューリング */
  if (type == FCFS_SCHEDULING) {
    dummy = *(--p);
    schcb->un.fcfs_schdul.rte = (void *)(*(--p)); /* スケジューラコントロールブロックの設定 */
  }
  
  /* ラウンドロビンスケジューリング */
  else if (type == RR_SCHEDULING) {
    /* スケジューラコントロールブロックの設定 */
    schcb->un.rr_schdul.tmout = (int)*(--p);
    schcb->un.rr_schdul.tobjp = 0;
    schcb->un.rr_schdul.rte = (void *)(*(--p));
  }
  
  /* 優先度スケジューリング */
  else if (type == PRI_SCHEDULING) {
    dummy = *(--p);
    schcb->un.ps_schdul.rte = (void *)(*(--p)); /* スケジューラコントロールブロックの設定 */
  }
  
  /* ラウンドロビン×優先度スケジューリング */
  else if (type == RR_PRI_SCHEDULING) {
    /* スケジューラコントロールブロックの設定 */
    schcb->un.rps_schdul.tmout = (int)*(--p);
    schcb->un.rps_schdul.tobjp = 0;
    schcb->un.rps_schdul.rte = (void *)(*(--p));
  }
  
  /* Multilevel Feedback Queue */
  else if (type == MFQ_SCHEDULING) {
    /* スケジューラコントロールブロックの設定 */
    schcb->un.mfq_schdul.tmout = (int)*(--p);
    schcb->un.mfq_schdul.tobjp = 0;
    schcb->un.mfq_schdul.rte = (void *)(*(--p));
  }
  
  /* 簡易O(1)スケジューリング */
  else if (type == ODRONE_SCHEDULING) {
    /* スケジューラコントロールブロックの設定 */
    schcb->un.odrone_schdul.tmout = (int)*(--p);
    schcb->un.odrone_schdul.tobjp = 0;
    schcb->un.odrone_schdul.rte = (void *)(*(--p));
  }
  
  /* Fair Scheduler */
  else if (type == FR_SCHEDULING) {
    /* スケジューラコントロールブロックの設定 */
    schcb->un.fr_schdul.tmout = *(--p);
    schcb->un.fr_schdul.tobjp = 0;
    schcb->un.fr_schdul.rte = (void *)(*(--p));
  }
  
  /* Priority Fair Scheduler */
  else if (type == PFR_SCHEDULING) {
    /* スケジューラコントロールブロックの設定 */
    schcb->un.pfr_schdul.tmout = (int)*(--p);
    schcb->un.pfr_schdul.tobjp = 0;
    schcb->un.pfr_schdul.rte = (void *)(*(--p));
  }
  
  /* Rate Monotonic */
  else if (type == RM_SCHEDULING) {
    dummy = *(--p);
    schcb->un.rms_schdul.unroll_rate = 0;
    schcb->un.rms_schdul.unroll_exetim = 0;
    schcb->un.rms_schdul.rte = (void *)(*(--p)); /* スケジューラコントロールブロックの設定 */
  }
  
  /* Deadline Monotonic */
  else if (type == DM_SCHEDULING) {
    dummy = *(--p);
    schcb->un.dms_schdul.unroll_dead = 0;
    schcb->un.dms_schdul.unroll_exetim = 0;
    schcb->un.dms_schdul.rte = (void *)(*(--p)); /* スケジューラコントロールブロックの設定 */
  }

  /* Earliest Deadline First */
  else if (type == EDF_SCHEDULING) {
    dummy = *(--p);
    schcb->un.edf_schdul.rte = (void *)(*(--p)); /* スケジューラコントロールブロックの設定 */
  }
  
  /* Least Laxity First */
  else if (type == LLF_SCHEDULING) {
    dummy = *(--p);
    schcb->un.llf_schdul.rte = (void *)(*(--p)); /* スケジューラコントロールブロックの設定 */
  }
  
  mg_schdul_info.entry = schcb;
}


/*!
 * スケジューラの初期化
 * typeはenumでやっているので，パラメータチェックはいらない
 * type : スケジューラのタイプ
 * *exinf : スケジューラに渡すパラメータ
 * (返却値)E_NOMEM : メモリ不足
 * (返却値)E_OK : 正常終了
 */
ER schdul_init(void)
{
  SCHDULCB *schcb;
  
  schcb = (SCHDULCB *)get_mpf_isr(sizeof(*schcb)); /* 動的メモリ取得 */
  if (schcb == NULL) {
    return E_NOMEM;
  }
  memset(schcb, 0, sizeof(*schcb));
  
  read_schdul(schcb); /* スケジューラ情報の読み込み */

  return E_OK;
}


/*!
 * 有効化されているスケジューラは分岐する
 * typeはenumでやっているので，パラメータチェックはいらない
 */
void schedule(void)
{
  /* 登録しておいたスケジューラを関数ポインタで呼ぶ */
  /* FCFSスケジューリング */
  if (mg_schdul_info.type == FCFS_SCHEDULING) {
    (*mg_schdul_info.entry->un.fcfs_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* ラウンドロビンスケジューリング */
  else if (mg_schdul_info.type == RR_SCHEDULING) {
    (*mg_schdul_info.entry->un.rr_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* 優先度スケジューリング */
  else if (mg_schdul_info.type == PRI_SCHEDULING) {
    (*mg_schdul_info.entry->un.ps_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* 優先度×ラウンドロビンスケジューリング */
  else if (mg_schdul_info.type == RR_PRI_SCHEDULING) {
    (*mg_schdul_info.entry->un.rps_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Multilevel Feedback Queue */
  else if (mg_schdul_info.type == MFQ_SCHEDULING) {
    (*mg_schdul_info.entry->un.mfq_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* 簡易O(1)スケジューリング */
  else if (mg_schdul_info.type == ODRONE_SCHEDULING) {
    (*mg_schdul_info.entry->un.odrone_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Fair Scheduler */
  else if (mg_schdul_info.type == FR_SCHEDULING) {
    (*mg_schdul_info.entry->un.fr_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Priority Fair Scheduler */
  else if (mg_schdul_info.type == PFR_SCHEDULING) {
    (*mg_schdul_info.entry->un.pfr_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Rate Monotonic */
  else if (mg_schdul_info.type == RM_SCHEDULING) {
    (*mg_schdul_info.entry->un.rms_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Deadline Monotonic */
  else if (mg_schdul_info.type == DM_SCHEDULING) {
    (*mg_schdul_info.entry->un.dms_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Earliest Deadline First */
  else if (mg_schdul_info.type == EDF_SCHEDULING) {
    (*mg_schdul_info.entry->un.edf_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
  /* Least Laxity First */
  else if (mg_schdul_info.type == LLF_SCHEDULING) {
    (*mg_schdul_info.entry->un.llf_schdul.rte)(); /* 関数ポインタ呼び出し */
  }
}


/*!
 * First Come First Sarvedスケジューラ
 * 優先度はなく，到着順にスケジューリングする
 * p : FCFCスケジューラ使用時のレディーキューのポインタ
 */
static void schedule_fcfs(void)
{
  RQUECB *p = &mg_ready_info.entry->un.single.ready;
  TCB **p_ique = &mg_ready_info.init_que;

  /* 単一のレディーキューの先頭は存在するか */
  if (!p->head) {
    /* initタスクは存在する場合 */
    if (*p_ique) {
      current = *p_ique;
    }
    /* initタスクは存在しない場合 */
    else {
      KERNEL_OUTMSG("error: schdule_fcfs() \n");
      down_system();
    }
  }
  /* 実行するタスクが存在する場合(initタスク以外) */
  else {
    /*
     * 単一のレディーキューの先頭をスケジュール
     * (実行状態タスクとしてディスパッチャへ渡す)
     */
    current = p->head;
  }
}


/*!
 * タイムスライス型スケジューリング環境下で割込みが発生しタイマブロックを排除するか検査する関数
 * type : 割込みの種類
 * (kernel.cのthread_intr()から呼ばれる)
 * 割込みがかかると，タスクが切り替わるのでスケジューラの
 * タイムスライス情報をリセット
 * スケジューラが起こすタイマ割込み以外は抜かす．抜かさないとタイムスライススケジューリングの
 * コールバックルーチンが実行されない．
 * スケジューラ以外のタイマ割込み時は，スケジューラのタイマブロックを排除する
 * syscall.cと下のif分の間にタイマが満了し，タイマ割込みが起こるとタイミング依存なバグになるかも
 */
//void check_tmslice_schedul(SOFTVEC type)
//{
//TMRCB *tbf;
//SCHDUL_TYPE schdul_type = mg_schdul_info.type;
//TMR_OBJP *p_tobjp;

/* if文条件内で代入比較はサポートさせていない */

/* ラウンドロビンスケジューリングかつ，タイマブロックが作成されているか */
//if (schdul_type == RR_SCHEDULING) {
//p_tobjp = &mg_schdul_info.entry->un.rr_schdul.tobjp;
//if (*p_tobjp != 0) {
//tbf = (TMRCB *)*p_tobjp;
//*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
//}
//else {
/* 何もしない */
//return;
//}
//}
/* ラウンドロビン×優先度スケジューリングかつ，タイマブロックが作成されているか */
//else if (schdul_type == RR_PRI_SCHEDULING) {
//p_tobjp = &mg_schdul_info.entry->un.rps_schdul.tobjp;
//if (*p_tobjp != 0) {
//tbf = (TMRCB *)*p_tobjp;
//*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
//}
//else {
/* 何もしない */
//return;
//}
//}
/* Multilevel Feedback Queueかつ，タイマブロックが作成されているか */
//else if (schdul_type == MFQ_SCHEDULING) {
//p_tobjp = &mg_schdul_info.entry->un.mfq_schdul.tobjp;
//if (*p_tobjp != 0) {
//tbf = (TMRCB *)*p_tobjp;
//*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
//}
//else {
/* 何もしない */
//return;
//}
//}
/* 簡易O(1)スケジューリングかつ，タイマブロックが作成されているか */
//else if (schdul_type == ODRONE_SCHEDULING) {
//p_tobjp = &mg_schdul_info.entry->un.odrone_schdul.tobjp;
//if (*p_tobjp != 0) {
//tbf = (TMRCB *)*p_tobjp;
//*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
//}
//else {
/* 何もしない */
//return;
//}
//}
/* Fairスケジューリングかつ，タイマブロックが作成されているか */
//else if (schdul_type == FR_SCHEDULING) {
//p_tobjp = &mg_schdul_info.entry->un.fr_schdul.tobjp;
//if (*p_tobjp != 0) {
//tbf = (TMRCB *)*p_tobjp;
/* 実行時間の加算(CPUバウンドかI/Oバウンドの公平性実現) */
//current->schdul_info.un.fr_schdul.rel_exetim += get_frs_exection_timer(tbf);
//*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
//}
//else {
/* 何もしない */
//return;
//}
//}
/* Priority Fairスケジューリングかつ，タイマブロックが作成されているか */
//else if (schdul_type == PFR_SCHEDULING) {
//p_tobjp = &mg_schdul_info.entry->un.pfr_schdul.tobjp;
//if (*p_tobjp != 0) {
//tbf = (TMRCB *)*p_tobjp;
/* 実行時間の加算(CPUバウンドかI/Oバウンドの公平性実現) */
//current->schdul_info.un.fr_schdul.rel_exetim += get_frs_exection_timer(tbf);
//*p_tobjp = 0; /* タイマブロックとの接続をクリアにしておく */
//}
//else {
/* 何もしない */
//return;
//}
//}
/* それ以外のスケジューリング */
//else {
/* 何もしない */
//return;
//}

/* タイマ割込みではない場合 */
//if (type != SOFTVEC_TYPE_TIMINTR) {
/* タイマブロックを排除 */
//delete_tmrcb_diffque(tbf);
//}
/*
 * タイマ割込みの場合であり，かつソフトタイマキュー先頭がスケジューラのタイマブロックでない場合
 * タイマ割込みが起こった時のタイマブロックは必ずソフトタイマキューの先頭である
 */
//else if (mg_timerque.tmrhead->flag == OTHER_MAKE_TIMER) {
/* タイマブロックを排除 */
//delete_tmrcb_diffque(tbf);
//}
/* それ以外(スケジューラのタイマ割込みの場合) */
//else {
/* 何もしない */
//return;
//}
//}


/*!
 * 優先度ビットップを検索する
 * bitmap : 検索するスケジューラのビットマップ
 * (返却値)E_NG : ビットがない(この関数呼出側でOSをスリープさせる)
 * priority : 立っているビット(優先度となる)
 */
static ER_VLE bit_serch(UINT32 bitmap)
{
#if PRIORITY_NUM > 32
#error ビットマップを配列化する
#endif
  ER_VLE priority = 0; /* 下の処理で変化する */

  /*
   * ビットサーチアルゴリズム(検索の高速化)
   * ビットサーチ命令を持っていないので適用した
   * 32ビット変数を半分づつシフトし，下位4ビットを残し，削り取っていく方法でO(logn)となる
   * 下位4ビットのビットパターンは配列としてlsb_4bits_table[]で持っておく
   */
  if (!(bitmap & 0xffff)) {
    bitmap >>= (PRIORITY_NUM / 2);
    priority += (PRIORITY_NUM / 2);
  }
  if (!(bitmap & 0xff)) {
    bitmap >>= (PRIORITY_NUM / 4);
    priority += (PRIORITY_NUM / 4);
  }
  if (!(bitmap & 0xf)) {
    bitmap >>= (PRIORITY_NUM / 8);
    priority += (PRIORITY_NUM / 8);
  }

  /* 下位4ビット分 */
  priority += mg_ready_info.entry->un.pri.lsb_4bits_table[bitmap & 0xf];
    
  /* ビットが立っていないならば */
  if (priority < 0) {
    return E_NG;
  }
  /* ビットが立っているならば */
  else {
    return priority;
  }
}


/*!
 * 優先度スケジューラ
 * 優先度によって高いものから順にスケジューリングする
 */
static void schedule_ps(void)
{
  ER_VLE priority;
  TCB **p = &mg_ready_info.init_que;

  priority = bit_serch(mg_ready_info.entry->un.pri.ready.bitmap); /* ビットサーチ */
  
  /* 実行可能なタスクが存在しない場合 */
  if (priority == E_NG) {
    /* initタスクは存在する場合 */
    if (*p) {
      current = *p;
    }
    /* initタスクは存在しない場合 */
    else {
      KERNEL_OUTMSG("error: schdule_ps() \n");
      down_system();
    }
  }
  /* 実行するタスクが存在する場合(initタスク以外) */
  else {
    /*
     * ビットサーチ検索した優先度レベルのレディーキューの先頭をスケジュール
     * (実行状態タスクとしてディスパッチャへ渡す)
     */
    current = mg_ready_info.entry->un.pri.ready.que[priority].head;
  }
}

#ifdef SDIV

/*!
 * Rate Monotonic，Deadline Monotonic展開スケジューリングをするための関数
 *** もっと効率よく実装できると思う***
 * ・ユーグリッド互除法を改良したバージョン
 * ・create()システムコール(task_manage.cのacre_isr())で呼ばれ,create()されたタスクまでの周期の最小公倍数,
 *   周期の最小公倍数に沿った最大実行時間を求めて,スケジューラコントロールブロックへセットする
 * len : タスク一つあたりの周期またはデッドライン
 * exetim : タスク一つあたりの仮想実行時間
 */
void set_unrolled_schedule_val(int len, int exetim)
{
  SCHDUL_TYPE type = mg_schdul_info.type;
  int work_len;
  int work_exetim;
  int tmp_len; /* 現在の総合周期 */
  int rest; /* 余り */
  int quo; /* 商 */
  int now_len = len; /* 退避用(現在の周期) */
  
  if (type == RM_SCHEDULING) {
    tmp_len = work_len = mg_schdul_info.entry->un.rms_schdul.unroll_rate;
    work_exetim = mg_schdul_info.entry->un.rms_schdul.unroll_exetim;
  }
  else {
    tmp_len = work_len = mg_schdul_info.entry->un.dms_schdul.unroll_dead;
    work_exetim = mg_schdul_info.entry->un.dms_schdul.unroll_exetim;
  }
  if (!len) {
    DEBUG_L1_KERNEL_SCHDUL_OUTMSG("not division.\n");
    return;
  }
  
  /* すでに周期の最小公倍数と最大実行時間がセットされている場合 */      
  if (work_len && work_exetim) {
    /* ユーグリッド互除法改良ver */
    while ((rest = work_len % len) != 0) {
      work_len = len;
      len = rest;
    }
    /* 周期最小公倍数の計算(スケジューラコントロールブロックへセット) */
    if (type == RM_SCHEDULING) {
      mg_schdul_info.entry->un.rms_schdul.unroll_rate = work_len = tmp_len * now_len / len;
    }
    else {
      mg_schdul_info.entry->un.dms_schdul.unroll_dead = work_len = tmp_len * now_len / len;
    }
    /* 周期に沿った最大実行時間の計算 */
    quo = work_len / tmp_len; /* 乗算する値の決定 */
    work_exetim *= quo; /* すでにセットされているスケジューラコントロールブロックの実行時間メンバの乗算 */
    /* 現在の分を加算(16ビット同士のmsecあたりの乗算となるのでオーバーフローしないように注意) */
    work_exetim += (exetim * (work_len / now_len));
    if (type == RM_SCHEDULING) {
      mg_schdul_info.entry->un.rms_schdul.unroll_exetim = work_exetim; /* スケジューラコントロールブロックへセット */
    }
    else {
      mg_schdul_info.entry->un.dms_schdul.unroll_exetim = work_exetim; /* スケジューラコントロールブロックへセット */
    }
  }
  /* 初期セット */
  else {
    if (type == RM_SCHEDULING) {
      mg_schdul_info.entry->un.rms_schdul.unroll_rate = len;
      mg_schdul_info.entry->un.rms_schdul.unroll_exetim = exetim;
    }
    else {
      mg_schdul_info.entry->un.dms_schdul.unroll_dead = len;
      mg_schdul_info.entry->un.dms_schdul.unroll_exetim = exetim;
    }
  }
}

#endif


/*!
 * スケジューラのデッドラインミスハンドラ(条件よりスケジュールできない)
 * Rate Monotonicに展開スケジューリング専用
 */
static void rmschedule_miss_handler(void)
{
  KERNEL_OUTMSG(" Rate Monotonic Deadline Miss task set\n");
  KERNEL_OUTMSG(" OS sleep... Please push reset button\n");
  down_system(); /* kernelのフリーズ */
  
}


/*!
 * Rate Monotonic Schduler
 * 周期的タスクセットに対して起動周期の短いタスクの順にスケジューリングする
 * OS実装レベルでは周期順に優先度スケジューリングを行えばよい
 */
static void schedule_rms(void)
{
  ER_VLE priority;
  TCB **p = &mg_ready_info.init_que;
  
  /* Deadline(周期と同じ)ミスをしないかチェック */
  if (mg_schdul_info.entry->un.rms_schdul.unroll_rate < mg_schdul_info.entry->un.rms_schdul.unroll_exetim) {
    rmschedule_miss_handler(); /* Deadlineミスハンドラを呼ぶ */
  }
  
  priority = bit_serch(mg_ready_info.entry->un.pri.ready.bitmap); /* ビットサーチ */
  
  /* 実行可能なタスクが存在しない場合 */
  if (priority == E_NG) {
    /* initタスクは存在する場合 */
    if (*p) {
      current = *p;
    }
    /* initタスクは存在しない場合 */
    else {
      KERNEL_OUTMSG("error: schdule_rms() \n");
      down_system();
    }
  }
  /* 実行するタスクが存在する場合(initタスク以外) */
  else {
    /*
     * ビットサーチ検索した優先度レベルのレディーキューの先頭をスケジュール
     * (実行状態タスクとしてディスパッチャへ渡す)
     */
    current = mg_ready_info.entry->un.pri.ready.que[priority].head;
  }
}


/*!
 * スケジューラのデッドラインミスハンドラ(条件よりスケジュールできない)
 * Rate Monotonicに展開スケジューリング専用
 */
static void dmschedule_miss_handler(void)
{
  KERNEL_OUTMSG(" Deadline Monotonic Deadline Miss task set\n");
  KERNEL_OUTMSG(" OS sleep... Please push reset button\n");
  down_system(); /* kernelのフリーズ */
  
}


/*!
 * Deadline Monotonic Scheduler
 * 周期タスクセットに対してデッドラインの短い順にスケジューリングする
 * OS実装レベルではデッドライン順に優先度スケジューリングを行えばよい
 */
static void schedule_dms(void)
{
  ER_VLE priority;
  TCB **p = &mg_ready_info.init_que;
  
  /* Deadline(周期と異なる)ミスをしないかチェック */
  if (mg_schdul_info.entry->un.dms_schdul.unroll_dead < mg_schdul_info.entry->un.dms_schdul.unroll_exetim) {
    dmschedule_miss_handler(); /* Deadlineミスハンドラを呼ぶ */
  }
  
  priority = bit_serch(mg_ready_info.entry->un.pri.ready.bitmap); /* ビットサーチ */
  
  /* 実行可能なタスクが存在しない場合 */
  if (priority == E_NG) {
    /* initタスクは存在する場合 */
    if (*p) {
      current = *p;
    }
    /* initタスクは存在しない場合 */
    else {
      KERNEL_OUTMSG("error: schdule_dms() \n");
      down_system();
    }
  }
  /* 実行するタスクが存在する場合(initタスク以外) */
  else {
    /*
     * ビットサーチ検索した優先度レベルのレディーキューの先頭をスケジュール
     * (実行状態タスクとしてディスパッチャへ渡す)
     */
    current = mg_ready_info.entry->un.pri.ready.que[priority].head;
  }
}
