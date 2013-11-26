#ifndef _MMC_H_INCLUDED_
#define _MMC_H_INCLUDED_

#include "part.h"
#include "target/driver/io.h"

/*
 * uboot
 * arch/arm/include/asm/arch-omap3/mmc_host_def.h
 */

/* T2 Register definitions */
#define T2_BASE     0x48002000

typedef struct t2 {
  unsigned char res1[0x274];
  unsigned int devconf0;    /* 0x274 */
  unsigned char res2[0x2A8];
  unsigned int pbias_lite;  /* 0x520 */
} t2_t;

#define MMCSDIO1ADPCLKISEL    (1 << 24)

#define PBIASLITEPWRDNZ0    (1 << 1)
#define PBIASSPEEDCTRL0     (1 << 2)
#define PBIASLITEPWRDNZ1    (1 << 9)

/*
 * OMAP HSMMC register definitions
 */
#define OMAP_HSMMC_BASE   0x4809C000

typedef struct hsmmc {
  unsigned char res1[0x10];
  unsigned int sysconfig;   /* 0x10 */
  unsigned int sysstatus;   /* 0x14 */
  unsigned char res2[0x14];
  unsigned int con;   /* 0x2C */
  unsigned char res3[0xD4];
  unsigned int blk;   /* 0x104 */
  unsigned int arg;   /* 0x108 */
  unsigned int cmd;   /* 0x10C */
  unsigned int rsp10;   /* 0x110 */
  unsigned int rsp32;   /* 0x114 */
  unsigned int rsp54;   /* 0x118 */
  unsigned int rsp76;   /* 0x11C */
  unsigned int data;    /* 0x120 */
  unsigned int pstate;    /* 0x124 */
  unsigned int hctl;    /* 0x128 */
  unsigned int sysctl;    /* 0x12C */
  unsigned int stat;    /* 0x130 */
  unsigned int ie;    /* 0x134 */
  unsigned char res4[0x8];
  unsigned int capa;    /* 0x140 */
} hsmmc_t;

/*
 * OMAP HS MMC Bit definitions
 */
#define MMC_SOFTRESET     (0x1 << 1)
#define RESETDONE     (0x1 << 0)
#define NOOPENDRAIN     (0x0 << 0)
#define OPENDRAIN     (0x1 << 0)
#define OD        (0x1 << 0)
#define INIT_NOINIT     (0x0 << 1)
#define INIT_INITSTREAM     (0x1 << 1)
#define HR_NOHOSTRESP     (0x0 << 2)
#define STR_BLOCK       (0x0 << 3)
#define MODE_FUNC     (0x0 << 4)
#define DW8_1_4BITMODE      (0x0 << 5)
#define MIT_CTO       (0x0 << 6)
#define CDP_ACTIVEHIGH      (0x0 << 7)
#define WPP_ACTIVEHIGH      (0x0 << 8)
#define RESERVED_MASK     (0x3 << 9)
#define CTPL_MMC_SD       (0x0 << 11)
#define BLEN_512BYTESLEN    (0x200 << 0)
#define NBLK_STPCNT     (0x0 << 16)
#define DE_DISABLE      (0x0 << 0)
#define BCE_DISABLE     (0x0 << 1)
#define ACEN_DISABLE      (0x0 << 2)
#define DDIR_OFFSET     (4)
#define DDIR_MASK     (0x1 << 4)
#define DDIR_WRITE      (0x0 << 4)
#define DDIR_READ     (0x1 << 4)
#define MSBS_SGLEBLK      (0x0 << 5)
#define RSP_TYPE_OFFSET     (16)
#define RSP_TYPE_MASK     (0x3 << 16)
#define RSP_TYPE_NORSP      (0x0 << 16)
#define RSP_TYPE_LGHT136    (0x1 << 16)
#define RSP_TYPE_LGHT48     (0x2 << 16)
#define RSP_TYPE_LGHT48B    (0x3 << 16)
#define CCCE_NOCHECK      (0x0 << 19)
#define CCCE_CHECK      (0x1 << 19)
#define CICE_NOCHECK      (0x0 << 20)
#define CICE_CHECK      (0x1 << 20)
#define DP_OFFSET     (21)
#define DP_MASK       (0x1 << 21)
#define DP_NO_DATA      (0x0 << 21)
#define DP_DATA       (0x1 << 21)
#define CMD_TYPE_NORMAL     (0x0 << 22)
#define INDEX_OFFSET      (24)
#define INDEX_MASK      (0x3f << 24)
#define INDEX(i)      (i << 24)
#define DATI_MASK     (0x1 << 1)
#define DATI_CMDDIS     (0x1 << 1)
#define DTW_1_BITMODE     (0x0 << 1)
#define DTW_4_BITMODE     (0x1 << 1)
#define SDBP_PWROFF     (0x0 << 8)
#define SDBP_PWRON      (0x1 << 8)
#define SDVS_1V8      (0x5 << 9)
#define SDVS_3V0      (0x6 << 9)
#define ICE_MASK      (0x1 << 0)
#define ICE_STOP      (0x0 << 0)
#define ICS_MASK      (0x1 << 1)
#define ICS_NOTREADY      (0x0 << 1)
#define ICE_OSCILLATE     (0x1 << 0)
#define CEN_MASK      (0x1 << 2)
#define CEN_DISABLE     (0x0 << 2)
#define CEN_ENABLE      (0x1 << 2)
#define CLKD_OFFSET     (6)
#define CLKD_MASK     (0x3FF << 6)
#define DTO_MASK      (0xF << 16)
#define DTO_15THDTO     (0xE << 16)
#define SOFTRESETALL      (0x1 << 24)
#define CC_MASK       (0x1 << 0)
#define TC_MASK       (0x1 << 1)
#define BWR_MASK      (0x1 << 4)
#define BRR_MASK      (0x1 << 5)
#define ERRI_MASK     (0x1 << 15)
#define IE_CC       (0x01 << 0)
#define IE_TC       (0x01 << 1)
#define IE_BWR        (0x01 << 4)
#define IE_BRR        (0x01 << 5)
#define IE_CTO        (0x01 << 16)
#define IE_CCRC       (0x01 << 17)
#define IE_CEB        (0x01 << 18)
#define IE_CIE        (0x01 << 19)
#define IE_DTO        (0x01 << 20)
#define IE_DCRC       (0x01 << 21)
#define IE_DEB        (0x01 << 22)
#define IE_CERR       (0x01 << 28)
#define IE_BADA       (0x01 << 29)

