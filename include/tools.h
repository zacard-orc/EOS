#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

typedef struct{
        int type;
        char msg_text[100];
} msg_t;

#ifndef _TOOLS_H_
#define _TOOLS_H_

using namespace std;

//创建和启动MQ
int StartMQ(int proj_id);

//统一报错输出
//参数1：自定义报错信息
void ShowErr(string ermsg);


//设置文件描述符号的非阻塞模式
//参数1：sock文件描述符号
void SetUnBlock(int sock);  



#endif
