/*************************************************************************
	> File Name: udp_epoll.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月21日 星期五 15时30分34秒
 ************************************************************************/

#include "head.h"
extern int port;
extern int repollfd, bepollfd;
extern struct User *rteam, *bteam;

static void add_event_ptr(int epollfd, int fd, int events, struct User *user)
{
	struct epoll_event ev;
	ev.data.ptr = user;
	ev.events = events;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void del_event(int epollfd, int fd) 
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

static int udp_connect(struct sockaddr_in *client)
{
	int sockfd;
	if ((sockfd = socket_create_udp(port)) < 0) {
		perror("socket_udp()");
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *)client, sizeof(struct sockaddr)) < 0) {//udp connect
		return -1;
	}
	return sockfd;
}

static int check_online(struct LogRequest *request)
{
	for (int i = 0; i < MAX; i++) {
		if (rteam[i].online && !strcmp(request->name, rteam[i].name)) {
			return 1;
		}
		if (bteam[i].online && !strcmp(request->name, bteam[i].name)) {
			return 1;
		}

	}
	return 0;
}

int udp_accept(int fd, struct User *user) 
{
	int new_fd, ret;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	
	struct LogRequest request;
	struct LogResponse response;
	bzero(&request, sizeof(request));
	bzero(&response, sizeof(response));

	ret = recvfrom(fd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, &len);
	
	if (ret != sizeof(request)) {
		response.type = 1;//NO
		strcpy(response.msg, "Login Failed with Data errors!");
		sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
		return -1;
	}

	if (check_online(&request)) {//repeatively login
		response.type = 1;//NO
		strcpy(response.msg, "You are Already Loginning!");
		sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
		return -1;
	}

	if (request.team) {
		DBG(GREEN"Info"NONE" : "BLUE"%s Login in %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
	} else {
		DBG(GREEN"Info"NONE" : "RED"%s Login in %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
	}
	response.type = 0;
	strcpy(response.msg, "Login success!\n");
	sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);

	strcpy(user->name, request.name);
	user->team = request.team;
	new_fd = udp_connect(&client);
	user->fd = new_fd;
	return new_fd;
}

static int find_sub(struct User *team)
{
	for (int i = 0; i < MAX; i++) {
		if (team[i].online == 0) return i;
	}
	return -1;
}

void add_to_sub_reactor(struct User *user)
{
	struct User *team = (user->team ? bteam : rteam);
	int sub = find_sub(team);
	if (sub == -1) {
		fprintf(stderr, "Full Team!\n");
		return;
	}
	team[sub] = *user;
	team[sub].online = 1;
	team[sub].flag =  10;
	DBG(L_RED"sub = %d, name = %s\n", sub, team[sub].name);
	if (user->team)//blue
		add_event_ptr(bepollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
	else //red
		add_event_ptr(repollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
}
