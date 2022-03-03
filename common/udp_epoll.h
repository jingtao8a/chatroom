/*************************************************************************
	> File Name: udp_epoll.h
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月21日 星期五 15时30分43秒
 ************************************************************************/

#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H

void del_event(int epollfd, int fd);
int udp_accept(int fd, struct User *user);
void add_to_sub_reactor(struct User *user);

#endif

