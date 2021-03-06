#ifndef _PART_H_INCLUDED_
#define _PART_H_INCLUDED_

#include "target/driver/ide.h"

typedef struct block_dev_desc {
  int   if_type;  /* type of the interface */
  int   dev;    /* device number */
  unsigned char part_type;  /* partition type */
  unsigned char target;   /* target SCSI ID */
  unsigned char lun;    /* target LUN */
  unsigned char type;   /* device type */
  unsigned char removable;  /* removable device */
#ifdef CONFIG_LBA48
  unsigned char lba48;    /* device can use 48bit addr (ATA/ATAPI v7) */
#endif
  lbaint_t    lba;    /* number of blocks */
  unsigned long blksz;    /* block size */
  char    vendor [40+1];  /* IDE model, SCSI Vendor */
  char    product[20+1];  /* IDE Serial no, SCSI product */
  char    revision[8+1];  /* firmware revision */
  unsigned long (*block_read)(int dev,
                              unsigned long start,
                              lbaint_t blkcnt,
                              void *buffer);
  unsigned long (*block_write)(int dev,
                               unsigned long start,
                               lbaint_t blkcnt,
                               const void *buffer);
  void    *priv;    /* driver private struct pointer */
}block_dev_desc_t;

/* Interface types: */
#define IF_TYPE_UNKNOWN   0
#define IF_TYPE_IDE   1
#define IF_TYPE_SCSI    2
#define IF_TYPE_ATAPI   3
#define IF_TYPE_USB   4
#define IF_TYPE_DOC   5
#define IF_TYPE_MMC   6
#define IF_TYPE_SD    7
#define IF_TYPE_SATA    8


/* Part types */
#define PART_TYPE_UNKNOWN 0x00
#define PART_TYPE_MAC   0x01
#define PART_TYPE_DOS   0x02
#define PART_TYPE_ISO   0x03
#define PART_TYPE_AMIGA   0x04
#define PART_TYPE_EFI   0x05


/* device types */
#define DEV_TYPE_UNKNOWN  0xff  /* not connected */
#define DEV_TYPE_HARDDISK 0x00  /* harddisk */
#define DEV_TYPE_TAPE   0x01  /* Tape */
#define DEV_TYPE_CDROM    0x05  /* CD-ROM */
#define DEV_TYPE_OPDISK   0x07  /* optical disk */

typedef struct disk_partition {
  unsigned long start;    /* # of first block in partition  */
  unsigned long size;   /* number of blocks in partition  */
  unsigned long blksz;    /* block size in bytes      */
  unsigned char name[32]; /* partition name     */
  unsigned char type[32]; /* string type description    */
} disk_partition_t;

/* Misc _get_dev functions */
block_dev_desc_t* get_dev(char* ifname, int dev);

int get_partition_info (block_dev_desc_t *dev_desc, int part
                        , disk_partition_t *info);
void dev_print(block_dev_desc_t *dev_desc);


#ifdef CONFIG_MAC_PARTITION
/* disk/part_mac.c */
int get_partition_info_mac (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_mac (block_dev_desc_t *dev_desc);
int   test_part_mac (block_dev_desc_t *dev_desc);
#endif

#ifdef CONFIG_DOS_PARTITION
/* disk/part_dos.c */
int get_partition_info_dos (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_dos (block_dev_desc_t *dev_desc);
int   test_part_dos (block_dev_desc_t *dev_desc);
#endif

#ifdef CONFIG_ISO_PARTITION
/* disk/part_iso.c */
int get_partition_info_iso (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_iso (block_dev_desc_t *dev_desc);
int   test_part_iso (block_dev_desc_t *dev_desc);
#endif

#ifdef CONFIG_AMIGA_PARTITION
/* disk/part_amiga.c */
int get_partition_info_amiga (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_amiga (block_dev_desc_t *dev_desc);
int   test_part_amiga (block_dev_desc_t *dev_desc);
#endif

#ifdef CONFIG_EFI_PARTITION
/* disk/part_efi.c */
int get_partition_info_efi (block_dev_desc_t * dev_desc, int part, disk_partition_t *info);
void print_part_efi (block_dev_desc_t *dev_desc);
int   test_part_efi (block_dev_desc_t *dev_desc);
#endif

#endif
