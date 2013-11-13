/* CPSRののマクロを定義 */

#ifndef _CPU_CNTRL_H_INCLUDED_
#define _CPU_CNTRL_H_INCLUDED_


/*
 * ~命令モード定義~
 * 本OSではARM命令のみ使用している．
 */
#define ARM_INSTRUCTION 0xFEFFFFDF 		/* ARM命令モード */
#define THUMB_INSTRUCTION 0x20 				/* Thumb命令モード */
#define JAZELLE	0x1000000 						/* javaバイトコードモード */
#define THUMBEE_INSTRUCTION 0x1000020	/* Thumb命令モード */

/*
 * ~エンディアンモード定義~
 * 本OSではリトルエンディアンとしている
 */
#define LITTLE_ENDIAN FFFFFDFF 				/* リトルエンディアンモード */
#define BIG_ENDIAN 0x200 							/* ビッグエンディアンモード */

/* 割込みマスク定義 */
#define ABT_DISABLE (1 << 8) 					/* 非同期アボート無効 */
#define	IRQ_DISABLE		(1 << 7) 				/* IRQ割込み無効 */
#define	FIQ_DISABLE		(1 << 6) 				/* FIQ割込み無効 */

/* ~CPUモード定義~ */
#define	CPSR_USR_MODE	0x10 						/* ユーザーモード(非特権) */
#define	CPSR_FIQ_MODE	0x11 						/* FIQモード(特権) */
#define CPSR_IRQ_MODE	0x12 						/* IRQモード(特権) */
#define	CPSR_SVC_MODE	0x13 						/* SVCモード(特権) */
#define CPSR_MON_MODE 0x16 						/* モニタモード(特権) */
#define	CPSR_ABT_MODE	0x17 						/* アボートモード(特権) */
#define	CPSR_UND_MODE	0x1b 						/* 未定義モード(特権) */
#define	CPSR_SYS_MODE	0x1f 						/* システムモード(特権) */


#endif
