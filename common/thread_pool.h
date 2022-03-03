/*************************************************************************
	> File Name: thread_pool.h
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月21日 星期五 13时57分15秒
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
struct task_queue {
	int sum;
	int epollfd;//sub reactor
	struct User **team;
	int head;
	int tail;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd);
void task_queue_push(struct task_queue *taskQueue, struct User *user);
struct User *task_queue_pop(struct task_queue *taskQueue);

void *thread_run(void *arg);
#endif
