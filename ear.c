#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

//#define data_len 1024
#define Read_32(buffer, index) (*(unsigned int *)&buffer[index]) 
#define Read_16(buffer, index) (*(unsigned short int *)&buffer[index]) 
char changeLockflag(int fd, char flag)
{
//	printf("fd of ear in chnage : %d\n",fd);
	if((pwrite(fd,&flag,1,0)) < 0)
	{
		perror("Ear.c \n");
		flag = -1;
	}
	return flag;
}

int main(int argc, char * argv[])
{
	printf("ear is running on port : %s having capacity : %s Bytes\n",argv[1],argv[3]);
	int sockfd, n = 0,ret,fd;

	unsigned int data_len = atoi(argv[3]);

	char * buff =(char *)malloc(sizeof(char) * data_len);
	char flag = 0;
	struct sockaddr_in receiverAddr;
		
	if((fd = open(argv[2],O_RDWR)) < 0)
	{
		perror("ERROR : while opening the b_e file \n");
		exit(EXIT_FAILURE);
	}
	
	
	lseek(fd,1,SEEK_SET);

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("ERROR : while creating the socket ");
		exit(EXIT_FAILURE);
	}
	
	//printf("Socket created\n");
	memset(&receiverAddr,0,sizeof(receiverAddr));
		
	receiverAddr.sin_family = AF_INET;
	receiverAddr.sin_addr.s_addr = INADDR_ANY;
	//printf("port : %d",atoi(argv[1]));
	receiverAddr.sin_port = htons(atoi(argv[1]));

	if(bind(sockfd,(const struct sockaddr *)&receiverAddr,sizeof(receiverAddr)) < 0)
	{
		perror("ERROR : while binding to reciver socket ");
		exit(EXIT_FAILURE);
	}
	
	while(1)
	{
		printf("Waiting for the data packets......\n");
		n = recvfrom(sockfd,buff,data_len,MSG_WAITALL,NULL,NULL);
	
		//file lock
		if((ret = changeLockflag(fd,1)) < 0)
		{
			continue;
		}

		if((ret = write(fd,buff,data_len)) < 0)
		{
			perror("ERROR while writing data into file\n");
			continue;
		}
		//unlock file
		if((ret = changeLockflag(fd,0)) < 0)
		{
			continue;
		}
		printf("data reciveed\n");
		printf("message form socket into the ear :  des : %hd | src : %hd | type : %d | datalen : %d| data : %s\n",Read_16(buff,0),Read_16(buff,2),buff[4],Read_32(buff,5),&buff[9]);
	}
		
	close(fd);
	close(sockfd);
	return 0;
}
