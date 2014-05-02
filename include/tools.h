#ifndef _TOOLS_H_
#define _TOOLS_H_

using namespace std;

//统一报错输出
//参数1：自定义报错信息
void ShowErr(string ermsg);

//创建和启动MQ
int StartMQ(int proj_id);

//设置文件描述符号的非阻塞模式
//参数1：sock文件描述符号
void Setnonblocking(int sock);  



#endif
