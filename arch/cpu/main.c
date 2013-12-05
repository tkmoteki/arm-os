/* BSSセクションの初期化より前に変数を使用しない */
/* private interface */

/* public interface */
/* include/cpu */
#include "intr.h"
/* include/kernel */
#include "debug.h"
#include "command.h"
#include "io_cntrl_macros.h"
#include "kernel.h"
#include "message.h"
#include "scr_symbols.h"
/* include/c */
#include "lib.h"
/* include/driver */
#include "mmc.h"
#include "uart.h"
#include "resources/kernel_obj_id.h" /* リソース情報 */


void uart_handler(void);


/* irqハンドラ */
void uart_handler(void)
{
  unsigned char c;
  static char buf[32];
  static int len;
  int it_type;

  /*
   * UART関連のレジスタであるIIRレジスタから割込みタイプの取得
   * (IIRレジスタは下位5ビットで割込みタイプを保持している)
   * シリアル受信割込み : 0x2
   * タイムアウト割込み(シリアル受信割込みを有効化すると同時に有効化される) : 0x6
   */
  it_type = (REG8_READ(UIIR) & 0x3E) >> 1;

  if (it_type == 2 || it_type == 6) {

    c = getc(); /* 受信FIFOからデータを読み出す事によって，割込み要因をクリア */

    if (c != '\n') {
      buf[len++] = c;
    }
    else {
      buf[len++] = '\0';
      /* echoコマンドの場合 */
      if (!strncmp(buf, "echo ", 5)) {
        echo_command(buf); /* echoコマンド(標準出力にテキストを出力する)呼び出し */
      }
      /* helpコマンドの場合 */
      else if (!strncmp(buf, "help", 4)) {
        help_command(&buf[4]); /* helpコマンド呼び出し */
      }
      /* runコマンドの場合 */
      else if (!strncmp(buf, "run", 3)) {
        run_command(&buf[3]); /* runコマンド(タスクセットの起動)呼び出し */
      }
      /* sendlogの場合 */
      else if (!strncmp(buf, "sendlog", 7)) {
        sendlog_command(); /* sendlogコマンド(xmodem送信モード)呼び出し */
      }
      /* recvlogの場合 */
      else if (!strncmp(buf, "recvlog", 7)) {
        recvlog_command(); /* recvlogコマンド(xmodem受信モード)呼び出し */
      }
      /* dumpの場合 */
      else if (!strncmp(buf, "dump", 4)) {
        dump_command(); /* dumpコマンド */
      }
      /* fatloadの場合 */
      else if (!strncmp(buf, "fatload", 7)) {
        fatload_command(&buf[0]); /* fatloadコマンド */
      }
      /* fatlsの場合 */
      else if (!strncmp(buf, "fatls", 5)) {
        fatls_command(&buf[0]); /* fatlsコマンド */
      }
      /* fatinfoの場合 */
      else if (!strncmp(buf, "fatinfo", 7)) {
        fatinfo_command(&buf[0]); /* fatinfoコマンド */
      }
      /* tsetloadの場合 */
      else if (!strncmp(buf, "tsetload", 8)) {
        tsetload_command(&buf[8]); /* tsetloadコマンド */
      }
      /* 本システムに存在しないコマンド */
      else {
        puts("command unknown.\n");
      }
      puts("> ");
      len = 0;
    }
  }
  else {
    DEBUG_L1_ARCH_CPU_MAIN_OUTMSG("not uart3 handler.\n");
  }
}


/*! OSメイン関数 */
/*  CPSRの外部割込み無効モードとして起動 */
int main(void)
{
  
  UINT32 *s_ptr;
  unsigned char *d_ptr;
  int i;

  /* BSSセクション(静的セクション)の初期化 */
  for (s_ptr = (UINT32 *)&_bss_start; s_ptr < &_bss_end; s_ptr++) {
    *s_ptr = 0;
  }
  /* logbuffuerセクション(動的セクション)の初期化 */
  for (i = 0, d_ptr = (unsigned char *)&_logbuffer_start; i < LOGBUFFER_SIZE; i++, d_ptr++) {
    *d_ptr = '\0';
  }
  /* tskbuffuerセクション(動的セクション)の初期化 */
  for (i = 0, d_ptr = (unsigned char *)&_tskbuffer_start; i < TSKBUFFER_SIZE; i++, d_ptr++) {
    *d_ptr = '\0';
  }

  uart3_init(); /* シリアルの初期化 */

  mmc_legacy_init(0); /* mmcの初期化(dev:0) */

  KERNEL_OUTMSG("kernel boot OK!\n");

  /* OSの動作開始 */
  kernel_init(start_threads, "init tsk", 0, 0x100, 0, NULL); /* initタスク起動 */
  
  /* 正常ならばここには戻ってこない */

  return 0;
}

