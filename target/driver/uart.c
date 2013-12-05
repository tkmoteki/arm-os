/* UART */
/* private interface */

/* public interface */
/* include/driver */
#include "uart.h"                                                /*! arm cortex-A8 UARTシリアル定義 */
/* include/kernel */
#include "io_cntrl_macros.h"


#define UARTClock   (48000000 / 1)                                        /*! クロック(Hz) */
#define UARTSpeed   115200                                                /*! bps */
#define UARTDivisor   (UARTClock / (UARTSpeed * 16))


/* 各レジスタ定義 */
#define URBR      (UART3_BASE_ADR + 0x00)                                 /*! 文字の読み込み */
#define UTHR URBR                                                         /*! レジスタリネーム．文字の書き込み */
#define UIER      (UART3_BASE_ADR + 0x04)                                 /*! 割込みチェックと有効無効化の操作 */
#define ULCR      (UART3_BASE_ADR + 0x0c)                                 /*! データーフォーマット設定 */
#define UMCR      (UART3_BASE_ADR + 0x10)
#define ULSR      (UART3_BASE_ADR + 0x14)                                 /*! 送信FIFOバッファチェック */
//#define UMSR      (UART3_BASE_ADR + 0x18) // R-

#define UDLL URBR                                                         /*! レジスタリネーム．ボーレートの設定など */
#define UDLM      (UART3_BASE_ADR + 0x04)                                 /*! ボーレートの設定など */



/*! 送信可能かチェック */
static int is_send_serial_enable(void);


/*! デバイス初期化 */
void uart3_init(void)
{

  REG8_WRITE(UIER, 0x00); /* 割込み無効化 */

  /* 送信完了まで待機(FIFOバッファを使用しない) */
  while (!(REG8_READ(ULSR) & 0x40)) {
    ;
  }
  REG8_WRITE(ULCR, 0x80); /* 分周比レジスタ(操作レジスタ)の変更 */
  REG8_WRITE(UDLM, 0xff); /* (分周比0の防止する */
  REG8_WRITE(UDLL, UARTDivisor & 0xff); /* ボーレート(115200bpsの設定) */
  REG8_WRITE(UDLM, (UARTDivisor >> 8) & 0xff); /* ボーレート(115200bpsの設定) */
  REG8_WRITE(ULCR, 0x03); /* データ長，パリティ，ストップビットに設定 */
  REG8_WRITE(UMCR, 0x03); /* RTS#, DTR# assert */
  REG8_WRITE(UFCR, 0x07); /* FIFOバッファの有効化と送受信FIFOバッファリセット */

}


/*!
 * 送信可能かチェック
 * (返却値) :
 */
static int is_send_serial_enable(void)
{
  return REG8_READ(ULSR) & 0x20;
}


/*!
 * 1文字送信
 * c:
 */
void send_serial_byte(unsigned char c)
{
  /* 送信完了まで待機(FIFOバッファを使用しない) */
  while (!is_send_serial_enable()) {
    ;
  }

  /* 送信開始 */
  REG8_WRITE(UTHR, c);
}


/*!
 * 受信可能かチェック
 * (返却値) :
 */
int is_recv_serial_enable(void)
{
  return REG8_READ(ULSR) & 0x1f;
}


/*!
 * １文字受信
 * (返却値) :
 */
unsigned char recv_serial_byte(void)
{
  unsigned char s, c;

  while (1) {

    /* 受信完了まで待機(FIFOバッファを使用しない) */
    while (0x00 == (s = is_recv_serial_enable())) {
      ;
    }

    /* 受信終了 */
    c = REG8_READ(URBR);

    /* 受信エラーチェック */
    if (!(s & 0x1e)) {
      break;
    }
  }

  return c;
}


/*
 * 送信割込みが有効か検査
 * (返却値)
 */
int serial_intr_is_send_enable(void)
{
  return REG8_READ(UIER) & 0x02;
}


/* 送信割込みを有効化する */
void serial_intr_send_enable(void)
{
  REG8_WRITE(UIER, REG8_READ(UIER) | 0x02);
}


/* 送信割込みを無効化する */
void serial_intr_send_disable(void)
{
  REG8_WRITE(UIER, REG8_READ(UIER) & ~0x02);
}


/* 
 * 受信割込みが有効か検査
 * (返却値)
 */
int serial_intr_is_recv_enable(void)
{
  return REG8_READ(UIER) & 0x01;
}


/* 受信割込みを有効化する */
void serial_intr_recv_enable(void)
{
  REG8_WRITE(UIER, REG8_READ(UIER) | 0x01);
}


/* 受信割込みを無効化する */
void serial_intr_recv_disable(void)
{
  REG8_WRITE(UIER, REG8_READ(UIER) & ~0x01);
}

