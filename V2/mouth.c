#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define Read_16(buff,index) (*(unsigned short int *)&buff[index])
#define Read_32(buff,index) (*(unsigned int *)&buff[index])

char is_nextReqArrived(int fd,int data_len)
{
	int curr_op = lseek(fd,0,SEEK_CUR);
	int temp_op = lseek(fd,0,SEEK_END);

	lseek(fd,curr_op,SEEK_SET);

	if((temp_op - curr_op) < data_len)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}
char checkLockflag(int fd)
{
	char flag = 0;
	
	if((pread(fd,&flag,1,0)) < 0)
	{
		perror("Mouth ");
		flag = -1;
	}

	return flag;
}

int main(int argc, char * argv[])
{
  int sockfd,n = 0, fd = 0,data_len = atoi(argv[2]);
  char buffer[data_len];
  int actualdatalen = data_len;

  struct sockaddr_in   receiverAddr;

  if((fd = open(argv[1],O_RDWR)) < 0)
  {
	  perror("ERROR : while opening file \n");
	  exit(EXIT_FAILURE);
  }

  lseek(fd,1,SEEK_SET);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    	perror("socket failed");
    	exit(EXIT_FAILURE);
  }


  printf("Mouth is created with capcity : %s\n",argv[2]);

  while(1)
  {
	// T && T => brain is writting new req
	// T && F => panding request
	// F && T => no pending req
	// F && F => panding req

	//printf("waiting for the data \n");
	 //brain writting new req

	//if((n = read(fd,&actualdatalen,2)) < 0)
	//{
	//	perror("mouth : ");
	//	exit(EXIT_FAILURE);
	//}
//	printf("data length : %d\n",actualdatalen);

	if((((checkLockflag(fd)) == 1) && ((is_nextReqArrived(fd,actualdatalen)) == 0)) ||
		(((checkLockflag(fd)) == 0) && ((is_nextReqArrived(fd,actualdatalen)) == 0)))
	{
		continue;
	}

	if((n = read(fd,buffer,actualdatalen)) < 0)
	{
		perror("ERROR : While reading from the file\n");
		exit(EXIT_FAILURE);
	}

	//printf("message from brain in mouth : des : %hd | src : %hd | packet type : %d |connection id: %d |seq no : %d |Ack : %d | data length: %d | data: %s\n ",
	//Read_16(buffer,0), Read_16(buffer,2),buffer[4],Read_32(buffer,5),Read_32(buffer,9),buffer[13],Read_32(buffer,14),&buffer[19]);
  
	memset(&receiverAddr, 0, sizeof(receiverAddr));
  	receiverAddr.sin_family = AF_INET;
  	receiverAddr.sin_port = htons(Read_16(buffer,0));
  	receiverAddr.sin_addr.s_addr = INADDR_ANY;
  	sendto(sockfd,buffer, actualdatalen, 0, (const struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
//	printf("Data sent.....\n");
	
  }  

  close(sockfd);
  return 0;
}

