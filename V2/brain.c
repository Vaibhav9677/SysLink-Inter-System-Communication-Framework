#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <math.h>
#include"sys_structs.h"
#include"sys_func.h"

#define data_len 1024

//SYS _system;

int main(int argc, char * argv[])
{
	int re = 0;

	if(argc != 2)
	{
		printf("Invalid arguments...\n");
		printf("Usage : ./swb config");
		exit(EXIT_FAILURE);
	}
	//comunication file for the ear
	
	if((re = config_system(&_system,argv[1])) < 0)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("configuration file retiveing successfully....\n");
	}

	//printf("Host system Id : %s\n",_system.host.id);
	//create ear ass per system varible
	printf("Configuration of ear part.....\n");
	re = configure_All_ear(&_system.host.ear_Info);
	if(re == -1)
	{
		printf("ERROR : while configuring the ear part...\n");
		exit(EXIT_FAILURE);
	}

	printf("Configuration of ear part.....done\n");

	//create a mouth as per config

	printf("Configuration of mouth part....\n");
	
	re = configure_mouth(&_system.host.mouth_Info,_system.host.ear_Info.ears[0].port.port_no);

	if(re == -1)
	{
		printf("ERROR : while configuring the mouth part...\n");
		exit(EXIT_FAILURE);
	}
	printf("Configuration of mouth part.....done\n");
	
	//creating the UI 
	printf("Configuration of UI part.....\n");

	re = configure_UI(&_system.host.ui_info,_system.host.ear_Info.ears[0].port.port_no);

	if(re == -1)
	{
		printf("ERROR : while configuring the UI part...\n");
		exit(EXIT_FAILURE);
	}
	printf("Configuration of UI part.....Done\n");

	re = craete_MCB_control(&_system);

	if(re == -1)
	{
		printf("ERROR : while creating the MCB control...\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		re = handleEarreq(_system);
		if(re == -1)
		{
			printf("brain : error while handling ear request\n");
		}
		
		re = 0;
		re = handleUIreq(_system);
		if(re == -1)
		{
			printf("brain : error while handling ui request\n");
		}
		else if(re == -2)
		{
			break;
		}
	}
	return 0;
}

