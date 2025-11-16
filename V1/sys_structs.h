#ifndef sys_structs
#define sys_structs

#define id_len 20

typedef unsigned short int us_int;
typedef struct _system SYS;

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

struct _system
{
         struct host_Info host;
         struct peers_info peer;
};
#endif
