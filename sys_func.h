#define Read_32(buff,index) (*(unsigned int *)&buff[index])
#define Read_16(buff,index) (*(unsigned short int *)&buff[index])

int config_system(SYS * _system,char config_file[])
 {
         FILE * config_pid;
         char * buff = NULL;
         char * tok = NULL;
         size_t len;
         char ret = 0,cnt = 0,cnt1 = 0;
 
         if((config_pid = fopen(config_file,"r")) == NULL)
         {
                 perror("config ");
                 return -1;
         }
 
         printf("Retriving the host system configuration.....\n");
         //first line host system id
         if((ret = getline(&buff,&len,config_pid) < 0))
         {
                 perror("config ");
                 return -1;
         }
 
         if((ret = strlen(buff)) >= id_len)
         {
		 printf("System id : id should be less than or equal to %d\n",id_len);
                 return -1;
         }

	 buff[strcspn(buff, "\n")] = '\0';
         strcpy((*_system).host.id,buff);
         //printf("Host system id : %s\n",(*_system).host.id);
	    //second line no ear in host
         if((ret = getline(&buff,&len,config_pid) < 0))
         {
                 perror("config ");
                 return -1;
         }
         //printf("Buffer : %s\n",buff);
	 buff[strcspn(buff, "\n")] = '\0';
         (*_system).host.ear_Info.count = atoi(buff);
         //printf("Host system ear count : %d\n",(*_system).host.ear_Info.count);
         (*_system).host.ear_Info.ears = (struct ear *)malloc(sizeof(struct ear)* (*_system).host.ear_Info.count);
 
         while(cnt != (*_system).host.ear_Info.count)
         {
                 //printf("Host ear %d : \n",cnt);
                 if((ret = getline(&buff,&len,config_pid) < 0))
                 {
                         perror("config ");
                         return -1;
                 }
 
                 //printf("Buffer : %s\n",buff);
	 	 buff[strcspn(buff, "\n")] = '\0';
                 tok = strtok(buff," ");
                 if(atoi(tok) < 1000 && atoi(tok) > 65535)
		 {
                         printf("%s : port number should be 1000 < port < 65536\n",(*_system).host.id);
                         return -1;
                 }
                 (*_system).host.ear_Info.ears[cnt].port.port_no = atoi(tok);
                 //printf("port : %d\n",(*_system).host.ear_Info.ears[cnt].port.port_no);
 
                 tok = strtok(NULL," ");
                 (*_system).host.ear_Info.ears[cnt].port.capcity = atoi(tok);
                 //printf("capacity : %d\n",(*_system).host.ear_Info.ears[cnt].port.capcity);
                 cnt += 1;
         }
 
         //read mouth capcity
         if((ret = getline(&buff,&len,config_pid) < 0))
         {
                 perror("config ");
                 return -1;
         }
	 buff[strcspn(buff, "\n")] = '\0';
         (*_system).host.mouth_Info.capcity = atoi(buff);
         //printf("Mouth capcity : %d\n",(*_system).host.mouth_Info.capcity);

         printf("Retriving the host system configuration.....Done\n");
         printf("Retriving the peers system configuration.....\n");
         //line contains no of peer systems
         //no peer system
         if((ret = getline(&buff,&len,config_pid) < 0))
         {
		perror("config ");
		return -1;
         }
 
	 buff[strcspn(buff, "\n")] = '\0';
         (*_system).peer.count = atoi(buff);
         //printf("No peer system : %hd\n",atoi(buff));
 
         //create a array for the peer system
         (*_system).peer.peers = (struct peer *)malloc(sizeof(struct peer) * (*_system).peer.count);
 
         cnt = 0;
 
         while(cnt != (*_system).peer.count)
         {
                 //read id of system
                 if((ret = getline(&buff,&len,config_pid) < 0))
                 {
                         perror("config ");
                         return -1;
                 }
	 	
		 buff[strcspn(buff, "\n")] = '\0';
                 if((ret = strlen(buff)) >= id_len)
                 {
                         printf("System id : id should be less than or equal to %d\n",id_len);
                         return -1;
                 }
 
                 strcpy((*_system).peer.peers[cnt].id,buff);
                 //printf("peer system id : %s\n",(*_system).peer.peers[cnt].id);
 
                 //no ear system
                 if((ret = getline(&buff,&len,config_pid) < 0))
                 {
                         perror("config ");
                         return -1;
                 }
	 	 
		 buff[strcspn(buff, "\n")] = '\0';
                 (*_system).peer.peers[cnt].port_cnt = atoi(buff);
                 //printf("ear count : %d\n",(*_system).peer.peers[cnt].port_cnt);
 
                 //create array for each ear
                 (*_system).peer.peers[cnt].ports = (struct port_info*)malloc(
                         sizeof(struct port_info)* (*_system).peer.peers[cnt].port_cnt);
 
                 cnt1 = 0;
 
                 while(cnt1 != (*_system).peer.peers[cnt].port_cnt)
                 {
                         //read each port and its capcity
                         if((ret = getline(&buff,&len,config_pid) < 0))
                         {
                                 perror("config ");
                                 return -1;
                         }

	 		 buff[strcspn(buff, "\n")] = '\0';
                         tok = strtok(buff," ");
                         if(atoi(tok) < 1000 || atoi(tok) > 65535)
                         {
                                 printf("%s : port number should be 1000 < port < 65536\n",(*_system).peer.peers[cnt].id);
                                 return -1;
                         }
                         (*_system).peer.peers[cnt].ports[cnt1].port_no = atoi(tok);
                         //printf("port : %d\n",(*_system).peer.peers[cnt].ports[cnt1].port_no);
 
                         tok = strtok(NULL," ");
                         (*_system).peer.peers[cnt].ports[cnt1].capcity = atoi(tok);
                         //printf("capacity : %d\n",(*_system).peer.peers[cnt].ports[cnt1].capcity);
                         cnt1 += 1;
 
                 }
                 cnt +=1;
}
         printf("Retriving the peers system configuration.....Done\n");
 
         fclose(config_pid);
         return 0;
 }

