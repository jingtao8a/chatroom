/*************************************************************************
	> File Name: sub_reactor.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月21日 星期五 14时38分00秒
 ************************************************************************/

#include "head.h"

void *sub_reactor(void *arg)
{
	struct task_queue *taskQueue = (struct task_queue *)arg;
	pthread_t *tid = (pthread_t *)calloc(NTHREAD, sizeof(pthread_t));
	for (int i = 0; i < NTHREAD; i++) {
		pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
	}

	struct epoll_event ev, events[MAX];
	while (1) {
		DBG(L_RED"Sub Reactor"NONE" : Epoll_Waiting...\n");
		int nfds = epoll_wait(taskQueue->epollfd, events, MAX, -1);
		if (nfds < 0 ) {
			perror("epoll_wait()");
			exit(1);
		}
		for (int i = 0; i < nfds; i++) {
			struct User *user = (struct User *)events[i].data.ptr;
			DBG(L_RED"Sub Reactor"NONE" : %s Ready\n", user->name);
			if (events[i].events & EPOLLIN) {
				task_queue_push(taskQueue, user);
			}
		}
	}
	return NULL;
}

