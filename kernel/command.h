#ifndef _COMMAND_H_INCLUDED_
#define _COMMAND_H_INCLUDED_

#include "defines.h"


/*! echoコマンド */
void echo_command(char buf[]);

/*! helpコマンド */
void help_command(char *buf);

/*! sendlogコマンド */
void sendlog_command(void);

#ifdef TSK_LIBRARY
/*! runコマンド */
void run_command(char *buf);
#endif


#endif
