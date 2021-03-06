//============================================================================
// Name        : get_bytes.c
// Author      : Helena Wang
// Version     : 1.0.0
// Copyright   : Apache v2
// Description : Encapsulations of fread to get different bytes in C, Ansi-style
//============================================================================
#include "includes.h"
uint32_t get_32(FILE* fp, long offset)
{
	fseek(fp,offset,SEEK_CUR);
	uint32_t res;
	fread(&res,sizeof(uint32_t),1,fp);
	fseek(fp,-(offset+sizeof(uint32_t)),SEEK_CUR);
	return res;
}

uint16_t get_16(FILE* fp, long offset)
{
	fseek(fp,offset,SEEK_CUR);
	uint16_t res;
	fread(&res,sizeof(uint16_t),1,fp);
	fseek(fp,-(offset+sizeof(uint16_t)),SEEK_CUR);
	return res;
}

uint8_t get_8(FILE* fp, long offset)
{
	fseek(fp,offset,SEEK_CUR);
	uint8_t res;
	fread(&res,sizeof(uint8_t),1,fp);
	fseek(fp,-(offset+sizeof(uint8_t)),SEEK_CUR);
	return res;
}
void get_chars(FILE* fp, long offset, long len, char* str)
{
	fseek(fp,offset,SEEK_CUR);
	fread(str,sizeof(char),len - offset + 1,fp);
	fseek(fp,-(len+1),SEEK_CUR);
	return ;
}
