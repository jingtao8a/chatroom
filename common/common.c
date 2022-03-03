/*************************************************************************
	> File Name: common.c
	> Author: yuxintao
	> Mail: 1921056015@qq.com 
	> Created Time: 2022年01月16日 星期日 19时38分28秒
 ************************************************************************/
#include "head.h"

char conf_ans[50] = {0};

int socket_create(int port){
	//socket()
	//bind()
	//listen()
	int sockfd;
	struct sockaddr_in server;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return -1;//error
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("bind()");
		return -1;//error
	}

	if(listen(sockfd, 10) < 0){
		perror("listen()");
		return -1;//error
	}
	return sockfd;
}

void make_non_block(int fd)
{
	unsigned long ul = 1;
	ioctl(fd, FIONBIO, &ul);
}

void make_block(int fd)
{
	unsigned long ul = 0;
	ioctl(fd, FIONBIO, &ul);
}

char *get_conf_value(const char *path, const char *key)
{
	FILE *fp = NULL;
	char *line = NULL, *sub = NULL;
	long len, nrd;
	memset(conf_ans, 0, sizeof(conf_ans));
	if (path == NULL || key == NULL) {
		fprintf(stderr, "Error in argument!\n");
		return NULL;
	}
	if ((fp = fopen(path, "r")) == NULL) {
		perror("fopen()");
		return NULL;
	}
	while ((nrd = getline(&line, &len, fp)) != -1) {
		if ((sub = strstr(line, key)) == NULL) continue;

		if (line[strlen(key)] == '=') {
			strncpy(conf_ans, sub + strlen(key) + 1, nrd - strlen(key) - 2);
			break;
		}
	}
	free(line);
	fclose(fp);
	if (sub == NULL) {
		return NULL;
	}
	return conf_ans;
}