int create_ear(struct ear * ele)
{
	char re = 0;
	char port_str[10];
	char cap[10];
	char b_efile[12] = "b_e";
	char * child_argv[5];
	
	ele->status = false;
	sprintf(port_str,"%d",ele->port.port_no);
	strcat(b_efile,port_str);

	if((ele->fd = open(b_efile,O_RDONLY | O_CREAT | O_TRUNC)) < 0)
	{
		printf("%hd ",ele->port.port_no);
		perror(":");
		return -1;
	}

	lseek(ele->fd,1,SEEK_SET);
//	printf("file descriptor of the ear : %d\n",ele->fd);
	child_argv[0] = "ear";
	child_argv[1] = port_str;
	child_argv[2] = b_efile;
	sprintf(cap,"%d",ele->port.capcity);
	child_argv[3]= cap;
	child_argv[4] = NULL;
 
         ele->p_id = fork();
         if(ele->p_id >= 0)
         {
		if(ele->p_id == 0)
		{
                 	re = execv("./ear",child_argv);
 
	                 if(re == -1)
        	         {
				 printf("%hd ",ele->port.port_no);
				 perror(":");
				 return -1;
			 }
		}
	}
	else
	{
		printf("%hd ",ele->port.port_no);
		perror(":");
		return -1;
	}
	
	ele->status = true;
	return ele->p_id;
}
int configure_All_ear(struct _ear_Info * ear)
{
	char cnt_ear_fail,cnt = 0,ret;
	//printf("host ear count :%d\n",(*ear).count);
	while(cnt != (*ear).count)
	{
		ret = create_ear(&((*ear).ears[cnt]));
		if(ret == -1)
		{
			cnt_ear_fail += 1;
			printf("Faild create ear on port : %hd\n",(*ear).ears[cnt].port.port_no);
			cnt += 1;
			continue;
		}

		cnt += 1;
	}
	if(cnt_ear_fail == (*ear).count)
	{
		return -1;
	}
	else
	{
		return cnt;
	}
}

