/*************************************************************************
	> File Name: head.h
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月16日 星期日 19时47分44秒
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "color.h"
#include "common.h"
#include "datatype.h"
#include "udp_server.h"
#include "udp_client.h"
#include "thread_pool.h"
#include "sub_reactor.h"
#include "udp_epoll.h"
#endif 

