/* 作者:Zacard
*  EPOLL 服务器 用于收取来自外部的消息，并转存至本机消息队列
*/
#include "tools.h"

#define MAXCHARNUM 512         //从SOCKET读取的字符数
#define LISTENQ 2000        //监听队列长度
#define SERV_PORT 9999      //对外提供的服务端口
#define EP_INS_NUM 2048     //总共监控多少个EPOLL实例数
#define EP_HINS_NUM 2000	//每一次WAIT时能处理的EPOLL实例数
#define EP_TXT_LEN 512     //每次收到的报文长度设定
#define HELLO  "ICU"

using namespace std;

int nfds=0;


//监控处理的EPOLL实时事件数
void *dmlog(void *arg)
{
   while(1)
   {
	  cout<<"ONLINE PERS:";
      (nfds<=1)?cout<<1<<endl:cout<<nfds<<endl;
	  usleep((long)arg*1000);
   }
   return (void *)0;
}


int main()
{
	 //定义服务器SOCKET参数
     struct sockaddr_in clientaddr,serveraddr;
	 int listenfd,connfd,sockfd,epfd;  
	 int n_call=0;    //被调用的次数
     int n_client_len;   //客户端的地址长度
     int n_readflag;     //从端口中读数据的成功失败标示
	 listenfd=socket(AF_INET,SOCK_STREAM,0);
	 SetUnBlock(listenfd);

	 
     bzero(&serveraddr, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;  //设定协议簇
     serveraddr.sin_addr.s_addr =htons(INADDR_ANY);  //本机地址
     serveraddr.sin_port=htons(SERV_PORT); //端口号
	 
     bind(listenfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));
     listen(listenfd,LISTENQ);

     
     //启动在线用户数统计的线程
     pthread_t dm;
     pthread_attr_t attr; 
     pthread_attr_init (&attr);   
     pthread_attr_setscope (&attr, PTHREAD_SCOPE_PROCESS);   
     pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
     pthread_create(&dm,&attr,dmlog,(void *)10000);

     
     //定义EPOLL事件基础信息
	 struct epoll_event ev,events[EP_HINS_NUM];
	 char RECV_MSG[EP_HINS_NUM][EP_TXT_LEN];
	 char ANSW_MSG[EP_TXT_LEN]=HELLO;
	 epfd=epoll_create(EP_INS_NUM);  //注册监控实例数
     ev.data.fd=listenfd;
     ev.events=EPOLLIN|EPOLLET;
     epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev); //注册描述符



	 while(1)
	 {
          nfds=epoll_wait(epfd,events,EP_HINS_NUM,-1); //轮训发生数

          for(int i=0;i<nfds;++i)
          {
               if(events[i].data.fd==listenfd)    
               {
					//cout<<"Step 1 start"<<endl;
					n_client_len = sizeof(clientaddr); 
                    connfd = accept(listenfd,(struct sockaddr *)&clientaddr, (socklen_t *)&n_client_len);
                    if(connfd<0){
                         perror("connfd<0");
                         exit(1);
                    }
					cout<<"RECV..."<<endl;
					SetUnBlock(connfd);
                    ev.data.fd=connfd;
                    ev.events=EPOLLIN|EPOLLET; 
                    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
					//cout<<"Step 1 end"<<endl;
               }
			   //如果是已经连接的用户，并且收到数据，那么进行读入。
			   //把新数据这件事儿在EPOLL EVENT 中注册新FD，如果没数据，则关闭EPOLL.EVENTS.DATA.FD
               else if(events[i].events&EPOLLIN)    
               {
				    //cout<<"Step 2 start"<<endl;
                    if ((sockfd = events[i].data.fd) < 0) continue;
                    if ((n_readflag= read(sockfd, RECV_MSG[i], MAXCHARNUM)) < 0) {
                         if (errno == ECONNRESET) {
                              close(sockfd);
                              events[i].data.fd = -1;
                         } else
	    				cout<<"readmsg error\n"<<endl;
                    } else if (n_readflag == 0) {
                        events[i].data.fd = -1;
                    } else {
						cout<<RECV_MSG[i]<<endl;
						//cout<<n_call++<<"__"<<RECV_MSG[i]<<endl;
					}
					ev.data.fd=sockfd;
                    ev.events=EPOLLOUT|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
					//cout<<"Step 2 end"<<endl;
               }
               else if(events[i].events&EPOLLOUT)
               {   
			   		//cout<<"Step 3 start"<<endl;
                    sockfd = events[i].data.fd;
                   	write(events[i].data.fd,ANSW_MSG,strlen(ANSW_MSG));
                    bzero(RECV_MSG[i],EP_TXT_LEN);
                    ev.data.fd=sockfd;
                    ev.events=EPOLLIN|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
					//cout<<"Step 3 end"<<endl;
               }
          }
     }
}



