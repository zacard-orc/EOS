//EPOLL ʵ��
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
#define LISTENQ 2000    //�������г���
#define SERV_PORT 8888  //�˿�
#define INFTIM 1000
#define EVTOTAL 2000	//EPOLL���ļ��������
#define HashTbSize 0x156AF9 

 
int nfds=0;

//��ش����EPOLL�¼���
void *dmlog(void *arg)
{
   while(1)
   {
//	 (nfds<=1)?printf("ONLINE PERS:%d\n",1):printf("ONLINE PERS:%d\n",nfds);
   // usleep(500000);
	sleep(1);
     }
}


//���÷�����
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
     //����epoll_event�ṹ��ı���,ev����ע���¼�,�������ڻش�Ҫ������¼�
     struct epoll_event ev,events[EVTOTAL];
     //�������ڴ���accept��epollר�õ��ļ�������
     epfd=epoll_create(2048);
     
     //���������û���ͳ�Ƶ��߳�
     pthread_t dm;
     pthread_attr_t attr={0}; 
     
     pthread_attr_init (&attr);   
     pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);   
     pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
     pthread_create(&dm,NULL,dmlog,NULL);
       
	 //���������SOCKET 
     struct sockaddr_in clientaddr;
     struct sockaddr_in serveraddr;
     
     //����ÿ��EPOLL�¼����շ���Ϣ
     char           RECV_MSG[100][30];
     unsigned long  HashTbInd;
     
     listenfd = socket(AF_INET,SOCK_STREAM,0);

     //��socket����Ϊ��������ʽ
	 Setnonblocking(listenfd);

     //������Ҫ������¼���ص��ļ�������
     ev.data.fd=listenfd;

     //����Ҫ������¼�����
     ev.events=EPOLLIN|EPOLLET;

     //ע��epoll�¼�
     epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

 
     //����SOCKET SERVER��������
     bzero(&serveraddr, sizeof(serveraddr));
     serveraddr.sin_family = AF_INET;  //�趨Э���
     serveraddr.sin_addr.s_addr =htons(INADDR_ANY);  
     serveraddr.sin_port=htons(SERV_PORT);
     bind(listenfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));
     listen(listenfd, LISTENQ);

 

     //maxi=0;
     for (;;) {

          //�ȴ�epoll�¼��ķ���
          nfds=epoll_wait(epfd,events,EVTOTAL,-1);
          //printf("online number:%d\n",nfds);
         //�����������������¼�     

          for(i=0;i<nfds;++i)
          {
               //printf("epollnum,epolldatafd,epolleventst,listenfd:%d,%d,%d,%d\n",i,events[i].data.fd,events[i].events,listenfd);  
               //����¼�⵽һ��SOCKET�û����ӵ��˰󶨵�SOCKET�˿ڣ������µ����ӡ�
               //��EPOLL EVENT��ע��������
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
			
                    //�������ڶ��������ļ�������
                    ev.data.fd=connfd;
                    //��������ע��Ķ������¼�
                    ev.events=EPOLLIN|EPOLLET;
                    //ע��ev
                    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
               }
			   //������Ѿ����ӵ��û��������յ����ݣ���ô���ж��롣
			   //������������¶���EPOLL EVENT ��ע����FD�����û���ݣ���ر�EPOLL.EVENTS.DATA.FD
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
                    //��������д�������ļ�������
                    ev.data.fd=sockfd;
                    //��������ע���д�����¼�
                    ev.events=EPOLLOUT|EPOLLET;
                    //�޸�sockfd��Ҫ������¼�ΪEPOLLOUT
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
					//��ձ���
                    bzero(RECV_MSG[i],30);
                    ev.data.fd=sockfd;
                    //֪ͨ������״̬�ÿɶ������¼�
                    ev.events=EPOLLIN|EPOLLET;
                    //close(sockfd);
                    //�޸�sockfd��Ҫ������¼�ΪEPOLIN
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
               }
          }
     }
}



