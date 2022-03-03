/*************************************************************************
	> File Name: thread_pool.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月21日 星期五 13时57分22秒
 ************************************************************************/

#include "head.h"

extern int repollfd, bepollfd;
extern struct User *rteam, *bteam;
extern pthread_mutex_t rmutex, bmutex;

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd)
{
	taskQueue->sum = sum;
	taskQueue->epollfd = epollfd;
	taskQueue->team = calloc(sum, sizeof(void *));
	taskQueue->head = taskQueue->tail = 0;
	pthread_mutex_init(&taskQueue->mutex, NULL);
	pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue *taskQueue, struct User *user)
{
	pthread_mutex_lock(&taskQueue->mutex);
	if ((taskQueue->tail + 1) % taskQueue->sum == taskQueue->head) {
		DBG(RED"task_queue is full"NONE" : push %s fail\n", user->name);
		return;
	}
	taskQueue->team[taskQueue->tail] = user;
	taskQueue->tail = (taskQueue->tail + 1) % taskQueue->sum;
	DBG(L_GREEN"Thread Pool"NONE" : Task push %s\n", user->name);
	pthread_cond_signal(&taskQueue->cond);
	pthread_mutex_unlock(&taskQueue->mutex);
}

struct User *task_queue_pop(struct task_queue *taskQueue)
{
	pthread_mutex_lock(&taskQueue->mutex);
	while (taskQueue->head == taskQueue->tail) {
		DBG(L_GREEN"Thread Pool"NONE" : Task Queue Empty, Waiting For Task\n");	
		pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
	}
	struct User *user = taskQueue->team[taskQueue->head];
	taskQueue->head = (taskQueue->head + 1) % taskQueue->sum;
	DBG(L_GREEN"Thread Pool"NONE" : Task Pop %s\n", user->name);	
	pthread_mutex_unlock(&taskQueue->mutex);
	return user;
}
static void send_all(struct ChatMsg *msg) {
	for (int i = 0; i < MAX; i++) {
		if (bteam[i].online) send(bteam[i].fd, (void *)msg, sizeof(struct ChatMsg), 0);
		if (rteam[i].online) send(rteam[i].fd, (void *)msg, sizeof(struct ChatMsg), 0);
	}
}
static void send_to(char *to, struct ChatMsg *msg, int fd)
{
	int flag = 0;
	for (int i = 0; i < MAX; i++) {
		if (rteam[i].online && (!strcmp(to, rteam[i].name))) {
			send(rteam[i].fd, msg, sizeof(struct ChatMsg), 0);
			flag = 1;
			break;
			
		}
		if (bteam[i].online && (!strcmp(to, bteam[i].name))) {
			send(bteam[i].fd, msg, sizeof(struct ChatMsg), 0);
			flag = 1;
			break;
		}
	}
	if (!flag) {
		memset(msg->msg, 0, sizeof(msg->msg));
		sprintf(msg->msg, "receiver %s is not online", to);
		msg->type = CHAT_SYS;//系统消息
		send(fd, msg, sizeof(struct ChatMsg), 0);
	}
}
static void do_work(struct User *user)
{
	struct ChatMsg r_msg, msg;
	bzero(&msg, sizeof(msg));
	bzero(&r_msg, sizeof(r_msg));
	
	recv(user->fd, (void *)&msg, sizeof(msg), 0);

	if(msg.type & CHAT_WALL) {//公聊
		printf("<%s> ~ %s\n", user->name, msg.msg);
		strcpy(msg.name, user->name);
		send_all(&msg);
	} else if (msg.type & CHAT_MSG) {//私聊
		char to[20] = {0};
		int i = 1;
		for(; i <= 21; i++) {
			if(msg.msg[i] == ' ') break;
		}
		if (msg.msg[i] != ' ' || msg.msg[0] != '@') {
			memset(&r_msg, 0, sizeof(r_msg));
			r_msg.type = CHAT_SYS;
			sprintf(r_msg.msg, "msg format error!");
			send(user->fd, (void *)&r_msg, sizeof(r_msg), 0);
		} else {
			msg.type = CHAT_MSG;
			strcpy(msg.name, user->name);
			strncpy(to, msg.msg + 1, i - 1);
			send_to(to, &msg, user->fd);
		}
	} else if (msg.type & CHAT_FIN) {
		bzero(msg.msg, sizeof(msg.msg));
		msg.type = CHAT_SYS;//系统消息
		sprintf(msg.msg, "%s log out\n", user->name);
		strcpy(msg.name, user->name);
		send_all(&msg);
		if(user->team)
			pthread_mutex_lock(&bmutex);
		else 
			pthread_mutex_lock(&rmutex);

		user->online = 0;
		int epollfd = user->team == 1 ? bepollfd : repollfd;
		del_event(epollfd, user->fd);
		
		if (user->team)
			pthread_mutex_unlock(&bmutex);
		else 
			pthread_mutex_unlock(&rmutex);
		printf(GREEN"Server Info"NONE" : %s log out\n", user->name);
		close(user->fd);
	}

}

void *thread_run(void *arg)
{
	pthread_detach(pthread_self());
	struct task_queue *taskQueue = (struct task_queue *)arg;
	while(1) {
		struct User *user = task_queue_pop(taskQueue);
		do_work(user);
	}
}