#define VS30_3V0SUP     (1 << 25)
#define VS18_1V8SUP     (1 << 26)

/* Driver definitions */
#define MMCSD_SECTOR_SIZE   512
#define MMC_CARD      0
#define SD_CARD       1
#define BYTE_MODE     0
#define SECTOR_MODE     1
#define CLK_INITSEQ     0
#define CLK_400KHZ      1
#define CLK_MISC      2

typedef struct {
  unsigned int card_type;
  unsigned int version;
  unsigned int mode;
  unsigned int size;
  unsigned int RCA;
} mmc_card_data;

#define mmc_reg_out(addr, mask, val)                            \
  writel((readl(addr) & (~(mask))) | ((val) & (mask)), (addr))


/*
 * uboot
 * arch/arm/include/asm/arch-omap3/mmc.h
 */
/* Responses */
#define RSP_TYPE_NONE (RSP_TYPE_NORSP   | CCCE_NOCHECK | CICE_NOCHECK)
#define RSP_TYPE_R1 (RSP_TYPE_LGHT48  | CCCE_CHECK   | CICE_CHECK)
#define RSP_TYPE_R1B  (RSP_TYPE_LGHT48B | CCCE_CHECK   | CICE_CHECK)
#define RSP_TYPE_R2 (RSP_TYPE_LGHT136 | CCCE_CHECK   | CICE_NOCHECK)
#define RSP_TYPE_R3 (RSP_TYPE_LGHT48  | CCCE_NOCHECK | CICE_NOCHECK)
#define RSP_TYPE_R4 (RSP_TYPE_LGHT48  | CCCE_NOCHECK | CICE_NOCHECK)
#define RSP_TYPE_R5 (RSP_TYPE_LGHT48  | CCCE_CHECK   | CICE_CHECK)
#define RSP_TYPE_R6 (RSP_TYPE_LGHT48  | CCCE_CHECK   | CICE_CHECK)
#define RSP_TYPE_R7 (RSP_TYPE_LGHT48  | CCCE_CHECK   | CICE_CHECK)

