typedef uint32_t LBA;//Logical Block Address, the LBA of a data field represents its offset in the image.

typedef struct File
{
	char name[12];
	uint8_t attr;
	uint16_t start_clus;
	uint32_t size;
}File;

typedef struct Directory
{
	LBA offset;
}Directory;

typedef struct FAT_VBR_35 {// the first 35 bytes of a FAT boot sector
	LBA offset;
	char OEM_name[10];					// 3-10: created by 
	uint16_t bps;					// 11-12: bytes per sector
	uint8_t spc;					// 13-13: sectors per cluster
	uint16_t resArea_size;			// 14-15: size of reserved area(in sectors)
	uint8_t FAT_num;				// 16-16: number of FATs
	uint16_t FAT_size; 				// 22-23: size in sectors of each FAT
}FAT_VBR_35;

typedef struct Par_entry {// each 16-byte entry of a partition table
	LBA offset;
	uint8_t bootable;		// 0-0: bootable flag
	uint8_t  type;		// 4-4: type of file system
	uint32_t start_addr;// 8-11: bytes starting LBA of each partition
	uint32_t size;		// 12-15: bytes size of each partition(in sectors)
}Par_entry;

typedef struct MBR { // the master boot record of a DOS partition system
	LBA offset;
	LBA bootCode;			// 0-445: boot code
	Par_entry par_entry[4];	// 446-510: partition table
	char* signature;		// 511-512: signature(aa55)
}MBR;

typedef struct Image { // the image you want to parse
	char name[30];
	char type[20];
	MBR mbr;
}Image;