int configure_mouth(struct _mouth_Info * mouth, int port)
{
	char re = 0;
	char port_str[10];
	char cap[10];
	char b_efile[12] = "b_m";
	char * child_argv[5];

	sprintf(port_str,"%d",port);
	strcat(b_efile,port_str);
	sprintf(cap,"%d",mouth->capcity);

	if((mouth->fd = open(b_efile,O_WRONLY | O_CREAT | O_TRUNC)) < 0)
	{
		perror("Mouth ");
		return -1;
	}
	
	lseek(mouth->fd,1,SEEK_SET);

	child_argv[0] = "mouth";
	child_argv[1] = b_efile;
	child_argv[2] = cap;
	child_argv[3] = NULL;

	mouth->p_id = fork();

	if(mouth->p_id >= 0)
	{
		if(mouth->p_id == 0)
		{
			re = execv("./mouth",child_argv);

			if(re == -1)
			{
				perror("Mouth ");
				return -1;
			}
		}
	}
	else
	{
		perror("Mouth ");		
		return -1;
	}

	return mouth->p_id;
}

int configure_UI(struct _ui_info * ui,int port)
{
	char re = 0;
	char port_str[10];
	char b_efile[12] = "b_u";
	char * child_argv[3];

	sprintf(port_str,"%d",port);
	strcat(b_efile,port_str);
	
	if((ui->fd = open(b_efile,O_RDWR | O_CREAT | O_TRUNC)) < 0)
	{
		perror("Mouth ");
		return -1;
	}

	child_argv[0] = "UI";
	child_argv[1] = b_efile;
	child_argv[2] = NULL;

	ui->p_id = fork();

	if(ui->p_id >= 0)
	{
		if(ui->p_id == 0)
		{
			re = execv("./UI",child_argv);

			if(re == -1)
			{
				perror("UI ");
				return -1;
			}
		}
	}
	else
	{
		perror("UI ");
		return -1;
	}

	return ui->p_id;
}

//handling fucntions

char checkLockflag(int fd)
{
	char flag = 0;
	if((pread(fd,&flag,1,0)) < 0)
	{
		perror("Brain : check flag ");
		flag = -1;
	}
	return flag;
}

char changeLockflag(int fd,char arg)
{
	if((pwrite(fd,&arg,1,0)) < 0)
	{
		perror("UI ");
		arg = -1;
	}
	return arg;
}
int writeIntoUI(int fd,char * buffer)
{
	if((pwrite(fd,buffer,1024,1)) < 0)
	{
		perror("UI ");
		return -1;
	}

	return changeLockflag(fd,3);
}

short int check_id(struct peers_info ptr, char * id)
{
	short int cnt = 0;

	while(cnt != ptr.count)
	{
		if(strcmp(ptr.peers[cnt].id,id) == 0)
		{
			break;
		}
		cnt += 1;
	}
	return (cnt == ptr.count) ? -1 : cnt;
}

char * create_message(us_int msg_size,us_int des,us_int src,char type,int size, char * data)
{
	char * msg = (char *)malloc(1 * msg_size);
	
	//des 0
	Read_16(msg,0) = des;
	//src 2
	Read_16(msg,2) = src;
	//type 
	msg[4] = type;
	//size 5
	Read_32(msg,5) = size;
	//copy data 
	strcpy(&msg[9],data);

	return msg;
}

int writeIntomouthfile(int fd, char * buffer,us_int size)
{
	us_int ret = 0;

	//lock file for writting
	if((ret = changeLockflag(fd,1)) < 0)
	{
		return ret;
	}
	
	//write data size 
	if((ret = write(fd,&size,2)) < 0)
	{
		return ret;
	}
	//data write
	if((ret = write(fd,buffer,size)) < 0)
	{
		return ret;
	}

	//unlock file
	if((ret = changeLockflag(fd,0)) < 0)
	{
		return ret;
	}	
	return ret;
}

