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
#define data_buff_size (_system.host.mouth_Info.capcity - pack_Header_size)
#define packet_size _system.host.mouth_Info.capcity
#define clear_packet (memset(packet,-1,packet_size))
#define clear_databuff (memset(data_buff,-1,data_buff_size))

#define CNT_OF_TRY 3
#define SENDER 1
#define RECIVER 2

#define NEW 0
#define CONNECTING 1
#define CONNECT 2
#define DISCONNECT 3
#define RESET 4

#define DEFAULT_TIMMER 0
#define ALROFF 0

#define SYN 0
#define SYNACK 1
#define RST 2
#define SYNACKACK 3
#define FMETADATA 4

#define DEST_OFF 0
#define SRC_OFF 2
#define PACKTYPE_OFF 4
#define CONNID_OFF 5
#define SEQNO_OFF 9
#define ACK_OFF 13
#define DATALEN_OFF 14
#define DATA_OFF 19

typedef unsigned short int _us_int;
typedef unsigned char _u_char;
typedef unsigned int _u_int;

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
	_us_int src_port;			//sender address
	_us_int dest_port;			//destination address
	char con_type;				//sender
	int fd;						// file desciptor
	char state;					//new (next is connection)
	char sub_state;				//0
	char is_waiting;			// 0 => alarm off 1 => waiting for ack 2 => ack recived
	int temp;
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
       _us_int  port_no;
       _us_int capcity;
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
        _us_int count;
        struct ear * ears;
};

struct _mouth_Info
{
        _us_int capcity;
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
         _us_int count;
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
