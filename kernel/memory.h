#ifndef _MEMORY_H_INCLUDED_
#define _MEMORY_H_INCLUDED_


/*! 動的メモリの初期化 */
void mem_init(void);

/*! 動的メモリの獲得 */
void* get_mpf_isr(int size);

/*! メモリの解放 */
void rel_mpf_isr(void *mem);


#endif
