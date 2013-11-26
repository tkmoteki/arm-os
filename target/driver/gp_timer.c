/* General-Purpose Timers */

/* private interface */

/* public interface */
/* include/driver */
#include "gp_timer.h"
/* include/kernel */
#include "defines.h"


/* Timer Instance Summary(汎用タイマ1〜11のマップアドレス) */
#define GPT1_BASE_ADR           0x48318000                /* General Purpose Timer1(汎用タイマ1のマップアドレス) Module */
#define GPT2_BASE_ADR           0x49032000                /* General Purpose Timer2(汎用タイマ2のマップアドレス) Module */
#define GPT3_BASE_ADR           0x49034000                /* General Purpose Timer3(汎用タイマ3のマップアドレス) Module */
#define GPT4_BASE_ADR           0x49036000                /* General Purpose Timer4(汎用タイマ4のマップアドレス) Module */
#define GPT5_BASE_ADR           0x49038000                /* General Purpose Timer5(汎用タイマ5のマップアドレス) Module */
#define GPT6_BASE_ADR           0x4903A000                /* General Purpose Timer6(汎用タイマ6のマップアドレス) Module */
#define GPT7_BASE_ADR           0x4903C000                /* General Purpose Timer7(汎用タイマ7のマップアドレス) Module */
#define GPT8_BASE_ADR           0x4903E000                /* General Purpose Timer8(汎用タイマ8のマップアドレス) Module */
#define GPT9_BASE_ADR           0x49040000                /* General Purpose Timer9(汎用タイマ9のマップアドレス) Module */
/* 以下のGPT10とGPT11を使用する場合，初期化を行う */
#define GPT10_BASE_ADR          0x48086000                /* General Purpose Timer10(汎用タイマ10のマップアドレス) Module */
#define GPT11_BASE_ADR          0x48088000                /* General Purpose Timer11(汎用タイマ11のマップアドレス) Module */


/* General Purpose Timer1(汎用タイマ1)の各レジスタ定義 */
#define GPT1_TIDR               (GPT1_BASE_ADR + 0x00)
#define GPT1_TIOCP_CFG          (GPT1_BASE_ADR + 0x10)
#define GPT1_TISTAT             (GPT1_BASE_ADR + 0x14)
#define GPT1_TISR               (GPT1_BASE_ADR + 0x18)
#define GPT1_TIER               (GPT1_BASE_ADR + 0x1C)
#define GPT1_TWER               (GPT1_BASE_ADR + 0x20)
#define GPT1_TCLR               (GPT1_BASE_ADR + 0x24)
#define GPT1_TCRR               (GPT1_BASE_ADR + 0x28)
#define GPT1_TLDR               (GPT1_BASE_ADR + 0x2C)
#define GPT1_TTGR               (GPT1_BASE_ADR + 0x30)
#define GPT1_TWPS               (GPT1_BASE_ADR + 0x34)
#define GPT1_TMAR               (GPT1_BASE_ADR + 0x38)
#define GPT1_TCAR1              (GPT1_BASE_ADR + 0x3C)
#define GPT1_TSICR              (GPT1_BASE_ADR + 0x40)
#define GPT1_TCAR2              (GPT1_BASE_ADR + 0x44)
#define GPT1_TPIR               (GPT1_BASE_ADR + 0x48)
#define GPT1_TNIR               (GPT1_BASE_ADR + 0x4C)
#define GPT1_TCVR               (GPT1_BASE_ADR + 0x50)
#define GPT1_TOCR               (GPT1_BASE_ADR + 0x54)
#define GPT1_TOWR               (GPT1_BASE_ADR + 0x58)


