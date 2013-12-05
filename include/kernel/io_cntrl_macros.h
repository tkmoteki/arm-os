#ifndef _IO_CNTRL_MACROS_H_INCLUDED_
#define _IO_CNTRL_MACROS_H_INCLUDED_


/*! 8bit幅のレジスタ操作マクロ */
#define REG8_READ(adr)    *((volatile unsigned char *)(adr))                  /*! 各レジスタから読み出し */
#define REG8_WRITE(adr, dat)  {*((volatile unsigned char *)(adr)) = (dat);}   /*! 各レジスタから書き出し */

/*! 16bit幅のレジスタ操作マクロ */
#define REG16_READ(adr)   *((volatile unsigned char *)(adr))                  /*! 各レジスタから読み出し */
#define REG16_WRITE(adr, dat) {*((volatile unsigned char *)(adr)) = (dat);}   /*! 各レジスタから書き出し */

/* 32bit幅のレジスタ操作マクロ */
#define REG32_READ(addr)  *((volatile unsigned int *)(addr))                  /*! 各レジスタから読み出し */
#define REG32_WRITE(addr, dat)  {*((volatile unsigned int *)(addr)) = (dat);} /*! 各レジスタから書き出し */

#endif
