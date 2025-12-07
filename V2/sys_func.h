#include "sys_structs.h"
#define Read_32(buff,index) (*(unsigned int *)&buff[index])
#define Read_16(buff,index) (*(unsigned short int *)&buff[index])

SYS _system;
unsigned long sysTime = 0;
unsigned char TIMER_ON = 1;
pthread_t thread_id;
unsigned char * data_buff = NULL;
unsigned char * packet = NULL;

void * system_timmer()
{
	struct MCB * temp = NULL;
	while(TIMER_ON)
	{
		if(_system.MCB_C.head != NULL)
		{
			temp = _system.MCB_C.head;
			do{
				if(temp->alarm != 0)
				{
					temp->alarm -= 1;
					//printf("Connection id : %d 		Timmer : %d\n",temp->connection_id,temp->alarm);
				}
				temp = temp->next;
			}while(temp != _system.MCB_C.head);
		}
		usleep(1000);
		sysTime = (sysTime % 0xFFFFFFFF) + 1;
	}
}

int SetSystemtimmer()
{
	int re = 0;

	re = pthread_create(&thread_id,NULL,(void *)&system_timmer,NULL);

	if(re != 0)
	{
		return -1;
	}

	re = pthread_detach(thread_id);

	if(re != 0)
	{
		return -1;
	}

	return re;
}
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

