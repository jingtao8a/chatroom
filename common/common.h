/*************************************************************************
	> File Name: common.h
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月16日 星期日 19时56分19秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

int socket_create(int port);
void make_non_block(int fd);
void make_block(int fd);

char *get_conf_value(const char *path, const char *key);
extern char conf_ans[50];

#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else 
#define DBG(fmt, args...) 
#endif

#endif