/* General Purpose Timer2(汎用タイマ2)の各レジスタ定義 */
#define GPT2_TIDR               (GPT2_BASE_ADR + 0x00)
#define GPT2_TIOCP_CFG          (GPT2_BASE_ADR + 0x10)
#define GPT2_TISTAT             (GPT2_BASE_ADR + 0x14)
#define GPT2_TISR               (GPT2_BASE_ADR + 0x18)
#define GPT2_TIER               (GPT2_BASE_ADR + 0x1C)
#define GPT2_TWER               (GPT2_BASE_ADR + 0x20)
#define GPT2_TCLR               (GPT2_BASE_ADR + 0x24)
#define GPT2_TCRR               (GPT2_BASE_ADR + 0x28)
#define GPT2_TLDR               (GPT2_BASE_ADR + 0x2C)
#define GPT2_TTGR               (GPT2_BASE_ADR + 0x30)
#define GPT2_TWPS               (GPT2_BASE_ADR + 0x34)
#define GPT2_TMAR               (GPT2_BASE_ADR + 0x38)
#define GPT2_TCAR1              (GPT2_BASE_ADR + 0x3C)
#define GPT2_TSICR              (GPT2_BASE_ADR + 0x40)
#define GPT2_TCAR2              (GPT2_BASE_ADR + 0x44)
#define GPT2_TPIR               (GPT2_BASE_ADR + 0x48)
#define GPT2_TNIR               (GPT2_BASE_ADR + 0x4C)
#define GPT2_TCVR               (GPT2_BASE_ADR + 0x50)
#define GPT2_TOCR               (GPT2_BASE_ADR + 0x54)
#define GPT2_TOWR               (GPT2_BASE_ADR + 0x58)


/* General Purpose Timer3(汎用タイマ3)の各レジスタ定義 */
#define GPT3_TIDR               (GPT3_BASE_ADR + 0x00)
#define GPT3_TIOCP_CFG          (GPT3_BASE_ADR + 0x10)
#define GPT3_TISTAT             (GPT3_BASE_ADR + 0x14)
#define GPT3_TISR               (GPT3_BASE_ADR + 0x18)
#define GPT3_TIER               (GPT3_BASE_ADR + 0x1C)
#define GPT3_TWER               (GPT3_BASE_ADR + 0x20)
#define GPT3_TCLR               (GPT3_BASE_ADR + 0x24)
#define GPT3_TCRR               (GPT3_BASE_ADR + 0x28)
#define GPT3_TLDR               (GPT3_BASE_ADR + 0x2C)
#define GPT3_TTGR               (GPT3_BASE_ADR + 0x30)
#define GPT3_TWPS               (GPT3_BASE_ADR + 0x34)
#define GPT3_TMAR               (GPT3_BASE_ADR + 0x38)
#define GPT3_TCAR1              (GPT3_BASE_ADR + 0x3C)
#define GPT3_TSICR              (GPT3_BASE_ADR + 0x40)
#define GPT3_TCAR2              (GPT3_BASE_ADR + 0x44)


/* General Purpose Timer4(汎用タイマ4)の各レジスタ定義 */
#define GPT4_TIDR               (GPT4_BASE_ADR + 0x00)
#define GPT4_TIOCP_CFG          (GPT4_BASE_ADR + 0x10)
#define GPT4_TISTAT             (GPT4_BASE_ADR + 0x14)
#define GPT4_TISR               (GPT4_BASE_ADR + 0x18)
#define GPT4_TIER               (GPT4_BASE_ADR + 0x1C)
#define GPT4_TWER               (GPT4_BASE_ADR + 0x20)
#define GPT4_TCLR               (GPT4_BASE_ADR + 0x24)
#define GPT4_TCRR               (GPT4_BASE_ADR + 0x28)
#define GPT4_TLDR               (GPT4_BASE_ADR + 0x2C)
#define GPT4_TTGR               (GPT4_BASE_ADR + 0x30)
#define GPT4_TWPS               (GPT4_BASE_ADR + 0x34)
#define GPT4_TMAR               (GPT4_BASE_ADR + 0x38)
#define GPT4_TCAR1              (GPT4_BASE_ADR + 0x3C)
#define GPT4_TSICR              (GPT4_BASE_ADR + 0x40)
#define GPT4_TCAR2              (GPT4_BASE_ADR + 0x44)


/* General Purpose Timer5(汎用タイマ5)の各レジスタ定義 */
#define GPT5_TIDR               (GPT5_BASE_ADR + 0x00)
#define GPT5_TIOCP_CFG          (GPT5_BASE_ADR + 0x10)
#define GPT5_TISTAT             (GPT5_BASE_ADR + 0x14)
#define GPT5_TISR               (GPT5_BASE_ADR + 0x18)
#define GPT5_TIER               (GPT5_BASE_ADR + 0x1C)
#define GPT5_TWER               (GPT5_BASE_ADR + 0x20)
#define GPT5_TCLR               (GPT5_BASE_ADR + 0x24)
#define GPT5_TCRR               (GPT5_BASE_ADR + 0x28)
#define GPT5_TLDR               (GPT5_BASE_ADR + 0x2C)
#define GPT5_TTGR               (GPT5_BASE_ADR + 0x30)
#define GPT5_TWPS               (GPT5_BASE_ADR + 0x34)
#define GPT5_TMAR               (GPT5_BASE_ADR + 0x38)
#define GPT5_TCAR1              (GPT5_BASE_ADR + 0x3C)
#define GPT5_TSICR              (GPT5_BASE_ADR + 0x40)
#define GPT5_TCAR2              (GPT5_BASE_ADR + 0x44)