int craete_MCB_control(SYS * _system)
{
	_system->MCB_C.count = 0;
	_system->MCB_C.head = NULL;
	_system->MCB_C.tail = NULL;

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

int writeIntomouthfile(int fd, char * buffer,_us_int size)
{
	_us_int ret = 0;

	//lock file for writting
	if((ret = changeLockflag(fd,1)) < 0)
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

int createCleanBuff()
{
	data_buff = (unsigned char *)malloc(sizeof(unsigned char) * data_buff_size);
	
	if(data_buff == NULL)
	{
		return -1;
	}

	packet = (unsigned char *)malloc(sizeof(unsigned char) * packet_size);

	if(packet == NULL)
	{
		return -1;
	}

	clear_databuff;
	clear_packet;
}
char * create_message(_us_int src_port, _us_int dest_port,_u_char pack_type, _u_int connection_id,
					_u_int seq_no, _u_char ack, _u_int dataLen, _u_char * buffer)
{
	clear_packet;
	Read_16(packet,0) = dest_port;		//dest 0
	Read_16(packet,2) = src_port;		//src 	2
	packet[4] = pack_type;				//packet type 4
	Read_32(packet,5) = connection_id;	//conn id 5
	Read_32(packet,9) = seq_no;			//seq no 9
	packet[13] = ack;					//ack 13
	Read_32(packet,14) = dataLen;		//datalen 14

	//data	19 to min(src,dst)

	memcpy(&packet[19],buffer,dataLen);

	return packet;
}

int send_System_message(_us_int src_port,_us_int dest_port,_u_char pack_type, _u_int connection_id,_u_int seq_no,
						_u_char ack,_u_int dataLen,_u_char * buffer)
{
	char * mesg = create_message(src_port,dest_port,pack_type,connection_id,seq_no,ack,dataLen,buffer);

	//printf("Messgae create and packetsize L %d\n",packet_size);
	//printf("message created in the brain :\n des : %hd |\n src : %hd |\n packet type : %d |\nconnection id: %d |\nseq no : %d |\nAck : %d |\n data length: %d |\n data: %s\n ",
	//Read_16(mesg,0), Read_16(mesg,2),mesg[4],Read_32(mesg,5),Read_32(mesg,9),mesg[13],Read_32(mesg,14),&mesg[19]);
	return writeIntomouthfile(_system.host.mouth_Info.fd,mesg,packet_size);
}

int killAllChild(struct host_Info host)
{
	int i = 0;
	int ret = 0;
	while(i < host.ear_Info.count)
	{
		ret = kill(host.ear_Info.ears[i].p_id,SIGKILL);
		if(ret == -1)
		{
			printf("error while killing");
			return -1;
		}
		ret = close(host.ear_Info.ears[i].fd);
		i++;
	}

	i = 0;

	ret = kill(host.mouth_Info.p_id,SIGKILL);
	ret = close(host.mouth_Info.fd);

		if(ret == -1)
		{
			printf("error while killing");
			return -1;
		}
	ret = kill(host.ui_info.p_id,SIGKILL);
		if(ret == -1)
		{
			printf("error while killing");
			return -1;
		}
	ret = close(host.ui_info.fd);
	return ret;
}

int checkfilexsits(char * file_name)
{
	printf("file name : %s\n",file_name);
	return open(file_name,O_RDONLY);
}

void printMCB(struct MCB * mcb)
{
	//print general infomation 

	printLine;
	printf("General Information : \n");
	printLine;
	printf("Connection Id : %d\n",mcb->connection_id);
	printf("Source port : %ld	| Destination port : %ld\n",mcb->src_port,mcb->dest_port);
	printf("Connection type : ");
	(mcb->con_type == SENDER )? printf("SENDER\n") : printf("RECIVER");

	printf("connection state : ");

	switch(mcb->state){
		case NEW :
			printf("NEW\n");
		default :
			printf("Undefined\n");
	}

	printf("Alram : %d\n",mcb->alarm);
	printf("Ack : %s\n");

	printLine;
	printf("Handshake information : \n");
	printf("is connect : ");
	(mcb->handshake->is_connect == CONNECT) ? printf("CONNECT\n") : printf("DISCONNECT\n");
	printf("No of try to connect : %d\n",mcb->handshake->count_of_try);
	printLine;

	printf("Meta Data of the message : \n");
	printf("File name : %s\n",mcb->metadata->file_name);
	printf("Total size of file : %ld\n",mcb->metadata->total_size);
	printf("size of one packet : %d\n",mcb->metadata->size_of_packet);
	printf("Total number of packets : %d\n",mcb->metadata->total_no_packet);
	printf("Count of sub info in one packet : %d\n",mcb->metadata->count_of_sub_pack);

	printLine;

	printf("Info track of packets : ");
	printf("Total number of packet sent and recived : %d\n",mcb->infopack->count_AckPack);
	printf("count of cuurent senf packet : %d\n",mcb->infopack->curr_count);
	printf("packets are :\n");
	printLine;
	int i = 0;
	while(i < mcb->metadata->count_of_sub_pack)
	{
		printf("|%d		|%d		|\n",i+1,mcb->infopack->sub_Info_pack[i]);
		printLine;
		i++;
	}

	printf("Size of bit vector : %d\n",mcb->infopack->bitVectsize);
	printf("bit vector : %s\n",mcb->infopack->ackBitv);

	printLine;
}
struct MCB * create_new_node()
{
	struct MCB * newn = (struct MCB *)malloc(sizeof(struct MCB));

	if(newn == NULL)
	{
		return newn;
	}

	if((_system.MCB_C.head == NULL) && (_system.MCB_C.tail == NULL))
	{
		newn->next = newn;
		newn->prev = newn;
		_system.MCB_C.head = newn;
		_system.MCB_C.tail = newn;
	}
	else
	{
		_system.MCB_C.tail->next = newn;
		newn->next = _system.MCB_C.head;
		newn->prev = _system.MCB_C.tail;
		_system.MCB_C.head->prev = newn;
		_system.MCB_C.tail = newn;		
	}

	_system.MCB_C.count += 1;

	return newn;
}

//it will craete default node and add into the list or table
struct MCB * create_new_MCB()
{
	struct MCB * newnode = create_new_node();

	if(newnode == NULL)
	{
		return newnode;
	}

	//connection id
	newnode->connection_id = 0;
	newnode->con_type = SENDER;
	newnode->src_port = 0;
	newnode->dest_port = 0;

	newnode->state = NEW;
	newnode->sub_state = 0;
	newnode->alarm = DEFAULT_TIMMER;
	newnode->is_waiting = 0;
	newnode->ack = NULL;	
	newnode->handshake = NULL;
	newnode->metadata = NULL;
	newnode->infopack = NULL;	

	return newnode;
}

int getfilesizeByfd(int fd)
{
	struct stat sb;
	int re = 0;

	re = fstat(fd,&sb);

	if(re == -1)
	{
		return re;
	}
	//printf("#########size of file : %d\n",sb.st_size);
	return sb.st_size;
}

struct HandShake * create_new_handshake()
{
	struct HandShake * newh = (struct HandShake *)malloc(sizeof(struct HandShake));

	if(newh == NULL)
	{
		return NULL;
	}

	newh->is_connect = DISCONNECT;
	newh->count_of_try = 0;

	return newh;
}

struct MCB * getElementByid(int connection_id,_u_char con_type)
{
	struct MCB * temp = NULL;
	_u_char flag = 0;
	if(_system.MCB_C.count < 1)
	{
		printf("Connection list is empty\n");
		return temp;
	}

	temp = _system.MCB_C.head;

	do
	{
		if((temp->connection_id == connection_id) && (temp->con_type == con_type))
		{
			flag = 1;
			break;
		}
		temp = temp->next;
	}while(temp != _system.MCB_C.head);

	if(flag == 0)
	{
		temp = NULL;
	}
	return temp;
}

int closeConnt(int connection_id,_u_char con_type)
{
	//get that element
	struct MCB * element= NULL;

	element = getElementByid(connection_id,con_type);
	if(element == NULL)
	{
		return -1;
	}

	//no of element is  1
	if(_system.MCB_C.count == 1)
	{
		_system.MCB_C.head = NULL;
		_system.MCB_C.tail = NULL;
	}
	else
	{
		(element->next)->prev = element->prev;
		(element->prev)->next = element->next;

		if(element == _system.MCB_C.head)
		{
			_system.MCB_C.head = element->next;
		}
		
		if(element == _system.MCB_C.tail)
		{
			_system.MCB_C.tail = element->prev;
		}
	}

	if(_system.MCB_C.count > 0)
	{
		_system.MCB_C.count--;
	}
	else
	{
		_system.MCB_C.count = 0;
	}

	free(element);

	return 1;
}

struct MetaData * create_new_metadata(char * file_name, int fd,int id)
{
	struct MetaData * newm = (struct MetaData *)malloc(sizeof(struct MetaData));

	if(newm == NULL)
	{
		return NULL;
	}

	strcpy(newm->file_name,file_name);
	newm->total_size = getfilesizeByfd(fd);
	newm->size_of_packet = min(_system.host.mouth_Info.capcity,_system.peer.peers[id].ports[0].capcity) - pack_Header_size;
	newm->total_no_packet = div(newm->total_size,newm->size_of_packet);
	newm->count_of_sub_pack = (newm->size_of_packet/sizeof(int));

	if(newm->total_no_packet < newm->count_of_sub_pack)
	{
		newm->count_of_sub_pack = newm->total_no_packet;
	}

	return newm;
}

struct InfoPack * create_new_Infopack(int No_sub_pack)
{
	struct InfoPack * newi = (struct InfoPack *)malloc(sizeof(struct InfoPack));

	if(newi == NULL)
	{
		return NULL;
	}

	newi->count_AckPack = 0;
	newi->curr_count= 0;
	newi->sub_Info_pack = (int *)malloc(sizeof(int) * No_sub_pack);
	newi->bitVectsize = (unsigned char)(div(No_sub_pack,8));
	newi->ackBitv = (unsigned char *)malloc(newi->bitVectsize);

	return newi;
}
int assign_value_MCB(struct MCB * newnode, char * file_name,int fd, int id, char type)
{
	if(type == SENDER)
	{
		newnode->connection_id = generate_id(_system.host.ear_Info.ears[0].port.port_no,_system.peer.peers[id].ports[0].port_no);
		newnode->src_port = _system.host.ear_Info.ears[0].port.port_no;
		newnode->dest_port =  _system.peer.peers[id].ports[0].port_no;
	}
	else
	{
		newnode->connection_id = id;
	}

	newnode->con_type = type;
	

	//create a hanshake object for the new connection
	newnode->handshake = create_new_handshake();
	if(newnode->handshake == NULL)
	{
		return -1;
	}

	if(type != SENDER)
	{
		return 0;
	}

	//now create the metadata all message
	newnode->metadata = create_new_metadata(file_name,fd,id);
	if(newnode->metadata == NULL)
	{
		return -1;
	}

	//now create subinfo packet which is use to track the packets
	newnode->infopack = create_new_Infopack(newnode->metadata->count_of_sub_pack);
	if(newnode->infopack == NULL)
	{
		return -1;
	}

	return newnode->connection_id;
}

void setAlram(int * Alram,int tik)
{
	*(Alram) = tik;
}

_u_int generateRandomnumber()
{
	static _u_int cnt = 1;
	_u_int rnd = rand() & 0xFFFF;
	_u_int id = (cnt << 16) | rnd;
	cnt++;
	return id;
}

int actionFornew(struct MCB * mcb)
{
	int re = 0,data_len = 4;
	_us_int alarm = 500;
	_u_char pack_type = SYN, seq_no = 0, ack = 0;
	clear_databuff;
	mcb->temp = generateRandomnumber();
	printf("RANDOM NUMBER IS : %d\n",mcb->temp);
	Read_32(data_buff,0) = mcb->temp;

	if(mcb->con_type == RECIVER)
	{
		printf("RANDOM NUMBER IT RECIVER SIDE : %d\n",Read_32(mcb->ack,DATA_OFF));
		Read_32(data_buff,0) = Read_32(mcb->ack,DATA_OFF) + 1;
		Read_32(data_buff,4) = mcb->temp;
		printf("Random which send back with SYNACK : %d\n",Read_32(data_buff,4));
		alarm = 1000;
		pack_type = SYNACK;
		seq_no = 1;
		ack = 1;
		data_len = 8;
	}

	//printMCB(mcb);
	re = send_System_message(mcb->src_port,mcb->dest_port,pack_type,mcb->connection_id,seq_no,ack,data_len,data_buff);
	
	if(re == -1)
	{
		return re;
	}

	setAlram(&mcb->alarm,alarm);
	mcb->is_waiting = 1;
	mcb->handshake->count_of_try += 1;
	mcb->state = CONNECTING;
	mcb->sub_state = SYN;

	return 0;
}
int send_RST_mesg(struct MCB * mcb)
{
	int re = 0;
	clear_databuff;
	_u_char ack = 0;

	//when server send the ack byte as 0 and when reciver send the RST ack will have 1
	if(mcb->con_type == RECIVER)
	{
		ack = 1;
	}

	re = send_System_message(mcb->src_port,mcb->dest_port,RST,mcb->connection_id,0,0,ack,data_buff);
	return 0;
}

int restAndstart(struct MCB * mcb)
{
	int re = 0;

	re = send_RST_mesg(mcb);

	if(re == -1)
	{
		return re;
	}

	mcb->handshake->count_of_try = 0;
	mcb->state = NEW;
	mcb->sub_state = 0;

	return 0;
}

int restAndClose(struct MCB * mcb)
{
	int re = 0;
	re = send_RST_mesg(mcb);

	if(re == -1)
	{
		return re;
	}

	//close connection
	return closeConnt(mcb->connection_id,mcb->con_type);
}

int sendACKforconnect(struct MCB * mcb)
{
	int re = 0;
	clear_databuff;
	Read_32(data_buff,0) = Read_32(mcb->ack,DATA_OFF+4) + 1;

	printf("SYNACKACK send back to the sericer is : %d\n",Read_32(data_buff,0));
	re = send_System_message(mcb->src_port,mcb->dest_port,SYNACKACK,mcb->connection_id,3,1,4,data_buff);

	if(re == -1)
	{
		return re;
	}

	setAlram(&mcb->alarm,0);
	mcb->is_waiting = 0;
	mcb->handshake->is_connect = 1;
	mcb->state = CONNECT;
	mcb->sub_state = FMETADATA;

	printf("HANDSHAKE DONE BETWEEN : %d AND %d\n",mcb->src_port,mcb->dest_port);
	return 0;
}

int changeStateToconnect(struct MCB * mcb)
{
	clear_databuff;
	_u_int temp = Read_32(mcb->ack,DATA_OFF);

	printf("DIFF : %d\n",((temp)-(mcb->temp)));
	if((temp-mcb->temp) != 1)
	{
		printf("DIFF : %d\n",((temp)-(mcb->temp)));
		//send RST packet and start process again
		printf("Random number is incorrect RESTING the connection\n");
		return (mcb->con_type == SENDER ? restAndstart(mcb) : restAndClose(mcb));
	}

	//ack come correct

	if(mcb->con_type == SENDER)
	{
		printf("SYNACK random number at sender side : %d\n",Read_32(mcb->ack,DATA_OFF+4));
		return sendACKforconnect(mcb);
	}

	//RECVER state chnage and wait for the METADATA of the file 

	setAlram(&mcb->alarm,0);
	mcb->is_waiting = 1;
	mcb->handshake->is_connect = 1;
	mcb->state = CONNECT;
	mcb->sub_state = FMETADATA;

	printf("Handshake done between %d and %d waiting for the FILE METADATA\n",mcb->src_port,mcb->dest_port);

	return 0;
}
int MCB_Scheduler(struct MCB * mcb)
{
	//printf("Inside the scheduler\n");

	int re = 0;
	//check the node status and the is waiting for the ack

	//state is NEW

	if(mcb->state == NEW)
	{
		return actionFornew(mcb);
	}
	
			//alarm is not off and waiting for ack
		// 2type access 1 interut and outine
	if((mcb->is_waiting == 1) && (mcb->alarm > 0))
	{
		return 0;
	}

	if(mcb->state == CONNECTING)
	{
		//check the count of try to connect and if try greater than CNT_OF_TRY then close the connection 
		printf("count of try : %d\n",mcb->handshake->count_of_try);
		if((mcb->handshake->count_of_try > CNT_OF_TRY) && (mcb->is_waiting == 1) && (mcb->sub_state == SYN))
		{
			re = writeIntoUI(_system.host.ui_info.fd,"Unable to connect...try again");
			if(re == -1)
			{
				return re;
			}

			re = closeConnt(mcb->connection_id,mcb->con_type);
			return re;
		}

		if((mcb->sub_state == SYN) && (mcb->alarm == ALROFF))
		{
			return actionFornew(mcb);
		}

		if((mcb->sub_state == SYNACK) && (mcb->is_waiting == 0))
		{
			return changeStateToconnect(mcb);	
		}
	}
	else if(mcb->state == CONNECT)
	{
		printf("CONNECTION DONE\n");
		return 0;
	}
	else if(mcb->state == RESET)
	{
		return (mcb->con_type == SENDER) ? restAndstart(mcb) : restAndClose(mcb);
	}

	return 0;
}
int send_file(char * file_name,int fd, int id)
{
	printf("file fd : %d\n",fd);
	printf("user id : %s\n",_system.peer.peers[id].id);
	int re = 0;
	struct MCB * newnode;

	//create a MCB assign default value for it
	newnode = create_new_MCB();
	if(newnode == NULL)
	{
		return -1;
	}

	printf("New node created\n");
	//Asign the value as per the file and message

	re = assign_value_MCB(newnode,file_name,fd,id,SENDER);

	if(re == -1)
	{
		return -1;
	}

	//printMCB(newnode);
	printf("connection id : %d\n",re);

	re = MCB_Scheduler(newnode);

	//printMCB(newnode);
	return re;
}


int handleUIreq(SYS _system)
{
	int fd = _system.host.ui_info.fd;
	int id = 0, sendfd = 0, re = 0;

	char buffer[1024] = {};
	unsigned short int type = 0;

	if((re = (int)checkLockflag(fd)) != 1)
	{
		//printf("flag is : %d\n",re);
		return re;
	}
	
	if((re = (changeLockflag(fd,2))) < 0)
	{
		return re;
	}
	//writeIntoUI(fd,"Message arived");
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
		id = check_id(_system.peer,&buffer[2]);	
		if(id < 0)
		{	
			re = writeIntoUI(fd,"Id not found..!!");
			return re;
		}
		sendfd = checkfilexsits(&buffer[26]);
		if(sendfd == -1)
		{
			re = writeIntoUI(fd,"File not found..!!\n");
			return re;
		}
		
		re = getfilesizeByfd(sendfd);

		if(re < 1)
		{
			re = writeIntoUI(fd,"File is empty...!!!\n");
			return re;
		}

		re = send_file(&buffer[26],sendfd,id);

		if(re < 0)
		{
			re = writeIntoUI(fd,"ERROR : while sending the file....");
			return re;
		}

		//re = writeIntoUI(fd,"Message sent\n");
		return re;
	}
	else if(type == 2)
	{
//		printf("message for the shutdown\n");
		re = writeIntoUI(fd,"shutdown.....");
		re = killAllChild(_system.host);
		
		int i = 0;

		while(i < _system.host.ear_Info.count)
		{
			waitpid(_system.host.ear_Info.ears[i].p_id,NULL,0);
			i++;
		}

		waitpid(_system.host.mouth_Info.p_id,NULL,0);
		waitpid(_system.host.ui_info.p_id,NULL,0);
		//re = shutdown(_system);
		return -2;
	}
	else
	{
		writeIntoUI(fd,"Unable to process this request");
	}
	return 0;
}

int checkAndProcessConnt()
{
	if((_system.MCB_C.head == NULL) && (_system.MCB_C.tail == NULL))
	{
		return 0;
	}

	int re = 0;
	struct MCB * temp = _system.MCB_C.head;
	struct MCB * temp2 = NULL;

	//printf("inside the check and process\n");

	do
	{
		temp2 = temp->next;
		if(temp->con_type == SENDER)
		{
			re = MCB_Scheduler(temp);
		}

		if(re == -1)
		{
			printf("ERROR : while handling connection : %d\n",temp->connection_id);
		}
		else if(re == 1)
		{
			//there was only one element
			if(_system.MCB_C.count == 0)
			{
				break;
			}
		}
	
		temp = temp2;
		//printf("connection id : %d\n",temp->connection_id);
	}while(temp != _system.MCB_C.head);

	return 0;
}

char is_nextReq(int fd,_us_int cap)
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

short int getReq(int fd,char * buff,_us_int cap)
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
	//printf("message from the ear after : des : %hd | src : %hd | packet type : %d |connection id: %d |seq no : %d |Ack : %d | data length: %d | data: %s\n ",
	//Read_16(buff,0), Read_16(buff,2),buff[4],Read_32(buff,5),Read_32(buff,9),buff[13],Read_32(buff,14),&buff[19]);
	return n;
}