/* All supported commands */
#define MMC_CMD0  (INDEX(0)  | RSP_TYPE_NONE | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD1  (INDEX(1)  | RSP_TYPE_R3   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD2  (INDEX(2)  | RSP_TYPE_R2   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD3  (INDEX(3)  | RSP_TYPE_R1   | DP_NO_DATA | DDIR_WRITE)
#define MMC_SDCMD3  (INDEX(3)  | RSP_TYPE_R6   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD4  (INDEX(4)  | RSP_TYPE_NONE | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD6  (INDEX(6)  | RSP_TYPE_R1B  | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD7_SELECT (INDEX(7)  | RSP_TYPE_R1B  | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD7_DESELECT (INDEX(7)| RSP_TYPE_NONE | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD8  (INDEX(8)  | RSP_TYPE_R1   | DP_DATA    | DDIR_READ)
#define MMC_SDCMD8  (INDEX(8)  | RSP_TYPE_R7   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD9  (INDEX(9)  | RSP_TYPE_R2   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD12 (INDEX(12) | RSP_TYPE_R1B  | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD13 (INDEX(13) | RSP_TYPE_R1   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD15 (INDEX(15) | RSP_TYPE_NONE | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD16 (INDEX(16) | RSP_TYPE_R1   | DP_NO_DATA | DDIR_WRITE)
#define MMC_CMD17 (INDEX(17) | RSP_TYPE_R1   | DP_DATA    | DDIR_READ)
#define MMC_CMD24 (INDEX(24) | RSP_TYPE_R1   | DP_DATA    | DDIR_WRITE)
#define MMC_ACMD6 (INDEX(6)  | RSP_TYPE_R1   | DP_NO_DATA | DDIR_WRITE)
#define MMC_ACMD41  (INDEX(41) | RSP_TYPE_R3   | DP_NO_DATA | DDIR_WRITE)
#define MMC_ACMD51  (INDEX(51) | RSP_TYPE_R1   | DP_DATA    | DDIR_READ)
#define MMC_CMD55 (INDEX(55) | RSP_TYPE_R1   | DP_NO_DATA | DDIR_WRITE)

#define MMC_AC_CMD_RCA_MASK (unsigned int)(0xFFFF << 16)
#define MMC_BC_CMD_DSR_MASK (unsigned int)(0xFFFF << 16)
#define MMC_DSR_DEFAULT   0x0404
#define SD_CMD8_CHECK_PATTERN 0xAA
#define SD_CMD8_2_7_3_6_V_RANGE (0x01 << 8)

/* Clock Configurations and Macros */

#define MMC_CLOCK_REFERENCE   96
#define MMC_RELATIVE_CARD_ADDRESS 0x1234
#define MMC_INIT_SEQ_CLK    (MMC_CLOCK_REFERENCE * 1000 / 80)
#define MMC_400kHz_CLK      (MMC_CLOCK_REFERENCE * 1000 / 400)
#define CLKDR(r, f, u)      ((((r)*100) / ((f)*(u))) + 1)
#define CLKD(f, u)      (CLKDR(MMC_CLOCK_REFERENCE, f, u))

#define MMC_OCR_REG_ACCESS_MODE_MASK  (0x3 << 29)
#define MMC_OCR_REG_ACCESS_MODE_BYTE  (0x0 << 29)
#define MMC_OCR_REG_ACCESS_MODE_SECTOR  (0x2 << 29)

#define MMC_OCR_REG_HOST_CAPACITY_SUPPORT_MASK    (0x1 << 30)
#define MMC_OCR_REG_HOST_CAPACITY_SUPPORT_BYTE    (0x0 << 30)
#define MMC_OCR_REG_HOST_CAPACITY_SUPPORT_SECTOR  (0x1 << 30)

#define MMC_SD2_CSD_C_SIZE_LSB_MASK 0xFFFF
#define MMC_SD2_CSD_C_SIZE_MSB_MASK 0x003F
#define MMC_SD2_CSD_C_SIZE_MSB_OFFSET 16
#define MMC_CSD_C_SIZE_LSB_MASK   0x0003
#define MMC_CSD_C_SIZE_MSB_MASK   0x03FF
#define MMC_CSD_C_SIZE_MSB_OFFSET 2

#define MMC_CSD_TRAN_SPEED_UNIT_MASK  (0x07 << 0)
#define MMC_CSD_TRAN_SPEED_FACTOR_MASK  (0x0F << 3)
#define MMC_CSD_TRAN_SPEED_UNIT_100MHZ  (0x3 << 0)
#define MMC_CSD_TRAN_SPEED_FACTOR_1_0 (0x01 << 3)
#define MMC_CSD_TRAN_SPEED_FACTOR_8_0 (0x0F << 3)

