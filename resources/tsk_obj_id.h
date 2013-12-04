#ifndef _TSK_OBJ_ID_H_INCLUDED_
#define _TSK_OBJ_ID_H_INCLUDED_

/* include/kernel */
#include "defines.h"


#ifdef TSK_LIBRARY
/*! ユーザタスク資源 */
int sample_tsk1_main(int argc, char *argv[]);
int sample_tsk2_main(int argc, char *argv[]);
int sample_tsk3_main(int argc, char *argv[]);
int sample_tsk4_main(int argc, char *argv[]);
int sample_tsk5_main(int argc, char *argv[]);
int sample_tsk6_main(int argc, char *argv[]);
int sample_tsk7_main(int argc, char *argv[]);
int sample_tsk8_main(int argc, char *argv[]);
#else
#define SAMPLE_TSK1_ENTRY_ADDR ((TSK_FUNC)0x90010000)
#define SAMPLE_TSK2_ENTRY_ADDR ((TSK_FUNC)0x90020000)
#define SAMPLE_TSK3_ENTRY_ADDR ((TSK_FUNC)0x90030000)
#define SAMPLE_TSK4_ENTRY_ADDR ((TSK_FUNC)0x90040000)
#define SAMPLE_TSK5_ENTRY_ADDR ((TSK_FUNC)0x90050000)
#define SAMPLE_TSK6_ENTRY_ADDR ((TSK_FUNC)0x90060000)
#define SAMPLE_TSK7_ENTRY_ADDR ((TSK_FUNC)0x90070000)
#endif

extern ER_ID sample_tsk1_id;
extern ER_ID sample_tsk2_id;
extern ER_ID sample_tsk3_id;
extern ER_ID sample_tsk4_id;
extern ER_ID sample_tsk5_id;
extern ER_ID sample_tsk6_id;
extern ER_ID sample_tsk7_id;
extern ER_ID sample_tsk8_id;

#endif
