//EPOLL 实例
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
 

#define MAXLINE 30
//#define OPEN_MAX 100
#define LISTENQ 2000    //监听队列长度
#define SERV_PORT 8888  //端口
#define INFTIM 1000
#define EVTOTAL 2000	//EPOLL的文件句柄符号
#define HashTbSize 0x156AF9 

 
int nfds=0;

//监控处理的EPOLL事件数
void *dmlog(void *arg)
{
   while(1)
   {
//	 (nfds<=1)?printf("ONLINE PERS:%d\n",1):printf("ONLINE PERS:%d\n",nfds);
   // usleep(500000);
	sleep(1);
     }
}


//设置非阻塞
void Setnonblocking(int sock)
{
   int opts;
   opts=fcntl(sock,F_GETFL);
   if(opts<0)
   {
      perror("fcntl(sock,GETFL)");
      exit(1);
   }
   opts = opts|O_NONBLOCK;
   if(fcntl(sock,F_SETFL,opts)<0)
   {
      perror("fcntl(sock,SETFL,opts)");
      exit(1);
   }   
}

int pp=0;

int main()
{
     int i,maxi,listenfd,connfd,sockfd,epfd;
     int server_len,client_len;
     ssize_t n;
     char line[MAXLINE];
     socklen_t clilen;
     //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
     struct epoll_event ev,events[EVTOTAL];
     //生成用于处理accept的epoll专用的文件描述符
     epfd=epoll_create(2048);
     
     //启动在线用户数统计的线程
     pthread_t dm;
     pthread_attr_t attr={0}; 
     
     pthread_attr_init (&attr);   
     pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);   
     pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
     pthread_create(&dm,NULL,dmlog,NULL);
       
	 //定义服务器SOCKET 
     struct sockaddr_in clientaddr;
     struct sockaddr_in serveraddr;
     
     //定义每个EPOLL事件的收发消息
     char           RECV_MSG[100][30];
     unsigned long  HashTbInd;
     
     listenfd = socket(AF_INET,SOCK_STREAM,0);

     //把socket设置为非阻塞方式
	 Setnonblocking(listenfd);

     //设置与要处理的事件相关的文件描述符
     ev.data.fd=listenfd;

     //设置要处理的事件类型
     ev.events=EPOLLIN|EPOLLET;

     //注册epoll事件
     epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

 
     //设置SOCKET SERVER基本参数
     bzero(&serveraddr, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;  //设定协议簇
     serveraddr.sin_addr.s_addr =htons(INADDR_ANY);  
     serveraddr.sin_port=htons(SERV_PORT);
     bind(listenfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));
     listen(listenfd, LISTENQ);

 

     //maxi=0;
     for (;;) {

          //等待epoll事件的发生
          nfds=epoll_wait(epfd,events,EVTOTAL,-1);
          //printf("online number:%d\n",nfds);
         //处理所发生的所有事件     

          for(i=0;i<nfds;++i)
          {
               //printf("epollnum,epolldatafd,epolleventst,listenfd:%d,%d,%d,%d\n",i,events[i].data.fd,events[i].events,listenfd);  
               //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
               //在EPOLL EVENT中注册新连接
               if(events[i].data.fd==listenfd)    
               {
					client_len = sizeof(clientaddr); 
                    connfd = accept(listenfd,(struct sockaddr *)&clientaddr, (socklen_t *)&client_len);
                    if(connfd<0){
                         perror("connfd<0");
                         exit(1);
                    }
		     		char *str = inet_ntoa(clientaddr.sin_addr);
                    //printf("sourd addr:%s;connfd=%d\n",str,connfd);
					Setnonblocking(connfd);
			
                    //设置用于读操作的文件描述符
                    ev.data.fd=connfd;
                    //设置用于注测的读操作事件
                    ev.events=EPOLLIN|EPOLLET;
                    //注册ev
                    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
               }
			   //如果是已经连接的用户，并且收到数据，那么进行读入。
			   //把新数据这件事儿在EPOLL EVENT 中注册新FD，如果没数据，则关闭EPOLL.EVENTS.DATA.FD
               else if(events[i].events&EPOLLIN)    
               {
                    //printf(" IN:events&EPOLLIN&EPOLLOUT=%d,%d,%d\n",events[i].events,EPOLLIN,EPOLLOUT);
                    if ( (sockfd = events[i].data.fd) < 0) continue;
                    if ( (n = read(sockfd, RECV_MSG[i], MAXLINE)) < 0) {
                         if (errno == ECONNRESET) {
                         	  //usleep(1000);
                              close(sockfd);
                              events[i].data.fd = -1;

                         } else
	    				printf("readline error\n");

                    } else if (n == 0) {
                    	   //usleep(1000);
                         //close(sockfd);
                         events[i].data.fd = -1;
                    }

				    printf("%d:%s",pp++,RECV_MSG[i]);
                    //设置用于写操作的文件描述符
                    ev.data.fd=sockfd;
                    //设置用于注册的写操作事件
                    ev.events=EPOLLOUT|EPOLLET;
                    //修改sockfd上要处理的事件为EPOLLOUT
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
               }
               else if(events[i].events&EPOLLOUT)
               {   
                    //printf("OUT:events&EPOLLIN&EPOLLOUT=%d,%d,%d\n",events[i].events,EPOLLIN,EPOLLOUT);
                    sockfd = events[i].data.fd;
                    //HashTbInd=(HashString(RECV_MSG[i],0)%HashTbSize);
                    //printf("%s:%X:%s ",RECV_MSG[i],HashTbInd,(RO_HPP)->Raw_str);
                    //printf("%s:%X:%s \n",RECV_MSG[i],HashTbInd,(RO_HPP+HashTbInd)->Raw_str);
                    //if ((*(RO_HPP+HashTbInd)).Hash1==HashString(RECV_MSG[i],1)&&((*(RO_HPP+HashTbInd)).bExist==0))
                    	
                    //	{
                    //		write(sockfd,(RO_HPP+HashTbInd)->Raw_str,strlen((RO_HPP+HashTbInd)->Raw_str));}
                    		
                    //else
                   	write(events[i].data.fd,"Hello world",9);
                    //printf("RECV MSG=%s,SOCKFD=%d,nfds=%d,nfds_i=%d,RESPONSE OK\n",line,events[i].data.fd,nfds,i);
                    //close(sockfd);
					//清空报文
                    bzero(RECV_MSG[i],30);
                    ev.data.fd=sockfd;
                    //通知保安把状态置可读操作事件
                    ev.events=EPOLLIN|EPOLLET;
                    //close(sockfd);
                    //修改sockfd上要处理的事件为EPOLIN
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
               }
          }
     }
}



