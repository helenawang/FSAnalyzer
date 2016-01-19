//filename: get_information.h 		author: Wang Ting 	date:2015.8.17.
//main function----int get_info(char arr[],int start,int end)
//parameters:
//arr----the array of data,  obtained from "xxd -p" (no extra space between characters)
//start----the first byte you want to get 
//end----the last byte you want to get
//return:
//an integer equals to the specified hexdecimal string
char str[1024];
int base_table[8]={16,1,4096,256,1048576,65536,268435456,16777216};
int change_to_value(char arr[],int cnt)
{//change ascii to its value
	int i;
	for(i=0;i<cnt;i++)
	{
		if(arr[i]>='0' && arr[i]<='9') arr[i]-='0';
		else if(arr[i]>='a' && arr[i]<='f') arr[i]-=87;
		else if(arr[i]>='A' && arr[i]<='F') arr[i]-=55;
		else return -1;
	}
	return 0;
}
int get_dec(char arr[],int cnt)
{//change hexdecimal to decimal
	int i,sum=0;
	if(change_to_value(arr,cnt)) return -1;
	for(i=0;i<cnt;i++)
	{
		if(arr[i]==0) continue;
		sum+=base_table[i]*arr[i];
	}
	return sum;
}
int get_info(char arr[],int start,int end)
{//get information from specified bytes
	if(start>end || start<0 || end>=512) {printf("Range Error!\n"); return -1;}
	start*=2;
	end*=2;
	int i,p=start;
	char temp[8];
	int cnt=end-start+2;
	for(i=0;i<cnt;i++,p++)
	{
		temp[i]=arr[p];
	}
	return get_dec(temp,cnt);
}