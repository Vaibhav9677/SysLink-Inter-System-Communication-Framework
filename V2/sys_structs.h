#ifndef sys_structs
#define sys_structs

#define id_len 20
#define MSGIDLEN 9
#define pack_Header_size 18

#define generate_id(x,y) (((unsigned int) x << 16) | y)
#define min(x,y) (x >= y ? x : y)
#define div(x,y) (((x%y) != 0) ? (x/y)+1 : (x/y))
#define printLine (printf("------------------------------------------------------------------\n"))
#define getSystemtime (printf("System time : %ld\n",sysTime))
#define SENDER 1
#define RECIVER 2
#define NEW 0
#define CONNECT 0
#define DISCONNECT 1
#define DEFAULT_TIMMER 50

typedef unsigned short int us_int;
typedef struct _system SYS;


struct HandShake
{
	char is_connect;
	char count_of_try;
};

struct MetaData
{
	char file_name[id_len];
	long total_size;
	int size_of_packet;
	int total_no_packet;
	int count_of_sub_pack;
};

struct InfoPack
{
	int count_AckPack;
	int curr_count;
	int * sub_Info_pack;
	unsigned char bitVectsize;
	unsigned char * ackBitv;
};

struct MCB
{
	//connection status
	unsigned int connection_id;			//generate using src + dest
	us_int src_port;			//sender address
	us_int dest_port;			//destination address
	char con_type;				//sender
	int fd;						// file desciptor
	char state;					//new (next is connection)
	char sub_state;				//0
	char is_waiting;
	int alarm;					//0
	char * ack;					//null
	//connection 
	struct HandShake * handshake;	//null

	//metadata
	struct MetaData * metadata;		//null

	//infopacket
	struct InfoPack * infopack;		//null

	struct MCB * next;
	struct MCB * prev;
};

struct port_info{
       us_int  port_no;
       us_int capcity;
};

struct ear
{
        bool status;
        struct port_info port;
        int p_id;
        int fd;
};

struct _ear_Info
{
        us_int count;
        struct ear * ears;
};

struct _mouth_Info
{
        us_int capcity;
        int p_id;
	int fd;
};

struct _ui_info{
         int p_id;
         int fd;
 };

 struct host_Info{
         char id[id_len];
         struct _ear_Info ear_Info;
         struct _mouth_Info mouth_Info;
         struct _ui_info ui_info;
 };

 struct peer{
         char id[id_len];
         char port_cnt;
         struct port_info * ports;
 };

 struct peers_info{
         us_int count;
         struct peer * peers;
 };

struct MCB_control
{
	int count;
	struct MCB * head;
	struct MCB * tail;
};
struct _system
{
        struct host_Info host;
        struct peers_info peer;
	 	struct MCB_control MCB_C;
};

#endif