int processEareq(char * buff, _us_int cap)
{
	int re = 0;
	struct MCB * newm = NULL;
	_u_char type = buff[PACKTYPE_OFF];
	
	//req for the new connection
	if(type == SYN)
	{
		//create the one node of MCB of type reciver 
		//check the SYN packet already exits or not
		newm = getElementByid(Read_32(buff,CONNID_OFF),RECIVER);
		printf("New connection req for reciver came\n");

		if((_system.MCB_C.count > 0) && (newm != NULL))
		{
			printf("This connection already exist\n");
			return 0;
		}

		printf("*****************************************************************************\n");
		printf("This connection is new and came for first\n");
		newm = create_new_MCB();

		if(newm == NULL)
		{
			return -1;	
		}

		//now assign with value
		newm->dest_port =Read_16(buff,SRC_OFF); 
		newm->src_port = Read_16(buff,DEST_OFF);

		re = assign_value_MCB(newm,"DEFAULT",0,Read_32(buff,CONNID_OFF),RECIVER);

		if(re == -1)
		{
			return -1;
		}
		newm->ack = buff;
	}
	else if((type == SYNACK) && (buff[ACK_OFF] == 1))
	{
		//connection ack come
		newm = getElementByid(Read_32(buff,CONNID_OFF),SENDER);
		printf("ACK for connection came \n");
		if(newm == NULL)
		{
			return 0;
		}

		newm->is_waiting = 0;
		setAlram(&newm->alarm,0);
		newm->ack = buff;
		newm->sub_state = SYNACK;
	}
	else if((type == SYNACKACK) && (buff[ACK_OFF] == 1))
	{
		newm = getElementByid(Read_32(buff,CONNID_OFF),RECIVER);

		if(newm == NULL)
		{
			return 0;
		}

		printf("SYNACKACK : %d\n",Read_32(buff,DATA_OFF));
		newm->is_waiting = 0;
		newm->ack = buff;
		newm->sub_state = SYNACK;
	}
	/*
	else if((type == RST))
	{
		newm = (buff[ACK_OFF] == 1) ? getElementByid(Read_32(buff,CONNID_OFF),SENDER) : getElementByid(Read_32(buff,CONNID_OFF),RECIVER);

		newm->is_waiting = 0;
		newm->ack = buff;
		newm->state = RESET;
	}*/
	else
	{
		return 0;
	}

	return MCB_Scheduler(newm);

}
int handleEarreq(SYS _system)
{
	_us_int cap = _system.host.ear_Info.ears[0].port.capcity;
	int fd = _system.host.ear_Info.ears[0].fd;
	_us_int uifd = _system.host.ui_info.fd;
	short int ret = 0;
	char * msg = NULL;
	
	char buff[cap];

	if((ret = getReq(fd,buff,cap)) < 1)
	{
		//printf("get req\n");
		return ret;
	}
	

	//printf("req arrived..........................\n");
	//printf("cap of ear : %d\n",cap);
	printf("Reqest recivced : des : %hd|\nsrc : %hd|\npacket type : %d|\nconnection id: %d|\nseq no : %d|\nAck : %d|\ndata length: %d|\ndata: %s\n ",
	Read_16(buff,0), Read_16(buff,2),buff[4],Read_32(buff,5),Read_32(buff,9),buff[13],Read_32(buff,14),&buff[19]);

	//create a function which handle the reqest as per the type of the packet

	ret = processEareq(buff,cap);

	if(ret == -1)
	{
		printf("ERROR : while proccessing the ear reqest of connection : %d\n",Read_16(buff,2));
	}
	else
	{
		printf("process req done : %d\n",Read_16(buff,DEST_OFF));
	}

	printLine;
	return ret;
}
