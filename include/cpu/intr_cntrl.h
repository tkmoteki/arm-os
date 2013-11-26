/* MIR周り */


#ifndef _INTR_CNTRL_H_INCLUDED_
#define _INTR_CNTRL_H_INCLUDED_

#include "intr.h"


#define INTC_DEFAULT_BASE     0x48200000
#define INTCPS_SIR_IRQ        (INTC_DEFAULT_BASE + 0x40)
#define INTCPS_CONTROL        (INTC_DEFAULT_BASE + 0x48)

#define INTCPS_MIR_CLEAR(n)   (INTC_DEFAULT_BASE + 0x88 + 0x20 * (n))
#define INTCPS_MIR_SET(n)     (INTC_DEFAULT_BASE + 0x8C + 0x20 * (n))


/*! 割込みコントローラ(MIR有効化) */
void intc_enable_irq(INTRPT_TYPE irq);

/*! 割込みコントローラ(MIR無効化) */
void intc_disable_irq(INTRPT_TYPE irq);

/*! CPSRの外部割込み(IRQとFIQ)有効化チェック */
int is_ext_intr_enable(void);

/*! CPSRの外部割込み(IRQとFIQ)有効化 */
void enable_exter_intr(void);

/*! CPSRの外部割込み(IRQとFIQ)無効化 */
void disable_exter_intr(void);

/*! CPSRのIRQ割込み有効化チェック */
int is_irq_enable(void);

/*! CPSRの有効化 */
void enable_irq(void);

/*! CPSRの無効化 */
void disable_irq(void);

/*! CPSRのFIQ割込み有効化チェック */
int is_fiq_enable(void);

/*! CPSRのFIQ有効化 */
void enable_fiq(void);

/*! CPSRのFIQ無効化 */
void disable_fiq(void);


#endif