/* General Purpose Timer6(汎用タイマ6)の各レジスタ定義 */
#define GPT6_TIDR               (GPT6_BASE_ADR + 0x00)
#define GPT6_TIOCP_CFG          (GPT6_BASE_ADR + 0x10)
#define GPT6_TISTAT             (GPT6_BASE_ADR + 0x14)
#define GPT6_TISR               (GPT6_BASE_ADR + 0x18)
#define GPT6_TIER               (GPT6_BASE_ADR + 0x1C)
#define GPT6_TWER               (GPT6_BASE_ADR + 0x20)
#define GPT6_TCLR               (GPT6_BASE_ADR + 0x24)
#define GPT6_TCRR               (GPT6_BASE_ADR + 0x28)
#define GPT6_TLDR               (GPT6_BASE_ADR + 0x2C)
#define GPT6_TTGR               (GPT6_BASE_ADR + 0x30)
#define GPT6_TWPS               (GPT6_BASE_ADR + 0x34)
#define GPT6_TMAR               (GPT6_BASE_ADR + 0x38)
#define GPT6_TCAR1              (GPT6_BASE_ADR + 0x3C)
#define GPT6_TSICR              (GPT6_BASE_ADR + 0x40)
#define GPT6_TCAR2              (GPT6_BASE_ADR + 0x44)


/* General Purpose Timer7(汎用タイマ7)の各レジスタ定義 */
#define GPT7_TIDR               (GPT7_BASE_ADR + 0x00)
#define GPT7_TIOCP_CFG          (GPT7_BASE_ADR + 0x10)
#define GPT7_TISTAT             (GPT7_BASE_ADR + 0x14)
#define GPT7_TISR               (GPT7_BASE_ADR + 0x18)
#define GPT7_TIER               (GPT7_BASE_ADR + 0x1C)
#define GPT7_TWER               (GPT7_BASE_ADR + 0x20)
#define GPT7_TCLR               (GPT7_BASE_ADR + 0x24)
#define GPT7_TCRR               (GPT7_BASE_ADR + 0x28)
#define GPT7_TLDR               (GPT7_BASE_ADR + 0x2C)
#define GPT7_TTGR               (GPT7_BASE_ADR + 0x30)
#define GPT7_TWPS               (GPT7_BASE_ADR + 0x34)
#define GPT7_TMAR               (GPT7_BASE_ADR + 0x38)
#define GPT7_TCAR1              (GPT7_BASE_ADR + 0x3C)
#define GPT7_TSICR              (GPT7_BASE_ADR + 0x40)
#define GPT7_TCAR2              (GPT7_BASE_ADR + 0x44)


/* General Purpose Timer8(汎用タイマ8)の各レジスタ定義 */
#define GPT8_TIDR               (GPT8_BASE_ADR + 0x00)
#define GPT8_TIOCP_CFG          (GPT8_BASE_ADR + 0x10)
#define GPT8_TISTAT             (GPT8_BASE_ADR + 0x14)
#define GPT8_TISR               (GPT8_BASE_ADR + 0x18)
#define GPT8_TIER               (GPT8_BASE_ADR + 0x1C)
#define GPT8_TWER               (GPT8_BASE_ADR + 0x20)
#define GPT8_TCLR               (GPT8_BASE_ADR + 0x24)
#define GPT8_TCRR               (GPT8_BASE_ADR + 0x28)
#define GPT8_TLDR               (GPT8_BASE_ADR + 0x2C)
#define GPT8_TTGR               (GPT8_BASE_ADR + 0x30)
#define GPT8_TWPS               (GPT8_BASE_ADR + 0x34)
#define GPT8_TMAR               (GPT8_BASE_ADR + 0x38)
#define GPT8_TCAR1              (GPT8_BASE_ADR + 0x3C)
#define GPT8_TSICR              (GPT8_BASE_ADR + 0x40)
#define GPT8_TCAR2              (GPT8_BASE_ADR + 0x44)


