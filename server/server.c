/*************************************************************************
	> File Name: server.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月20日 星期四 16时43分50秒
 ************************************************************************/
#define _D
#include "head.h"

char *conf = "./footballd.conf";

int repollfd, bepollfd;//sub reactor
int epollfd;//main reactor
int port;//port
struct User *rteam, *bteam;

pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
	int opt, listener; 
	pthread_t red_t, blue_t;
	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch(opt) {
			case 'p':
				port = atoi(optarg);
			default:
				fprintf(stderr, "Usage : %s -p port\n", argv[0]);
				exit(1);
		}
	}
	if (!port) port = atoi(get_conf_value(conf, "PORT"));
	if ((listener = socket_create_udp(port)) < 0) {
		perror("socket_create_udp()");
		exit(1);
	}
	
	DBG(GREEN"INFO"NONE" : Server start ON port %d.\n", port);
	
	rteam = (struct User *)calloc(MAX, sizeof(struct User));
	bteam = (struct User *)calloc(MAX, sizeof(struct User));
	
	epollfd = epoll_create(MAX * 2);//main reactor
	repollfd = epoll_create(MAX);//sub reactor
	bepollfd = epoll_create(MAX);

	if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
		perror("epoll_create()");
		exit(1);
	}

	struct task_queue redQueue;
	struct task_queue blueQueue;
	
	task_queue_init(&redQueue, MAX, repollfd);
	task_queue_init(&blueQueue, MAX, bepollfd);

	pthread_create(&red_t, NULL, sub_reactor, (void *)&redQueue);
	pthread_create(&blue_t, NULL, sub_reactor, (void *)&blueQueue);

	struct epoll_event ev, events[MAX];
	ev.events = EPOLLIN;
	ev.data.fd = listener;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
		perror("epoll_ctl()");
		exit(1);
	}

	struct sockaddr_in client;
	bzero(&client, sizeof(client));
	socklen_t len = sizeof(client);

	while (1) {
		DBG(YELLOW"Main Reactor"NONE" : Waiting for client.\n");
		int nfds = epoll_wait(epollfd, events, MAX, -1);
		if (nfds < 0) {
			perror("epoll_wait()");
			exit(1);
		}
		for(int i = 0; i < nfds; i++) {
			struct User user;
			char buff[512] = {0};
			if (events[i].data.fd == listener) {
				int new_fd = udp_accept(listener, &user);
				if (new_fd > 0) {
					add_to_sub_reactor(&user);
				}
			}
		}
	}
	return 0;
}

