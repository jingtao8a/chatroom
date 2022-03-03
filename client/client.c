/*************************************************************************
	> File Name: client.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月20日 星期四 11时07分54秒
 ************************************************************************/

#define _D
#include "head.h"

int server_port = 0;
char server_ip[20] = {0};

char *path = "./football.conf";

int sockfd = -1;

void *do_recv(void *arg)
{
	struct ChatMsg msg;
	while(1) {
		bzero(&msg, sizeof(msg));
		recv(sockfd, (void *)&msg, sizeof(msg), 0);
		if (msg.type & CHAT_WALL) {
			printf(""BLUE"%s"NONE" : %s\n", msg.name, msg.msg);
		} else if (msg.type & CHAT_MSG) {
			printf(""RED"%s"NONE" : %s\n", msg.name, msg.msg);
		} else if (msg.type & CHAT_SYS) {
			printf(""GREEN"Server Info"NONE" : %s\n", msg.msg);
		} else if(msg.type & CHAT_FIN) {// server down;
			printf(L_RED"Server Info"NONE" : Server Down!\n");
			exit(1);
		}
	}
}
void logout(int signum)
{
	struct ChatMsg msg;
	msg.type = CHAT_FIN;
	send(sockfd, (void *)&msg, sizeof(msg), 0);
	close(sockfd);
	DBG(RED"Bye!\n"NONE);
	exit(1);
}

int main(int argc, char **argv)
{	
	//h:p:n:t:m
	//Host_ip_of_server, Port_of_server, Name_of_player, Team_num(1:blue, 0:red), Message_for_login
	int opt;
	struct LogRequest request;
	struct LogResponse response;
	bzero(&request, sizeof(request));
	bzero(&response, sizeof(response));

	while ((opt = getopt(argc, argv, "h:p:t:m:n:")) != -1) {
		switch(opt) {
			case 'h':
				strcpy(server_ip, optarg);
				break;
			case 'p':
				server_port = atoi(optarg);
				break;
			case 't':
				request.team = atoi(optarg);
				break;
			case 'm':
				strcpy(request.msg, optarg);
				break;
			case 'n':
				strcpy(request.name, optarg);
				break;
			default:
				fprintf(stderr, "Usage : %s [-hptmn]!\n", argv[0]);
				exit(1);
		}
	}
	if (!server_port) server_port = atoi(get_conf_value(path, "SERVERPORT"));
	if(!request.team) request.team = atoi(get_conf_value(path, "TEAM"));
	if(!strlen(server_ip)) strcpy(server_ip, get_conf_value(path, "SERVERIP"));
	if(!strlen(request.name)) strcpy(request.name, get_conf_value(path, "NAME"));
	if(!strlen(request.msg)) strcpy(request.msg, get_conf_value(path, "LOGMSG"));

	DBG("<"GREEN"Config Show"NONE"> : server_ip = %s, server_port = %d, team = %s, name = %s, log_msg = %s\n", server_ip, server_port, request.team ? "BLUE" : "RED", request.name, request.msg);
	
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);
	server.sin_addr.s_addr = inet_addr(server_ip);

	socklen_t len = sizeof(server);

	if ((sockfd = socket_udp()) < 0) {
		perror("socket_udp()\n");
		exit(1);
	}
	
	sendto(sockfd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&server, len);
	
	fd_set set;
	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	int retval = select(sockfd + 1, &set, NULL, NULL, &tv);
	if (retval < 0) {
		perror("select()");
		exit(1);
	} else if (retval) {//ready for receiving data
		int ret = recvfrom(sockfd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&server, &len);
		if (ret != sizeof(response) || response.type == 1) {
			DBG(RED"Error"NONE"The Game Server refused your login.\n\tThis May be helpful:%s\n", response.msg);
			exit(1);
		}
	} else {
		DBG(RED"Error"NONE"The Game Server is out of service!\n");
		exit(1);
	}
	
	DBG(GREEN"Server"NONE" : %s\n", response.msg);

	connect(sockfd, (struct sockaddr *)&server, len);

	pthread_t recv_t;
	pthread_create(&recv_t, NULL, do_recv, NULL);

	signal(SIGINT, logout);

	struct ChatMsg msg;
	while(1) {
		bzero(&msg, sizeof(msg));
		msg.type = CHAT_WALL;//公聊
		printf(RED"Please Input: \n"NONE);
		scanf("%[^\n]", msg.msg); getchar();
		if (strlen(msg.msg)) {
			if (msg.msg[0] == '@') msg.type = CHAT_MSG;//私聊
			if (msg.msg[0] == '#') msg.type = CHAT_FUNC;//功能
			send(sockfd, (void *)&msg, sizeof(msg), 0);
		}
	}
	return 0;
}
