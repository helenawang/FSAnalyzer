/******************************************************************************
  Copyright:2015,S.I.X. All Rights Reserved.
  File name: test.c
  Description: extract and analyse the information of a disk(or a disk image)
	       such as MBR,VBR and the root direcotry
  Author: S.I.X.()
  Version:4.0
  Date:2015.08.20 14:13(UTC+2)
*******************************************************************************/
#include <stdio.h>
#include "mvd.h"

int main(int argc,char *argv[])
{
	system("clear");
	system("mkdir files");

	sprintf(disk,"%s",argv[1]);
	//use dd command to extract the MBR and save it in a file(without any extra characters and skip the first 446 bytes)
	sprintf(cmd,"sudo dd if='%s' bs=1 skip=446 count=66 2> log | xxd -p -c 66> ./files/MBR.dat",disk);// test:change /dev/%s to /tmp/%s
	system(cmd);

	//read the file and store MBR in the MBR array
	freopen("./files/MBR.dat","r",stdin);
	i=0;
	while(scanf("%c",&MBR[i]) != EOF )
		i++;	

	//check the "aa55" signature
	if(MBR[128] != '5' || MBR[129] != '5' || MBR[130] != 'a' || MBR[131] != 'a'){
		printf("ERROR!\n%s is invalid!Please check the Log file for more information.\n",disk);
		exit(0);
	}
	
	offset=0;
	int partition_type[4]={0,0,0,0};	
	int LBA_address[4]={0,0,0,0};
	int size_in_sectors[4]={0,0,0,0};
	printf("==================MBR==================\n");
	//extract information from specific bytes and print them
	for(NumberOfPartition=0;NumberOfPartition<4;NumberOfPartition++){
		offset=NumberOfPartition*16;
		partition_type[NumberOfPartition] = get_info(MBR,offset+4,offset+4);
		LBA_address[NumberOfPartition] = get_info(MBR,offset+8,offset+11);
		size_in_sectors[NumberOfPartition] = get_info(MBR,offset+12,offset+15);
		if(LBA_address[NumberOfPartition] == 0) break;
		printf("information about partition %d:\n",NumberOfPartition+1);
		printf("partition type: %s\n",type_judge(partition_type[NumberOfPartition]));
		printf("starting LBA address: %d\n", LBA_address[NumberOfPartition]);
		printf("size in sectors: %d\n",size_in_sectors[NumberOfPartition]);
		printf("\n");
	}
	
	char VBR_dat[10];
	int cur_partition;
	printf("==================VBR==================\n");
	for(cur_partition=0;cur_partition<NumberOfPartition;cur_partition++){
		//use dd command to extract the VBR and save it in another file
		// test:change /dev/%s to /tmp/%s
		sprintf(cmd,"sudo dd if='%s' bs=512 skip=%d count=1 2>>log| xxd -p -c 512 > ./files/VBR%d.dat",disk,LBA_address[cur_partition],cur_partition+1);
		system(cmd);
		
		//read the file and store VBR in the VBR array
		sprintf(VBR_dat,"./files/VBR%d.dat",cur_partition+1);
		freopen(VBR_dat,"r",stdin);
		i=0;
		while(scanf("%c",&VBR[i]) != EOF )
			i++;

		//check the "aa55" signature
		if(VBR[1020] != '5' || VBR[1021] != '5' || VBR[1022] != 'a' || VBR[1023] != 'a'){
			printf("ERROR!The program may not support this system format.\n%s is invalid!Please check the Log file for more information.\n",disk);
			exit(0);
		}

		//extract information from specific bytes and print them
		printf("information of VBR about partition %d:\n",cur_partition+1);
		switch(partition_type[cur_partition])
		{
			case 0x01:
			case 0x04:
			case 0x06: 
			case 0x0e: type_FAT(LBA_address[cur_partition],16); break;
			case 0x0c:
			case 0x0b: type_FAT(LBA_address[cur_partition],32); break;
			case 0x05:
			case 0x0f: type_EXTEND(LBA_address[cur_partition]);break;
		}
	printf("\n");
	}
	sprintf(cmd,"sudo dd if='%s' bs=512 skip=%d count=1 2>>log| xxd -p -c 512 > ./files/VBR%d.dat",disk,LBA_address[cur_partition],cur_partition+1);
	printf("\t\tS.I.X. 2015 All Rights Reserved.\n\n");
	return 0;
}