/* General Purpose Timer9(汎用タイマ9)の各レジスタ定義 */
#define GPT9_TIDR               (GPT9_BASE_ADR + 0x00)
#define GPT9_TIOCP_CFG          (GPT9_BASE_ADR + 0x10)
#define GPT9_TISTAT             (GPT9_BASE_ADR + 0x14)
#define GPT9_TISR               (GPT9_BASE_ADR + 0x18)
#define GPT9_TIER               (GPT9_BASE_ADR + 0x1C)
#define GPT9_TWER               (GPT9_BASE_ADR + 0x20)
#define GPT9_TCLR               (GPT9_BASE_ADR + 0x24)
#define GPT9_TCRR               (GPT9_BASE_ADR + 0x28)
#define GPT9_TLDR               (GPT9_BASE_ADR + 0x2C)
#define GPT9_TTGR               (GPT9_BASE_ADR + 0x30)
#define GPT9_TWPS               (GPT9_BASE_ADR + 0x34)
#define GPT9_TMAR               (GPT9_BASE_ADR + 0x38)
#define GPT9_TCAR1              (GPT9_BASE_ADR + 0x3C)
#define GPT9_TSICR              (GPT9_BASE_ADR + 0x40)
#define GPT9_TCAR2              (GPT9_BASE_ADR + 0x44)


/* GPT10を使用する場合，初期化を行う */
/* General Purpose Timer10(汎用タイマ10)の各レジスタ定義 */
#define GPT10_TIDR              (GPT10_BASE_ADR + 0x00)
#define GPT10_TIOCP_CFG         (GPT10_BASE_ADR + 0x10)
#define GPT10_TISTAT            (GPT10_BASE_ADR + 0x14)
#define GPT10_TISR              (GPT10_BASE_ADR + 0x18)
#define GPT10_TIER              (GPT10_BASE_ADR + 0x1C)
#define GPT10_TWER              (GPT10_BASE_ADR + 0x20)
#define GPT10_TCLR              (GPT10_BASE_ADR + 0x24)
#define GPT10_TCRR              (GPT10_BASE_ADR + 0x28)
#define GPT10_TLDR              (GPT10_BASE_ADR + 0x2C)
#define GPT10_TTGR              (GPT10_BASE_ADR + 0x30)
#define GPT10_TWPS              (GPT10_BASE_ADR + 0x34)
#define GPT10_TMAR              (GPT10_BASE_ADR + 0x38)
#define GPT10_TCAR1             (GPT10_BASE_ADR + 0x3C)
#define GPT10_TSICR             (GPT10_BASE_ADR + 0x40)
#define GPT10_TCAR2             (GPT10_BASE_ADR + 0x44)
#define GPT10_TPIR              (GPT10_BASE_ADR + 0x48)
#define GPT10_TNIR              (GPT10_BASE_ADR + 0x4C)
#define GPT10_TCVR              (GPT10_BASE_ADR + 0x50)
#define GPT10_TOCR              (GPT10_BASE_ADR + 0x54)
#define GPT10_TOWR              (GPT10_BASE_ADR + 0x58)


/* GPT11を使用する場合，初期化を行う */
/* General Purpose Timer11(汎用タイマ11)の各レジスタ定義 */
#define GPT11_TIDR              (GPT11_BASE_ADR + 0x00)
#define GPT11_TIOCP_CFG         (GPT11_BASE_ADR + 0x10)
#define GPT11_TISTAT            (GPT11_BASE_ADR + 0x14)
#define GPT11_TISR              (GPT11_BASE_ADR + 0x18)
#define GPT11_TIER              (GPT11_BASE_ADR + 0x1C)
#define GPT11_TWER              (GPT11_BASE_ADR + 0x20)
#define GPT11_TCLR              (GPT11_BASE_ADR + 0x24)
#define GPT11_TCRR              (GPT11_BASE_ADR + 0x28)
#define GPT11_TLDR              (GPT11_BASE_ADR + 0x2C)
#define GPT11_TTGR              (GPT11_BASE_ADR + 0x30)
#define GPT11_TWPS              (GPT11_BASE_ADR + 0x34)
#define GPT11_TMAR              (GPT11_BASE_ADR + 0x38)
#define GPT11_TCAR1             (GPT11_BASE_ADR + 0x3C)
#define GPT11_TSICR             (GPT11_BASE_ADR + 0x40)
#define GPT11_TCAR2             (GPT11_BASE_ADR + 0x44)


