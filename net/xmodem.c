/* private interface */
#include "jis_ctrl_crd.h"

/* public interface */
/* include/net */
#include "xmodem.h"
/* include/kernel */
#include "debug.h"
/* include/driver */
#include "uart.h"


#define XMODEM_BLOCK_SIZE 128 /* XMODEMのブロックサイズ */


/*! NAK待ち(データ送信開始前に呼ぶ) */
static void wait_nak_xmodem(void);

/*! ブロックの送信 */
static void write_xmodem_block(UINT8 block_number, UINT8 *logbuf, int data_len);

/* 受信開始されるまで送信要求を出す */
static void xmodem_sending_request(void);

/* ブロック単位での受信 */
static int read_xmodem_block(unsigned char block_number, UINT8 *buf);


/*! NAK待ち(データ送信開始前に呼ぶ) */
static void wait_nak_xmodem(void)
{
  /* 受信側からNAKを受信するまで */
  while(JIS_X_0211_NAK != recv_serial_byte()) {
    ;
  }
}


/*!
 * ブロックの送信
 * block_number : ブロック番号
 * *logbuf : 送信するデータがあるポインタ
 * data_len : ブロック内のデータ長
 */
static void write_xmodem_block(UINT8 block_number, UINT8 *logbuf, int data_len)
{
  UINT8 check_sum;
  int i;

  send_serial_byte(block_number); /* ブロック番号の送信 */

  send_serial_byte((~block_number)); /* 反転したブロック番号の送信 */

  check_sum = 0;
  /* 128バイト分のデータを送信 */
  for (i = 0; i < XMODEM_BLOCK_SIZE; i++) {
    /* 送信dataが128バイトに満たない場合 */
    if (data_len <= i) {
      send_serial_byte(JIS_X_0211_SUB); /* 足りない分をEOFで埋める */
      check_sum += JIS_X_0211_SUB; /* EOF分のチェックサムの計算 */
    }
    /* 送信dataが128バイトに満たす場合 */
    else {
      send_serial_byte(*logbuf); /* ログバッファの内容を1バイトずつ送信 */
      check_sum += *logbuf; /* チェックサムの計算 */
      logbuf++;
    }
  }

  send_serial_byte(check_sum); /* チェックサムの送信 */

}


/*!
 * XMODEMでの送信制御
 * *bufp : 送信するログバッファポインタの先頭
 * size : 送信するログサイズ
 * (返却値)TRUE : 成功
 * (返却値)FALSE : 失敗
 */
BOOL send_xmodem(UINT8 *bufp, UINT32 size)
{
  /*
   * 送信データの終端のため,1つ余分にとる(サイズが128の倍数の場合)
   */
  int cont = size / XMODEM_BLOCK_SIZE + 2; /* 送信するブロック数 */
  int data = size;
  int data_len; /* ブロック内のデータ長 */
  UINT8 recv_crd, block_number = 1; /* ブロック番号は1からスタート */

  wait_nak_xmodem(); /* 受信側のNAK待ち */  

  while (1) {
    /* 送信データ量の処理 */
    if (128 < data) {
      data -= 128;
      data_len = 128;
    }
    else {
      data_len = data;
    }
    send_serial_byte(JIS_X_0211_SOH); /* データ送信開始の合図 */
    write_xmodem_block(block_number, bufp, data_len); /* ブロック送信 */
    recv_crd = recv_serial_byte(); /* 受信側から制御コードを受信 */

    /* 引き続き送信する */
    if (recv_crd == JIS_X_0211_ACK) {
      block_number++; /* 次のブロックへ */
      bufp += XMODEM_BLOCK_SIZE; /* ログバッファポインタを更新 */
    }
    /* 同じブロックの再送 */
    else if (recv_crd == JIS_X_0211_NAK) {
      /* 処理なし(ブロック再送となる) */
    }
    /* 中断(コンソールでCtrl-cが入力された場合) */
    else if (recv_crd == JIS_X_0211_CAN) {
      send_serial_byte(JIS_X_0211_CAN); /* データ送信中断の合図 */
    }
    /* ACKとNAK以外はエラーとする */
    else {
      return FALSE;
    }

    /* 送信終了 */
    if (block_number == (UINT8)cont) {
      send_serial_byte(JIS_X_0211_EOT); /* データ送信終了(ブロックの終了)の合図 */
      recv_crd = recv_serial_byte(); /* 受信側から制御コードを受信 */
      /* 受信側が正常ならば,ACKを受信 */
      if (JIS_X_0211_ACK == recv_crd) {
        putxval((--block_number), 0);
        DEBUG_L1_NET_XMODEM_OUTMSG(" block number value.\n");
        return TRUE;
      }
      /* ACK以外をエラーとする */
      else {
        return FALSE;
      }
    }
  }

  return FALSE;
}


/* 受信開始されるまで送信要求を出す */
static void xmodem_sending_request(void)
{
  INT32 cnt = 0;

  while (0x00 == is_recv_serial_enable()) {
    if (++cnt >= 2000000) {
      cnt = 0;
      send_serial_byte(JIS_X_0211_NAK);
    }
  }

  return;
}


/*!
 * ブロック単位での受信
 * block_number : ブロック番号
 * *buf : 受信先のバッファポインタ
 */
static int read_xmodem_block(unsigned char block_number, UINT8 *buf)
{
  unsigned char c, block_num, check_sum;
  int i;

  block_num = recv_serial_byte();
  if (block_num != block_number) {
    return -1;
  }

  block_num ^= recv_serial_byte();
  if (block_num != 0xff) {
    return -1;
  }

  check_sum = 0;
  for (i = 0; i < XMODEM_BLOCK_SIZE; i++) {
    c = recv_serial_byte();
    *(buf++) = c;
    check_sum += c;
  }

  check_sum ^= recv_serial_byte();
  if (check_sum) {
    return -1;
  }

  return i;
}


/*!
 * XMODEMでの受信制御
 * *buf : 受信先のバッファポインタ
 */
INT32 recv_xmodem(UINT8 *buf)
{
  int r, receiving = 0;
  INT32 size = 0;
  unsigned char c = 0, block_number = 1;

  while (1) {
    if (!receiving) {
      xmodem_sending_request(); /* 受信開始されるまで送信要求を出す */
    }
    c = recv_serial_byte();
    putxval((unsigned long)c, 0);

    if (c == JIS_X_0211_EOT) { /* 受信終了 */
      send_serial_byte(JIS_X_0211_ACK);
      break;
    }
    else if (c == JIS_X_0211_CAN) { /* 受信中断 */
      return -1;
    }
    else if (c == JIS_X_0211_SOH) { /* 受信開始 */
      receiving++;
      r = read_xmodem_block(block_number, buf); /* ブロック単位での受信 */
      if (r < 0) { /* 受信エラー */
        send_serial_byte(JIS_X_0211_NAK);
      } else { /* 正常受信 */
        block_number++;
        size += r;
        buf  += r;
        send_serial_byte(JIS_X_0211_ACK);
      }
    }
    else {
      if (receiving) {
        return (INT32)-1;
      }
    }
  }

  return size;
}


/*!
 * XMODEM受信時でのホストとのタイミング調整
 */
void adjust_timing_xmodem(void)
{
  volatile UINT32 i;
  for (i = 0; i < 300000; i++) {
    ;
  }
}
