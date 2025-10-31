#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

#define data_len 1024

#define Read_32(buff,index) (*(unsigned int *)&buff[index])
#define Read_16(buff,index) (*(unsigned short int *)&buff[index])

#define id_len 20
#define data_len 1024

char checkLockflag(int fd)
{
	char flag = 0;
	if((pread(fd,&flag,1,0) < 0))
	{
		perror("check flag ");
		flag = -1;
	}
        return flag;
}
char changeLockflag(int fd,char arg)
 {
 	if((pwrite(fd,&arg,1,0) < 0))
	{
		perror("lock flag ");
		arg = -1;
	}
	return arg;
 }
int write_ubfile(char * buffer,int fd)
{
	if((changeLockflag(fd,0)) < 0)  //put 0
	{
		return -1;
	}
	//write from 1 byte
	if(pwrite(fd,buffer,data_len,1) < 0)
	{
		perror("ERROR : while writing into file\n");
		return -1;
	}
	if((changeLockflag(fd,1)) < 0)  //put 1
	{
		return -1;
	}

	return 0;
}

int fetch_response(char * buff,int fd)
{
	char ret = 0;
	
	printf("waiting for the output\n");
	while((ret = checkLockflag(fd)) != 3)
	{
		if(ret == -1)
		{
			return ret;
		}
	}

	return (pread(fd,buff,data_len,1));
}

void shut_down(unsigned short int op,int fd)
{
	char buffer[data_len];
	Read_16(buffer,0) = op;  		//type	
	printf("Inside shut down : %d\n",Read_16(buffer,0));
	if(write_ubfile(buffer,fd) == -1)
	{
		exit(EXIT_FAILURE);
	}

	printf("Request sent....\n");
	printf("Waiting for output...\n");
	
	if(fetch_response(buffer,fd) < 0)
	{
		printf("ERROR : while fatching resopnce\n");
		return;
	}

	printf("Output : %s\n",buffer);
}

void send_message(unsigned short int op,int fd)
{
	char buffer[data_len];
	Read_16(buffer,0) = op;  		//type	
	printf("Inside send message : %d\n",Read_16(buffer,0));

	printf("Enter the id : \n");
	getchar();
	scanf("%[^\n]",&buffer[2]);
	printf("Enter the data :");
	getchar();
	scanf("%[^\n]",&buffer[26]);
	Read_32(buffer,22) = strlen(&buffer[26]);

	if(write_ubfile(buffer,fd) == -1)
	{
		exit(EXIT_FAILURE);
	}
	if(fetch_response(buffer,fd) < 0)
	{
		printf("ERROR : while fatching resopnce\n");
		return;
	}
	printf("Output : %s\n",buffer);
}

int main(int argc, char * argv[])
{
	unsigned short int op = 0;
	int fd = 0;
	char buffer[data_len];

	if((fd = open(argv[1],O_RDWR)) < 0)
	{
		perror("UI-open ");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		printf("--------------------------Application Menu------------------------\n");
		printf("1 : Send the message \n");
		printf("2 : Shut down the system \n");
		printf("3 : check Arrived message\n");		
		printf("Enter the opration : \n");
		scanf("%hd",&op);

		if(op == 1)
		{
			send_message(op,fd);			
		}
		else if(op == 2)
		{
			shut_down(op,fd);
		}
		else if(op == 3)
		{
			if((fetch_response(buffer,fd)) < 0)
			{
				printf("error");
				continue;
			}
			printf("message : %s\n",buffer);
		}
		else
		{
			printf("Enter Valid option \n");
		}
		getchar();
	}

	return 0;
}