/* 32-kHz Sync Timer */
//#define REG_32KSYNCNT_REV       0x48320000
//#define REG_32KSYNCNT_SYSCONFIG 0x48320004
//#define REG_32KSYNCNT_CR        0x48320010

#define OVF_IT_BIT 0x2
#define MAT_IT_BIT 0x1

//#define SYS_CLK_HZ 13*1000*1000
#define NUM_TIMER 11


/*! タイマ動作中か検査する関数 */
static BOOL is_running_timer(int index);


/* ~タイマ周りのレジスタ定義~ */
/*! This register controls optional features specific to the timer functionality */
static unsigned long gpt_tclr[NUM_TIMER] = {
  GPT1_TCLR,
  GPT2_TCLR,
  GPT3_TCLR,
  GPT4_TCLR,
  GPT5_TCLR,
  GPT6_TCLR,
  GPT7_TCLR,
  GPT8_TCLR,
  GPT9_TCLR,
  GPT10_TCLR,
  GPT11_TCLR
};

/*! This register holds the timer load values. */
static unsigned long gpt_tldr[NUM_TIMER] = {
  GPT1_TLDR,
  GPT2_TLDR,
  GPT3_TLDR,
  GPT4_TLDR,
  GPT5_TLDR,
  GPT6_TLDR,
  GPT7_TLDR,
  GPT8_TLDR,
  GPT9_TLDR,
  GPT10_TLDR,
  GPT11_TLDR
};

/*! This register  triggers a counter reload of timer by writing any value in it. */
static unsigned long gpt_ttgr[NUM_TIMER] = {
  GPT1_TTGR,
  GPT2_TTGR,
  GPT3_TTGR,
  GPT4_TTGR,
  GPT5_TTGR,
  GPT6_TTGR,
  GPT7_TTGR,
  GPT8_TTGR,
  GPT9_TTGR,
  GPT10_TTGR,
  GPT11_TTGR
};

/*! This register holds the vlaue of the internal counter. */
static unsigned long gpt_tcrr[NUM_TIMER] = {
  GPT1_TCRR,
  GPT2_TCRR,
  GPT3_TCRR,
  GPT4_TCRR,
  GPT5_TCRR,
  GPT6_TCRR,
  GPT7_TCRR,
  GPT8_TCRR,
  GPT9_TCRR,
  GPT10_TCRR,
  GPT11_TCRR
};

/*! This register shows which interrput events are pending inside the module. */
static unsigned long gpt_tisr[NUM_TIMER] = {
  GPT1_TISR,
  GPT2_TISR,
  GPT3_TISR,
  GPT4_TISR,
  GPT5_TISR,
  GPT6_TISR,
  GPT7_TISR,
  GPT8_TISR,
  GPT9_TISR,
  GPT10_TISR,
  GPT11_TISR
};

/*! This register controls (enable/disable) the interrput events. */
static unsigned long gpt_tier[NUM_TIMER] = {
  GPT1_TIER,
  GPT2_TIER,
  GPT3_TIER,
  GPT4_TIER,
  GPT5_TIER,
  GPT6_TIER,
  GPT7_TIER,
  GPT8_TIER,
  GPT9_TIER,
  GPT10_TIER,
  GPT11_TIER
};

/*
 * 汎用タイマのtmarは本OSでは使用しない.
 * 使用するならば，以下のように定義する
 static unsigned long gpt_tmar[NUM_TIMER] = {
 GPT1_TMAR,
 GPT2_TMAR,
 GPT3_TMAR,
 GPT4_TMAR,
 GPT5_TMAR,
 GPT6_TMAR,
 GPT7_TMAR,
 GPT8_TMAR,
 GPT9_TMAR,
 GPT10_TMAR,
 GPT11_TMAR
 };
*/


/*!
 * 周期タイマスタート
 * index : タイマ資源番号
 * タイマの割込みイベントは以下がある
 * ・コンペアマッチ
 * ・オーバーフロー
 * ・キャプチャ
 * ここでは，オーバーフローとする
 */
