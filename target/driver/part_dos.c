#include "part.h"
#include "part_dos.h"
#include "ide.h"
#include "kernel/debug.h"


#if defined(CONFIG_CMD_IDE) ||                  \
  defined(CONFIG_CMD_MG_DISK) ||                \
  defined(CONFIG_CMD_SATA) ||                   \
  defined(CONFIG_CMD_SCSI) ||                   \
  defined(CONFIG_CMD_USB) ||                    \
  defined(CONFIG_MMC) ||                        \
  defined(CONFIG_SYSTEMACE)

/* Convert char[4] in little endian format to the host format integer
 */
static inline int le32_to_int(unsigned char *le32)
{
  return ((le32[3] << 24) +
          (le32[2] << 16) +
          (le32[1] << 8) +
          le32[0]
          );
}


static inline int is_extended(int part_type)
{
  return (part_type == 0x5 ||
          part_type == 0xf ||
          part_type == 0x85);
}


/*  Print a partition that is relative to its Extended partition table
 */
static int get_partition_info_extended (block_dev_desc_t *dev_desc, int ext_part_sector,
                                        int relative, int part_num,
                                        int which_part, disk_partition_t *info)
{
  unsigned char buffer[DEFAULT_SECTOR_SIZE];
  dos_partition_t *pt;
  int i;

  if (dev_desc->block_read (dev_desc->dev, ext_part_sector, 1, (unsigned long *) buffer) != 1) {
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG("** Can't read partition table on ");
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTVLE(dev_desc->dev, 0);
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG(":");
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTVLE(dev_desc->dev, 0);
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG(" **\n");
    return -1;
  }
  if (buffer[DOS_PART_MAGIC_OFFSET] != 0x55 ||
      buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa) {
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG("bad MBR sector signature 0x");
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTVLE(buffer[DOS_PART_MAGIC_OFFSET], 0);
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTVLE(buffer[DOS_PART_MAGIC_OFFSET + 1], 0);
    DEBUG_L1_TARGET_DRIVER_PART_DOS_OUTMSG("\n");
    return -1;
  }

  /* Print all primary/logical partitions */
  pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
  for (i = 0; i < 4; i++, pt++) {
    /*
     * fdisk does not show the extended partitions that
     * are not in the MBR
     */
    if (((pt->boot_ind & ~0x80) == 0) &&
        (pt->sys_ind != 0) &&
        (part_num == which_part) &&
        (is_extended(pt->sys_ind) == 0)) {
      info->blksz = 512;
      info->start = ext_part_sector + le32_to_int (pt->start4);
      info->size  = le32_to_int (pt->size4);
      switch(dev_desc->if_type) {
      case IF_TYPE_IDE:
      case IF_TYPE_SATA:
      case IF_TYPE_ATAPI:
#if 0
        sprintf ((char *)info->name, "hd%c%d",
                 'a' + dev_desc->dev, part_num);
#endif
        break;
      case IF_TYPE_SCSI:
#if 0
        sprintf ((char *)info->name, "sd%c%d",
                 'a' + dev_desc->dev, part_num);
#endif
        break;
      case IF_TYPE_USB:
#if 0
        sprintf ((char *)info->name, "usbd%c%d",
                 'a' + dev_desc->dev, part_num);
#endif
        break;
      case IF_TYPE_DOC:
#if 0
        sprintf ((char *)info->name, "docd%c%d",
                 'a' + dev_desc->dev, part_num);
#endif
        break;
      default:
#if 0
        sprintf ((char *)info->name, "xx%c%d",
                 'a' + dev_desc->dev, part_num);
#endif
        break;
      }
#if 0
      /* sprintf(info->type, "%d, pt->sys_ind); */
      sprintf ((char *)info->type, "U-Boot");
#endif
      return 0;
    }

    /* Reverse engr the fdisk part# assignment rule! */
    if ((ext_part_sector == 0) ||
        (pt->sys_ind != 0 && !is_extended (pt->sys_ind)) ) {
      part_num++;
    }
  }

  /* Follows the extended partitions */
  pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
  for (i = 0; i < 4; i++, pt++) {
    if (is_extended (pt->sys_ind)) {
      int lba_start = le32_to_int (pt->start4) + relative;

      return get_partition_info_extended (dev_desc, lba_start,
                                          ext_part_sector == 0 ? lba_start : relative,
                                          part_num, which_part, info);
    }
  }
  return -1;
}


int get_partition_info_dos (block_dev_desc_t *dev_desc, int part, disk_partition_t * info)
{
  return get_partition_info_extended (dev_desc, 0, 0, 1, part, info);
}


#endif
