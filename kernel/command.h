#ifndef _COMMAND_H_INCLUDED_
#define _COMMAND_H_INCLUDED_

#include "defines.h"

/*! echoコマンド */
void echo_command(char buf[]);

/*! helpコマンド */
void help_command(char *buf);

/*! sendlogコマンド */
void sendlog_command(void);

/*! recvlogコマンド */
void recvlog_command(void);

/* dumpコマンド */
int dump_command(void);

/*! fatloadコマンド */
void fatload_command(char *buf);

/*! fatload制御 */
int do_fat_fsload(int argc, char *argv[]);

/*! fatlsコマンド */
void fatls_command(char *buf);

/*! fatls制御 */
int do_fat_ls (int argc, char *argv[]);

/*! fatinfoコマンド */
void fatinfo_command(char *buf);

/*! fsinfo制御 */
int do_fat_fsinfo (int argc, char *argv[]);


#ifdef TSK_LIBRARY
/*! runコマンド */
void run_command(char *buf);
#endif


#endif