void start_cycle_timer(int index, int usec)
{
  /* タイマ初期設定 */
  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) & ~0x20); /* プリスケーラ(分周器)の無効化  */
  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) | 0x2); /* 周期タイマとして使用する事を設定 */

  /*
   * TLDRレジスタに初期値を書き込み，TTGRレジスタに任意の値(1)を書きこむ．
   * この方式だと，タイマーが満了した時に，TLDRの値が自動的にTCRRレジスタにコピーされる
   */
  REG32_WRITE(gpt_tldr[index], 0xFFFFFFFF - 13 * usec); /* タイマ値のセット */
  REG32_WRITE(gpt_ttgr[index], 0x1); /* TCRRレジスタにTLDRレジスタがコピーされる */

  REG32_WRITE(gpt_tisr[index], OVF_IT_BIT); /* 念のため，割込みを無効化しておく */
  REG32_WRITE(gpt_tier[index], OVF_IT_BIT); /* タイマのイベントをオーバーフロー割込みを有効化 */

  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) | 0x1); /* タイマカウントスタート */
}


/*!
 * ワンショットタイマスタート
 * index : タイマ資源番号
 * タイマの割込みイベントは以下がある
 * ・コンペアマッチ
 * ・オーバーフロー
 * ・キャプチャ
 * ここでは，オーバーフローとする
 */
void start_oneshot_timer(int index, int usec)
{
  /* タイマ初期設定 */
  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) & ~0x20); /* プリスケーラ(分周器)の無効化  */
  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) & ~0x2); /* ワンショットタイマとして使用する事を設定 */

  /*
   * TLDRレジスタに初期値を書き込み，TTGRレジスタに任意の値(1)を書きこむ．
   * この方式だと，タイマーが満了した時に，TLDRの値が自動的にTCRRレジスタにコピーされる
   */
  REG32_WRITE(gpt_tldr[index], 0xFFFFFFFF - 13 * usec); /* タイマ値のセット */
  REG32_WRITE(gpt_ttgr[index], 0x1); /* TCRRレジスタにTLDRレジスタがコピーされる */

  REG32_WRITE(gpt_tisr[index], OVF_IT_BIT); /* 念のため，割込みを無効化しておく */
  REG32_WRITE(gpt_tier[index], OVF_IT_BIT); /* タイマのイベントをオーバーフロー割込みを有効化(コンペアマッチとキャプチャは無効にする) */

  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) | 0x1); /* タイマカウントスタート */
  
  //stop_timer(index); /* タイマカウントの停止 */
}


/*!
 * 周期タイマ割込み満了処理
 * index : タイマ資源番号
 */
void expire_cycle_timer(int index)
{
  REG32_WRITE(gpt_tisr[index],OVF_IT_BIT);
}


/*!
 * ワンショットタイマ割込み満了処理
 * index : タイマ資源番号
 */
void expire_oneshot_timer(int index)
{
  REG32_WRITE(gpt_tisr[index], REG32_READ(gpt_tisr[index]));
}


/*!
 * タイマの停止(一度止めたタイマ資源はstart_timer()で再度起動できる)
 * index : タイマ資源番号
 */
void stop_timer(int index)
{
  REG32_WRITE(gpt_tclr[index], REG32_READ(gpt_tclr[index]) & ~0x01);
}


/*!
 * タイマキャンセル(一度キャンセルしたタイマ資源はstart_timer()で再度起動できる)
 * index : タイマ資源番号
 */
void cancel_timer(int index)
{
  stop_timer(index);

  REG32_WRITE(gpt_tier[index], ~OVF_IT_BIT); /* タイマのイベントをオーバーフロー割込みを無効化 */
}


/*!
 * タイマ動作中か検査する関数
 * index : タイマ資源番号
 * (返却値)TRUE : タイマ動作中
 * (返却値)FALSE : タイマ未動作
 */
static BOOL is_running_timer(int index)
{
  if (REG32_READ(gpt_tier[index]) && OVF_IT_BIT) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}


/*!
 * タイマの現在値を取得する関数
 * index : タイマ資源番号
 * (返却値)usec : タイマの現在値
 * (返却値)E_NG : タイマは動作してない
 */
ER_VLE get_timervalue(int index)
{
  int usec;
  usec = (0xFFFFFFFF - REG32_READ(gpt_tcrr[index])) / 13; /* 逆算 */

  if (is_running_timer(index)) {
    return (ER_VLE)usec;
  }
  else {
    return E_NG;
  }
}
