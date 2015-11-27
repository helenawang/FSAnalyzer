#include <stdlib.h>
#include <string.h>
#include "get_information.h"
#include "type_judge.h"
#include "type_fs.h"

char MBR[1024];//at most a sector(512 bytes)
char VBR[1024];
char cmd[200];
char disk[30];
char file_information[100][64];//at most 100 lines(1024 bytes) of allocated parts and  each 32 bytes one line
int i,j,k,offset=0,count;
int NumberOfPartition=0; // NumberOfPartition means the number of partition.


