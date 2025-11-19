#ifndef sys_structs
#define sys_structs

#define id_len 20
#define pack_Header_size 14

typedef unsigned short int us_int;
typedef struct _system SYS;

struct HandShake
{
	bool is_connect;
	char count_of_try;
};

struct MetaData
{
	int total_size;
	int count_of_sub_pack;
	int size_of_packet;
	int total_no_packet;
};

struct InfoPack
{
	int count;
	struct Meta_pack * Meta_pack;
	char * Ack_bitv;
};

struct MCB
{
	//connection status
	char con_type;
	us_int src_port;
	us_int dest_port;
	int msg_id;
	int fd;
	char state;
	char sub_state;
	int alarm;
	char * ack;
	//connection 
	struct HandShake * handshake;

	//metadata
	struct MetaData * metadata;

	//infopacket
	struct InfoPack * infopack;

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
	struct * MCB head;
	struct * MCB tail;
}
struct _system
{
         struct host_Info host;
         struct peers_info peer;
	 struct MCB_control;
};

#endif
