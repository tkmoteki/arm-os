#ifndef _PART_DOS_H
#define _PART_DOS_H


#ifdef CONFIG_ISO_PARTITION
/* Make the buffers bigger if ISO partition support is enabled -- CD-ROMS
   have 2048 byte blocks */
#define DEFAULT_SECTOR_SIZE   2048
#else
#define DEFAULT_SECTOR_SIZE	512
#endif
#define DOS_PART_TBL_OFFSET	0x1be
#define DOS_PART_MAGIC_OFFSET	0x1fe
#define DOS_PBR_FSTYPE_OFFSET	0x36
#define DOS_PBR_MEDIA_TYPE_OFFSET	0x15
#define DOS_MBR	0
#define DOS_PBR	1

typedef struct dos_partition {
	unsigned char boot_ind;		/* 0x80 - active			*/
	unsigned char head;		/* starting head			*/
	unsigned char sector;		/* starting sector			*/
	unsigned char cyl;		/* starting cylinder			*/
	unsigned char sys_ind;		/* What partition type			*/
	unsigned char end_head;		/* end head				*/
	unsigned char end_sector;	/* end sector				*/
	unsigned char end_cyl;		/* end cylinder				*/
	unsigned char start4[4];	/* starting sector counting from 0	*/
	unsigned char size4[4];		/* nr of sectors in partition		*/
} dos_partition_t;

#endif	/* _PART_DOS_H */
