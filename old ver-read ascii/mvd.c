
#include "mvd.h"

void mbr(int );

int main(int argc,char *argv[])
{
	system("clear");
	system("mkdir files");

	sprintf(disk,"%s",argv[1]);
	
	mbr(0);
	
	printf("\t\tS.I.X. 2015 All Rights Reserved.\n\n");
	return 0;
}

void vbr()
{
	char VBR_dat[10];
	int cur_partition;
	printf("==================VBR==================\n");
	for(cur_partition=0;cur_partition<par_num;cur_partition++){
		//use dd command to extract the VBR and save it in another file
		// test:change /dev/%s to /tmp/%s
		sprintf(cmd,"sudo dd if='%s' bs=512 skip=%d count=1 2>>log| xxd -p -c 512 > ./files/VBR%d.dat",
			disk,LBA_address[cur_partition],cur_partition+1);
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
	
}

void mbr(int skip)
{
	//use dd command to extract the MBR and save it in a file(without any extra characters and skip the first 446 bytes)
	sprintf(cmd,"sudo dd if='%s' bs=512 skip=%d count=1 2> log | xxd -p -c 512> ./files/MBR.dat",disk,skip);// test:change /dev/%s to /tmp/%s
	system(cmd);
	//read the file and store MBR in the MBR array
	freopen("./files/MBR.dat","r",stdin);
	i=0;
	while(scanf("%c",&MBR[i]) != EOF )
		i++;	

	//check the "aa55" signature
	if(MBR[1020] != '5' || MBR[1021] != '5' || MBR[1022] != 'a' || MBR[1023] != 'a'){
		printf("ERROR!\n%s is invalid!Please check the Log file for more information.\n",disk);
		exit(0);
	}
	
	offset=0;
	printf("==================MBR==================\n");
	printf("LBA address     size in sectors      partition type\n");

	//extract information from specific bytes and print them
	for(par_num=0;par_num<4;par_num++){
		offset=446+par_num*16;
		partition_type[par_num] = get_info(MBR,offset+4,offset+4);
		LBA_address[par_num] =skip+ get_info(MBR,offset+8,offset+11);
		size_in_sectors[par_num] = get_info(MBR,offset+12,offset+15);
		if(LBA_address[par_num] == 0) break;
		// printf("information about partition %d:\n",par_num+1);
		printf("%11d          %10d      %s\n",
			LBA_address[par_num],size_in_sectors[par_num],type_judge(partition_type[par_num]));
	}
	vbr();
}
