#include "defines.h"
#include "command.h"
#include "kernel.h"
#include "syscall.h"
#include "net/xmodem.h"
#include "kernel_svc/log_manage.h"


#ifdef TSK_LIBRARY

/*! tsk_set1の起動 */
static void tsk_set1_command(void);

/*! tsk_set2の起動 */
static void tsk_set2_command(void);

/*! tsk_set3の起動 */
static void tsk_set3_command(void);

#endif


/*! リンカのシンボルを参照 */
extern UINT32 _logbuffer_start;


/*!
 * echoコマンド
 * buf[] : 標準出力するバッファ
 */
void echo_command(char buf[])
{
	puts(buf + 5);
  puts("\n");
}


/*!
 * helpコマンド
 * *buf : 標準出力するバッファポインタ
 */
void help_command(char *buf)
{
	/* helpメッセージ */
	if (*buf == '\0') {
    puts("echo    - out text serial line.\n");
    puts("sendlog - send log file over serial line(xmodem mode)\n");
    puts("run     - run task sets.\n");
  }
	/* echo helpメッセージ */
  else if (!strncmp(buf, " echo", 5)) {
		puts("echo - out text serial line\n\n");
		puts("Usage:\n");
		puts("echo [arg...]\n");
		puts("  out [arg...] serial line\n");
  }
	/* sendlog helpメッセージ */
  else if (!strncmp(buf, " sendlog", 8)) {
		puts("sendlog - send log file over serial line(xmodem mode)\n");
  }
#ifdef TSK_LIBRARY
	/* run helpメッセージ */
  else if (!strncmp(buf, " run", 4)) {
    /* run help */
		puts("run - run task sets.\n\n");
		puts("Usage:\n");
		puts("run <tsk_set>\n");
		puts("  start the <tsk_set> that is specified in the argument\n");
  }
	/* tsk_set3 helpメッセージ */
	else if (!strncmp(buf, " tsk_set3", 9)) {
		puts("tsk_set3 - This sample task sets the \"TASK SYNCHRONIZATION SYSTEMCALLS\"\n\n");
		puts("Using system call:\n");
		puts("  acre_tsk(), sta_tsk(), slp_tsk(), wup_tsk(), rel_wai()\n");
	}
	/* tsk_set2 helpメッセージ */
	else if (!strncmp(buf, " tsk_set2", 9)) {
		puts("tsk_set2 - This sample task sets the \"TASK MANAGE SYSTEMCALLS\"\n\n");
		puts("Using system call:\n");
		puts("  run_tsk(), get_pri(), chg_pri()\n");
	}
	/* tsk_set1 helpメッセージ */
	else if (!strncmp(buf, " tsk_set1", 9)) {
		puts("tsk_set1 - This sample task sets the \"TASK MANAGE SYSTEMCALLS\"\n\n");
		puts("Using system call:\n");
		puts("  acre_tsk(), sta_tsk(), del_tsk(), ext_tsk(), exd_tsk(), ter_tsk()\n");
	}
	/* tsk_set 一覧 */
  else if (!strncmp(buf, " tsk_set", 8)) {
		puts("tsk_set1 - This sample task sets the \"TASK MANAGE SYSTEMCALLS\"\n");
		puts("tsk_set2 - This sample task sets the \"TASK MANAGE SYSTEMCALLS\"\n");
		puts("tsk_set3 - This sample task sets the \"TASK SYNCHRONIZATION SYSTEMCALLS\"\n");
  }
#endif
	/* helpに存在しないコマンド */
	else {
		puts("help unknown.\n");
	}
}


/*! sendlogコマンド */
void sendlog_command(void)
{
	UINT32 *logbuf;
	logbuf = (UINT32 *)&_logbuffer_start;

	/* ログをxmodemで送信して正常の場合 */
	if (send_xmodem((UINT8 *)logbuf, *logbuf + 4)) { /* +4はログ管理機構の先頭番地(サイズ格納位置分) */
		puts("log to xmodem OK.\n");
	}
	/* エラーの場合 */
	else {
		puts("log to xmodem error.\n");
	}
}


#ifdef TSK_LIBRARY

/*!
 * runコマンド
 * *buf : 起動するタスクセット名が格納されたバッファ
 */
void run_command(char *buf)
{
  if (!strncmp(buf, " tsk_set3", 10)) {
		tsk_set3_command();
	}
  else if (!strncmp(buf, " tsk_set2", 10)) {
		tsk_set2_command();
	}
  else if (!strncmp(buf, " tsk_set1", 10)) {
		tsk_set1_command();
	}
 	else {
		puts("tsk_set unknown.\n");
	}
}


/*! tsk_set1の起動 */
static void tsk_set1_command(void)
{
	SYSCALL_PARAMCB tsk1_param;

	tsk1_param.un.acre_tsk.func = sample_tsk1_main;
	tsk1_param.un.acre_tsk.name = "sample_tsk1";
	tsk1_param.un.acre_tsk.priority = 5;
	tsk1_param.un.acre_tsk.stacksize = 0x100;
	tsk1_param.un.acre_tsk.argc = 0;
	tsk1_param.un.acre_tsk.argv = NULL;

	sample_tsk3_id = mz_iacre_tsk(&tsk1_param);
	mz_ista_tsk(sample_tsk3_id);
}


/*! tsk_set2の起動 */
static void tsk_set2_command(void)
{
	SYSCALL_PARAMCB tsk1_param;

	tsk1_param.un.acre_tsk.func = sample_tsk4_main;
	tsk1_param.un.acre_tsk.name = "sample_tsk4";
	tsk1_param.un.acre_tsk.priority = 3;
	tsk1_param.un.acre_tsk.stacksize = 0x100;
	tsk1_param.un.acre_tsk.argc = 0;
	tsk1_param.un.acre_tsk.argv = NULL;

	sample_tsk4_id = mz_iacre_tsk(&tsk1_param);
	mz_ista_tsk(sample_tsk4_id);
}


/*! tsk_set3の起動 */
static void tsk_set3_command(void)
{
	SYSCALL_PARAMCB tsk1_param;

	tsk1_param.un.acre_tsk.func = sample_tsk6_main;
	tsk1_param.un.acre_tsk.name = "sample_tsk6";
	tsk1_param.un.acre_tsk.priority = 5;
	tsk1_param.un.acre_tsk.stacksize = 0x100;
	tsk1_param.un.acre_tsk.argc = 0;
	tsk1_param.un.acre_tsk.argv = NULL;

	sample_tsk6_id = mz_iacre_tsk(&tsk1_param);
	mz_ista_tsk(sample_tsk6_id);
}

#endif
