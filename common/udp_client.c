/*************************************************************************
	> File Name: upd_client.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月20日 星期四 17时57分53秒
 ************************************************************************/

#include "head.h"

int socket_udp() {
	int fd;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}
	make_non_block(fd);
	return fd;
}
