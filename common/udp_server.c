/*************************************************************************
	> File Name: udp_server.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月20日 星期四 17时08分16秒
 ************************************************************************/

#include "head.h"

int socket_create_udp(int port)
{
	int server_listen;
	if ((server_listen = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	setsockopt(server_listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


	if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
		return -1;
	}

	make_non_block(server_listen);
	return server_listen;
}

