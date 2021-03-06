//============================================================================
// Name        : type_judge.c
// Author      : Helena Wang
// Version     : 1.0.0
// Copyright   : Apache v2
// Description : return corresponding description of a type id
// Reference   : File System Forensic Anylasis P89-91
//============================================================================
#include "includes.h"
char ans[30]="";
char* file_type(uint8_t type){
	switch(type)
	{
		case 0x01: strcpy(ans,"Read only"); break;
		case 0x02: strcpy(ans,"Hidden file"); break;
		case 0x04: strcpy(ans,"System file"); break;
		case 0x08: strcpy(ans,"Volume label"); break;
		case 0x0f: strcpy(ans,"Long file name"); break;
		case 0x10: strcpy(ans,"Directory"); break;
		case 0x20: strcpy(ans,"Archive"); break;
		case 0x16: strcpy(ans,"Special system file"); break;
		default: strcpy(ans,"No match");
	}
	return ans;
}

char* fs_type(uint8_t type)
{
	switch(type)
	{
		case 0x00: strcpy(ans, "Empty"); break;
		case 0x01: strcpy(ans, "FAT12, CHS"); break;
		case 0x04: strcpy(ans, "FAT16, 16-32MB, CHS"); break;
		case 0x05: strcpy(ans, "Microsoft Extended, CHS"); break;
		case 0x06: strcpy(ans, "FAT16, 32MB-2GB, CHS"); break;
		case 0x07: strcpy(ans, "NTFS"); break;
		case 0x0b: strcpy(ans, "FAT32, CHS"); break;
		case 0x0c: strcpy(ans, "FAT32, LBA"); break;
		case 0x0e: strcpy(ans, "FAT16, 32MB-2GB, LBA"); break;
		case 0x0f: strcpy(ans, "Microsoft Extended, LBA"); break;
		case 0x11: strcpy(ans, "Hidden FAT12, CHS"); break;
		case 0x14: strcpy(ans, "Hidden FAT16, 16-32MB, CHS"); break;
		case 0x16: strcpy(ans, "Hidden FAT16, 32MB-2GB, CHS"); break;
		case 0x1b: strcpy(ans, "Hidden FAT32, CHS"); break;
		case 0x1c: strcpy(ans, "Hidden FAT32, LBA"); break;
		case 0x1e: strcpy(ans, "Hidden FAT16, 32MB-2GB, LBA"); break;
		case 0x42: strcpy(ans, "Microsoft MBR. Dynamic Disk"); break;
		case 0x82: strcpy(ans, "Solaris x86"); break;
		case 0x83: strcpy(ans, "Linux"); break;
		case 0x84: strcpy(ans, "Hibernation"); break;
		case 0x85: strcpy(ans, "Linux Extended"); break;
		case 0x86: strcpy(ans, "NTFS Volume Set"); break;
		case 0x87: strcpy(ans, "NTFS Volume Set"); break;
		case 0xa0: strcpy(ans, "Hibernation"); break;
		case 0xa1: strcpy(ans, "Hibernation"); break;
		case 0xa5: strcpy(ans, "FreeBSD"); break;
		case 0xa6: strcpy(ans, "OpenBSD"); break;
		default : strcpy(ans, "No Match");
	}
	return ans;	
}
