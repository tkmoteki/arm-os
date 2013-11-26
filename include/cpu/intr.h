/* IRQ周り */


#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_


/* 以下の割込み番号は，割込みハンドラから分岐する関数ポインタ配列のインデックスとなる */

/* 0~95は外部割込みとして使用 */
typedef enum {
  INTERRUPT_TYPE_EMUINT =             0,      /*! MPU emulation */
  INTERRUPT_TYPE_COMMTX,                  /*! MPU emulation */
  INTERRUPT_TYPE_COMMRX,                  /*! MPU emulation */
  INTERRUPT_TYPE_BENCH,                 /*! MPU emulation */
  INTERRUPT_TYPE_MCBSP2_ST_IRQ,         /*! Sidetone MCBSP2 overflow */
  INTERRUPT_TYPE_MCBSP3_ST_IRQ,         /*! Sidetone MCBSP3 overflow */
  INTERRUPT_TYPE_RESERVED1,             /* 予約(使用できない) */
  INTERRUPT_TYPE_SYS_NIRQ,                /*! Eternal source(active low) */
  INTERRUPT_TYPE_RESERVED2,             /* 予約(使用できない) */
  INTERRUPT_TYPE_SMX_DBG_IRQ,           /*! L3 interconnect error for debug */
  INTERRUPT_TYPE_SMX_APP_IRQ,         /*! L3 interconnect error for debug */
  INTERRUPT_TYPE_PRCM_MPU_IRQ,          /*! PRCM module IRQ */
  INTERRUPT_TYPE_SDMA_IRQ_0,            /*! System DMA request 0 */
  INTERRUPT_TYPE_SDMA_IRQ_1,            /*! System DMA request 0 */
  INTERRUPT_TYPE_SDMA_IRQ_2,            /*! System DMA request 0 */
  INTERRUPT_TYPE_SDMA_IRQ_3,            /*! System DMA request 0 */
  INTERRUPT_TYPE_MCBSP1_IRQ,            /*! MCBSP module 1 IRQ */ 
  INTERRUPT_TYPE_MCBSP2_IRQ,            /*! MCBSP module 2 IRQ */
  INTERRUPT_TYPE_SR1_IRQ,             /*! SmartReflex 1 */
  INTERRUPT_TYPE_SR2_IRQ,             /*! SmartReflex 2 */
  INTERRUPT_TYPE_GPMC_IRQ,              /*! General-purpose memory controller module */
  INTERRUPT_TYPE_SGX_IRQ,             /*! 2D/3D graphics module */
  INTERRUPT_TYPE_MCBSP3_IRQ,            /*! MCBSP module 3 */
  INTERRUPT_TYPE_MCBSP4_IRQ,            /*! MCBSP module 4 */
  INTERRUPT_TYPE_CAM_IRQ0,              /*! Camera interface request */
  INTERRUPT_TYPE_DSS_IRQ,             /*! Display interface request 0 */
  INTERRUPT_TYPE_MAIL_U0_MPU_IRQ,     /*! Mailbox user 0 request */
  INTERRUPT_TYPE_MCBSP5_IRQ,            /*! MCBSP module 5 */
  INTERRUPT_TYPE_IVA2_MMU_IRQ,          /*! IVA MMU */
  INTERRUPT_TYPE_GPIO1_MPU_IRQ,       /*! GPIO module 1 */
  INTERRUPT_TYPE_GPIO2_MPU_IRQ,       /*! GPIO module 2 */
  INTERRUPT_TYPE_GPIO3_MPU_IRQ,       /*! GPIO module 3 */    
  INTERRUPT_TYPE_GPIO4_MPU_IRQ,       /*! GPIO module 4 */
  INTERRUPT_TYPE_GPIO5_MPU_IRQ,       /*! GPIO module 5 */
  INTERRUPT_TYPE_GPIO6_MPU_IRQ,       /*! GPIO module 6 */
  INTERRUPT_TYPE_RESERVED3,             /* 予約(使用できない) */
  INTERRUPT_TYPE_WDT3_IRQ,              /*! Watchdog timer module 3 overflow */
  INTERRUPT_TYPE_GPT1_IRQ,              /*! General-purpose timer module 1 */
  INTERRUPT_TYPE_GPT2_IRQ,              /*! General-purpose timer module 2 */
  INTERRUPT_TYPE_GPT3_IRQ,              /*! General-purpose timer module 3 */
  INTERRUPT_TYPE_GPT4_IRQ,              /*! General-purpose timer module 4 */
  INTERRUPT_TYPE_GPT5_IRQ,              /*! General-purpose timer module 5 */
  INTERRUPT_TYPE_GPT6_IRQ,              /*! General-purpose timer module 6 */
  INTERRUPT_TYPE_GPT7_IRQ,              /*! General-purpose timer module 7 */
  INTERRUPT_TYPE_GPT8_IRQ,              /*! General-purpose timer module 8 */
  INTERRUPT_TYPE_GPT9_IRQ,              /*! General-purpose timer module 9 */
  INTERRUPT_TYPE_GPT10_IRQ,           /*! General-purpose timer module 10 */
  INTERRUPT_TYPE_GPT11_IRQ,           /*! General-purpose timer module 11 */
  INTERRUPT_TYPE_SP14_IRQ,              /*! MCSPI module 4 */
  INTERRUPT_TYPE_RESERVED4,             /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED5,             /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED6,             /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED7,             /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED8,           /* 予約(使用できない) */
  INTERRUPT_TYPE_MCBSP4_IRQ_TX,       /*! MCBSP module 4 transmit */ 
  INTERRUPT_TYPE_MCBSP4_IRQ_RX,       /*! MCBSP module 4 receive */
  INTERRUPT_TYPE_I2C1_IRQ,              /*! I2C module 1 */
  INTERRUPT_TYPE_I2C2_IRQ,              /*! I2C module 2 */
  INTERRUPT_TYPE_HDQ_IRQ,             /*! HDQ/1-Wire */
  INTERRUPT_TYPE_MCBSP1_IRQ_TX,           /*! MCBSP module 1 transmit */
  INTERRUPT_TYPE_MCBSP1_IRQ_RX,           /*! MCBSP module 1 receive */
  INTERRUPT_TYPE_I2C3_IRQ,              /*! I2C module 2 */
  INTERRUPT_TYPE_MCBSP2_IRQ_TX,       /*! MCBSP module 2 transmit */
  INTERRUPT_TYPE_MCBSP2_IRQ_RX,       /*! MCBSP module 2 transmit */
  INTERRUPT_TYPE_RESERVED9,             /* 予約(使用できない) */
  INTERRUPT_TYPE_SP1_IRQ,             /*! MCSPI module 1 */
  INTERRUPT_TYPE_SP2_IRQ,             /*! MCSPI module 2 */
  INTERRUPT_TYPE_RESERVED10,              /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED11,              /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED12,              /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED13,              /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED14,              /* 予約(使用できない) */
  INTERRUPT_TYPE_UART1_IRQ,           /*! UART module 1 */
  INTERRUPT_TYPE_UART2_IRQ,           /*! UART module 2 */
  INTERRUPT_TYPE_UART3_IRQ,           /*! UART module 3 */
  INTERRUPT_TYPE_PBIAS_IRQ,           /*! Merged interrput for PBIASlite 1 and 2 */
  INTERRUPT_TYPE_OHCI_IRQ,              /*! OHCI controller HSUSB MP Host interrupt */
  INTERRUPT_TYPE_EHCI_IRQ,              /*! EHCI controller HSUSB MP Host interrupt */
  INTERRUPT_TYPE_TLL_IRQ,             /*! HSUSB MP TLL interrupt */
  INTERRUPT_TYPE_RESERVED15,              /* 予約(使用できない) */
  INTERRUPT_TYPE_UART4_IRQ,           /*! UART module 4 */
  INTERRUPT_TYPE_MCBSP5_IRQ_TX,       /*! MCBSP module 5 transmit */
  INTERRUPT_TYPE_MCBSP5_IRQ_RX,       /*! MCBSP module 5 receive */
  INTERRUPT_TYPE_MMC1_IRQ,              /*! MMC/SD module 1 */
  INTERRUPT_TYPE_RESERVED16,              /* 予約(使用できない) */
  INTERRUPT_TYPE_RESERVED17,              /* 予約(使用できない) */
  INTERRUPT_TYPE_MMC2_IRQ,              /*! MMC/SD module 2 */
  INTERRUPT_TYPE_MPU_ICR_IRQ,         /*! MPU ICR interrupt */
  INTERRUPT_TYPE_D2DFRINT,              /*! From 3G coprocessor hardware when used in stacked modem congiguration */
  INTERRUPT_TYPE_MCBSP3_IRQ_TX,       /*! MCBSP module 3 transmit */
  INTERRUPT_TYPE_MCBSP3_IRQ_RX,       /*! MCBSP module 3 receive */
  INTERRUPT_TYPE_SPI3_IRQ,              /*! MCSPI module 3 */
  INTERRUPT_TYPE_HSUSB_MC_NINT,       /*! High-Speed USB OTG controller */
  INTERRUPT_TYPE_HUSB_DMA_NINT,       /*! High-Speed USB OTG DMA controller */
  INTERRUPT_TYPE_MMC3_IRQ,              /*! MMC/SD module 3 */
  INTERRUPT_TYPE_RESERVED18,              /* 予約(使用できない) */
  EXTERNAL_INTERRUPT_NUM,                 /*! 外部割込みの数 */
} INTRPT_TYPE;


#endif
