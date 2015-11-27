extern char MBR[1024];//at most a sector(512 bytes)
extern char VBR[1024];
extern char cmd[200];
extern char disk[30];
extern char file_information[100][64];//at most 100 lines(1024 bytes) of allocated parts and  each 32 bytes one line
extern int i,j,k,offset,count;
extern int NumberOfPartition; // NumberOfPartition means the number of partition.


void type_FAT(int LBA_address,int FatType)
{
	offset=0;
	char OME_name[9]="";
	for(i=0;i<8;i++)
	{
		OME_name[i]=get_info(VBR,offset+3+i,offset+3+i);
	}
	OME_name[8]='\0';
	printf("OME Name in ASCII: %s\n",OME_name);
	int bytes_per_sector= get_info(VBR,offset+11,offset+12);
	printf("bytes per sector: %d\n", bytes_per_sector);
	int sectors_per_cluster= get_info(VBR,offset+13,offset+13);
	printf("sectors per cluster: %d\n", sectors_per_cluster);
	int size_in_sectors_of_reserved_area=get_info(VBR,offset+14,offset+15);
	printf("size in sectors of the reserved area: %d\n",size_in_sectors_of_reserved_area);
	int number_of_FATS=get_info(VBR,offset+16,offset+16);
	printf("number of FATS: %d\n",number_of_FATS);
	int size_in_sectors_of_one_FAT = 0 ;
	int root_cluster=0;
	if(FatType == 32){
		size_in_sectors_of_one_FAT=get_info(VBR,offset+36,offset+39);
		root_cluster = get_info(VBR, offset+44,offset+47);
	}
	else if(FatType == 16){
		size_in_sectors_of_one_FAT=get_info(VBR,offset+22,offset+23);
		root_cluster = 2;
	}
	else{
		printf("Something happened.\n ");
		exit(0);
	}
	printf("size in sectors of one FAT: %d\n", size_in_sectors_of_one_FAT);
	printf("Cluster where root directory can be found: %d\n", root_cluster);
	printf("\n");
	//find the root directory
	int root_directory = LBA_address + size_in_sectors_of_reserved_area + number_of_FATS*size_in_sectors_of_one_FAT + sectors_per_cluster*(root_cluster-2);
	sprintf(cmd,"sudo dd if='%s' bs=512 skip=%d count=2 2>> log | xxd -p -c 1024 > ./files/ROOT_Directory.dat",disk,root_directory);
	system(cmd);

	//read and store the file entries in the file_information array
	printf("information about files:\n");
	freopen("./files/ROOT_Directory.dat","r",stdin);
	i=0;
	while(scanf("%c%c",&file_information[i][0],&file_information[i][1])!=EOF)
	{
		if(file_information[i][0]=='0'&&file_information[i][1]=='0') break;
		int j;
		for(j=2;j<64;j++) 
		{
			scanf("%c",&file_information[i][j]);
		}
		i++;
	}
	//extract information of each file from specific bytes and print them
	int cnt=i;
	int file_number=0;
	int file_attribute;
	char file_attribute_description[20]="";
	char file_name[702]="";//at most 0x3f*13 characters in a filename
	int size_of_file;
	int name_entry=0;
	
	printf("No.   size(bytes)   attribute description\t file name\n");
	for(i=0;i<cnt;i++)
	{
		if(file_information[i][0]=='e'&&file_information[i][1]=='5') continue;
		offset=0;
		file_attribute=get_info(file_information[i],offset+11,offset+11);
		switch(file_attribute)
		{
			case 0x01: strcpy(file_attribute_description,"Read only"); break;
			case 0x02: strcpy(file_attribute_description,"Hidden file"); break;
			case 0x04: strcpy(file_attribute_description,"System file"); break;
			case 0x08: strcpy(file_attribute_description,"Volume label"); break;
			case 0x0f: strcpy(file_attribute_description,"Long file name"); break;
			case 0x10: strcpy(file_attribute_description,"Directory"); break;
			case 0x20: strcpy(file_attribute_description,"Archive"); break;
			case 0x16: strcpy(file_attribute_description,"Special system file"); break;
			default: strcpy(file_attribute_description,"No match");
		}
		if(file_attribute!=0x0f)//not long name file
		{
			name_entry=0;
			file_number++;
			for(j=0;j<11;j++)
			{
				file_name[j]=get_info(file_information[i],offset+j,offset+j);
			}
			file_name[11]='\0';
			size_of_file= get_info(file_information[i],offset+28,offset+31);
		}
		else//long name file, find every entry it contains and etract the name
		{
			int number_of_entries=get_info(file_information[i],offset+0,offset+0)-0x40;
			for(j=number_of_entries-1,count=0;j>=0;j--)
			{
				for(k=1;k<=10;k+=2)
				{
					file_name[count++]=get_info(file_information[i+j],k,k);
				}
				for(k=14;k<=25;k+=2)
				{
					file_name[count++]=get_info(file_information[i+j],k,k);
				}
				for(k=28;k<=31;k+=2)
				{
					file_name[count++]=get_info(file_information[i+j],k,k);
				}
				file_name[count]='\0';
			}
			file_number++;
			file_attribute=get_info(file_information[i+number_of_entries],offset+11,offset+11);
			switch(file_attribute)
			{
				case 0x01: strcpy(file_attribute_description,"Read only"); break;
				case 0x02: strcpy(file_attribute_description,"Hidden file"); break;
				case 0x04: strcpy(file_attribute_description,"System file"); break;
				case 0x08: strcpy(file_attribute_description,"Volume label"); break;
				case 0x0f: strcpy(file_attribute_description,"Long file name"); break;
				case 0x10: strcpy(file_attribute_description,"Directory"); break;
				case 0x20: strcpy(file_attribute_description,"Archive"); break;
				case 0x16: strcpy(file_attribute_description,"Special system file"); break;
				default: strcpy(file_attribute_description,"No match");
			}
			size_of_file= get_info(file_information[i+number_of_entries],offset+28,offset+31);
			i+=number_of_entries;
		}
		/*
		printf("File #%d==================\n", file_number);
		printf("file attribute description: %s\n", file_attribute_description);
		printf("file name in ASCII: %s\n",file_name);
		printf("size of file in bytes: %d\n", size_of_file);
		*/
		printf("%d",file_number);
		printf("%15d",size_of_file);
		printf("%24s",file_attribute_description);
		printf("\t %s\n",file_name);
	}
	printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	return ;
}

void type_EXTEND(int LBA_addr)
{
	sprintf(cmd,"sudo dd if='%s' bs=512 skip=%d count=1 2> log | xxd -p -c 512> ./files/MBR_e1.dat",disk,LBA_addr);
	system(cmd);

	//read the file and store MBR in the MBR array
	freopen("./files/MBR_e1.dat","r",stdin);
	i=0;
	while(scanf("%c",&MBR[i]) != EOF )
		i++;	

	//check the "aa55" signature
	if(MBR[1020] != '5' || MBR[1021] != '5' || MBR[1022] != 'a' || MBR[1023] != 'a'){
		printf("ERROR!\n%s is invalid!Please check the Log file for more information.\n",disk);
		exit(0);
	}
	
	offset=0;
	int partition_type[4]={0,0,0,0};	
	int LBA_address[4]={0,0,0,0};
	int size_in_sectors[4]={0,0,0,0};
	printf("==================MBR_e1==================\n");
	//extract information from specific bytes and print them
	for(NumberOfPartition=0;NumberOfPartition<4;NumberOfPartition++){
		offset=446+NumberOfPartition*16;
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
}
