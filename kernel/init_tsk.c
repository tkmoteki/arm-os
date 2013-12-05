/* private interface */
#include "multi_timer.h"

/* public interface */
/* include/kernel */
#include "kernel.h"
#include "message.h"
/* include/cpu */
#include "intr_cntrl.h"
/* include/lib/c */
#include "lib.h"
/* include/driver */
#include "uart.h"


extern void uart_handler(void);

/*! initタスク */
int start_threads(int argc, char *argv[])
{
  KERNEL_OUTMSG("init task started.\n");
  intc_enable_irq(INTERRUPT_TYPE_UART3_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT1_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT2_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT3_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT4_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT5_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT6_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT7_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT8_IRQ); /* MIRの有効化 */
  intc_enable_irq(INTERRUPT_TYPE_GPT9_IRQ); /* MIRの有効化 */
  serial_intr_recv_enable(); /* シリアル受信割込み有効化 */
  serial_intr_send_disable();

  mz_def_inh(INTERRUPT_TYPE_UART3_IRQ, uart_handler);

  enable_irq();

  while (1) {
    ;
  }

  return 0;
}