typedef struct {
  unsigned not_used:1;
  unsigned crc:7;
  unsigned ecc:2;
  unsigned file_format:2;
  unsigned tmp_write_protect:1;
  unsigned perm_write_protect:1;
  unsigned copy:1;
  unsigned file_format_grp:1;
  unsigned content_prot_app:1;
  unsigned reserved_1:4;
  unsigned write_bl_partial:1;
  unsigned write_bl_len:4;
  unsigned r2w_factor:3;
  unsigned default_ecc:2;
  unsigned wp_grp_enable:1;
  unsigned wp_grp_size:5;
  unsigned erase_grp_mult:5;
  unsigned erase_grp_size:5;
  unsigned c_size_mult:3;
  unsigned vdd_w_curr_max:3;
  unsigned vdd_w_curr_min:3;
  unsigned vdd_r_curr_max:3;
  unsigned vdd_r_curr_min:3;
  unsigned c_size_lsb:2;
  unsigned c_size_msb:10;
  unsigned reserved_2:2;
  unsigned dsr_imp:1;
  unsigned read_blk_misalign:1;
  unsigned write_blk_misalign:1;
  unsigned read_bl_partial:1;
  unsigned read_bl_len:4;
  unsigned ccc:12;
  unsigned tran_speed:8;
  unsigned nsac:8;
  unsigned taac:8;
  unsigned reserved_3:2;
  unsigned spec_vers:4;
  unsigned csd_structure:2;
} mmc_csd_reg_t;

/* csd for sd2.0 */
typedef struct {
  unsigned not_used:1;
  unsigned crc:7;
  unsigned reserved_1:2;
  unsigned file_format:2;
  unsigned tmp_write_protect:1;
  unsigned perm_write_protect:1;
  unsigned copy:1;
  unsigned file_format_grp:1;
  unsigned reserved_2:5;
  unsigned write_bl_partial:1;
  unsigned write_bl_len:4;
  unsigned r2w_factor:3;
  unsigned reserved_3:2;
  unsigned wp_grp_enable:1;
  unsigned wp_grp_size:7;
  unsigned sector_size:7;
  unsigned erase_blk_len:1;
  unsigned reserved_4:1;
  unsigned c_size_lsb:16;
  unsigned c_size_msb:6;
  unsigned reserved_5:6;
  unsigned dsr_imp:1;
  unsigned read_blk_misalign:1;
  unsigned write_blk_misalign:1;
  unsigned read_bl_partial:1;
  unsigned read_bl_len:4;
  unsigned ccc:12;
  unsigned tran_speed:8;
  unsigned nsac:8;
  unsigned taac:8;
  unsigned reserved_6:6;
  unsigned csd_structure:2;
} mmc_sd2_csd_reg_t;

/* extended csd - 512 bytes long */
typedef struct {
  unsigned char reserved_1[181];
  unsigned char erasedmemorycontent;
  unsigned char reserved_2;
  unsigned char buswidthmode;
  unsigned char reserved_3;
  unsigned char highspeedinterfacetiming;
  unsigned char reserved_4;
  unsigned char powerclass;
  unsigned char reserved_5;
  unsigned char commandsetrevision;
  unsigned char reserved_6;
  unsigned char commandset;
  unsigned char extendedcsdrevision;
  unsigned char reserved_7;
  unsigned char csdstructureversion;
  unsigned char reserved_8;
  unsigned char cardtype;
  unsigned char reserved_9[3];
  unsigned char powerclass_52mhz_1_95v;
  unsigned char powerclass_26mhz_1_95v;
  unsigned char powerclass_52mhz_3_6v;
  unsigned char powerclass_26mhz_3_6v;
  unsigned char reserved_10;
  unsigned char minreadperf_4b_26mhz;
  unsigned char minwriteperf_4b_26mhz;
  unsigned char minreadperf_8b_26mhz_4b_52mhz;
  unsigned char minwriteperf_8b_26mhz_4b_52mhz;
  unsigned char minreadperf_8b_52mhz;
  unsigned char minwriteperf_8b_52mhz;
  unsigned char reserved_11;
  unsigned int sectorcount;
  unsigned char reserved_12[288];
  unsigned char supportedcommandsets;
  unsigned char reserved_13[7];
} mmc_extended_csd_reg_t;

/* mmc sd responce */
typedef struct {
  unsigned int ocr;
} mmc_resp_r3;

typedef struct {
  unsigned short cardstatus;
  unsigned short newpublishedrca;
} mmc_resp_r6;

typedef union {
  unsigned int resp[4];
  mmc_resp_r3 r3;
  mmc_resp_r6 r6;
  mmc_csd_reg_t Card_CSD;
} mmc_resp_t;

extern mmc_card_data mmc_dev;

unsigned char mmc_lowlevel_init(void);
unsigned char mmc_send_command(unsigned int cmd, unsigned int arg,
                               unsigned int *response);
unsigned char mmc_setup_clock(unsigned int iclk, unsigned short clkd);
unsigned char mmc_set_opendrain(unsigned char state);
unsigned char mmc_read_data(unsigned int *output_buf);


/*
 * uboot
 * include/part.h
 */
block_dev_desc_t *mmc_get_dev(int dev);

/*
 * uboot
 * 
 */
int mmc_legacy_init(int verbose);

#endif
