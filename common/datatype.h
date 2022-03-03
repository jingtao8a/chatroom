/*************************************************************************
	> File Name: datatype.h
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月20日 星期四 16时03分36秒
 ************************************************************************/

#ifndef _DATATYPE_H
#define _DATATYPE_H

#define MAX 300//half number of clients
#define NTHREAD 3


struct User{
	int team;//0 RED, 1 BLUE
	int fd;//related socket
	char name[20];
	int online;//1 online, 0 not online
	int flag;//
};
//login realated
struct LogRequest{
	char name[20];
	int team;
	char msg[512];
};

struct LogResponse{
	int type;//0 OK, 1 NO
	char msg[512];
};


#define CHAT_FIN 0X01//断开
#define CHAT_HEART 0X02
#define CHAT_ACK 0x04
#define CHAT_WALL 0X08 //公聊
#define CHAT_MSG 0x10 //私聊
#define CHAT_FUNC 0X20//功能
#define CHAT_SYS 0x40//系统通知信息

struct ChatMsg {
	int type;
	char name[20];
	char msg[1024];
};


#endif
