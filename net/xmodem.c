#include "target/driver/serial_driver.h"
#include "jis_ctrl_crd.h"
#include "xmodem.h"


#define XMODEM_BLOCK_SIZE 128 /* XMODEMのブロックサイズ */


/*! NAK待ち(データ送信開始前に呼ぶ) */
static void wait_xmodem(void);

/*! ブロックの送信 */
static void write_xmodem_block(UINT8 block_number, UINT8 *logbuf, int data_len);


/*! NAK待ち(データ送信開始前に呼ぶ) */
static void wait_xmodem(void)
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

  wait_xmodem(); /* 受信側のNAK待ち */  

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
        DEBUG_OUTMSG(" block number value.\n");
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
