#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mvd.h"
#include "get_bytes.h"
#include "type_judge.h"
unsigned i=0, j=0;
Image image;
FAT_VBR_35 vbr[4];
Directory dir[10];
File file;

int main(int argc,char *argv[]){
	FILE* fp;
	char cmd[100];

	if(argc==1){
		printf("Error! No input image.\n");
		return 1;
	}
	if((fp=fopen(*++argv,"rb"))==NULL){ //open error.
		printf("fopen: can't open %s, no such image.\n",argv[0]);
		return 1;
	}

	//anaylize this image.

	strcpy(image.name,argv[0]);
	strcpy(image.type,"DOS partition");  //only support DOS partition system(MBR disk) now

	sprintf(cmd,"sudo dd if='%s' of=MBR.dat count=1 2>log",image.name); //make a raw copy of MBR
	system(cmd);

	printf("**************information of this image**************\n");
	printf("image name: %s\n", image.name);
	printf("image partition system: %s\n", image.type);

		//parse MBR
		image.mbr.bootCode = image.mbr.offset = 0;

		if(get_16(fp,510L) != 0xaa55){
			printf("Error! Invalid MBR(may be corrupted)\n"); //signature
			return 1;
		}

		printf("*************partition table of MBR******************\n");
		printf("bootable  start LBA       size   file system type\n");

		fseek(fp,image.mbr.offset*512 + 446L,SEEK_SET);		//locate to the start of partition table
		
		for(i=0;i<4;i++){		//at most 4 primary partitions
					//the start of each partition entry
			image.mbr.par_entry[i].bootable = get_8(fp,0L);	// 0-0: bootable flag;
			image.mbr.par_entry[i].type=get_8(fp,4L);		// 4-4: file system type
			vbr[i].offset= image.mbr.par_entry[i].start_addr=get_32(fp,8L);//8-11:bytes starting LBA address of each partition
			image.mbr.par_entry[i].size=get_32(fp,12L);		//12-15:bytes size of each partition(in sectors)

			printf("%8d %10d %10d  %s\n", image.mbr.par_entry[i].bootable,
										image.mbr.par_entry[i].start_addr,
										image.mbr.par_entry[i].size,
										fs_type(image.mbr.par_entry[i].type));
			
			fseek(fp,16,SEEK_CUR);// turn to the next entry
		}
		
		//parse each partition
		for(i=0;i<4;i++){
			vbr[i].offset = image.mbr.offset + image.mbr.par_entry[i].start_addr;

			sprintf(cmd,"sudo dd if='%s' of=VBR%d.dat skip=%d count=1 2>>log",image.name, i, vbr[i].offset);
			system(cmd); //make a copy of vbr file

			//parse VBR
			fseek(fp,vbr[i].offset*512L,SEEK_SET); //the start of each VBR

			if(get_16(fp,510L) != 0xaa55){
				printf("Error! Invalid VBR at sector %d (may be corrupted)\n", vbr[i].offset); //signature
				continue;
			}

			vbr[i].bps = get_16(fp,11L);
			vbr[i].spc = get_8(fp,13L);
			vbr[i].resArea_size = get_16(fp,14L);
			vbr[i].FAT_num = get_8(fp,16L);
			vbr[i].FAT_size = get_16(fp,22L);
			get_chars(fp,3L,10,vbr[i].OEM_name);

			printf("***********************partition %d *******************\n", i);
			printf("OEM name: %s\n", vbr[i].OEM_name);
			printf("bytes per sector: %d\n", vbr[i].bps);
			printf("sectors per cluster: %d\n", vbr[i].spc);
			printf("size of reserved area in sectors: %d\n", vbr[i].resArea_size);
			printf("number of FATs: %d\n", vbr[i].FAT_num);
			printf("size of each FAT in sectors: %d\n", vbr[i].FAT_size);

			dir[i].offset = vbr[i].offset + vbr[i].resArea_size + vbr[i].FAT_size * vbr[i].FAT_num;
			printf("cluster of root Directory: %d\n", dir[i].offset);

			sprintf(cmd, "sudo dd if=%s of=dir%d.dat skip=%d count=1 2>>log",image.name, i, dir[i].offset);
			system(cmd); //make a copy of root directory

			//parse root directory
			fseek(fp,dir[i].offset*512L, SEEK_SET);
			for(j=0;j<5; j++){
				file.attr = get_8(fp,11L);
				file.start_clus = get_16(fp,26L);
				file.size = get_32(fp,28L);
				get_chars(fp,0L,10, file.name);

				printf("file name: %s\n",file.name);
				printf("file attribute: %s\n", file_type(file.attr));
				printf("the first cluster: %d\n", file.start_clus);
				printf("file size: %d\n", file.size);

				fseek(fp,32,SEEK_CUR); //turn to the next dir_entry
			};
			
		}

	fclose(fp);
	return 0;
}
