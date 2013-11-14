#include "kernel/defines.h"
#include "intr_cntrl.h"


/*! 割込みコントローラ(MIR有効化) */
void intc_enable_irq(INTRPT_TYPE irq)
{
  int n, bit;

  n = irq / 32;
  bit = irq % 32;
  REG32_WRITE(INTCPS_MIR_CLEAR(n), (1<<bit));
}


/*! 割込みコントローラ(MIR無効化) */
void intc_disable_irq(INTRPT_TYPE irq)
{
  int n, bit;

  n = irq / 32;
  bit = irq % 32;
  REG32_WRITE(INTCPS_MIR_SET(n), REG32_READ(INTCPS_MIR_SET(n)) & ~(1<<bit));
}


/*! 
 * CPSRの外部割込み(IRQとFIQ)有効化チェック
 * (返却値)0 : IRQ割込み有効
 * (返却値)1 : IRQ割込み無効
 */
int is_exter_intr_enable(void)
{
  int state;

  asm volatile("mrs %0, cpsr\n":"=r"(state));
  
  /* irq割込み有効の場合 */
  if (state & 0x84) {
    return 0;
  }
  /* irq割込み無効の場合 */
  else {
    return 1;
  }
}


/*! CPSRの外部割込み(IRQとFIQ)有効化 */
void enable_exter_intr(void)
{
  asm volatile("mrs r0, cpsr\n\t"
               "bic r0, r0, #0x84\n\t"
               "msr cpsr, r0\n");
}


/*! CPSRの外部割込み(IRQとFIQ)無効化 */
void disable_ext_intr(void)
{
  asm volatile("mrs r0, cpsr\n\t"
               "orr r0, r0, #0x84\n\t"
               "msr cpsr, r0\n");
}


/*! 
 * CPSRのIRQ割込み有効化チェック
 * (返却値)0 : IRQ割込み有効
 * (返却値)1 : IRQ割込み無効
 */
int is_irq_enable(void)
{
  int state;

  asm volatile("mrs %0, cpsr\n":"=r"(state));
  
  /* irq割込み有効の場合 */
  if (state & 0x80) {
    return 0;
  }
  /* irq割込み無効の場合 */
  else {
    return 1;
  }
}


/*! CPSRのIRQ有効化 */
/*! CPSRの7ビット目が0だとirq割込み有効化 */
void enable_irq(void)
{
  asm volatile("mrs r0, cpsr\n\t"
               "bic r0, r0, #0x80\n\t"
               "msr cpsr, r0\n");
}


/*! CPSRのIRQ無効化 */
/*! CPSRの7ビット目に1を立てるとirq割込み無効化 */
void disable_irq(void)
{
  asm volatile("mrs r0, cpsr\n\t"
               "orr r0, r0, #0x80\n\t"
               "msr cpsr, r0\n");
}


/*! 
 * CPSRのFIQ割込み有効化チェック
 * (返却値)0 : FIQ割込み有効
 * (返却値)1 : FIQ割込み無効
 */
int is_fiq_enable(void)
{
  int state;

  asm volatile("mrs %0, cpsr\n":"=r"(state));
  
  /* fiq割込み有効の場合 */
  if (state & 0x40) {
    return 0;
  }
  /* fiq割込み無効の場合 */
  else {
    return 1;
  }
}


/*! CPSRのFIQ有効化 */
/*! CPSRの6ビット目が0だとFIQ割込み有効化 */
void enable_fiq(void)
{
  asm volatile("mrs r0, cpsr\n\t"
               "bic r0, r0, #0x40\n\t"
               "msr cpsr, r0\n");
}


/*! CPSRのFIQ無効化 */
/*! CPSRの6ビット目に1を立てるとFIQ割込み無効化 */
void disable_fiq(void)
{
  asm volatile("mrs r0, cpsr\n\t"
               "orr r0, r0, #0x40\n\t"
               "msr cpsr, r0\n");
}

