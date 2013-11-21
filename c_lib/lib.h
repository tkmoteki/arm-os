#ifndef _LIB_H_INCLUDED_
#define _LIB_H_INCLUDED_

#include "ctype.h"

/* ターゲット非依存部 */

/*! メモリセット */
void *memset(void *b, int c, long len);
/*! メモリコピー */
void *memcpy(void *dst, const void *src, long len);

/*! メモリ内容比較 */
int memcmp(const void *b1, const void *b2, long len);

/*! 文字列の長さ */
int strlen(const char *s);

/*! 文字列のコピー */
char *strcpy(char *dst, const char *src);

/*! 文字列の比較 */
int strcmp(const char *s1, const char *s2);

/*! 文字列の比較(文字数制限付き) */
int strncmp(const char *s1, const char *s2, int len);

/* 数値へ変換 */
int atoi(char str[]);

#if 0
int isdigit(int c);

int islower(int c);

int isupper(int c);

int tolower(int c);

int toupper(int c);
#endif

/* lib/vsprintf.c */
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);

/* 文字列の分解 */
unsigned char *strtok(unsigned char *s1, const unsigned char s2);


/* ターゲット依存部 */

/*! １文字送信 */
void putc(unsigned char c);

/*! 文字列送信 */
int puts(char *str);

/*! 数値の16進表示 */
int putxval(unsigned long value, int column);

/*! １文字受信 */
unsigned char getc(void);

/*! 文字列受信 */
int gets(char *buf);

#endif
