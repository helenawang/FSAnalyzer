//============================================================================
// Name        : analyze.c
// Author      : Helena Wang
// Version     : 1.0.0
// Copyright   : Apache v2
// Description : main program to analyze a specific file system(support FAT now)
//============================================================================
#include "includes.h"

unsigned i=0, j=0;
Image image;
FAT_VBR_35 vbr[4];
Directory dir[10];
Dir_entry dir_entry;
uint16_t fat16[512];
FILE *fp, *fp2;
char cmd[100];

int analyze();
int analyzeMBR();
void analyzePartition();

int main(int argc,char *argv[]){
	if(argc==1){
		printf("Error! No input image.\n");
		return 1;
	}
	if((fp=fopen(*++argv,"rb"))==NULL){ //open error.
		printf("fopen: can't open %s, no such image.\n",argv[0]);
		return 1;
	}
	strcpy(image.name,argv[0]);
	strcpy(image.type,"DOS partition");  //only support DOS partition system(MBR disk) now
	analyze();
	fclose(fp);
	return 0;
}

int analyze(){
//analyze this image.

	sprintf(cmd,"sudo dd if='%s' of=../tmp/MBR.dat count=1 2>../tmp/log",image.name); //make a raw copy of MBR
	system(cmd);

	printf("**************information of this image**************\n");
	printf("image name: %s\n", image.name);
	printf("image partition system: %s\n", image.type);
	
	//analyze the MBR of this image
	if(analyzeMBR()) return 1;
	printf("cannot analyze this image!\n");
	return 0;
}

int analyzeMBR(){
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
	
	//analyze all partitions
	analyzePartition();
	return 1;
}
void analyzePartition(){
//parse each partition
	for(i=0;i<4;i++){
		if(image.mbr.par_entry[i].type==0) continue; //empty volume
		vbr[i].offset = image.mbr.offset + image.mbr.par_entry[i].start_addr;
		sprintf(cmd,"sudo dd if='%s' of=../tmp/VBR%d.dat skip=%d count=1 2>>../tmp/log",image.name, i, vbr[i].offset);
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
		if(vbr[i].FAT_size == 0){
			vbr[i].FAT_size = get_32(fp, 36L); //FAT32
			dir[i].offset = vbr[i].offset + vbr[i].resArea_size + vbr[i].FAT_size * vbr[i].FAT_num + (get_32(fp, 44L) - 2) * vbr[i].spc;
		}else 
		
		dir[i].offset = vbr[i].offset + vbr[i].resArea_size + vbr[i].FAT_size * vbr[i].FAT_num;
		get_chars(fp,3L,10L,vbr[i].OEM_name);
		printf("***********************partition %d *******************\n", i);
		printf("OEM name: %s\n", vbr[i].OEM_name);
		printf("bytes per sector: %d\n", vbr[i].bps);
		printf("sectors per cluster: %d\n", vbr[i].spc);
		printf("size of reserved area in sectors: %d\n", vbr[i].resArea_size);
		printf("number of FATs: %d\n", vbr[i].FAT_num);
		printf("size of each FAT in sectors: %d\n", vbr[i].FAT_size);

		printf("cluster of root Directory: %d\n", dir[i].offset);

		sprintf(cmd, "sudo dd if=%s of=../tmp/dir%d.dat skip=%d count=1 2>>../tmp/log",image.name, i, dir[i].offset);
		system(cmd); //make a copy of root directory

		sprintf(cmd,"sudo dd if=%s of=../tmp/fat.dat skip=%d count=%d 2>>../tmp/log",image.name, vbr[i].offset+vbr[i].resArea_size,vbr[i].FAT_num);
		system(cmd); //make a copy of fat
		fseek(fp, (vbr[i].offset+vbr[i].resArea_size)*512L, SEEK_SET);
		for(j=0;j<256;j++){ //read fat in fat16 array
			fat16[j]=get_16(fp,(long)(j*2));
//			printf("fat[%d]=%d\n",i,fat16[i]);
		}

		//parse root directory
		fseek(fp,dir[i].offset*512L, SEEK_SET);
		do{
			dir_entry.attr = get_8(fp,11L);
			dir_entry.start_clus = get_16(fp,26L);
			dir_entry.size = get_32(fp,28L);
			get_chars(fp,0L,10L, dir_entry.name);
			//printf("ftell--%ld\n",ftell(fp));
			fseek(fp,32,SEEK_CUR); //turn to the next dir_entry
			if(dir_entry.name[0]==0x00) break; //unallocated entry
			if(dir_entry.name[0]==0xe5) continue; //deleted entry
			if(dir_entry.attr == 0x0f) continue; //long dir_entry name entry

			printf("******************dir_entry***************\n");
			printf("dir_entry name: %s\n",dir_entry.name);
			printf("dir_entry attribute: %s\n", file_type(dir_entry.attr));
			printf("the cluster chain: %d --> ",dir_entry.start_clus);
			uint16_t clus=fat16[dir_entry.start_clus]; //check the fat for the next cluster
			while(clus<0xfff7 && clus!=0){ //read the cluster chain from fat
				printf("%d -->", clus);
				clus = fat16[clus];
			}
			printf("EOF\n");
			printf("dir_entry size: %d\n", dir_entry.size);
			
		}while(dir_entry.name[0]!=0x00);
			
	}
}