int send_message(SYS _system,char * buffer, us_int id)
{
	char * msg = NULL;
	us_int mesg_size;
	us_int src = _system.host.ear_Info.ears[0].port.port_no;
	us_int des = _system.peer.peers[id].ports[0].port_no;

	//define message size
	mesg_size = (_system.host.mouth_Info.capcity <= _system.peer.peers[id].ports[0].capcity) ?
		_system.host.mouth_Info.capcity : _system.peer.peers[id].ports[0].capcity;

	//create a message to mouthi messagesize,des,src,type,datalength,data
	msg = create_message(mesg_size,des,src,1,Read_32(buffer,22),&buffer[26]);

	if(msg == NULL)
	{
		return -1;
	}

	printf("message to send mouth : des : %hd\n src : %hd\n type : %d\n data length: %d\n data: %s\n ",
		Read_16(msg,0), Read_16(msg,2),msg[4],Read_32(msg,5),&msg[9]);
		
	//write into the mouth file
	
	return writeIntomouthfile(_system.host.mouth_Info.fd,msg,mesg_size);
	//get port number destination
}
int handleUIreq(SYS _system)
{
	int fd = _system.host.ui_info.fd;
	int re = 0;

	char buffer[1024] = {};
	unsigned short int type = 0;

	if((re = (int)checkLockflag(fd)) != 1)
	{
		return re;
	}
	
	if((re = (changeLockflag(fd,2))) < 0)
	{
		return re;
	}
	printf("Message arived\n");

	if((pread(fd,buffer,1024,1)) < 0)
	{
		perror("UI ");
		return -1;
	}

	type = Read_16(buffer,0);
	
	if(type == 1)
	{
		//printf("message of type 1 send message\n");
		//check id exits or not
		re = check_id(_system.peer,&buffer[2]);	
		if(re < 0)
		{	
			re = writeIntoUI(fd,"Id not found..!!");
			return re;
		}
		
		re = send_message(_system,buffer,(us_int)re);

		if(re < 0)
		{
			re = writeIntoUI(fd,"ERROR : while sending the messgae...");
			return re;
		}

		re = writeIntoUI(fd,"Message sent");
		return re;
	}
	else if(type == 2)
	{
		printf("message for the shutdown\n");
		re = writeIntoUI(fd,"shutdown.....");
		//re = shutdown(_system);
		return re;
	}

	return 0;
}

char is_nextReq(int fd,us_int cap)
{
	int cop = lseek(fd,0,SEEK_CUR);
	int top = lseek(fd,0,SEEK_END);

	lseek(fd,cop,SEEK_SET);

	if((top - cop) < cap)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

short int getReq(int fd,char * buff,us_int cap)
{	
	short int n = 0;

	if((((checkLockflag(fd)) == 1) && ((is_nextReq(fd,cap)) == 0)) ||
		(((checkLockflag(fd)) == 0) && ((is_nextReq(fd,cap)) == 0)))
	{
		return 0;
	}
	
	if((n = read(fd,buff,cap)) < 0)
	{
		perror("ear ");
	}
	printf("inside get req message from the ear : des : %hd | src : %hd | type : %d | data length: %d | data: %s\n ",
	Read_16(buff,0), Read_16(buff,2),buff[4],Read_32(buff,5),&buff[9]);
	return n;
}

int handleEarreq(SYS _system)
{
	us_int cap = _system.host.ear_Info.ears[0].port.capcity;
	int fd = _system.host.ear_Info.ears[0].fd;
	us_int uifd = _system.host.ui_info.fd;
	short int ret = 0;
	
	char buff[cap];

	if((ret = getReq(fd,buff,cap)) < 1)
	{
		//printf("get req\n");
		return ret;
	}
	
	printf("cap of ear : %d\n",cap);
	printf("message from the ear after : des : %hd | src : %hd | type : %d | data length: %d | data: %s\n ",
	Read_16(buff,0), Read_16(buff,2),buff[4],Read_32(buff,5),&buff[9]);
	
	if((writeIntoUI(uifd,&buff[9])) < 0)
	{
		return -1;
	}
	return ret;
}
